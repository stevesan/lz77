
#include <iostream>
#include "BitWriter.hpp"
#include "BitReader.hpp"

using namespace std;

int main( int argc, char** argv )
{
	BitWriter bw;

	unsigned int val = atoi(argv[1]);
	unsigned int nplaces = atoi(argv[2]);

	bw.write_bits( val, nplaces );
	bw.write_bits( val, nplaces );
	bw.to_ascii( cout );
	bw.save_binary( "test.bin" );

	BitReader br;
	br.load_binary( "test.bin" );

	while( true )
	{
		unsigned int read_val = 0;
		bool ok = br.read_bits( read_val, nplaces );
		if( !ok ) break;
		assert( read_val == val );
		cout << "Read back val = " << read_val << endl;
	}
}
