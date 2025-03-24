#pragma once

//  Фрагменты для реализации сбалансированных деревьев поиска - заготовка, не рабочая, доделать

#include <iostream>
#include <cassert>
#include <queue>
#include <vector>
#include <string>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <initializer_list>
#include <functional>
#include <exception>
#include <algorithm>
#include <fstream>
#include <string>

template<typename T, class Compare = std::less<T>, class Allocator = std::allocator<T>>
class Binary_Search_Tree
{
	//  Объект для сравнения ключей. Должен удовлетворять требованию строго слабого порядка, т.е. иметь свойства:
	//    1. Для любого x => cmp(x,x) == false (антирефлексивность)
	//    2. Если cmp(x,y) == true  =>  cmp(y,x) == false (асимметричность)
	//    3. Если cmp(x,y) == cmp(y,z) == true  =>  cmp(x,z) == true  (транзитивность)
	//    4. Если cmp(x,y) == cmp(y,z) == false  =>  cmp(x,z) == false  (транзитивность несравнимости)
	//  Этим свойством обладает, к примеру, оператор <. Если нужно использовать оператор <= , который не обладает
	//     нужными свойствами, то можно использовать его отрицание и рассматривать дерево как инвертированное от требуемого.
	Compare cmp = Compare();

	//  Узел бинарного дерева, хранит ключ, три указателя и признак nil для обозначения фиктивной вершины
	class Node
	{
	public:  //  Все поля открыты (public), т.к. само определение узла спрятано в private-части дерева
		//  Возможно, добавить поле типа bool для определения того, является ли вершина фиктивной (листом)
		Node* parent;
		Node* left;
		Node* right;
		bool color;
		//  Хранимый в узле ключ
		T data;
		Node(T value = T(), Node* p = nullptr, Node* l = nullptr, Node* r = nullptr,bool c = false) : parent(p), data(value), left(l), right(r),color(c) {}
	};

	//  Стандартные контейнеры позволяют указать пользовательский аллокатор, который используется для
	//  выделения и освобождения памяти под узлы (реализует замену операций new/delete). К сожалению, есть 
	//  типичная проблема – при создании дерева с ключами типа T параметром шаблона традиционно указывается
	//  std::allocator<T>, который умеет выделять память под T, а нам нужен аллокатор для выделения/освобождения
	//  памяти под Node, т.е. std::allocator<Node>. Поэтому параметр шаблона аллокатора нужно изменить
	//  с T на Node, что и делается ниже. А вообще это одна из самых малополезных возможностей - обычно мы
	//  пользовательские аллокаторы не пишем, это редкость.

	//  Определяем тип аллокатора для Node (Allocator для T нам не подходит)
	using AllocType = typename std::allocator_traits<Allocator>::template rebind_alloc < Node >;
	//  Аллокатор для выделения памяти под объекты Node
	AllocType Alc;
	
	//  Рекурсивное клонирование дерева (не включая фиктивную вершину)
	//  Идея так себе - вроде пользуемся стандартной вставкой, хотя явное клонирование вершин было бы лучше
	void clone(Node * from, Node * other_dummy)
	{
		if (from == other_dummy)
			return;
		//	клонирование через insert? оно же будет переразвешиваться
		// Это ещё и рекурсивный проход в ширину, выраждает дево в список
		insert(from->data);	
		clone(from->right, other_dummy);
		clone(from->left, other_dummy);
	}
public:
	//  Эти типы должен объявлять контейнер - для функциональности
	using key_type = T;
	using key_compare = Compare;
	using value_compare = Compare;
	using value_type = typename T;
	using allocator_type = typename AllocType;
	using size_type = typename size_t;
	using difference_type = typename size_t;
	using pointer = typename T *;
	using const_pointer = typename const pointer;
	using reference = value_type & ;
	using const_reference = const value_type &;
	//using iterator = typename _Mybase::iterator;   //  Не нужно! Явно определили iterator внутри данного класса
	class iterator;   //  Предварительное объявление класса iterator, т.к. он определён ниже
	using const_iterator = iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
public:
	// Указательно на фиктивную вершину
	Node* dummy;
private:
	//  Количесто элементов в дереве
	size_type tree_size = 0;

