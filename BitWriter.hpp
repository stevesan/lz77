#ifndef __BITWRITER_HEADER_GUARD__
#define __BITWRITER_HEADER_GUARD__

#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cassert>
#include <cmath>

//----------------------------------------
//  Facilitates bit-level output
//----------------------------------------
class BitWriter
{
	private:

		std::vector<unsigned char> bytes;
		int next_bit;

	public:

		template <typename T>
		static void set_bit( T& x, int place, bool val )
		{
			T mask = 1;
			mask = mask << place;
			if( val )
				x |= mask;
			else
				x &= ~mask;
		}

		template <typename T>
		static bool get_bit( T x, int place )
		{
			T mask = 1;
			mask = mask << place;
			return (x & mask) != 0;
		}

		//----------------------------------------
		//  Mostly for debugging
		//----------------------------------------
		template < typename T >
		static std::string bits2string( T x )
		{
			std::stringstream s;
			for( int i = sizeof(x)*8-1; i >= 0; i-- )
			{
				s << get_bit( x, i );
				if( i % 8 == 0 )
					s << " ";
			}
			return s.str();
		}

		void to_ascii( std::ostream& os )
		{
			for( int i = 0; i < bytes.size(); i++ )
			{
				os << bits2string( bytes[i] ) << std::endl;
			}
		}

		BitWriter() :
			next_bit(0)
		{
		}

		//----------------------------------------
		//  Writes a single bit to the stream
		//----------------------------------------
		void write_bit( bool val )
		{
			if( next_bit%8 == 0 )
				bytes.push_back(0);
			set_bit( bytes.back(), (next_bit%8), val );
			next_bit++;
		}

		bool save_binary( const std::string& fname )
		{
			std::ofstream fout( fname.c_str(), std::ios::binary );
			if( !fout.good() )
			{
				std::cerr << "** Could not open file for write '" << fname << "'" << std::endl;
				return false;
			}
			else
			{
				for( int i = 0; i < bytes.size(); i++ )
				{
					fout.write( (char*)&bytes[i], sizeof( bytes[i] ) );
				}
			
				fout.close();
				std::cout << "OK Saved " << bytes.size() << " bytes to " << fname << std::endl;
				return true;
			}
		}

		//----------------------------------------
		//  Writes the given value as num_places bits, starting from LSB
		//----------------------------------------
		template <typename T>
		void write_bits( T value, unsigned int num_places )
		{
			for( int i = 0; i < std::min((long unsigned int)num_places, 8*sizeof(T)); i++ )
			{
				write_bit( get_bit( value, i ) );
			}
		}
};


#endif /* end of include guard: __BITWRITER_HEADER_GUARD__ */
