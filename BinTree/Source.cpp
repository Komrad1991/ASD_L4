#include <iostream>
#include <set>
#include <iterator>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <random>
#include "BStree.h"
#include <windows.h>

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

	//v0.clear();
	//std::pair<Binary_Search_Tree<char>::iterator, bool> pib = v0.insert('d');
	//pib = v0.insert('d');
	//v0.insert(v0.begin(), 'e');
	//v0.insert(carr, carr + 3);
	//v0.insert(carr2, carr2 + 3);
	//v0.erase(v0.begin());
	//v0.erase(v0.begin(), ++v0.begin());
	//v0.erase('x');
	//v0.erase('e');
	//v0.clear();
	//std::cout << "1: " << v0.empty() << std::endl;
	//v0.swap(v1);
	std::cout << v0.dummy->left->data << std::endl;
	std::cout << v1.dummy->left->data << std::endl;
	//std::cout << "2: " << (!v0.empty() && v1.empty()) << std::endl;
	std::swap(v1, v0);
	std::cout << v0.dummy->left->data << std::endl;
	std::cout << v1.dummy->left->data << std::endl;
	std::cout << "3: " << (v0.empty() && !v1.empty()) << std::endl;
	{
		auto b1 = v0.begin();
		auto b2 = v1.begin();
		auto e1 = v0.end();
		auto e2 = v1.end();
		std::cout << "v0: " << std::endl;
		while (b1 != e1)
		{
			std::cout << *b1++ << " ";
		}
		std::cout << std::endl;
		std::cout << "v1: " << std::endl;
		while (b2 != e2)
		{
			std::cout << *b2++ << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "4: " << (v1 == v0 && !(v0 < v1)) << std::endl;
	std::cout << "5: " << (!(v0 != v1)) << std::endl;
	std::cout << "6: " << (v0 <= v1 && v1 >= v0) << std::endl;
	cout << " -------------------------------- \n";
	Binary_Search_Tree<char> S1{ 'a','b','c','d' };
	Binary_Search_Tree<char> S2 = S1;
	{
		auto sb = S1.begin();
		auto se = S1.end();
		while (sb != se)
		{
			std::cout << *sb++ << " ";
		}
		std::cout << std::endl;

		auto s2b = S2.begin();
		auto s2e = S2.end();
		while (s2b != s2e)
		{
			std::cout << *s2b++ << " ";
		}
		std::cout << std::endl;
	}
	std::cout << S1.dummy->left->data << std::endl;
	std::cout << S2.dummy->left->data << std::endl;
	std::swap(S2, S1);
	std::cout << S1.dummy->left->data << std::endl;
	std::cout << S2.dummy->left->data << std::endl;
	{
		auto sb = S1.begin();
		auto se = S1.end();
		while (sb != se)
		{
			std::cout << *sb++ << " ";
		}
		std::cout << std::endl;

		auto s2b = S2.begin();
		auto s2e = S2.end();
		while (s2b != s2e)
		{
			std::cout << *s2b++ << " ";
		}
		std::cout << std::endl;
	}
	std::cout << S1.dummy->left->data << std::endl;
	std::cout << S2.dummy->left->data << std::endl;
	std::swap(S1, S2);
	std::cout << S1.dummy->left->data << std::endl;
	std::cout << S2.dummy->left->data << std::endl;
	cout << " -------------------------------- \n";
	{
		char carr[] = "abc", carr2[] = "def";
		Binary_Search_Tree<char> v0;
		Binary_Search_Tree<char> v1(carr, carr + 3);
		v0 = v1;

		v0.clear();
		std::cout << (*v0.insert('d') == 'd') << std::endl;
		std::cout << (*--v0.end() == 'd') << std::endl;
		std::cout << (*v0.insert('d') == 'd') << std::endl;
		std::cout << (v0.size() == 2) << std::endl;
		std::cout << (*v0.insert(v0.begin(), 'e') == 'e') << std::endl;
		v0.insert(carr, carr + 3);
		std::cout << (v0.size() == 6 && *v0.begin() == 'a') << std::endl;
		v0.insert(carr2, carr2 + 3);
		std::cout << (v0.size() == 9 && *--v0.end() == 'f') << std::endl;
		std::cout << (*v0.erase(v0.begin()) == 'b' && v0.size() == 8) << std::endl;
		std::cout << (*v0.erase(v0.begin(), ++v0.begin()) == 'c' && v0.size() == 7) << std::endl;
		std::cout << (v0.erase('x') == 0) << std::endl;
		v0.printTree();
		v0.printTree();
		{
			Binary_Search_Tree<std::string> v0{ "1","2", "abc","356" };
			v0.printTree();
			v0.saveToFile("2.txt");
			Binary_Search_Tree<std::string> v3 = uploadFromFile("2.txt");
			std::cout << "v3 tree" << std::endl;
			v3.printTree();
		}
	}
	#ifdef _WIN32
		system("pause");
	#endif //_WIN32
	return 0;
}