	// Создание фиктивной вершины - используется только при создании дерева
	inline Node* make_dummy()
	{
		// Выделяем память по размеру узла без конструирования
		dummy = Alc.allocate(1);
		
		//  Все поля, являющиеся указателями на узлы (left, right, parent) инициализируем и обнуляем
		std::allocator_traits<AllocType>::construct(Alc, &(dummy->parent));
		dummy->parent = dummy;

		std::allocator_traits<AllocType>::construct(Alc, &(dummy->left));
		dummy->left = dummy;

		std::allocator_traits<AllocType>::construct(Alc, &(dummy->right));
		dummy->right = dummy;

		std::allocator_traits<AllocType>::construct(Alc, &(dummy->color));
		dummy->color = true;
		
		//  Возвращаем указатель на созданную вершину
		return dummy;
	}

	// Создание узла дерева 
	template <typename T>
	inline Node* make_node(T&& elem, Node * parent, Node* left, Node* right, bool color)
	{
		// Создаём точно так же, как и фиктивную вершину, только для поля данных нужно вызвать конструктор
		Node * new_node = Alc.allocate(1);
		
		//  Все поля, являющиеся указателями на узлы (left, right, parent) инициализируем и обнуляем
		std::allocator_traits<AllocType>::construct(Alc, &(new_node->parent));
		new_node->parent = parent;

		std::allocator_traits<AllocType>::construct(Alc, &(new_node->left));
		new_node->left = left;

		std::allocator_traits<AllocType>::construct(Alc, &(new_node->right));
		new_node->right = right;

		std::allocator_traits<AllocType>::construct(Alc, &(new_node->color));
		new_node->color = color;

		//  Конструируем поле данных
		std::allocator_traits<AllocType>::construct(Alc, &(new_node->data), std::forward<T>(elem));

		//  Возвращаем указатель на созданную вершину
		return new_node;
	}

	// Удаление фиктивной вершины
	inline void delete_dummy(Node* node) {
		std::allocator_traits<AllocType>::destroy(Alc, &(node->parent));
		std::allocator_traits<AllocType>::destroy(Alc, &(node->left));
		std::allocator_traits<AllocType>::destroy(Alc, &(node->right));
		std::allocator_traits<AllocType>::destroy(Alc, &(node->color));
		std::allocator_traits<AllocType>::deallocate(Alc, node, 1);
	}
	
	// Удаление вершины дерева
	inline void delete_node(Node * node) {
		//  Тут удаляем поле данных (вызывается деструктор), а остальное удаляем так же, как и фиктивную
		std::allocator_traits<AllocType>::destroy(Alc, &(node->data));
		delete_dummy(node);
	}

public:
	//  Класс итератора для дерева поиска
	class iterator 
	{
		friend class Binary_Search_Tree;
	protected:
		//  Указатель на узел дерева
		Node* data;
		Node* dummy;
		explicit iterator(Node* d, Node* dum) : data(d), dummy(dum) {	}
		
		//  Указатель на узел дерева
		inline Node* &_data()
		{
			return data;
		}

		//  Родительский узел дерева
		inline iterator Parent() const noexcept
		{
			return data->parent;
		}
		//  Левый дочерний узел (если отсутствует, то фиктивная вершина)
		inline iterator Left() const noexcept
		{
			return iterator(data->left, dummy);
		}
		//  Правый дочерний узел (если отсутствует, то фиктивная вершина)
		inline iterator Right() const noexcept
		{
			return iterator(data->right,dummy);
		}
		//  Является ли узел дерева левым у своего родителя
		inline bool IsLeft() const noexcept
		{
			//if (data->parent == dummy) return false;
			if (data->parent)
			{
				return data->parent->left == data;
			}
			else return false;
		}
		//  Является ли узел дерева правым у своего родителя
		inline bool IsRight() const noexcept
		{
			//if (data->parent == dummy) return false;
			if (data->parent)
			{
				return data->parent->right == data;
			}
			else return false;
		}
		//  Поиск «самого левого» элемента
		iterator GetMin() {
			Node* l = data;
			while (l->left != dummy) l = l->left;
			return iterator(l,dummy);
		}
		//  Поиск «самого правого» элемента
		iterator GetMax() {
			Node* r = data;
			while (r->right != dummy) r = r->right;
			return iterator(r,dummy);
		}
	public:
		//  Определяем стандартные типы в соответствии с требованиями стандарта к двунаправленным итераторам
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = Binary_Search_Tree::value_type;
		using difference_type = Binary_Search_Tree::difference_type;
		using pointer = Binary_Search_Tree::pointer;
		using reference = Binary_Search_Tree::reference;

