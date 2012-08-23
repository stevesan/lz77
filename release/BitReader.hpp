
#ifndef __BITREADER_HEADER_GUARD__
#define __BITREADER_HEADER_GUARD__

#include "BitWriter.hpp" // some common utility functions

class BitReader
{
	private:

		std::vector<BYTE> bytes;
		unsigned int next_bit;

	public:

		static bool load_bytes_binary( std::vector<BYTE>& bytes, const std::string& fname )
		{
			std::ifstream fin( fname.c_str(), std::ios::binary );
			if( !fin.good() )
			{
				std::cerr << "** Could not open file for read '" << fname.c_str() << "'" << std::endl;
				return false;
			}
			else
			{
				bytes.clear();

				while( true )
				{
					BYTE byte;
					fin.read( (char*)&byte, sizeof(byte) );

					if( fin.eof() )
						break;

					bytes.push_back( byte );
				}

				fin.close();
				std::cout << "OK Loaded " << bytes.size() << " bytes from " << fname << std::endl;
				return true;
			}
		}

		BitReader() :
			next_bit(0)
		{
		}

		void reset()
		{
			next_bit = 0;
		}

		bool load_binary( const std::string& fname )
		{
			return load_bytes_binary( bytes, fname );
		}

		//----------------------------------------
		//  IMPORTANT: This returns TRUE is there was a bit to read, and false otherwise.
		//	The actual value is assigned to val
		//----------------------------------------
		bool read_bit( bool& val )
		{
			if( next_bit < bytes.size()*8 )
			{
				val = BitWriter::get_bit( bytes[ next_bit / 8 ], next_bit % 8 );
				next_bit++;
				return true;
			}
			else
				return false;
		}

		template <typename T>
		bool read_bits( T& out, unsigned int num_places )
		{
			for( int i = 0; i < std::min((size_t)num_places, 8*sizeof(T)); i++ )
			{
				bool val;
				if( read_bit( val ) )
				{
					BitWriter::set_bit( out, i, val );
				}
				else
					// no more left to read
					return false;
				
			}
			return true;
		}

};


#endif /* end of include guard: __BITREADER_HEADER_GUARD__ */
