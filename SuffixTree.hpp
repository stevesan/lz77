
//----------------------------------------
//  Implementation of linear-time online suffix tree construction
//	Algorithm from [Ukkonen 1995]
//	Basically a port of the javascript version in view-source:http://www.allisons.org/ll/AlgDS/Tree/Suffix/
//----------------------------------------

#ifndef __SUFFIXTREE_HEADER_GUARD__
#define __SUFFIXTREE_HEADER_GUARD__

#include <vector>
#include <cassert>
#include <utility>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>

using namespace std;

typedef unsigned char T;
class SuffixTree
{
	public:

		typedef pair<size_t, size_t> Substring;

		static bool is_empty_sub( const Substring& s )
		{
			return s.second < s.first;
		}

		class State
		{
			public:
				typedef pair< Substring, State* > Transition;
				boost::unordered_map< T, Transition > transitions;

				bool is_leaf;

				State* suf_link;

				State() :
					is_leaf( true ),
					suf_link(NULL)
			{}

				void add_transition( T c, const Transition& t )
				{
					assert( t.second != NULL );
					transitions[ c ] = t;
					is_leaf = false;
				}

				bool has_transition( T c )
				{
					return transitions.find(c) != transitions.end();
				}

				Transition get_transition( T c )
				{
					if( !has_transition(c) )
					{
						cerr << "Could not find transition for " << c << endl;
						assert(false);
					}
					return transitions[c];
				}

				void output_dfs( ostream& os, const vector<T>& chars, size_t last, int depth )
				{
					os << depth << " ";
					for( int i = 0; i < depth; i++ )
					{
						cout << " ";
					}

				}

		};

		const vector<T>& chars;
		State* root;
		State* bt;

		typedef pair<State*, size_t> Suffix;

		Suffix canonize( State* s, size_t k, size_t p )
		{
			if( p < k )
				return Suffix( s, k );

			State::Transition e = s->get_transition( chars[k] );
			assert( e.second != NULL );
			State* s1 = e.second;
			size_t k1 = e.first.first;
			size_t p1 = e.first.second;

			while( (p1-k1) <= (p-k) )
			{
					k += p1 - k1 + 1;
					s = s1;
					if( k <= p )
					{
						e = s->get_transition( chars[k] );
						assert( e.second != NULL );
						s1 = e.second;
						k1 = e.first.first;
						p1 = e.first.second;
					}
			}

			return Suffix( s, k );
		}

		typedef pair<bool,State*> TestSplitRet;
		TestSplitRet test_and_split( State* s, size_t k, size_t p, T t )
		{
			if( k <= p )
			{
				State::Transition e = s->get_transition( chars[k] );
				State* s1 = e.second;
				assert( s1 != NULL );
				size_t k1 = e.first.first;
				size_t p1 = e.first.second;

				if( t == chars[ k1+p-k+1 ] )
					return TestSplitRet( true, s );
				else
				{
					State* r = new State();
					s->add_transition( chars[k1], State::Transition( Substring(k1, k1+p-k), r) );
					r->add_transition( chars[k1+p-k+1], State::Transition( Substring( k1+p-k+1, p1 ), s1) );
					return TestSplitRet( false, r );
				}
			}
			else
				return TestSplitRet( s->has_transition(t), s );
		}

		Suffix update( State* s, size_t k, size_t i )
		{
			State* oldr = root;
			TestSplitRet tsr = test_and_split( s, k, i-1, chars[i] );
			bool is_end = tsr.first;
			State* r = tsr.second;

			size_t inf = chars.size();

			while( !is_end )
			{
				r->add_transition( chars[i], State::Transition( Substring(i, inf), new State() ) );
				if( oldr != root )
					oldr->suf_link = r;

				oldr = r;
				Suffix suf = canonize( s->suf_link, k, i-1 );
				s = suf.first; k = suf.second;
				tsr = test_and_split( s, k, i-1, chars[i] );
				is_end = tsr.first;
				r = tsr.second;
			}

			if( oldr != root )
				oldr->suf_link = s;

			return Suffix( s, k );
		}

		SuffixTree( const vector<T>& _chars ) :
			chars(_chars)
		{
			root = new State();
			bt = new State();

			// create transitions for all posible chars..
			for( int i = 0; i < chars.size(); i++ )
			{
				bt->add_transition( chars[i],
						State::Transition( Substring(i,i), root) );
			}

			root->suf_link = bt;
			State* s = root;
			size_t k = 0;

			for( int i = 0; i < chars.size(); i++ )
			{
				Suffix rv = update( s, k, i );
				s = rv.first;
				k = rv.second;
				rv = canonize( s, k, i );
				s = rv.first;
				k = rv.second;
			}
		}
};


#endif /* end of include guard: __SUFFIXTREE_HEADER_GUARD__ */
