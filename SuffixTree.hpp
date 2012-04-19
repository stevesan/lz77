
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

		typedef pair<int, int> Substring;

		static bool is_empty_sub( const Substring& s )
		{
			return s.second < s.first;
		}

		class State
		{
			public:
				typedef pair< Substring, State* > Transition;
				typedef boost::unordered_map< T, Transition > TransMap;

		private:
				TransMap transitions;
				bool is_leaf;

		public:

				static void output_trans( ostream& os, const vector<T>& chars, int last, const Transition& e )
				{
					Substring sub = e.first;
					os << sub.first << "-" << sub.second << " '";
					for( int i = sub.first; i <= sub.second && i <= last; i++ )
					{
						os << chars[i];
					}
					os << "'";
				}

				State* suf_link;

				State() :
					is_leaf( true ),
					suf_link(NULL),
					transitions()
				{
//					transitions.clear();
				}

				void add_transition( T c, const Transition& t )
				{
					assert( t.second != NULL );
					//cout << "Adding transition for " << c << " obj = " << (void*)this << endl;
					transitions[ c ] = t;
					is_leaf = false;
				}

				typedef pair<bool, Transition> MaybeTrans;
				MaybeTrans maybe_get_transition( T c )
				{
					TransMap::iterator iter = transitions.find(c);
					if( iter == transitions.end() )
						return MaybeTrans( false, Transition(Substring(0,0), NULL) );
					else
						return MaybeTrans( true, transitions[c] );
				}

				bool has_transition( T c )
				{
					//cout << "Looking for transition to " << c << " |ts| = " << transitions.size() << " obj = " << (void*)this << endl;
					bool found = transitions.find(c) != transitions.end();
					//cout << "result = " << found << endl;
					return found;
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

				void output_dfs( ostream& os, const vector<T>& chars, int last, int depth ) const
				{

					BOOST_FOREACH( const TransMap::value_type& kv, transitions )
					{
						const Transition& e = kv.second;
						os << depth << " ";
						for( int i = 0; i < depth; i++ ) cout << " ";
						output_trans( os, chars, last, e );
						os << endl;
						e.second->output_dfs( os, chars, last, depth+1 );
					}

				}

		};

		const vector<T>& chars;
		State* root;
		State* bt;
		State* outer_s;
		int outer_k;
		int curr_i;

		typedef pair<State*, int> Suffix;

		Suffix canonize( State* s, int k, int p )
		{
			if( p < k )
				return Suffix( s, k );

			State::Transition e = s->get_transition( chars[k] );
			assert( e.second != NULL );
			State* s1 = e.second;
			int k1 = e.first.first;
			int p1 = e.first.second;

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
		TestSplitRet test_and_split( State* s, int k, int p, T t )
		{
			if( k <= p )
			{
				//cout << "Looking for " << chars[k] << endl;
				State::Transition e = s->get_transition( chars[k] );
				State* s1 = e.second;
				assert( s1 != NULL );
				int k1 = e.first.first;
				int p1 = e.first.second;

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

		Suffix update( State* s, int k, int i )
		{
			State* oldr = root;
			TestSplitRet tsr = test_and_split( s, k, i-1, chars[i] );
			bool is_end = tsr.first;
			State* r = tsr.second;

			int inf = chars.size();

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
				//cout << "Added bt-> root transition for char '" << chars[i] << "'" << endl;
			}

			root->suf_link = bt;
			outer_s = root;
			outer_k = 0;
			curr_i = 0;

			// TEMP
			root->has_transition('m');
		}

		//----------------------------------------
		//  Returns false if all done
		//----------------------------------------
		bool add_next_letter()
		{
			if( curr_i < chars.size() )
			{
				//cout << "Doing character " << chars[curr_i] << endl;
				Suffix rv = update( outer_s, outer_k, curr_i );
				outer_s = rv.first;
				outer_k = rv.second;
//				output(cout);
				rv = canonize( outer_s, outer_k, curr_i );
				outer_s = rv.first;
				outer_k = rv.second;

				curr_i++;
				return true;
			}
			else
				return false;
		}

		void add_all_remaining()
		{
			//output(cout);
			while( add_next_letter() ) {
				cout << "--" << endl;
				output(cout);
			}
		}

		void output( ostream& os ) const
		{
			root->output_dfs( os, chars, curr_i-1, 0 );
		}

		//----------------------------------------
		//  rv.first = index of longest match
		//	rv.second = length of longest match
		//----------------------------------------
		pair<int,int> find_longest_match( const vector<T>& target ) const
		{
			int j = 0;	// index into target

			State::MaybeTrans me = root->maybe_get_transition( target[j] );
			int last_char = -1;	// the last char we matched

			while( me.first )
			{
				State::Transition e = me.second;
				Substring sub = e.first;

				// found a transition starting with the next letter
				// see how much we get
				// make sure we don't go past the edge's bound
				// and also make sure we don't go past the current last letter
				int i = 0;
				for( i = sub.first; i <= sub.second && i < curr_i && j < target.size(); i++ )
				{
					if( chars[i] == target[j] )
					{
						last_char = i;
						j++;
					}
					else
						break;
				}

				// matched the whole thing?
				if( j == target.size() )
				{
					break;
				}

				if( i > sub.second )
				{
					// we got all the way to the end of this edge!
					// try finding the next edge
					State* end = e.second;
					assert( end != NULL );
					me = end->maybe_get_transition( target[j] );
				}
				else
					// didn't get to the end, we're done
					break;
			}

			// compute starting position by just backtracking from last char
			int start = last_char - j + 1;
			return pair<int,int>( start, j );
		}
};


#endif /* end of include guard: __SUFFIXTREE_HEADER_GUARD__ */
