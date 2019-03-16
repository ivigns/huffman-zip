#include "HuffmanZip.h"

#include "BitStream.h"
#include "MinHeap.h"
#include "Node.h"

#include <stack>
#include <utility>
#include <vector>

// Delete node with sub-tree
void deleteNode( CNode* node )
{
    if( node == 0 ) {
        return;
    }
    deleteNode( node->Left );
    deleteNode( node->Right );
    delete node;
}

// Transforms encoding tree into vector of codes for each symbol
void encodeTreeTraverseDFS( CNode* root, std::vector<ubyte>* codes,
    std::vector<ubyte>** doubledCodes )
{
    std::stack<std::pair<CNode*, std::vector<ubyte>>> callStack;
    std::vector<ubyte> tmp;
    callStack.push( std::make_pair( root, tmp ) );
    while( callStack.size() ) {
        CNode* node = callStack.top().first;
        std::vector<ubyte> buffer = callStack.top().second;
        callStack.pop();
        if( node->Left == 0 ) {
            if( node->Symbol.size() == 1 ) {
                codes[node->Symbol[0]] = buffer;
            } else if( node->Symbol.size() == 2 ) {
                doubledCodes[node->Symbol[0]][node->Symbol[1]] = buffer;
            }
            continue;
        }
        buffer.push_back( 0 );
        callStack.push( std::make_pair( node->Left, buffer ) );
        buffer.pop_back();
        buffer.push_back( 1 );
        callStack.push( std::make_pair( node->Right, buffer ) );
    }
}

// Writes encoding tree in stream
void writeTreeTraverseDFS( CNode* node, COutBitStream& stream )
{
    if( node == 0 ) {
        return;
    }
    if( node->Left == 0 ) {
        stream.WriteBit( 1 );
        if( node->Symbol.size() == 1 ) {
            stream.WriteBit( 0 );
            stream.WriteByte( node->Symbol[0] );
        } else {
            stream.WriteBit( 1 );
            stream.WriteByte( node->Symbol[0] );
            stream.WriteByte( node->Symbol[1] );
        }
        return;
    }
    writeTreeTraverseDFS( node->Left, stream );
    writeTreeTraverseDFS( node->Right, stream );
    stream.WriteBit( 0 );
}

