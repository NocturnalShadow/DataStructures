#pragma once

#include <climits>
#include <numeric>
#include <vector>


template<typename K, typename T>
class OptimalBSTree
{
public:
	class Node;

private:
	Node* m_root = nullptr;

public:
	OptimalBSTree(const std::vector<std::pair<K, T>>& elements, const std::vector<unsigned int>& frequency)
		: m_root{ Init(elements, frequency) }
	{
	}
	~OptimalBSTree() {
		delete m_root;
	}

public:
	OptimalBSTree(const OptimalBSTree& tree) = delete;
	OptimalBSTree& operator=(const OptimalBSTree& tree) = delete;

public:
	Node* Find(const K& key);
	Node* Root() {
		return m_root;
	}

	unsigned int Cost(Node* node);

	template<typename Func>
	void InOrder(Node* _root, Func func);

private:
	Node* Init(const std::vector<std::pair<K, T>>& elements, const std::vector<unsigned int>& frequency);
	void SetParent(Node* child, Node* parent);
	void KeepParent(Node* node);

public:
	Node* Find(Node* node, const K& key);
};

template<typename K, typename T>
class OptimalBSTree<K, T>::Node
{
	friend class OptimalBSTree<K, T>;
private:
	K key;
	T* data = nullptr;

	Node* parent = nullptr;
	Node* left = nullptr;
	Node* right = nullptr;

	unsigned int cost = 0;

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
	bool isRoot() const {
		return parent == nullptr;
	}
	bool isLeaf() const {
		return !left && !right;
	}
	bool isLeftChild() const {
		return parent->left == this;
	}
	bool isRightChild() const {
		return parent->right == this;
	}
	bool hasRightChild() const {
		return right != nullptr;
	}
	bool hasLeftChild() const {
		return left != nullptr;
	}

	bool isGreaterThen(Node* node) {
		return this->key > node->key;
	}
	bool isLessThen(Node* node) {
		return this->key < node->key;
	}

	const K& Key() const {
		return key;
	}
	T& Data() {
		return *data;
	}
};

template<typename K, typename T>
using OptimalNode = typename OptimalBSTree<K, T>::Node*;

template<typename K, typename T>
inline void OptimalBSTree<K, T>::SetParent(Node* child, Node* parent)
{
	if (child != nullptr) {
		child->parent = parent;
	}
}

template<typename K, typename T>
inline void OptimalBSTree<K, T>::KeepParent(Node* node)
{
	SetParent(node->left, node);
	SetParent(node->right, node);
}

template<typename K, typename T>
template<typename Func>
inline void OptimalBSTree<K, T>::InOrder(Node* _root, Func func)
{
	if (_root != nullptr)
	{
		InOrder(_root->left, func);
		func(_root);
		InOrder(_root->right, func);
	}
}

template<typename K, typename T>
OptimalNode<K, T> OptimalBSTree<K, T>::Find(Node* node, const K& key)
{
	if (node != nullptr) {
		if (node->key < key) {
			return Find(node->right, key);
		}
		else if (node->key > key) {
			return Find(node->left, key);
		}
		else {
			return node;
		}
	}
	return nullptr;
}

template<typename K, typename T>
OptimalNode<K, T> OptimalBSTree<K, T>::Init(const std::vector<std::pair<K, T>>& elements, const std::vector<unsigned int>& frequency)
{
	using namespace std;

	if (elements.size() != frequency.size()) {
		throw std::runtime_error("OptimalBST construction error: elements and frequencies sizes mismatch.");
	}

	const size_t size = elements.size();

	if (size == 0) {
		return nullptr;
	}

	vector<Node*> nodes(size);
	vector<vector<pair<unsigned int, Node*>>> cache(size);
	for (size_t i = 0; i < size; i++)
	{
		nodes[i] = new Node(elements[i].first, elements[i].second);
		cache[i].resize(size);
		cache[i][i] = { frequency[i], nodes[i] };
		for (size_t j = i + 1; j < size; ++j) {
			cache[i][j] = { UINT_MAX, nullptr };
		}
	}

	// Now we need to consider chains of length 2, 3, ... .
	for (size_t length = 1; length < size; length++)
	{
		for (size_t i = 0; i < size - length; i++)
		{
			size_t j = i + length;								// Get column number j from row number i and chain length 
			for (size_t root = i; root <= j; root++)			// Try making all keys in interval keys[i..j] as root
			{
				// cost when nodes[root] becomes root of this subtree
				unsigned int cost = accumulate(
					frequency.begin() + i, frequency.begin() + j + 1,
					(root > i) ? cache[i][root - 1].first : 0 +
					(root < j) ? cache[root + 1][j].first : 0
				);

				if (cost < cache[i][j].first) {
					nodes[root]->left = (root > i) ? cache[i][root - 1].second : nullptr;
					nodes[root]->right = (root < j) ? cache[root + 1][j].second : nullptr;
					cache[i][j] = { cost, nodes[root] };
					nodes[root]->cost = cost;
				}
			}
			KeepParent(cache[i][j].second);
		}
	}

	return cache[0][size - 1].second;
}

template<typename K, typename T>
inline OptimalNode<K, T> OptimalBSTree<K, T>::Find(const K& key)
{
	Node* result = Find(m_root, key);
	if (result && result->key != key) {
		result = nullptr;
	}
	return result;
}

template<typename K, typename T>
unsigned int OptimalBSTree<K, T>::Cost(Node* node)
{
	if (node != nullptr) {
		return node->cost;
	} else {
		return 0;
	}
}

