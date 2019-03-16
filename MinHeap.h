#pragma once
#include "Node.h"

class CMinHeap {
public:
    CMinHeap();
    CMinHeap( CNode** arr, int arr_size );
    ~CMinHeap();
    CNode* GetMin() const;
    int Size();
    // Merge of all nodes in heap into one whole tree
    void Merge();
private:
    CNode** buffer;
    int maxSize;
    int currentSize;
    void siftUp( int i );
    void siftDown( int i );
};