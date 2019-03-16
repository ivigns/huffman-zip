#pragma once
#include "Stream.h"

// How many times to apply encoding
#define COMPRESS_PARAM 1
// Minimal number of occurrences of char couple in file
#define DOUBLEDCHAR_PARAM 15

namespace HuffmanZip {
    // Encodes original stream into compressed
    void Encode( CInputStream& original, COutputStream& compressed );

    // Decodes compressed stream into original
    void Decode( CInputStream& compressed, COutputStream& original );
}
