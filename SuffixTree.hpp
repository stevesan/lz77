
//----------------------------------------
//  Implementation of linear-time online suffix tree construction
//	Algorithm from [Ukkonen 1995]
//	Basically a port of the javascript version in view-source:http://www.allisons.org/ll/AlgDS/Tree/Suffix/
//	In order to support a sliding window (since our pointer is limited to 12-bits), I also augmented the code
//	to implement the extension from Jesper Larsson's thesis: http://larsson.dogma.net/thesis.pdf
//----------------------------------------

#ifndef __SUFFIXTREE_HEADER_GUARD__
#define __SUFFIXTREE_HEADER_GUARD__

//#define VERBOSE

#include <vector>
#include <cassert>
#include <utility>
#include <map>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>

using namespace std;

typedef unsigned char T;
class SuffixTree
{
	public:

		typedef pair<int, int> Substring;

		//----------------------------------------
		//  Essentially represents an explicit node
		//----------------------------------------
		class Node
		{
			public:

				class Edge
				{
						Substring sub;
						Node* end;
						map< int, int > latest_occurr;

					public:

						Edge( Substring& _sub, Node* _end ) :
							sub(_sub),
							end( _end )
						{
						}

						Node* get_end() { return end; }
						const Node* get_end_const() const { return end; }
						Substring get_sub() const { return sub; }

						//----------------------------------------
						//  Returns the index of the latest occurrence of the implicit node (ie. a substring) given by the first occurrence index 'first_idx'
						//----------------------------------------
						int get_latest_occurrence( int first_idx )
						{
							assert( first_idx >= sub.first );
							assert( first_idx <= sub.second );

							map<int,int>::iterator it = latest_occurr.find( first_idx );
							if( it != latest_occurr.end() )
							{
								return it->second;
							}
							else
								// there has only been one occurrence so far
								return first_idx;
						}

						void update_latest_occurrence( int first_idx, int latest_idx )
						{
							// make sure it's actually later
							if( latest_idx > get_latest_occurrence(first_idx) )
								latest_occurr[ first_idx ] = latest_idx;
						}
				};

				typedef boost::unordered_map< T, Edge* > EdgeMap;

		private:
				EdgeMap edges;
				bool is_leaf;

		public:

				static void output_trans( ostream& os, const vector<T>& chars, int last, const Edge* e )
				{
					Substring sub = e->get_sub();
					os << sub.first << "-" << sub.second << " '";
					for( int i = sub.first; i <= sub.second && i <= last; i++ )
					{
						os << chars[i];
					}
					os << "'";
				}

				Node* suf_link;

				Node() :
					is_leaf( true ),
					suf_link(NULL),
					edges()
				{
				}

				void add_edge( T c, Edge* t )
				{
					assert( t->get_end() != NULL );
					//cout << "Adding transition for " << c << " obj = " << (void*)this << endl;
					edges[ c ] = t;
					is_leaf = false;
				}

				Edge* get_edge( T c )
				{
					return edges[c];
				}

				void output_dfs( ostream& os, const vector<T>& chars, int last, int depth ) const
				{

					BOOST_FOREACH( const EdgeMap::value_type& kv, edges )
					{
						const Edge* e = kv.second;
						os << depth << " ";
						for( int i = 0; i < depth; i++ ) cout << " ";
						output_trans( os, chars, last, e );
						os << endl;
						e->get_end_const()->output_dfs( os, chars, last, depth+1 );
					}

				}

		};

		typedef pair<Node*, int> Suffix;

	private:

		const vector<T>& chars;
		Node* root;
		Node* bt;
		Node* outer_s;
		int outer_k;
		int curr_i;

		Suffix canonize( Node* s, int k, int p )
		{
			if( p < k )
				return Suffix( s, k );

			Node::Edge* e = s->get_edge( chars[k] );
			assert( e != NULL );
			Node* s1 = e->get_end();
			int k1 = e->get_sub().first;
			int p1 = e->get_sub().second;

			while( (p1-k1) <= (p-k) )
			{
					k += p1 - k1 + 1;
					s = s1;
					if( k <= p )
					{
						e = s->get_edge( chars[k] );
						assert( e != NULL );
						s1 = e->get_end();
						k1 = e->get_sub().first;
						p1 = e->get_sub().second;
					}
			}

			return Suffix( s, k );
		}

		typedef pair<bool,Node*> TestSplitRet;
		TestSplitRet test_and_split( Node* s, int k, int p, T t )
		{
			if( k <= p )
			{
				//cout << "Looking for " << chars[k] << endl;
				Node::Edge* e = s->get_edge( chars[k] );
				assert( e != NULL );
				Node* s1 = e->get_end();
				assert( s1 != NULL );
				int k1 = e->get_sub().first;
				int p1 = e->get_sub().second;

				if( t == chars[ k1+p-k+1 ] )
					return TestSplitRet( true, s );
				else
				{
					Node* r = new Node();
					s->add_edge( chars[k1], new Node::Edge( Substring(k1, k1+p-k), r ) );
					r->add_edge( chars[k1+p-k+1], new Node::Edge( Substring( k1+p-k+1, p1 ), s1) );
					return TestSplitRet( false, r );
				}
			}
			else
				return TestSplitRet( s->get_edge(t) != NULL, s );
		}

		Suffix update( Node* s, int k, int i )
		{
			Node* oldr = root;
			TestSplitRet tsr = test_and_split( s, k, i-1, chars[i] );
			bool is_end = tsr.first;
			Node* r = tsr.second;

			int inf = chars.size();

			while( !is_end )
			{
				r->add_edge( chars[i], new Node::Edge( Substring(i, inf), new Node() ) );
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

	public:

		SuffixTree( const vector<T>& _chars ) :
			chars(_chars)
		{
			root = new Node();
			bt = new Node();

			// create edges for all posible chars..
			for( int i = 0; i < chars.size(); i++ )
			{
				bt->add_edge( chars[i],
						new Node::Edge( Substring(i,i), root ) );
				//cout << "Added bt-> root transition for char '" << chars[i] << "'" << endl;
			}

			root->suf_link = bt;
			outer_s = root;
			outer_k = 0;
			curr_i = 0;
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
		//	This returns the position of the FIRST occurence of the longest.
		//	Kind of useless for our compression problem...
		//----------------------------------------
		pair<int,int> find_longest_match( const vector<T>& target ) const
		{
			int j = 0;	// index into target

			Node::Edge* e = root->get_edge( target[j] );
			int last_char = -1;	// the last char we matched

			while( e != NULL )
			{
				Substring sub = e->get_sub();

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
					Node* end = e->get_end();
					assert( end != NULL );
					e = end->get_edge( target[j] );
				}
				else
					// didn't get to the end, we're done
					break;
			}

			// compute starting position by just backtracking from last char
			int start = last_char - j + 1;
			return pair<int,int>( start, j );
		}

#if 0
		pair<int,int> find_longest_occurence_after( const vector<T>& target, int min_start ) const
		{
			int j = 0;	// index into target

			Node::MaybeTrans me = root->maybe_get_transition( target[j] );
			int best_len = -1;
			int best_start = -1;

			while( me.first )
			{
				Node::Transition e = me.second;
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
					Node* end = e.second;
					assert( end != NULL );
					me = end->maybe_get_transition( target[j] );
				}
				else
					// didn't get to the end, we're done
					break;
			}

			// compute starting position by just backtracking from last char
			int start = last_char - j + 1;
			return pair<int,int>( best_start, best_len );
		}
#endif
};


#endif /* end of include guard: __SUFFIXTREE_HEADER_GUARD__ */