void HuffmanZip::Encode( CInputStream& original, COutputStream& compressed )
{
    size_t charsCount[256];
    memset( charsCount, 0, 256 * sizeof( size_t ) );
    size_t doubledCharsCount[256][256];
    for( int i = 0; i < 256; ++i ) {
        memset( doubledCharsCount[i], 0, 256 * sizeof( size_t ) );
    }
    CInBitStream inStream;
    char c = 0;
    char prevc = 0;
    bool firstTime = true;
    while( original.Read( c ) ) {
        ++charsCount[(ubyte) c];
        inStream.PushBack( (ubyte) c );
        if( !firstTime ) {
            ++doubledCharsCount[(ubyte) prevc][(ubyte) c];
        } else {
            firstTime = false;
        }
        //if( charsCount[(ubyte) c] >= 100 ) {
        //    for( int i = 0; i < 256; ++i ) {
        //        if( charsCount[i] > 0 ) {
        //            charsCount[i] /= 2;
        //            ++charsCount[i];
        //        }
        //    }
        //}
        prevc = c;
    }
    COutBitStream outStream;
    const std::vector<ubyte>& inBuffer = inStream.GetBuffer();

    for( int k = 0; k < COMPRESS_PARAM; ++k ) {
        size_t originalCharsCount[256];
        memcpy( originalCharsCount, charsCount, 256 * sizeof( size_t ) );
        {
            outStream.Clear();

            CNode* root = 0;
            {
                for( int t = 0; t < 2; ++t ) {
                    memset( charsCount, 0, 256 * sizeof( size_t ) );
                    size_t tmpdoubledCharsCount[256][256];
                    for( int i = 0; i < 256; ++i ) {
                        memcpy( tmpdoubledCharsCount[i], doubledCharsCount[i], 256 * sizeof( size_t ) );
                        memset( doubledCharsCount[i], 0, 256 * sizeof( size_t ) );
                    }
                    ubyte iPrev = 0;
                    firstTime = true;
                    for( size_t i = 0; i < inBuffer.size(); ++i ) {
                        if( firstTime ) {
                            iPrev = inBuffer[i];
                            firstTime = false;
                            continue;
                        }
                        if( tmpdoubledCharsCount[iPrev][inBuffer[i]] >= DOUBLEDCHAR_PARAM ) {
                            ++doubledCharsCount[iPrev][inBuffer[i]];
                            firstTime = true;
                        } else {
                            ++charsCount[iPrev];
                        }
                        iPrev = inBuffer[i];
                    }
                    if( !firstTime ) {
                        ++charsCount[inBuffer.back()];
                    }
                }

                std::vector<CNode*> chars;
                for( int i = 0; i < 256; ++i ) {
                    if( charsCount[i] > 0 ) {
                        std::vector<ubyte> tmp;
                        tmp.push_back( i );
                        chars.push_back( new CNode( tmp, charsCount[i] ) );
                    }
                }
                for( int i = 0; i < 256; ++i ) {
                    for( int j = 0; j < 256; ++j ) {
                        if( doubledCharsCount[i][j] > 0 ) {
                            std::vector<ubyte> tmp;
                            tmp.push_back( i );
                            tmp.push_back( j );
                            chars.push_back( new CNode( tmp, doubledCharsCount[i][j] ) );
                        }
                    }
                }

                CMinHeap heap( &chars[0], chars.size() );
                heap.Merge();
                root = heap.GetMin();
            }

            std::vector<ubyte> charCodes[256];
            std::vector<ubyte>** doubledCharCodes = new std::vector<ubyte>*[256];
            for( int i = 0; i < 256; ++i ) {
                doubledCharCodes[i] = new std::vector<ubyte>[256];
            }
            encodeTreeTraverseDFS( root, charCodes, doubledCharCodes );

            writeTreeTraverseDFS( root, outStream );
            outStream.WriteBit( 0 );
            size_t inBufferSize = inBuffer.size();
            for( int i = 0; i < sizeof( size_t ); ++i ) {
                ubyte tmp = (inBufferSize >> ((sizeof( size_t ) - 1 - i) * 8)) & 255;
                outStream.WriteByte( tmp );
            }
            ubyte iPrev = 0;
            firstTime = true;
            for( size_t i = 0; i < inBuffer.size(); ++i ) {
                if( firstTime ) {
                    iPrev = inBuffer[i];
                    firstTime = false;
                    continue;
                }
                if( doubledCharCodes[iPrev][inBuffer[i]].size() > 0 ) {
                    for( size_t j = 0; j < doubledCharCodes[iPrev][inBuffer[i]].size(); ++j ) {
                        outStream.WriteBit( doubledCharCodes[iPrev][inBuffer[i]][j] );
                    }
                    firstTime = true;
                } else {
                    for( size_t j = 0; j < charCodes[iPrev].size(); ++j ) {
                        outStream.WriteBit( charCodes[iPrev][j] );
                    }
                }
                iPrev = inBuffer[i];
            }
            if( !firstTime ) {
                for( size_t j = 0; j < charCodes[inBuffer.back()].size(); ++j ) {
                    outStream.WriteBit( charCodes[inBuffer.back()][j] );
                }
            }

            deleteNode( root );
            for( int i = 0; i < 256; ++i ) {
                delete[] doubledCharCodes[i];
            }
            delete[] doubledCharCodes;
        }

        const std::vector<ubyte>& result = outStream.GetBuffer();
        if( inBuffer.size() > result.size() ) {
            outStream.Clear();

            CNode* root = 0;
            {
                std::vector<CNode*> chars;
                for( int i = 0; i < 256; ++i ) {
                    if( charsCount[i] > 0 ) {
                        std::vector<ubyte> tmp;
                        tmp.push_back( i );
                        chars.push_back( new CNode( tmp, charsCount[i] ) );
                    }
                }
                for( int i = 0; i < 256; ++i ) {
                    for( int j = 0; j < 256; ++j ) {
                        if( doubledCharsCount[i][j] > 0 ) {
                            std::vector<ubyte> tmp;
                            tmp.push_back( i );
                            tmp.push_back( j );
                            chars.push_back( new CNode( tmp, doubledCharsCount[i][j] ) );
                        }
                    }
                }

                CMinHeap heap( &chars[0], chars.size() );
                heap.Merge();
                root = heap.GetMin();
            }

            std::vector<ubyte> charCodes[256];
            std::vector<ubyte>** doubledCharCodes = new std::vector<ubyte>*[256];
            for( int i = 0; i < 256; ++i ) {
                doubledCharCodes[i] = new std::vector<ubyte>[256];
            }
            encodeTreeTraverseDFS( root, charCodes, doubledCharCodes );

            writeTreeTraverseDFS( root, outStream );
            outStream.WriteBit( 0 );
            size_t inBufferSize = inBuffer.size();
            for( int i = 0; i < sizeof( size_t ); ++i ) {
                ubyte tmp = (inBufferSize >> ((sizeof( size_t ) - 1 - i) * 8)) & 255;
                outStream.WriteByte( tmp );
            }
            ubyte iPrev = 0;
            firstTime = true;
            for( size_t i = 0; i < inBuffer.size(); ++i ) {
                if( firstTime ) {
                    iPrev = inBuffer[i];
                    firstTime = false;
                    continue;
                }
                if( doubledCharCodes[iPrev][inBuffer[i]].size() > 0 ) {
                    for( size_t j = 0; j < doubledCharCodes[iPrev][inBuffer[i]].size(); ++j ) {
                        outStream.WriteBit( doubledCharCodes[iPrev][inBuffer[i]][j] );
                    }
                    firstTime = true;
                } else {
                    for( size_t j = 0; j < charCodes[iPrev].size(); ++j ) {
                        outStream.WriteBit( charCodes[iPrev][j] );
                    }
                }
                iPrev = inBuffer[i];
            }
            if( !firstTime ) {
                for( size_t j = 0; j < charCodes[inBuffer.back()].size(); ++j ) {
                    outStream.WriteBit( charCodes[inBuffer.back()][j] );
                }
            }

            deleteNode( root );
            for( int i = 0; i < 256; ++i ) {
                delete[] doubledCharCodes[i];
            }
            delete[] doubledCharCodes;
        }

        inStream.Clear();
        memset( charsCount, 0, 256 * sizeof( size_t ) );
        for( size_t i = 0; i < result.size(); ++i ) {
            ++charsCount[result[i]];
            inStream.PushBack( result[i] );
        }
    }

    const std::vector<ubyte>& result = outStream.GetBuffer();
    for( size_t i = 0; i < result.size(); ++i ) {
        compressed.Write( result[i] );
    }
}