		//  Значение в узле, на который указывает итератор
		inline T& operator*() const
		{
			return data->data;
		}

		//  Преинкремент - следующий элемент множества
		iterator & operator++()
		{
			if (data == dummy)
			{
				return *this;
			}

			if (data == dummy->right)
			{
				data = dummy;
				return *this;
			}

			if (data->right != dummy)
			{
				data = data->right;
				while (data->left != dummy)
					data = data->left;
			}
			else
			{
				Node* parent = data->parent;
				while (parent != dummy && data == parent->right)
				{
					data = parent;
					parent = parent->parent;
				}
				data = parent;
			}

			return *this;
		}
		//  Предекремент - переход на предыдущий элемент множества
		iterator& operator--()
		{
			if (data == dummy) {
				data = dummy->right;
				return *this;
			}

			if (data->left != dummy) {
				data = data->left;
				while (data->right != dummy) {
					data = data->right;
				}
			}
			else {
				Node* parent = data->parent;
				while (parent != dummy && data == parent->left) {
					data = parent;
					parent = parent->parent;
				}
				data = parent;
			}

			return *this;
		}
		//  Постинкремент
		iterator operator++(int) {
			auto tmp = *this;
			++(*this);
			return tmp;
		}
		//  Постдекремент
		iterator operator--(int) {
			auto tmp = *this;
			--(*this);
			return tmp;
		}

		friend bool operator != (const iterator & it_1, const iterator & it_2)
		{
			return !(it_1 == it_2);
		}

		friend bool operator == (const iterator & it_1, const iterator & it_2)
		{
			return it_1.data == it_2.data;
		}
		
		//  Эти операции не допускаются между прямыми и обратными итераторами
		const iterator & operator=(const reverse_iterator& it) = delete;
		bool operator==(const reverse_iterator& it) = delete;
		bool operator!=(const reverse_iterator& it) = delete;
		iterator(const reverse_iterator& it) = delete;
	};
	
	iterator begin() const noexcept { return iterator(dummy->left, dummy);	}
	iterator end() const noexcept { return iterator(dummy, dummy);  }

	reverse_iterator rbegin() const	noexcept { return reverse_iterator(iterator(dummy,dummy)); }
	reverse_iterator rend() const noexcept { return reverse_iterator(iterator(dummy->left,dummy)); }

	Binary_Search_Tree(Compare comparator = Compare(), AllocType alloc = AllocType())
		: cmp(comparator), Alc(alloc) 
	{
		dummy = make_dummy();
	}

	Binary_Search_Tree(std::initializer_list<T> il) : dummy(make_dummy())
	{
		for (const auto &x : il)
			insert(x);
	}

	AllocType get_allocator() const noexcept { return Alc; }
	key_compare key_comp() const noexcept { return cmp; }
	value_compare value_comp() const noexcept { return cmp; }

	inline bool empty() const noexcept { return tree_size == 0; }

private:
	template <class RandomIterator>
	void ordered_insert(RandomIterator first, RandomIterator last, iterator position) {
		if (first >= last) return;
		RandomIterator center = first + (last - first)/2 ;
		iterator new_pos = insert(position, *center);  //  итератор на вставленный элемент
		ordered_insert(first, center, position);
		ordered_insert(center + 1, last, ++position);
	}

public:
	template <class InputIterator>
	Binary_Search_Tree(InputIterator first, InputIterator last, Compare comparator = Compare(), AllocType alloc = AllocType()) : dummy(make_dummy()), cmp(comparator), Alc(alloc)
	{
			std::for_each(first, last, [this](const T& x) { insert(x); });
	}

	Binary_Search_Tree(const Binary_Search_Tree & tree) : dummy(make_dummy())
	{	//  Размер задаём
		tree_size = tree.tree_size;
		if (tree.empty()) return;

		dummy->parent = recur_copy_tree(tree.dummy->parent, tree.dummy);
		dummy->parent->parent = dummy;

		//  Осталось установить min и max
		dummy->left = iterator(dummy->parent,dummy).GetMin()._data();
		dummy->right = iterator(dummy->parent,dummy).GetMax()._data();
	}

	private:

