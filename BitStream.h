#pragma once
#include <vector>

typedef unsigned char ubyte;

class CInBitStream {

public:
    CInBitStream() : position( 0 ) {}
    ~CInBitStream() {}

    ubyte ReadBit();
    ubyte ReadByte();

    void PushBack( ubyte _byte );
    const std::vector<ubyte>& GetBuffer() const;
    void Clear();

private:
    size_t position;
    std::vector<ubyte> buffer;
};

class COutBitStream {

public:
    COutBitStream() : bitsCount( 0 ) {}
    ~COutBitStream() {}

    void WriteBit( ubyte _bit );
    void WriteByte( ubyte _byte );

    const std::vector<ubyte>& GetBuffer() const;
    void Clear();

private:
    size_t bitsCount;
    std::vector<ubyte> buffer;
};