void HuffmanZip::Decode( CInputStream& compressed, COutputStream& original )
{
    CInBitStream inStream;
    {
        char c = 0;
        while( compressed.Read( c ) ) {
            inStream.PushBack( (ubyte) c );
        }
    }
    COutBitStream outStream;

    for( int k = 0; k < COMPRESS_PARAM; ++k ) {
        outStream.Clear();

        CNode* root = 0;
        {
            std::stack<CNode*> nodeStack;
            while( true ) {
                ubyte c = inStream.ReadBit();
                if( (c == 0) && (nodeStack.size() == 1) ) {
                    break;
                }
                if( !c ) {
                    CNode* nodeRight = nodeStack.top();
                    nodeStack.pop();
                    CNode* nodeLeft = nodeStack.top();
                    nodeStack.pop();
                    std::vector<ubyte> tmp;
                    CNode* node = new CNode( tmp, nodeLeft->Priority + nodeRight->Priority );
                    node->Left = nodeLeft;
                    node->Right = nodeRight;
                    nodeStack.push( node );
                } else {
                    ubyte isDoubledChar = inStream.ReadBit();
                    std::vector<ubyte> tmp;
                    ubyte sym = inStream.ReadByte();
                    tmp.push_back( sym );
                    if( isDoubledChar ) {
                        sym = inStream.ReadByte();
                        tmp.push_back( sym );
                    }
                    nodeStack.push( new CNode( tmp, 1 ) );
                }
            }
            root = nodeStack.top();
        }

        size_t outStreamSize = 0;
        for( int i = 0; i < sizeof( size_t ); ++i ) {
            outStreamSize <<= 8;
            outStreamSize |= inStream.ReadByte();
        }

        for( size_t i = 0; i < outStreamSize; ++i ) {
            CNode* node = root;
            do {
                if( node == 0 ) {
                    break;
                }
                ubyte c = inStream.ReadBit();
                if( !c ) {
                    node = node->Left;
                } else {
                    node = node->Right;
                }
            } while( node->Left != 0 );
            if( node->Symbol.size() == 1 ) {
                outStream.WriteByte( node->Symbol[0] );
            } else if( node->Symbol.size() == 2 ) {
                outStream.WriteByte( node->Symbol[0] );
                outStream.WriteByte( node->Symbol[1] );
                ++i;
            }
        }

        deleteNode( root );

        inStream.Clear();
        const std::vector<ubyte>& result = outStream.GetBuffer();
        for( size_t i = 0; i < result.size(); ++i ) {
            inStream.PushBack( result[i] );
        }
    }

    const std::vector<ubyte>& result = outStream.GetBuffer();
    for( size_t i = 0; i < result.size(); ++i ) {
        original.Write( result[i] );
    }
}