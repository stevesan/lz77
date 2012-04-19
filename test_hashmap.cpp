#include <boost/unordered_map.hpp>
#include <cassert>

using namespace std;

int main(void)
{
	typedef boost::unordered_map< string, int > map;
	map x;
	//x["one"] = 1;


	cout << x["one"] << endl;
	assert( x["one"] == 1 );
}

