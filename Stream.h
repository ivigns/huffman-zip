#pragma once
#include <fstream>

class CInputStream {

public:
    CInputStream( const char* fileName ) : in( fileName ) {}

    // Returns false if stream ended
    bool Read( char& value )
    {
        int c = in.get();
        value = static_cast<char>(c);
        return c != EOF;
    }

private:
    std::ifstream in;
};


class COutputStream {

public:
    COutputStream( const char* fileName ) : out( fileName ) {}

    void Write( char value )
    {
        out.put( value );
    }

private:
    std::ofstream out;
};
