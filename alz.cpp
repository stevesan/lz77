//----------------------------------------
//  This is the naive "ground truth" implementation, unoptimized greedy
//----------------------------------------

#include <iostream>
#include <cmath>
#include <vector>

#include "BitWriter.hpp"
#include "BitReader.hpp"
#include "SuffixTree.hpp"

static const unsigned int NUM_DELTA_BITS = 12;
static const unsigned int NUM_LEN_BITS = 4;

inline unsigned int get_max_delta()
{
	// subtract one, since we want inclusive max
	return (1 << NUM_DELTA_BITS) - 1;
}

inline unsigned int get_max_copy_len()
{
	return (1 << NUM_LEN_BITS) - 1;
}

using namespace std;

//----------------------------------------
//  Naive approach
//----------------------------------------
int find_longest_match( const vector<BYTE>& pile, int pile_start, int pile_end, const vector<BYTE>& target, int& best_len )
{
	assert( pile_end <= pile.size() );

	int best_match = -1;
	best_len = -1;
	for( int i = pile_start; i < pile_end; i++ )
	{
		int curr_len = 0;
		for( int j = 0; j < target.size() && (i+j) < pile_end; j++ )
		{
			if( pile[i+j] == target[j] )
				curr_len++;
			else
				break;
		}

		if( curr_len > best_len )
		{
			best_match = i;
			best_len = curr_len;
		}
	}

	return best_match;
}

//----------------------------------------
//  Compression
//----------------------------------------
int compress_main( const string& infile, const string& outfile, bool use_suffix_tree )
{
	//----------------------------------------
	//  Read the whole file into the array at once
	//----------------------------------------
	vector<BYTE> bytes;

	std::ifstream fin( infile.c_str(), ios::binary );
	if( !fin.good() )
	{
		std::cerr << "** Could not open file for read '" << infile << "'" << std::endl;
		return 1;
	}
	else
	{
		BYTE byte;
		while( true )
		{
			fin.read( (char*)&byte, sizeof(BYTE) );
			if( fin.eof() )
				// done
				break;

			bytes.push_back( byte );
		}
	
		fin.close();
	}

	cout << "Read in " << bytes.size() << " bytes" << endl;

	//----------------------------------------
	//	Main compression loop
	//----------------------------------------
	BitWriter bw;

	SuffixTree stree( bytes );
	vector<BYTE> target( get_max_copy_len() );

	for( int i = 0; i < bytes.size(); )
	{
		int target_len = min( (size_t)get_max_copy_len(), bytes.size()-i );
		target.resize( target_len );

		// copy the next target chunk
		for( int j = 0; j < target_len; j++ )
		{
			assert( i+j < bytes.size() );
			target[j] = bytes[i+j];
		}

		// search for it in previous bytes
		// but only look back 4096 bytes tops
		int pile_start = max( (int)0, (int)(i-get_max_delta()-1) );
		int best_len = -1;
		int longest_match = -1;
		
		if( use_suffix_tree )
		{
			pair<int,int> rv = stree.find_longest_match( target );
			longest_match = rv.first;
			best_len = rv.second;
			//cout << "found i = " << longest_match << " best_len = " << best_len << endl;
		}
		else
		{
			// slow brute force
			longest_match = find_longest_match( bytes, pile_start, i, target, best_len );
		}

		if( best_len >= 2 )
		{
			// compress it!
			bw.write_bit( 1 );
			unsigned int delta = i - longest_match - 1;
			bw.write_bits( delta, NUM_DELTA_BITS );
			bw.write_bits( best_len, NUM_LEN_BITS );
			cout << "copy " << delta << " " << best_len << endl;

			// advance cursor past the length
			i += best_len;
			// and the suffix tree
			if( use_suffix_tree )
			{
				for( int j = 0; j < best_len; j++ )
				{
					bool ok = stree.add_next_letter();
					assert( ok );
				}
			}
		}
		else
		{
			// didn't find any. just output it
			bw.write_bit( 0 );
			bw.write_bits( bytes[i], 8 );

			cout << "byte " << bytes[i] << endl;

			// advance cursor and suffix tree
			i++;
			if( use_suffix_tree )
			{
				bool ok = stree.add_next_letter();
				assert( ok );
			}
		}
	}

	//----------------------------------------
	//  Save
	//----------------------------------------
	bool ok = bw.save_binary( outfile );

	if( ok ) return 0;
	else return 1;
}

int decompress_main( const string& infile, const string& outfile )
{
	BitReader br;
	br.load_binary( infile );

	// the uncompressed bytes
	vector<BYTE> out;
	int num_commands_read = 0;

	while( true )
	{
		bool is_ptr = false;
		bool ok = br.read_bit( is_ptr );
		
		if( !ok )
			// done
			break;

		if( is_ptr )
		{
			size_t delta = 0;
			size_t num_bytes = 0;
			ok = br.read_bits( delta, NUM_DELTA_BITS ) && br.read_bits( num_bytes, NUM_LEN_BITS );

			if( !ok )
			{
				// Could not read the rest of the command. We must be done, and this wasn't really intended as a command.
				break;
			}

			size_t copy_start = out.size()-1-delta;
			size_t copy_end = copy_start + num_bytes;
			if( copy_start > out.size() || copy_end > out.size() )
			{
				cerr << "Bad pointer or length for copy command #" << num_commands_read << endl;
				return 1;
			}

			// copy bytes to end
			for( int i = copy_start; i < copy_end; i++ )
			{
				out.push_back( out[i] );
			}
		}
		else
		{
			// just read the byte and add it
			BYTE byte = 0;
			ok = br.read_bits( byte, 8 );
			if( !ok )
			{
				// Could not read the rest of the command. We must be done, and this wasn't really intended as a command.
				break;
			}
			out.push_back( byte );
		}

		num_commands_read++;
	}

	// write out!
	BitWriter::save_bytes_binary( out, outfile );

	return 0;
}

int main( int argc, char** argv )
{
	if( argc < 4 )
	{
		cerr << "Usage: " << argv[0] << " [c|d|s] infile outfile" << endl;
		cerr << "[c|d|s] indicates whether to compress or decompress. 's' indicates slow 'brute force' compression." << endl;
		return 1;
	}

	char mode = argv[1][0];
	string infile( argv[2] );
	string outfile( argv[3] );

	if( mode == 'c' )
		return compress_main( infile, outfile, true );
	else if( mode == 's' )
		// Use the 's'low compression method, just for testing
		return compress_main( infile, outfile, false );
	else
		return decompress_main( infile, outfile );
}