    //  Рекурсивное копирование дерева
	Node* recur_copy_tree(Node * source, const Node * source_dummy) 
	{
		//  Сначала создаём дочерние поддеревья
		Node* left_sub_tree;
		if (source->left != source_dummy)
			left_sub_tree = recur_copy_tree(source->left, source_dummy);
		else
			left_sub_tree = dummy;

		Node* right_sub_tree;
		if (source->right != source_dummy)
			right_sub_tree = recur_copy_tree(source->right, source_dummy);
		else
			right_sub_tree = dummy;
		
		//  Теперь создаём собственный узел
		Node* current = make_node(source->data, nullptr, left_sub_tree, right_sub_tree,source->color);
		//  Устанавливаем родителей
		if (source->right != source_dummy)
			current->right->parent = current;
		if (source->left != source_dummy)
			current->left->parent = current;
		//  Ну и всё, можно возвращать
		return current;
	}

	public:
	const Binary_Search_Tree & operator=(const Binary_Search_Tree &tree)
	{
		if (this == &tree) return *this;
		
		Binary_Search_Tree tmp{tree};
		swap(tmp);
		
		return *this;
	}

	size_type size() const { return tree_size; }

	// Обмен содержимым двух контейнеров
	void swap(Binary_Search_Tree & other) noexcept {
		std::swap(dummy, other.dummy);
		std::swap(tree_size, other.tree_size);
	}

	void saveToFile(std::string fileName)
	{
		std::ofstream file{fileName};
		if (dummy->parent != dummy)
		{
			Node* root = dummy->parent;
			file << root->data;
			std::queue<Node*> q;
			if (root->left != dummy) q.push(root->left);
			if (root->right != dummy) q.push(root->right);
			while (!q.empty())
			{
				Node* cur = q.front();
				q.pop();
				file << " " << cur->data;
				if (cur->left != dummy) q.push(cur->left);
				if (cur->right != dummy) q.push(cur->right);
			}
		}
	}

	

	iterator insert(const T& value) {
		T temp = value; 
		return insert(std::move(temp)); 
	}
	//  Вставка элемента по значению. 
	iterator insert(T && value)
	{
		Node* head = dummy->parent;
		Node* new_node = make_node(std::move(value),nullptr,dummy,dummy,false);
		if (head == dummy)
		{
			new_node->parent = dummy;
			new_node->left = dummy;
			new_node->right = dummy;
			dummy->parent = new_node;
			dummy->left = new_node;
			dummy->right = new_node;
			tree_size++;
			return iterator(new_node,dummy);
		}
		else
		{
			Node* prev = head;
			while (head != dummy)
			{
				if (cmp(new_node->data, head->data))
				{
					prev = head;
					head = head->left;
				}
				else
				{
					prev = head;
					head = head->right;
				}
			}
			if (cmp(new_node->data, prev->data))
			{
				prev->left = new_node;
				new_node->parent = prev;
				if (dummy->left == prev) dummy->left = new_node;
			}
			else
			{
				prev->right = new_node;
				new_node->parent = prev;
				if (dummy->right == prev) dummy->right = new_node;
			}
			++tree_size;
			fixUp(new_node);
			return iterator(new_node, dummy);
		}
	}	

	iterator insert(const_iterator position, const value_type& x) {
		if (empty()) {
			Node* new_node = make_node(x, dummy, dummy, dummy,false);
			dummy->parent = new_node;
			dummy->left = new_node;
			dummy->right = new_node;
			++tree_size;
			return iterator(new_node, dummy);
		}
		if (position == begin()) {
			if (!cmp(x, *position)) {
				return insert(x);
			}
		}
		else {
			auto prev_it = --position;
			if (!(cmp(*prev_it, x) && cmp(x, *position))) {
				return insert(x);
			}
		}
		Node* parent = position.data->parent;
		Node* new_node = make_node(x, parent, dummy, dummy,false);
		if (position.IsLeft()) {
			parent->left = new_node;
		}
		else {
			parent->right = new_node;
		}
		if (dummy->left == parent && cmp(x, parent->data)) {
			dummy->left = new_node;
		}
		if (dummy->right == parent && !cmp(x, parent->data)) {
			dummy->right = new_node;
		}
		++tree_size;
		fixUp(new_node);
		return iterator(new_node, dummy);
	}

