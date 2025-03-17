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
	Binary_Search_Tree<int> bst({ 40,50,30,35,10,75,23,87,68 });
	Binary_Search_Tree<int> bst2(bst.rend(), bst.rbegin());

	auto b = bst2.begin();
	auto e = bst2.end();
	while (b != e)
	{
		std::cout << *b << " ";
		++b;
	}
	std::cout << std::endl;
	cout << (bst == bst2) << endl;
	//char carr[] = "abc", carr2[] = "def";
	//Binary_Search_Tree<char> v0;
	//auto pib = v0.insert('d');
	//auto d0 = v0.end();
	//std::cout << *--d0 << std::endl;
	//v0.insert(carr, carr + 3);
	//v0.insert(carr2, carr2 + 3);
	//auto d = v0.end();
	//std::cout << *--d << std::endl;
	//std::cout << *v0.begin() << std::endl;
	//std::cout << *v0.erase(v0.begin()) << std::endl;
	cout << " -------------------------------- \n";
	char carr[] = "abc", carr2[] = "def";
	Binary_Search_Tree<char> v0;
	Binary_Search_Tree<char> v1(carr, carr + 3);

	v0.clear();
	std::pair<Binary_Search_Tree<char>::iterator, bool> pib = v0.insert('d');
	pib = v0.insert('d');
	v0.insert(v0.begin(), 'e');
	v0.insert(carr, carr + 3);
	v0.insert(carr2, carr2 + 3);
	v0.erase(v0.begin());
	v0.erase(v0.begin(), ++v0.begin());
	v0.erase('x');
	v0.erase('e');
	{
		auto b1 = v0.begin();
		auto b2 = v1.begin();
		auto e1 = v0.end();
		auto e2 = v1.end();
		while (b1 != e1)
		{
			std::cout << *b1++ << " ";
		}
		std::cout << std::endl;
		while (b2 != e2)
		{
			std::cout << *b2++ << " ";
		}
		std::cout << std::endl;
	}
	v0.clear();
	std::cout << "1: " << v0.empty() << std::endl;
	v0.swap(v1);
	{
		auto b1 = v0.begin();
		auto b2 = v1.begin();
		auto e1 = v0.end();
		auto e2 = v1.end();
		while (b1 != e1)
		{
			std::cout << *b1++ << " ";
		}
		std::cout << std::endl;
		while (b2 != e2)
		{
			std::cout << *b2++ << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "2: " << (!v0.empty() && v1.empty()) << std::endl;
	std::swap(v0, v1);
	std::cout << "3: " << (v0.empty() && !v1.empty()) << std::endl;
	{
		auto b1 = v0.begin();
		auto b2 = v1.begin();
		auto e1 = v0.end();
		auto e2 = v1.end();
		while (b1 != e1)
		{
			std::cout << *b1++ << " ";
		}
		std::cout << std::endl;
		while (b2 != e2)
		{
			std::cout << *b2++ << " ";
		}
		std::cout << std::endl;
	}



	std::cout << "4: " << (v1 == v0 && !(v0 < v1)) << std::endl;
	std::cout << "5: " << (!(v0 != v1)) << std::endl;
	std::cout << "6: " << (v0 <= v1 && v1 >= v0) << std::endl;
	#ifdef _WIN32
		system("pause");
	#endif //_WIN32
	return 0;
}