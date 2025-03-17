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
		//  Хранимый в узле ключ
		T data;
		Node(T value = T(), Node* p = nullptr, Node* l = nullptr, Node* r = nullptr) : parent(p), data(value), left(l), right(r) {}
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
	// Указательно на фиктивную вершину
	Node* dummy;

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
		
		//  Возвращаем указатель на созданную вершину
		return dummy;
	}

	// Создание узла дерева 
	template <typename T>
	inline Node* make_node(T&& elem, Node * parent, Node* left, Node* right)
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
			Node* l = data->left;
			if (l)
			{
				while (l->left != dummy) l = l->left;
			}
			return iterator(l,dummy);
		}
		//  Поиск «самого правого» элемента
		iterator GetMax() {
			Node* r = data->right;
			if (r)
			{
				while (r->right != dummy) r = r->right;
			}
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
	reverse_iterator rend() const noexcept { return reverse_iterator(iterator(dummy->left, dummy)); }

	Binary_Search_Tree(Compare comparator = Compare(), AllocType alloc = AllocType())
		: dummy(make_dummy()), cmp(comparator), Alc(alloc) {}

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
		using IteratorCategory = typename std::iterator_traits<InputIterator>::iterator_category;

		//if constexpr (std::is_same_v<IteratorCategory, std::random_access_iterator_tag>) {
		//	// Если итератор произвольного доступа, используем ordered_insert
		//	ordered_insert(first, last, end());
		//}
		//else 
		if constexpr (std::is_same_v<IteratorCategory, std::bidirectional_iterator_tag>) {
			// Если итератор двунаправленный (включая reverse_iterator), преобразуем его
			auto base_first = first.base();
			auto base_last = last.base();
			while (base_first != base_last) {
				insert(*base_first);
				++base_first;
			}
		}
		else 
		{
			// Для остальных типов итераторов используем последовательную вставку
			std::for_each(first, last, [this](const T& x) { insert(x); });
		}
	}

	Binary_Search_Tree(const Binary_Search_Tree & tree) : dummy(make_dummy())
	{	//  Размер задаём
		tree_size = tree.tree_size;
		if (tree.empty()) return;

		dummy->parent = recur_copy_tree(tree.dummy->parent, tree.dummy);
		dummy->parent->parent = dummy;

		//  Осталось установить min и max
		Node* r = dummy->parent->right;
		Node* l = dummy->parent->left;
		while (r->right != dummy) r = r->right;
		while (l->left != dummy) l = l->left;
		
		dummy->left = l;
		dummy->right = r;
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
		Node* current = make_node(source->data, nullptr, left_sub_tree, right_sub_tree);
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

		//  Обмен размера множеств
		std::swap(tree_size, other.tree_size);
	}

	std::pair<iterator, bool> insert(const T& value) {
		T temp = value; // Создаем временную копию
		return insert(std::move(temp)); // Перемещаем временную копию
	}
	//  Вставка элемента по значению. 
	std::pair<iterator, bool> insert(T && value)
	{
		Node* head = dummy->parent;
		Node* new_node = make_node(std::move(value),nullptr,dummy,dummy);
		if (head == dummy)
		{
			new_node->parent = dummy;
			new_node->left = dummy;
			new_node->right = dummy;
			dummy->parent = new_node;
			dummy->left = new_node;
			dummy->right = new_node;
			tree_size++;
			return std::make_pair(iterator(new_node,dummy), true);
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
					if (!cmp(head->data, new_node->data)) return std::make_pair(iterator(head,dummy), false);
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
			return std::make_pair(iterator(new_node,dummy), true);
		}
	}	

	iterator insert(const_iterator position, const value_type& x) {
		if (empty()) {
			Node* new_node = make_node(x, dummy, dummy, dummy);
			dummy->parent = new_node;
			dummy->left = new_node;
			dummy->right = new_node;
			++tree_size;
			return iterator(new_node, dummy);
		}
		if (position == begin()) {
			if (!cmp(x, *position)) {
				return insert(x).first;
			}
		}
		else {
			auto prev_it = --position;
			if (!(cmp(*prev_it, x) && cmp(x, *position))) {
				return insert(x).first;
			}
		}
		Node* parent = position.data->parent;
		Node* new_node = make_node(x, parent, dummy, dummy);
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
		return iterator(new_node, dummy);
	}

	//  Не самый лучший вариант.
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		while (first != last) insert(*first++);
	}

	iterator find(const value_type& value) const {
		
		iterator current = iterator(dummy->parent,dummy);
		while (current.data != dummy)
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
	iterator erase(iterator elem) {
		if (elem == iterator(dummy, dummy)) {
			return end();
		}

		Node* node = elem.data;
		Node* parent = node->parent;
		auto temp = ++elem;

		if (node->left == dummy && node->right == dummy) {
			if (parent->left == node) {
				parent->left = dummy;
			}
			else {
				parent->right = dummy;
			}
			if (dummy->left == node) dummy->left = parent;
			if (dummy->right == node) dummy->right = parent;
			if (dummy->parent == node) dummy->parent = dummy;
		}
		else if (node->left == dummy) {
			Node* right_child = node->right;
			if (parent->left == node) {
				parent->left = right_child;
			}
			else {
				parent->right = right_child;
			}
			right_child->parent = parent;
			if (dummy->left == node) dummy->left = right_child;
			if (dummy->parent == node) dummy->parent = right_child;
		}
		else if (node->right == dummy) {
			Node* left_child = node->left;
			if (parent->left == node) {
				parent->left = left_child;
			}
			else {
				parent->right = left_child;
			}
			left_child->parent = parent;
			if (dummy->right == node) dummy->right = left_child;
			if (dummy->parent == node) dummy->parent = left_child;
		}
		else {
			elem = replace_with_max_left(elem);
			node = elem.data;
			parent = node->parent;

			if (parent->left == node) {
				parent->left = dummy;
			}
			else {
				parent->right = dummy;
			}
		}

		delete_node(node);
		--tree_size;

		return temp;
	}
	
	size_type erase(const value_type& elem) {
		iterator it = find(elem);
		if (it != end()) {
			erase(it);
			return 1;
		}
		return 0;
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



