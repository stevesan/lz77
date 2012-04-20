

#include <iostream>
#include "SuffixTree.hpp"

using namespace std;

int main( int argc, char** argv )
{
	vector<unsigned char> chars;

	assert( argc > 2 );

	for( int i = 0; argv[1][i] != '\0'; i++ )
	{
		chars.push_back( argv[1][i] );
	}

	SuffixTree tree( chars, 16 );
	tree.add_all_remaining();
	cout << "Final tree: " << endl;
	tree.output(cout);

	vector<unsigned char> target;
	for( int i = 0; argv[2][i] != '\0'; i++ )
	{
		target.push_back( argv[2][i] );
	}
	pair<int,int> rv = tree.find_longest_match( target );
	cout << rv.first << " " << rv.second << endl;
}