	//false - red, true - black
	void fixUp(Node* node)
	{
		int rotateCount = 0;
		while (!node->parent->color)
		{
			if (node->parent == node->parent->parent->left)
			{
				Node* y = node->parent->parent->right;
				if (!y->color)
				{
					node->parent->color = true;
					y->color = true;
					node->parent->parent->color = false;
					node = node->parent->parent;
				}
				else 
				{
					if (node == node->parent->right)
					{
						node = node->parent;
						rotateCount++;
						leftRotate(node);
					}
					node->parent->color = true;
					node->parent->parent->color = false;
					rotateCount++;
					rightRotate(node->parent->parent);
				}
			}
			else
			{
				Node* y = node->parent->parent->left;
				if (!y->color)
				{
					node->parent->color = true;
					y->color = true;
					node->parent->parent->color = false;
					node = node->parent->parent;
				}
				else 
				{
					if (node == node->parent->left)
					{
						node = node->parent;
						rotateCount++;
						rightRotate(node);
					}
					node->parent->color = true;
					node->parent->parent->color = false;
					rotateCount++;
					leftRotate(node->parent->parent);
				}
			}
			dummy->color = true;
			dummy->parent->color = true;
		}
		std::cout << "RotatesNum: " << rotateCount << std::endl;
	}

	void fixDelete(Node* x) 
	{
		int rotateCount = 0;
		while (x != dummy->parent && x->color) {
			if (x == x->parent->left) {
				Node* w = x->parent->right;
				if (!w->color) {
					w->color = true;
					x->parent->color = false;
					rotateCount++;
					leftRotate(x->parent);
					w = x->parent->right;
				}

				if (w->left->color && w->right->color) 
				{
					w->color = false;
					x = x->parent;
				}
				else 
				{
					if (w->right->color) 
					{
						w->left->color = true;
						w->color = false;
						rotateCount++;
						rightRotate(w);
						w = x->parent->right;
					}

					w->color = x->parent->color;
					x->parent->color = true;
					w->right->color = true;
					rotateCount++;
					leftRotate(x->parent);
					x = dummy->parent;
				}
			}
			else 
			{
				Node* w = x->parent->left;
				if (!w->color) {
					w->color = true;
					x->parent->color = false;
					rotateCount++;
					rightRotate(x->parent);
					w = x->parent->left;
				}

				if (w->right->color && w->left->color) 
				{
					w->color = false;
					x = x->parent;
				}
				else 
				{
					if (w->left->color) 
					{
						w->right->color = true;
						w->color = false;
						rotateCount++;
						leftRotate(w);
						w = x->parent->left;
					}

					w->color = x->parent->color;
					x->parent->color = true;
					w->left->color = true;
					rotateCount++;
					rightRotate(x->parent);
					x = dummy->parent;
				}
			}
		}
		x->color = true;
		std::cout << "RotatesNum: " << rotateCount << std::endl;
	}

	void rightRotate(Node* node)
	{
		Node* y = node->left;
		node->left = y->right;

		if (y->right != dummy)
		{
			y->right->parent = node;
		}

		y->parent = node->parent;

		if (node->parent == dummy)
		{
			dummy->parent = y;
		}
		else if (node == node->parent->right)
		{
			node->parent->right = y;
		}
		else
		{
			node->parent->left = y;
		}

		y->right = node;
		node->parent = y;
	}

	void leftRotate(Node* node)
	{
		Node* y = node->right;
		node->right = y->left;

		if (y->left != dummy)
		{
			y->left->parent = node;
		}
		y->parent = node->parent;
		if (node->parent == dummy)
		{
			dummy->parent = y;
		}
		else if (node == node->parent->left)
		{
			node->parent->left = y;
		}
		else
		{
			node->parent->right = y;
		}
		y->left = node;
		node->parent = y;
	}

	void printTreeSideways(Node* node, const std::string& prefix = "", bool isLeft = false) {
		if (node == dummy) {
			std::cout << prefix << (isLeft ? "+-- " : "+-- ") << "dummy" << std::endl;
			return;
		}

		std::cout << prefix;
		std::cout << (isLeft ? "+-- " : "+-- ");
		std::cout << node->data << (node->color ? " (b)" : " (r)") << std::endl;

		printTreeSideways(node->left, prefix + (isLeft ? "|   " : "    "), true);
		printTreeSideways(node->right, prefix + (isLeft ? "|   " : "    "), false);
	}

