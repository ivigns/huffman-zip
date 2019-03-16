#include "BitStream.h"

// CInBitStream

ubyte CInBitStream::ReadBit()
{
    if( position / 8 >= buffer.size() ) {
        return 0;
    }
    ubyte result = (buffer[position / 8] >> (position % 8)) & 1;
    ++position;
    return result;
}

ubyte CInBitStream::ReadByte()
{
    if( position / 8 >= buffer.size() ) {
        return 0;
    }
    ubyte result = 0;
    if( !(position % 8) ) {
        result = buffer[position / 8];
        position += 8;
        return result;
    }

    result = buffer[position / 8] >> (position % 8);
    result |= buffer[position / 8 + 1] << (8 - position % 8);
    position += 8;
    return result;
}

void CInBitStream::PushBack( ubyte _byte )
{
    buffer.push_back( _byte );
}

const std::vector<ubyte>& CInBitStream::GetBuffer() const
{
    return buffer;
}

void CInBitStream::Clear()
{
    buffer.clear();
    position = 0;
}

// COutBitStream

void COutBitStream::WriteBit( ubyte _bit )
{
    if( bitsCount + 1 > buffer.size() * 8 ) {
        buffer.push_back( 0 );
    }
    if( _bit == 1 ) {
        buffer.back() |= 1 << (bitsCount % 8);
    }
    ++bitsCount;
}

void COutBitStream::WriteByte( ubyte _byte )
{
    if( bitsCount == buffer.size() * 8 ) {
        buffer.push_back( _byte );
        bitsCount += 8;
        return;
    }

    ubyte leftPart = _byte << (bitsCount % 8);
    buffer.back() |= leftPart;

    ubyte rightPart = _byte >> (8 - bitsCount % 8);
    buffer.push_back( rightPart );

    bitsCount += 8;
}

const std::vector<ubyte>& COutBitStream::GetBuffer() const
{
    return buffer;
}

void COutBitStream::Clear()
{
    buffer.clear();
    bitsCount = 0;
}