#pragma once

#include <utility>

template<typename K, typename T>
class SplayBST
{
public:
	class Node;

private:
	Node* m_root	= nullptr;

public:
	SplayBST()
		{	}
	~SplayBST()
		{ delete m_root; }

public:
	SplayBST(const SplayBST& tree) = delete;
	SplayBST& operator=(const SplayBST& tree) = delete;

public:
	void Insert(const K& key, const T& data);
	void Erase(const K& key)
		{ Erase(m_root, key); }

	Node* Find(const K& key);

	Node* Root()
		{ return m_root; }

	template<typename Func>
	void InOrder(Node* _root, Func func);

private:
	void SetParent(Node* child, Node* parent);
	void KeepParent(Node* node);
	void Rotate(Node* parent, Node* child);
	Node* Splay(Node* node);
	std::pair<Node*, Node*> Split(Node* root, const K& key);

public:
	Node* Find(Node* node, const K& key);
	Node* Merge(Node* left, Node* right)
	{
		if (right == nullptr) {
			return left;
		}
		if (left == nullptr) {
			return right;
		}
		right = Find(right, left->key);
		right->left = left;
		left->parent = right;
		return right;
	}

	void Insert(Node* root, const K& key, const T& data);
	void Erase(Node* root, const K& key);
};

template<typename K, typename T>
class SplayBST<K, T>::Node
{
	friend class SplayBST<K, T>;
private:
	K key;
	T* data = nullptr;

	Node* parent = nullptr;
	Node* left = nullptr;
	Node* right = nullptr;

public:
	Node(const K& _key, const T& _data)
		: key{ _key }, data{ new T(_data) }
	{
	}
	Node(const K& _key, const T& _data, Node* _parent, Node* _left, Node* _right)
		: key{ _key }, data{ new T(_data) }, parent{ _parent }, left{ _left }, right{ _right }
	{
	}
	~Node()
	{
		if (parent)
		{
			if (this->isLeftChild()) {
				parent->left = nullptr;
			}
			else if (this->isRightChild()) {
				parent->right = nullptr;
			}
		}
		delete left;
		delete right;
		delete data;
	}

public:
	Node(const Node& node) = delete;
	Node& operator=(const Node& node) = delete;

public:
	bool isRoot() const
	{
		return parent == nullptr;
	}
	bool isLeaf() const
	{
		return !left && !right;
	}
	bool isLeftChild() const
	{
		return parent->left == this;
	}
	bool isRightChild() const
	{
		return parent->right == this;
	}
	bool hasRightChild() const
	{
		return right != nullptr;
	}
	bool hasLeftChild() const
	{
		return left != nullptr;
	}

	bool isGreaterThen(Node* node)
	{
		return this->key > node->key;
	}
	bool isLessThen(Node* node)
	{
		return this->key < node->key;
	}

	const K& Key() const
	{
		return key;
	}
	T& Data()
	{
		return *data;
	}

public:
	
};


template<typename K, typename T>
using SplayNode = typename SplayBST<K,T>::Node;

template<typename K, typename T>
template<typename Func>
inline void SplayBST<K,T>::InOrder(Node* _root, Func func)
{
	if (_root != nullptr)
	{
		InOrder(_root->left, func);
		func(_root);
		InOrder(_root->right, func);
	}
}

template<typename K, typename T>
inline void SplayBST<K,T>::Insert(const K& key, const T& data)
{
	Insert(m_root, key, data);
}

template<typename K, typename T>
inline SplayNode<K,T>* SplayBST<K,T>::Find(const K& key)
{
	Node* result = Find(m_root, key);
	if (result && result->key != key) {
		result = nullptr;
	}
	return result;
}

template<typename K, typename T>
inline void SplayBST<K,T>::SetParent(Node* child, Node* parent)
{
	if (child != nullptr) {
		child->parent = parent;
	}
}

template<typename K, typename T>
inline void SplayBST<K,T>::KeepParent(Node* node)
{
	SetParent(node->left, node);
	SetParent(node->right, node);
}

template<typename K, typename T>
inline void SplayBST<K,T>::Rotate(Node* parent, Node* child)
{
	Node* grand_parent = parent->parent;

	if (grand_parent != nullptr)
	{
		if (parent->isLeftChild()) {
			grand_parent->left = child;
		}
		else {
			grand_parent->right = child;
		}
	}
	else {
		m_root = child;
	}

	if (child->isLeftChild())
	{
		parent->left = child->right;
		child->right = parent;
	}
	else
	{
		parent->right = child->left;
		child->left = parent;
	}

	KeepParent(child);
	KeepParent(parent);

	child->parent = grand_parent;
}

template<typename K, typename T>
inline SplayNode<K,T>* SplayBST<K,T>::Splay(Node* node)
{
	if (node->parent == nullptr) {
		return node;
	}
	Node* parent = node->parent;
	Node* grand_parent = parent->parent;

	if (grand_parent == nullptr)
	{
		Rotate(parent, node);
		return node;
	}
	else
	{
		bool zigzig = (parent->isLeftChild() == node->isLeftChild());
		if (zigzig)
		{
			Rotate(grand_parent, parent);
			Rotate(parent, node);
		}
		else	// zig-zag
		{
			Rotate(parent, node);
			Rotate(grand_parent, node);
		}
	}
	return Splay(node);
}

template<typename K, typename T>
inline std::pair<SplayNode<K,T>*, SplayNode<K,T>*> SplayBST<K,T>::Split(Node* root, const K& key)
{
	if (root == nullptr) {
		return{ nullptr, nullptr };
	}
	root = Find(root, key);
	if (root->key == key)
	{
		SetParent(root->left, nullptr);
		SetParent(root->left, nullptr);
		return{ root->left, root->right };
	}
	if (root->key < key)
	{
		Node* right = root->right;
		root->right = nullptr;
		SetParent(right, nullptr);
		return{ root, right };
	}
	else
	{
		Node* left = root->left;
		root->left = nullptr;
		SetParent(left, nullptr);
		return{ left, root };
	}
}

template<typename K, typename T>
inline SplayNode<K,T>* SplayBST<K, T>::Find(Node* node, const K& key)
{
	if (node == nullptr) {
		return nullptr;
	}
	if (key == node->key) {
		return Splay(node);
	}
	if (key < node->key && node->hasLeftChild()) {
		return Find(node->left, key);
	}
	if (key > node->key && node->hasRightChild()) {
		return Find(node->right, key);
	}
	return Splay(node);
}

template<typename K, typename T>
inline void SplayBST<K,T>::Insert(Node* root, const K& key, const T& data)
{
	auto subtree = Split(root, key);
	root = new Node(key, data, nullptr, subtree.first, subtree.second);
	KeepParent(root);
	if (root->isRoot()) {
		m_root = root;
	}
}

template<typename K, typename T>
inline void SplayBST<K,T>::Erase(Node* root, const K& key)
{
	root = Find(root, key);
	if (!root || root->Key() != key) {
		throw std::runtime_error("Erasure failed: key does not exist.");
	}
	SetParent(root->left, nullptr);
	SetParent(root->right, nullptr);
	Node* result = Merge(root->left, root->right);
	if (root == m_root) {
		m_root = result;
	}
	root->left = nullptr;
	root->right = nullptr;
	delete root;
}

// InOrder traversal example
//tree.InOrder(tree.Root(), [](auto node) {
//	std::cout << node->Key() << " ";
//});