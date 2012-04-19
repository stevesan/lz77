

#include <iostream>
#include "SuffixTree.hpp"

using namespace std;

int main( int argc, char** argv )
{
	vector<unsigned char> chars;

	assert( argc > 1 );

	for( int i = 0; argv[1][i] != '\0'; i++ )
	{
		chars.push_back( argv[1][i] );
	}

	SuffixTree tree( chars );
}
