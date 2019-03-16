#pragma once
#include <vector>

typedef unsigned char ubyte;

// Node for encoding tree
struct CNode {
    std::vector<ubyte> Symbol;
    size_t Priority;    // Priority for MinHeap
    CNode* Left;
    CNode* Right;
    CNode( std::vector<ubyte> sym, size_t pri ) :
        Symbol( sym ), Priority( pri ), Left( 0 ), Right( 0 )
    {
    }
};