	void printTree() {
		if (dummy->parent == dummy) {
			std::cout << "Tree is empty" << std::endl;
			return;
		}
		printTreeSideways(dummy->parent);
	}

	//  Не самый лучший вариант.
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		while (first != last) insert(*first++);
	}

	iterator find(const value_type& value) const {
		
		iterator current = iterator(dummy->parent,dummy);
		while (current != end())
		{
			if (cmp(value,*current))
				current = current.Left();
			else
			{
				if (!cmp(*current,value)) return current;
				current = current.Right();
			}
		}
		return current;
	}

	iterator lower_bound(const value_type& key) {
		Node* current = dummy->parent;
		Node* result = dummy; 

		while (current != dummy) {
			if (!cmp(current->data, key)) {
				result = current;
				current = current->left;
			}
			else {
				current = current->right;
			}
		}

		return iterator(result, dummy);
	}

	const_iterator lower_bound(const value_type& key) const {
		return const_iterator(const_cast<Binary_Search_Tree *>(this)->lower_bound(key));
	}

	iterator upper_bound(const value_type& key) {
		Node* current = dummy->parent;
		Node* result = dummy; 

		while (current != dummy) {
			if (cmp(key, current->data)) {
				result = current;
				current = current->left;
			}
			else {
				current = current->right;
			}
		}

		return iterator(result, dummy);
	}

	const_iterator upper_bound(const value_type& key) const {
		return const_iterator(const_cast<Binary_Search_Tree*>(this)->upper_bound(key));
	}

	size_type count(const value_type& key) const {
		return find(key) != end() ? 1 : 0;
	}

	std::pair<const_iterator, const_iterator> equal_range(const value_type& key) const {
		return { lower_bound(key), upper_bound(key) };
	}

protected:
	//  Удаление листа дерева. Возвращает количество удалённых элементов
	size_type delete_leaf(iterator leaf) {
		#ifdef _DEBUG
		//if (leaf.isNil()) return 0; // Стоит потом убрать, так как уже проверяем, что итератор валидный в erase
		#endif
		Node* p = leaf.data->parent;
		if (leaf.IsLeft())
		{
			p->left = dummy;
		}
		else p->right = dummy;
		delete_node(leaf.data);
		return 1;
	}

	//  Меняет местами текущий узел и максимальный в левом поддеревею Возвращает тот же итератор, но теперь он без правого поддерева
	iterator replace_with_max_left(iterator node)
	{
		//  node имеет обоих дочерних - левое и правое поддерево, т.е. из особых вершин может быть только корнем

		//  Находим максимальный элемент слева. У него нет правого дочернего, и он не может быть корнем или самым правым
		iterator left_max = node.Left().GetMax();
		//if (left_max == dummy->right || left_max == dummy->parent) return node;
		Node* l = left_max.data;
		Node* n = node.data;
		if (node.IsLeft())
		{
			n->parent->left = l;
		}
		else if (node.IsRight())
		{
			n->parent->right = l;
		}

		l->left = n->left;
		l->right = n->right;
		if (n->left != dummy) n->left->parent = l;
		if (n->right != dummy) n->right->parent = l;
		n->left = dummy;
		n->right = dummy;
		Node* lp = l->parent;
		lp->right = n;
		l->parent = n->parent;
		n->parent = lp;

		return node;
	} 	

