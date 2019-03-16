#include <iostream>
#include <string>

#include "HuffmanZip.h"
#include "Stream.h"

int main( int argc, char** argv )
{
    if( argc >= 2 && (std::string( argv[1] ) == "-h" || std::string( argv[1] ) == "--help") ) {
        std::cout << " -- Huffman Zip -- \n"
            "Lightweight utility for encoding/decoding files. Use to compress files (works bad with already compressed files).\n"
            "Encode file:\thuffzip encode <file_to_encode> [result_file]\n"
            "Decode file:\thuffzip decode <file_to_decode> [result_file]\n";
        return 0;
    }

    if( argc < 3 ) {
        std::cerr << argv[0] << ": Not enough arguments\n";
        return 1;
    }

    std::string inFile = argv[2];
    std::string outFile;
    if( argc >= 4 ) {
        outFile = argv[3];
    } else {
        if( std::string( argv[1] ) == "encode" ) {
            outFile = inFile + ".huffzip";
        } else {
            size_t newSize = inFile.find( ".huffzip" );
            outFile = inFile;
            if( newSize == std::string::npos ) {
                std::cerr << "Warning! You are trying to decode not a .huffzip file.\n";
                outFile = "unzipped_" + outFile;
            } else {
                outFile.resize( newSize );
            }
        }
    }
    CInputStream input( inFile.data() );
    COutputStream output( outFile.data() );

    if( std::string( argv[1] ) == "encode" ) {
        HuffmanZip::Encode( input, output );
    } else if( std::string( argv[1] ) == "decode" ) {
        HuffmanZip::Decode( input, output );
    } else {
        std::cerr << argv[0] << ": " << argv[1] << ": Unknown command\n";
    }

    return 0;
}