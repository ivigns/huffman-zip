#include "MinHeap.h"

#include <memory.h>
#include <utility>

CMinHeap::CMinHeap()
{
    currentSize = 0;
    maxSize = 3;
    buffer = new CNode*[maxSize];
}

CMinHeap::CMinHeap( CNode** arr, int arr_size )
{
    maxSize = currentSize = arr_size;
    buffer = new CNode*[maxSize];
    memcpy( buffer, arr, arr_size * sizeof( CNode* ) );
    for( int i = (arr_size - 1) / 2; i >= 0; --i ) {
        siftDown( i );
    }
}

CMinHeap::~CMinHeap()
{
    delete[] buffer;
}

void CMinHeap::siftUp( int i )
{
    while( i > 0 ) {
        int parent = (i - 1) / 2;
        if( buffer[i]->Priority >= buffer[parent]->Priority ) {
            return;
        }
        std::swap( buffer[i], buffer[parent] );
        i = parent;
    }
}

void CMinHeap::siftDown( int i )
{
    while( 2 * i + 1 < currentSize ) {
        int child1 = 2 * i + 1;
        int child2 = (2 * i + 2 < currentSize) ? (2 * i + 2) : child1;
        int minChild = (buffer[child1]->Priority < buffer[child2]->Priority) ? child1 : child2;
        if( buffer[minChild]->Priority >= buffer[i]->Priority ) {
            return;
        }
        std::swap( buffer[minChild], buffer[i] );
        i = minChild;
    }
}


CNode* CMinHeap::GetMin() const
{
    return currentSize ? buffer[0] : 0;
}

int CMinHeap::Size()
{
    return currentSize;
}

void CMinHeap::Merge()
{
    while( currentSize >= 2 ) {
        int tmp = (currentSize > 2) ? 2 : 1;
        int minChild = (buffer[1]->Priority <= buffer[tmp]->Priority) ? 1 : tmp;
        CNode* tmpNode = buffer[minChild];
        std::vector<ubyte> tmpChar;
        buffer[minChild] = new CNode( tmpChar, buffer[0]->Priority + tmpNode->Priority );
        buffer[minChild]->Left = buffer[0];
        buffer[minChild]->Right = tmpNode;
        std::swap( buffer[0], buffer[--currentSize] );

        for( int i = (currentSize - 1) / 2; i >= 0; --i ) {
            siftDown( i );
        }
    }
}