public:
	//  Удаление элемента, заданного итератором. Возвращает количество удалённых элементов (для set - 0/1)
	iterator erase(iterator elem) 
	{
		if (elem == end()) {
			return end();
		}

		Node* node = elem.data;
		Node* parent = node->parent;
		bool needFix = node->color;
		iterator next = ++iterator(node, dummy);

		if (node->left != dummy && node->right != dummy) {
			iterator left_max = iterator(node->left, dummy).GetMax();
			Node* replacement = left_max.data;
			bool replacement_original_color = replacement->color;

			if (parent != dummy) {
				if (node == parent->left) {
					parent->left = replacement;
				}
				else {
					parent->right = replacement;
				}
			}
			else {
				dummy->parent = replacement;
			}

			replacement->parent = parent;
			replacement->color = node->color;

			if (replacement != node->left) {
				replacement->parent->right = replacement->left;
				if (replacement->left != dummy) {
					replacement->left->parent = replacement->parent;
				}
				replacement->left = node->left;
				node->left->parent = replacement;
			}

			replacement->right = node->right;
			node->right->parent = replacement;

			if (dummy->left == node) {
				dummy->left = iterator(replacement, dummy).GetMin().data;
			}
			if (dummy->right == node) {
				dummy->right = iterator(replacement, dummy).GetMax().data;
			}

			needFix = replacement_original_color;
		}
		else {
			Node* child = (node->left != dummy) ? node->left : node->right;

			if (parent != dummy) {
				if (node == parent->left) {
					parent->left = child;
				}
				else {
					parent->right = child;
				}
			}
			else {
				dummy->parent = child;
			}

			if (child != dummy) {
				child->parent = parent;
			}

			if (dummy->left == node) {
				dummy->left = (child != dummy) ? iterator(child, dummy).GetMin().data : parent;
			}
			if (dummy->right == node) {
				dummy->right = (child != dummy) ? iterator(child, dummy).GetMax().data : parent;
			}
		}

		delete_node(node);
		--tree_size;

		if (needFix && next != end()) {
			fixDelete(next.data);
		}

		return next;
	}
	
	size_type erase(const value_type& elem) {
		iterator it = find(elem);
		size_type res = 0;

		while (it != end())
		{
			erase(it);
			++res;
			it = find(elem);
		}
		return res;
	}
	
	//  Проверить!!!
	iterator erase(const_iterator first, const_iterator last) {
		while (first != last)
			first = erase(first);
		return last;
	}

	//Если передавать по ссылкам,все хорошо. Конструктор копий принескольких деревьях ломается.
	friend bool operator== (const Binary_Search_Tree<T> &tree_1, const Binary_Search_Tree<T> & tree_2)
	{
		auto i = tree_1.begin(), ii = tree_2.begin();
		for (; (i != tree_1.end()) && (ii != tree_2.end()); ++i, ++ii)
		{
			if (*i != *ii)
				return false;
		}
		return i == tree_1.end() && ii == tree_2.end();
	}

	//  Очистка дерева (без удаления фиктивной вершины)
	void clear() {
		Free_nodes(dummy->parent);
		tree_size = 0;
		dummy->parent = dummy->left = dummy->right = dummy;
	}

private:
	//  Рекурсивное удаление узлов дерева, не включая фиктивную вершину
	void Free_nodes(Node* node)
	{ 
		if (node != dummy)
		{
			Free_nodes(node->left);
			Free_nodes(node->right);
			delete_node(node);
		}
	}
	
public:
	~Binary_Search_Tree()
	{
		clear(); // рекурсивный деструктор
		delete_dummy(dummy);
	}
};

template <class Key, class Compare, class Allocator>
void swap(Binary_Search_Tree<Key, Compare, Allocator>& x, Binary_Search_Tree<Key, Compare, Allocator>& y) noexcept(noexcept(x.swap(y))) {
	x.swap(y);
};


template <class Key, class Compare, class Allocator>
bool operator==(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	typename Binary_Search_Tree<Key, Compare, Allocator>::const_iterator it1{ x.begin() }, it2{ y.begin() };
	while (it1 != x.end() && it2 != y.end() && *it1 == *it2) {
		++it1; ++it2;
	}

	return it1 == x.end() && it2 == y.end();
}

template <class Key, class Compare, class Allocator>
bool operator<(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	
	typename Binary_Search_Tree<Key, Compare, Allocator>::const_iterator it1{ x.begin() }, it2{ y.begin() };
	while (it1 != x.end() && it2 != y.end() && *it1 == *it2) {
		++it1; ++it2;
	}

	if (it1 == x.end())
		return it2 != y.end();
	
	return it2 != y.end() && *it1 < *it2;
}

template <class Key, class Compare, class Allocator>
bool operator!=(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return !(x == y);
}

template <class Key, class Compare, class Allocator>
bool operator>(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return y < x;
}

template <class Key, class Compare, class Allocator>
bool operator>=(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return !(x<y);
}

template <class Key, class Compare, class Allocator>
bool operator<=(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return   !(y < x);
}

Binary_Search_Tree<std::string> uploadFromFile(std::string fileName)
{
	std::ifstream file;
	Binary_Search_Tree<std::string> res;
	file.open(fileName);
	if (!file.fail())
	{
		std::string word;
		while (file >> word)
		{
			res.insert(word);
		}
	}
	return res;
}


