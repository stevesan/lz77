
//----------------------------------------
//  Implementation of linear-time online suffix tree construction
//	Algorithm from [Ukkonen 1995]
//----------------------------------------

#ifndef __SUFFIXTREE_HEADER_GUARD__
#define __SUFFIXTREE_HEADER_GUARD__

#include <vector>
#include <cassert>

using namespace std;

template <typename T>
class SuffixTree
{
	public:

		class Edge
		{
			public:

				vector<Edge*> children;
				size_t first_id;
				size_t num_chars;

				Edge* next_smaller_edge;

				Edge() :
					first_id(0), num_chars(0), next_smaller_edge(NULL)
			{
			}

				Edge( size_t _first, size_t _num ) :
					first_id( _first ),
					num_chars( _num ),
					next_smaller_edge(NULL)
				{
				}

				bool is_leaf() { return children.size() == 0; }
				bool is_empty() { return num_chars == 0; }
				size_t get_last_id() { return first_id + num_chars - 1; }

				void add_child( Edge* e ) { children.push_back(e); }

				void output_dfs( ostream& os, const vector<T>& chars, int last, int depth = 0 )
				{
					os << depth << ": ";
					for( int i = 0; i < depth; i++ )
					{
						os << " ";
					}

					for( int i = 0; i < (is_leaf()? last-first_id+1 : num_chars); i++ )
					{
						os << chars[ first_id + i ];
					}
					os << endl;

					for( int i = 0; i < children.size(); i++ )
					{
						children[i]->output_dfs( os, chars, last, depth+1 );
					}

				}
		};

		class Suffix
		{
			public:

				Edge* edge;
				size_t last_id;

				Suffix() : edge( NULL ), last_id(0)
			{
			}

				void set( Edge* _edge, size_t _last_id )
				{
					edge = _edge;
					last_id = _last_id;
				}

				bool is_implicit_node() { return last_id < edge->get_last_id(); }
				bool is_explicit_node(size_t curr_end) {
					if( edge->is_empty() ) return true;

					if( edge->is_leaf() )
					{
						return( last_id == curr_end );
					}
					else
						return last_id == edge->get_last_id();
				}
				bool is_empty() { return edge->is_empty(); }

				void move_to_next_smaller_suffix()
				{
					edge = edge->next_smaller_edge;
					if( edge == NULL )
					{
						throw new std::runtime_error("No pointer to smaller suffix!");
					}
					last_id = edge->first_id;
				}

		};

		const vector<T>& chars;
		Edge* root;
		Suffix active_point;

		SuffixTree( const vector<T>& _chars ) :
			chars( _chars ),
			root( NULL )
		{
			root = new Edge;
			active_point.set( root, 0 );

			// build
			for( size_t i = 0; i < chars.size(); i++ )
			{
				update( i );

				cout << "--------------" << endl;
				root->output_dfs( cout, chars, i );
			}
		}

		bool edge_starts_with( Edge* e, T c )
		{
			if( e->is_empty() )
				return false;
			assert( e->first_id < chars.size() );
			return chars[ e->first_id ] == c;
		}

		//----------------------------------------
		//  TODO - should optimize with hash table
		//----------------------------------------
		Edge* get_child_starting_with( Edge* e, T c )
		{
			for( int i = 0; i < e->children.size(); i++ )
			{
				if( edge_starts_with( e->children[i], c ) )
					return e->children[i];
			}
			return NULL;
		}

		void update( size_t next_id )
		{
			T next_char = chars[ next_id ];
			Suffix suffix = active_point;
			Edge* last_leaf_parent = active_point.edge;

			while( true )
			{
				if( suffix.is_explicit_node(next_id) )
				{
					Edge* match_child = get_child_starting_with( suffix.edge, next_char );
					if( match_child == NULL )
					{
						// create leaf
						suffix.edge->add_child( new Edge( next_id, 1 ) );

						// update suffix pointers
						if( last_leaf_parent != NULL )
							last_leaf_parent->next_smaller_edge = suffix.edge;
						last_leaf_parent = suffix.edge;
					}
					else
					{
						// done!
						suffix.edge = match_child;
						suffix.last_id = match_child->first_id;
						break;
					}
				}
				else
				{
					// check next character of the implicit node
					if( chars[ suffix.last_id+1 ] != next_char )
					{
						// need to split
						Edge* e = suffix.edge;
						Edge* f = new Edge( suffix.last_id+1, e->get_last_id()-suffix.last_id );

						// shorten original edge
						e->num_chars = suffix.last_id - e->first_id + 1;

						// add latter part
						e->add_child( f );

						// create new edge with new character
						Edge* n = new Edge( next_id, 1 );
						e->add_child( n );

						// update suffix pointers
						if( last_leaf_parent != NULL )
							last_leaf_parent->next_smaller_edge = e;
						last_leaf_parent = e;
					}
					else
					{
						suffix.last_id++;
 						break;
					}
				}

				if( suffix.is_empty() )
					break;
				else
					suffix.move_to_next_smaller_suffix();
			}

			active_point = suffix;
		}

};


#endif /* end of include guard: __SUFFIXTREE_HEADER_GUARD__ */
