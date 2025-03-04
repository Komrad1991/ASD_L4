#include "CppUnitTest.h"
#include "..\BinTree\BStree.h"
#include <set>
#include <functional>
#include <memory_resource>
#include <iterator>

  //Тестирование заголовка <set>, основанное на книге «The C++ Standard Template Library» P.J. Plauger, Alexander A. Stepanov,
  //    Meng Lee, David R. Musser. Немного модифицировано, и разбито на отдельные тесты. 
	 // 
	 // Эти тесты могут быть использованы для тестирования бинарного дерева поиска, написанного по аналогии с библиотечной реализацией STL,
	 // причём их можно использовать как для тестирования шаблона set на основе RBT, так и на основе AVL.
	 // 
	 // Следует иметь в виду, авторы тестов являются также и авторами реализации контейнера std::set, так что тесты так себе, далеко не 
	 // все проблемные случаи покрывают, но хотя бы так.

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TreeTest
{
	TEST_CLASS(BaseTests)
	{
	public:

		template<typename T> using Myal = std::allocator<T>;
		template<typename T> using Mypred = std::less<T>;

		//  Для того, чтобы выполнить тестирование одного из указанных контейнеров (std::set или Binary_Tree_Search)
		//    должна быть раскомментирована одна из следующих строк:
		template<typename T> using ContainerTemplate = std::set<T, Mypred<T>, Myal<T>>;
		//template<typename T> using ContainerTemplate = Binary_Search_Tree<T, Mypred<T>, Myal<T>>;

		TEST_METHOD(TreeSizeTest)
		{
			//  Тестирование размера дерева при разных сценариях использования
			ContainerTemplate<int> T1;
			Assert::AreEqual(T1.size(), ContainerTemplate<int>::size_type(0), L"Пустое дерево имеет ненулевой размер!");
			ContainerTemplate<int> Tree = { 40,50,30,35,10,75,23,87,68 };
			Assert::AreEqual(Tree.size(), ContainerTemplate<int>::size_type(9), L"Неверно указывается размер дерева!");
			ContainerTemplate<int> Tree2(Tree);
			Assert::AreEqual(Tree.size(), Tree2.size(), L"Неверно указывается размер после копирования!");
		}

		TEST_METHOD(TreesEqualityTest)
		{
			//  Тестирование оператора == для дерева
			ContainerTemplate<int> T1, T2;
			Assert::IsTrue(T1 == T2, L"Неверно выполняется сравнение пустых деревьев!");
			ContainerTemplate<int> Tree = { 40,50,30,35,10,75,23,87,68 };
			ContainerTemplate<int> Tree2(Tree);
			Assert::IsTrue(Tree == Tree2, L"Ошибка сравнения деревьев на равенство после копирования!");
			ContainerTemplate<int> Tree3(Tree.rbegin(), Tree.rend());
			Assert::IsTrue(Tree2 == Tree3, L"Ошибка сравнения деревьев на равенство после копирования в обратном порядке!");
			
			Logger::WriteMessage("Ого, кажется всё хорошо!");
		}
		TEST_METHOD(TreeAssignmentTests)
		{
			//  Тестирование оператора == для дерева
			ContainerTemplate<int> T1 = { 40,50,30,35,10,75,23,87,68 };
			ContainerTemplate<int> T2;
			T2 = T1;
			Assert::IsTrue(T1 == T2, L"Ошибка в операторе присваивания!");
			T2.clear();
			T1 = T2;
			Assert::IsTrue(T1 == T2, L"Ошибка в операторе присваивания пустых деревьев!");
			ContainerTemplate<int> T3 = { 40,50,30,35,10,75,23,87,68 };
			T1 = T3;
			T2 = std::move(T3);
			Assert::IsTrue(T1 == T2, L"Ошибка при перемещающем операторе присваивания!");
		}
	};
	
	TEST_CLASS(SetTests)
	{
		//  Тесты стандартного контейнера std::set, из книги "The C++ Standard template library" Плаугера, Степанова и др.
	public:
		
		template<typename T> using Myal = std::allocator<T>;
		template<typename T> using Mypred = std::less<T>;

		//  Для того, чтобы выполнить тестирование одного из указанных контейнеров (std::set или Binary_Tree_Search)
		//    должна быть раскомментирована одна из следующих строк:
		template<typename T> using ContainerTemplate = std::set<T, Mypred<T>, Myal<T>>;
		//template<typename T> using ContainerTemplate = Binary_Search_Tree<T, Mypred<T>, Myal<T>>;

		using Mycont = ContainerTemplate<char>;

		TEST_METHOD(SetSize)
		{
			Mycont v0;
			Myal<char> al = v0.get_allocator();
			Mypred<char> pred;
			Mycont v0a(pred), v0b(pred, al);
			Assert::IsTrue(v0.empty() && v0.size() == 0, L"Неверный размер пустого set");
			Assert::IsTrue(v0a.size() == 0 && v0a.get_allocator() == al, L"Неверный размер или аллокатор");
			Assert::IsTrue(v0b.size() == 0 && v0b.get_allocator() == al, L"Неверный размер или аллокатор");
		}

		TEST_METHOD(SetCreation)
		{
			char carr[] = "abc", carr2[] = "def";
			Mycont v0;
			Myal<char> al = v0.get_allocator();
			Mypred<char> pred;
			Mycont v0a(pred);

			Mycont v1(carr, carr + 3);
			Assert::IsTrue(v1.size() == 3 && *v1.begin() == 'a', L"Неверно создаётся set символов");

			Mycont v2(carr, carr + 3, pred);
			Assert::IsTrue(v2.size() == 3 && *v2.begin() == 'a', L"Неверно создаётся set символов");

			Mycont v3(carr, carr + 3, pred, al);
			Assert::IsTrue(v3.size() == 3 && *v3.begin() == 'a', L"Неверно создаётся set символов");

			const Mycont v4(carr, carr + 3);
			v0 = v4;
			Assert::IsTrue(v1.size() == 3 && *v1.begin() == 'a', L"Неверно работает оператор присваивания для set");
		}

		TEST_METHOD(SetIterators)
		{
			char ch('a'), carr[] = "abc", carr2[] = "def";
			Mycont v0;
			Myal<char> al = v0.get_allocator();
			Mypred<char> pred;
			Mycont v0a(pred);

			Mycont v1(carr, carr + 3);
			Mycont v2(carr, carr + 3, pred);
			Mycont v3(carr, carr + 3, pred, al);
			const Mycont v4(carr, carr + 3);
			v0 = v4;

			Mycont::iterator p_it(v1.begin());
			Mycont::const_iterator p_cit(v4.begin());
			Mycont::reverse_iterator p_rit(v1.rbegin());
			Mycont::const_reverse_iterator p_crit(v4.rbegin());

			/*Assert::IsTrue(*p_it == 'a' && *--(p_it = v1.end()) == 'c', L"Декремент end() не корректен?");
			Assert::IsTrue(*p_cit == 'a' && *--(p_cit = v4.end()) == 'c', L"Декремент для const iterator на end() не корректен?");
			//Assert::IsTrue(*p_rit == 'c' && *--(p_rit = v1.rend()) == 'a', L"Reverse iterator не корректен?");
			Assert::IsTrue(*p_crit == 'c' && *--(p_crit = v4.rend()) == 'a', L"Const reverse iterator не корректен?");*/
		}

		TEST_METHOD(SetInsertEraseTests)
		{
			char carr[] = "abc", carr2[] = "def";
			Mycont v0;
			Myal<char> al = v0.get_allocator();
			Mypred<char> pred;
			Mycont v0a(pred);

			Mycont v1(carr, carr + 3);
			Mycont v2(carr, carr + 3, pred);
			Mycont v3(carr, carr + 3, pred, al);
			const Mycont v4(carr, carr + 3);
			v0 = v4;

			Mycont::iterator p_it(v1.begin());
			Mycont::const_iterator p_cit(v4.begin());
			Mycont::reverse_iterator p_rit(v1.rbegin());
			Mycont::const_reverse_iterator p_crit(v4.rbegin());

			v0.clear();
			std::pair<Mycont::iterator, bool> pib = v0.insert('d');
			Assert::IsTrue(*pib.first == 'd' && pib.second);
			Assert::IsTrue(*--v0.end() == 'd');
			pib = v0.insert('d');
			Assert::IsTrue(*pib.first == 'd' && !pib.second);
			Assert::IsTrue(*v0.insert(v0.begin(), 'e') == 'e');
			v0.insert(carr, carr + 3);
			Assert::IsTrue(v0.size() == 5 && *v0.begin() == 'a');
			v0.insert(carr2, carr2 + 3);
			Assert::IsTrue(v0.size() == 6 && *--v0.end() == 'f');
			Assert::IsTrue(*v0.erase(v0.begin()) == 'b' && v0.size() == 5);
			Assert::IsTrue(*v0.erase(v0.begin(), ++v0.begin()) == 'c' && v0.size() == 4);
			Assert::IsTrue(v0.erase('x') == 0 && v0.erase('e') == 1);
		}

		TEST_METHOD(SetSwapAndCompTests)
		{
			char carr[] = "abc", carr2[] = "def";
			Mycont v0;
			Mycont v1(carr, carr + 3);

			v0.clear();
			std::pair<Mycont::iterator, bool> pib = v0.insert('d');
			pib = v0.insert('d');
			v0.insert(v0.begin(), 'e');
			v0.insert(carr, carr + 3);
			v0.insert(carr2, carr2 + 3);
			v0.erase(v0.begin());
			v0.erase(v0.begin(), ++v0.begin());
			v0.erase('x');
			v0.erase('e');
			v0.clear();
			Assert::IsTrue(v0.empty());
			v0.swap(v1);
			Assert::IsTrue(!v0.empty() && v1.empty());
			std::swap(v0, v1);
			Assert::IsTrue(v0.empty() && !v1.empty());
			Assert::IsTrue(v1 == v1 && v0 < v1, L"Сравнение множеств некорректно!");
			Assert::IsTrue(v0 != v1 && v1 > v0, L"Сравнение множеств некорректно!");
			Assert::IsTrue(v0 <= v1 && v1 >= v0, L"Сравнение множеств некорректно!");
		}

		TEST_METHOD(SetComparatorTests)
		{
			Mycont v0;
			Assert::IsTrue(v0.key_comp()('a', 'c') && !v0.key_comp()('a', 'a'), L"Некорректный компаратор!");
			Assert::IsTrue(v0.value_comp()('a', 'c') && !v0.value_comp()('a', 'a'), L"Некорректный компаратор!");
		}

		TEST_METHOD(SetAlgTests)
		{
			char carr[] = "abc";
			const Mycont v4(carr, carr+3);

			Assert::IsTrue(*v4.find('b') == 'b');
			Assert::IsTrue(v4.count('x') == 0 && v4.count('b') == 1);
			Assert::IsTrue(*v4.lower_bound('a') == 'a', L"Метод lower_bound");
			Assert::IsTrue(*v4.upper_bound('a') == 'b', L"Метод upper_bound");
			std::pair<Mycont::const_iterator, Mycont::const_iterator> pcc = v4.equal_range('a');
			Assert::IsTrue(*pcc.first == 'a' && *pcc.second == 'b', L"Ошибка метода equal_range");
		}
	};

	TEST_CLASS(MultiSetTests)
	{
		///  Тесты стандартного контейнера std::multiset, из книги "The C++ Standard template library" Плаугера, Степанова и др.
	public:

		using Myal = std::allocator<char>;
		using Mypred = std::less<char>;

		//  Для того, чтобы выполнить тестирование одного из указанных контейнеров (std::set или Binary_Tree_Search)
		//    должна быть раскомментирована одна из следующих строк:
		template<typename T> using ContainerTemplate = std::multiset<T, Mypred, Myal>;
		//template<typename T> using ContainerTemplate = Binary_Search_Tree<T, Mypred, Myal>;

		using Mycont = ContainerTemplate<char>;

		TEST_METHOD(MultiSetSize)
		{
			Mycont v0;
			Myal al = v0.get_allocator();
			Mypred pred;
			Mycont v0a(pred), v0b(pred, al);
			Assert::IsTrue(v0.empty() && v0.size() == 0, L"Неверный размер пустого set");
			Assert::IsTrue(v0a.size() == 0 && v0a.get_allocator() == al, L"Неверный размер или аллокатор");
			Assert::IsTrue(v0b.size() == 0 && v0b.get_allocator() == al, L"Неверный размер или аллокатор");
		}

		TEST_METHOD(MultiSetCreation)
		{
			char carr[] = "abc", carr2[] = "def";
			Mycont v0;
			Myal al = v0.get_allocator();
			Mypred pred;
			Mycont v0a(pred);

			Mycont v1(carr, carr + 3);
			Assert::IsTrue(v1.size() == 3 && *v1.begin() == 'a', L"Неверно создаётся set символов");

			Mycont v2(carr, carr + 3, pred);
			Assert::IsTrue(v2.size() == 3 && *v2.begin() == 'a', L"Неверно создаётся set символов");

			Mycont v3(carr, carr + 3, pred, al);
			Assert::IsTrue(v3.size() == 3 && *v3.begin() == 'a', L"Неверно создаётся set символов");

			const Mycont v4(carr, carr + 3);
			v0 = v4;
			Assert::IsTrue(v1.size() == 3 && *v1.begin() == 'a', L"Неверно работает оператор присваивания для set");
		}

		TEST_METHOD(MultiSetIterators)
		{
			char ch('a'), carr[] = "abc", carr2[] = "def";
			Mycont v0;
			Myal al = v0.get_allocator();
			Mypred pred;
			Mycont v0a(pred);

			Mycont v1(carr, carr + 3);
			Mycont v2(carr, carr + 3, pred);
			Mycont v3(carr, carr + 3, pred, al);
			const Mycont v4(carr, carr + 3);
			v0 = v4;

			Mycont::iterator p_it(v1.begin());
			Mycont::const_iterator p_cit(v4.begin());
			Mycont::reverse_iterator p_rit(v1.rbegin());
			Mycont::const_reverse_iterator p_crit(v4.rbegin());

			Assert::IsTrue(*p_it == 'a' && *--(p_it = v1.end()) == 'c', L"Декремент end() не корректен?");
			Assert::IsTrue(*p_cit == 'a' && *--(p_cit = v4.end()) == 'c', L"Декремент для const iterator на end() не корректен?");
			Assert::IsTrue(*p_rit == 'c' && *--(p_rit = v1.rend()) == 'a', L"Reverse iterator не корректен?");
			Assert::IsTrue(*p_crit == 'c' && *--(p_crit = v4.rend()) == 'a', L"Const reverse iterator не корректен?");
		}

		TEST_METHOD(MultiSetInsertEraseTests)
		{
			char carr[] = "abc", carr2[] = "def";
			Mycont v0;
			Myal al = v0.get_allocator();
			Mypred pred;
			Mycont v0a(pred);

			Mycont v1(carr, carr + 3);
			Mycont v2(carr, carr + 3, pred);
			Mycont v3(carr, carr + 3, pred, al);
			const Mycont v4(carr, carr + 3);
			v0 = v4;

			Mycont::iterator p_it(v1.begin());
			Mycont::const_iterator p_cit(v4.begin());
			Mycont::reverse_iterator p_rit(v1.rbegin());
			Mycont::const_reverse_iterator p_crit(v4.rbegin());
		}

		TEST_METHOD(MultiSetInsEraseTests)
		{
			char carr[] = "abc", carr2[] = "def";
			Mycont v0;
			Mycont v1(carr, carr + 3);
			v0 = v1;

			v0.clear();
			Assert::IsTrue(*v0.insert('d') == 'd');
			Assert::IsTrue(*--v0.end() == 'd');
			Assert::IsTrue(*v0.insert('d') == 'd', L"Повторная вставка не сработала");
			Assert::IsTrue(v0.size() == 2, L"Неправильный размер multiset");
			Assert::IsTrue(*v0.insert(v0.begin(), 'e') == 'e', L"Вставка с указанием итератора");
			v0.insert(carr, carr + 3);
			Assert::IsTrue(v0.size() == 6 && *v0.begin() == 'a');
			v0.insert(carr2, carr2 + 3);
			Assert::IsTrue(v0.size() == 9 && *--v0.end() == 'f');
			Assert::IsTrue(*v0.erase(v0.begin()) == 'b' && v0.size() == 8);
			Assert::IsTrue(*v0.erase(v0.begin(), ++v0.begin()) == 'c' && v0.size() == 7);
			Assert::IsTrue(v0.erase('x') == 0 && v0.erase('e') == 2);
		}

		TEST_METHOD(MultiSetSwapTests)
		{
			char carr[] = "abc", carr2[] = "def";
			Mycont v0;
			Mycont v1(carr, carr + 3);
			v0.clear();
			v0 = v1;
			v0.insert('d'); v0.insert('d');
			v0.insert(v0.begin(), 'e');
			v0.insert(carr, carr + 3);
			v0.insert(carr2, carr2 + 3);
			v0.erase(v0.begin());
			v0.erase(v0.begin(), ++v0.begin());
			v0.erase('x');
			v0.erase('e');
			v0.clear();
			Assert::IsTrue(v0.empty(), L"А должно быть пустым!");
			v0.swap(v1);
			Assert::IsTrue(!v0.empty() && v1.empty(), L"Метод swap на multiset сработала неправильно!");
			swap(v0, v1);
			Assert::IsTrue(v0.empty() && !v1.empty(), L"Операция swap на multiset сработала неправильно!");
			Assert::IsTrue(v1 == v1 && v0 < v1, L"Сравнение некорректно");
			Assert::IsTrue(v0 != v1 && v1 > v0, L"Сравнение некорректно");
			Assert::IsTrue(v0 <= v1 && v1 >= v0, L"Сравнение некорректно");
		}

		TEST_METHOD(MultiSetAlgTests)
		{
			char carr[] = "abc", carr2[] = "def";
			Mycont v0;
			Mycont v1(carr, carr + 3);
			Mycont v4(carr, carr + 3);
			v0.clear();
			v0 = v1;
			v0.insert('d'); v0.insert('d');
			v0.insert(v0.begin(), 'e');
			v0.insert(carr, carr + 3);
			v0.insert(carr2, carr2 + 3);
			v0.erase(v0.begin());
			v0.erase(v0.begin(), ++v0.begin());
			v0.erase('x');
			v0.erase('e');
			v0.clear();
			v0.swap(v1);
			swap(v0, v1);

			Assert::IsTrue(v0.key_comp() ('a', 'c') && !v0.key_comp() ('a', 'a'), L"Проблема с компаратором ключей");
			Assert::IsTrue(v0.value_comp() ('a', 'c') && !v0.value_comp() ('a', 'a'), L"Проблема с компаратором значений");
			Assert::IsTrue(*v4.find('b') == 'b', L"Метод find");

			Assert::IsTrue(v4.count('x') == 0 && v4.count('b') == 1);
			Assert::IsTrue(*v4.lower_bound('a') == 'a', L"Метод lower_bound");
			Assert::IsTrue(*v4.upper_bound('a') == 'b', L"Метод upper_bound");

			std::pair<Mycont::const_iterator, Mycont::const_iterator> pcc = v4.equal_range('a');
			Assert::IsTrue(*pcc.first == 'a' && *pcc.second == 'b', L"Ошибка метода equal_range");
			Logger::WriteMessage("Вот так оно и бывает: тесты говорят, что всё в норме. Но верить им нельзя!");
		}
	};

	//-------------------------------------------------------------------------------------------------------------------
	// Класс Elem для тестирования правильной работы с памятью
	class Elem {
		int data;
		uint32_t watch;
		static size_t elem_counter;
		void check() const { Assert::IsTrue(watch == 0xDEADBEEF, L"Повреждение памяти!! (Обращение к неинициализированному экземпляру класса Elem)"); }
	public:
		Elem(const Elem&) = delete;
		Elem& operator=(const Elem&) = delete;
		explicit Elem(int value) : data(value), watch(0xDEADBEEF) { ++elem_counter; }
		Elem(Elem&& e) noexcept { e.check(); data = e.data; watch = e.watch; ++elem_counter; }
		Elem& operator=(Elem&& e) noexcept { check(); e.check(); data = e.data; watch = e.watch; }
		bool operator<(const Elem& e) const { check(); e.check(); return data < e.data; }
		~Elem() { check(); watch = 0; --elem_counter; }
		static size_t count() { return elem_counter; }
	};
	size_t Elem::elem_counter = 0;

	TEST_CLASS(ComplexTypeTests)
	{
		///  Немного отсебятины - тесты на сложных структурах данных
	public:

		template<typename T> using Myal = std::allocator<T>;
		template<typename T> using Mypred = std::less<T>;

		//  Для того, чтобы выполнить тестирование одного из указанных контейнеров (std::set или Binary_Tree_Search)
		//    должна быть раскомментирована одна из следующих строк:
		template<typename T> using ContainerTemplate = std::set<T, Mypred<T>, Myal<T>>;
		//template<typename T> using ContainerTemplate = Binary_Search_Tree<T, Mypred<T>, Myal<T>>;


		TEST_METHOD(StringTests)
		{
			ContainerTemplate<std::string> T1{ "abc", "cde", "123", "AAAAAAAA" };
			std::vector<std::string> check1{ "123", "AAAAAAAA", "abc", "cde" };

			Assert::IsTrue(T1.size() == 4, L"Неправильно считается количество строковых элементов");
			Assert::IsTrue(std::equal(T1.begin(), T1.end(), check1.begin(), check1.end()), L"Неправильный порядок строковых элементов");
			Assert::IsTrue(std::equal(T1.rbegin(), T1.rend(), check1.rbegin(), check1.rend()), L"Неправильный порядок строковых элементов");

			for (const auto& str : check1)
				T1.erase(str);

			Assert::IsTrue(T1.size() == 0, L"Сет не пуст после удаления всех элементов");
			for (const auto& elem : T1)
				Assert::Fail(L"Что-то лежит в сете после удаления всех элементов");

			T1.insert("test-1");
			T1.insert("test-4");
			T1.insert("test-2");
			std::vector<std::string> check2{ "test-1", "test-2", "test-4" };
			Assert::IsTrue(std::equal(T1.begin(), T1.end(), check2.begin(), check2.end()), L"Неправильный порядок строковых элементов");
			Assert::IsTrue(std::equal(T1.rbegin(), T1.rend(), check2.rbegin(), check2.rend()), L"Неправильный порядок строковых элементов");
		}

		TEST_METHOD(ElemTests)
		{
			size_t init_count = Elem::count();
			{
				ContainerTemplate<Elem> T1;
				Assert::IsTrue(Elem::count() - init_count == 0, L"Создан лишний экземпляр класса Elem");

				for (const auto& elem : T1)
					Assert::Fail(L"Что-то лежит в пустом сете");

				T1.insert(Elem(40));
				T1.insert(Elem(75));
				T1.insert(Elem(50));
				T1.insert(Elem(23));
				T1.insert(Elem(87));
				Assert::IsTrue(Elem::count() - init_count == 5, L"Создан лишний экземпляр класса Elem");
				T1.erase(Elem(75));
				Assert::IsTrue(Elem::count() - init_count == 4, L"Неправильно работает удаление элементов дерева");
				T1.erase(Elem(100));
				Assert::IsTrue(Elem::count() - init_count == 4, L"Неправильно работает удаление несуществующих элементов");
			}
			Assert::IsTrue(Elem::count() - init_count == 0, L"Утечка памяти!!");
		}
	};
}