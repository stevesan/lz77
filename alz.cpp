
#include <iostream>
#include <cmath>

#include "BitWriter.hpp"

using namespace std;

int main( int argc, char** argv )
{
	if( argc < 4 )
	{
		cerr << "Usage: " << argv[0] << " [c|d] infile outfile" << endl;
		cerr << "[c|d] indicates whether to compress or decompress" << endl;
		return 1;
	}
	return 0;
}
