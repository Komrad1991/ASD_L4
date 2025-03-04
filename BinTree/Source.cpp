#include <iostream>
#include <set>
#include <iterator>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <random>
#include "BStree.h"

using namespace std;

template<typename InputIter>
void print(InputIter first, InputIter last) {

	if (std::is_same<typename iterator_traits<InputIter>::iterator_category, std::random_access_iterator_tag>::value) {
		cout << "Random access iterator range : ";
		while (first != last)
			cout << *first++ << " ";
		cout << endl;
	}
	else {
		cout << "Any input iterator range : ";
		while (first != last)
			cout << *first++ << " ";
		cout << endl;
	}
}

int main() {

	const size_t sz = 15;
	vector<int> v;
	v.reserve(sz);
	std::random_device rd;  
	std::mt19937 gen(rd()); 
	std::uniform_int_distribution<> dis(0,+1000);
	generate_n(back_inserter(v), sz, [&]() {return (dis(gen) % 5000); });
	sort(v.begin(), v.end());
	cout << "\n -------------------------------- \n";
	copy(v.begin(), v.end(), ostream_iterator<int>(cout, " "));
	cout << "\n -------------------------------- \n";
	set<int> bb(v.begin(), v.end());
	print(bb.begin(), bb.end());
	Binary_Search_Tree<int> bst;
	cout << bst.size() << endl;
	cout << " -------------------------------- \n";


	#ifdef _WIN32
		system("pause");
	#endif //_WIN32
	return 0;
}