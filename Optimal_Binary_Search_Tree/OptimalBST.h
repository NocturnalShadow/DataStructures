#pragma once

#include <climits>
#include <numeric>
#include <vector>
#include <functional>


template<typename K, typename T>
class OptimalBST
{
public:
	class Node;

private:
	Node* root = nullptr;

public:
	OptimalBST() = default;
	OptimalBST(const std::vector<std::pair<K, T>>& elements, const std::vector<unsigned int>& frequency)
		: root{ Init(elements, frequency) }
	{
	}
	~OptimalBST() {
		delete root;
	}

public:
	OptimalBST(const OptimalBST& tree) = delete;
	OptimalBST& operator=(const OptimalBST& tree) = delete;

public:
	Node* Find(const K& key);
	Node* Root() {
		return root;
	}

	unsigned int Cost(Node* node);

	template<typename Func>
	void InOrder(Node* _root, Func func);

private:
	Node* Init(const std::vector<std::pair<K,T>>& elements, const std::vector<unsigned int>& frequency);

	void SetParent(Node* child, Node* parent);
	void KeepParent(Node* node);
	void Optimize(Node* node, int costInc = 1);

	void RotateLeft(Node* node);
	void RotateRight(Node* node);

public:
	Node* Find(Node* node, const K& key);
};

template<typename K, typename T>
class OptimalBST<K,T>::Node
{
	friend class OptimalBST<K,T>;
private:
	K key;
	T* data = nullptr;

	Node* parent 	= nullptr;
	Node* left 		= nullptr;
	Node* right 	= nullptr;

	unsigned int cost = 0;
	unsigned int freq = 0;

public:
	Node(const K& _key, const T& _data, unsigned int _freq = 0)
		: key{ _key }, data{ new T(_data) }, freq{ _freq }
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

	unsigned int visitCount() const {
		return cost 
		- (left ? left->cost : 0) 
		- (right ? right->cost : 0);
	}

	const K& Key() const {
		return key;
	}
	T& Data() {
		return *data;
	}
};

template<typename K, typename T>
using OptimalNode = typename OptimalBST<K,T>::Node;

template<typename K, typename T>
inline void OptimalBST<K,T>::SetParent(Node* child, Node* parent)
{
	if (child != nullptr) {
		child->parent = parent;
	}
}

template<typename K, typename T>
inline void OptimalBST<K,T>::KeepParent(Node* node)
{
	SetParent(node->left, node);
	SetParent(node->right, node);
}


template<typename K, typename T>
inline void OptimalBST<K,T>::RotateLeft(Node* node)
{
	Node* pivot = node->right;
	pivot->parent = node->parent; 
	if (!node->isRoot()) 
	{
		if (node->isLeftChild()) {
			node->parent->left = pivot;
		} else {
			node->parent->right = pivot;
		}
	} else {
		root = pivot;
	}

	node->right = pivot->left;
	if (pivot->hasLeftChild()) {
		pivot->left->parent = node;
	}

	node->parent = pivot;
	pivot->left = node;
}

template<typename K, typename T>
inline void OptimalBST<K,T>::RotateRight(Node* node)
{
	Node* pivot = node->left;
	pivot->parent = node->parent; 
	if (!node->isRoot())
	{
		if (node->isLeftChild()) {
			node->parent->left = pivot;
		} else {
			node->parent->right = pivot;
		}
	} else {
		root = pivot;
	}

	node->left = pivot->right;
	if (pivot->hasRightChild()) {
		pivot->right->parent = node;
	}

	node->parent = pivot;
	pivot->right = node;
}

template<typename K, typename T>
inline void OptimalBST<K,T>::Optimize(Node* node, int costInc)
{
	if(!node->isRoot()) 
	{
		Node* parent = node->parent;
		node->parent->cost += costInc + 1;
		if(node->isLeftChild()) {
			unsigned int altParentVisitCount = 			
				parent->freq +
				(node->right ? node->right->visitCount() : 0) +
				(parent->right ? parent->right->visitCount() : 0);
			unsigned int altParentCost =
				altParentVisitCount +
				(node->right ? node->right->cost : 0) +
				(parent->right ? parent->right->cost : 0);
			unsigned int altCost =
				parent->visitCount() +
				(node->left ? node->left->cost : 0) +
				altParentCost; 
			if(parent->cost > altCost) {
				RotateRight(node->parent);
				node->cost = altCost;
				parent->cost = altParentCost;
				parent = node;
			} else {
				costInc++;
			}
		} else {	// node is right child
			unsigned int altParentVisitCount =		
				parent->freq +
				(node->left ? node->left->visitCount() : 0) +
				(parent->left ? parent->left->visitCount() : 0);
			unsigned int altParentCost =
				altParentVisitCount +
				(node->left ? node->left->cost : 0) +
				(parent->left ? parent->left->cost : 0);
			unsigned int altCost =
				parent->visitCount() +
				(node->right ? node->right->cost : 0) +
				altParentCost; 
			if(parent->cost > altCost) {
				RotateLeft(node->parent);
				node->cost = altCost;
				parent->cost = altParentCost;
				parent = node;
			} else {
				costInc++;
			}
		}
		if (parent) {
			Optimize(parent, costInc);
		}
	}
}

template<typename K, typename T>
template<typename Func>
inline void OptimalBST<K,T>::InOrder(Node* _root, Func func)
{
	if (_root != nullptr)
	{
		InOrder(_root->left, func);
		func(_root);
		InOrder(_root->right, func);
	}
}

template<typename K, typename T>
OptimalNode<K,T>* OptimalBST<K, T>::Find(Node* node, const K& key)
{
	if (node != nullptr) {
		if (node->key < key) {
			return Find(node->right, key);
		}
		else if (node->key > key) {
			return Find(node->left, key);
		}
		else {
			node->freq++;
			node->cost++;
			Optimize(node);
			return node;
		}
	}
	return nullptr;
}


template<typename K, typename T>
OptimalNode<K,T>* OptimalBST<K,T>::Init(const std::vector<std::pair<K, T>>& elements, const std::vector<unsigned int>& frequency)
{
	using namespace std;

	struct CachedData 
	{
		unsigned int root = UINT_MAX;
		unsigned int cost = UINT_MAX;

		CachedData() = default;
		CachedData(unsigned int _cost, unsigned int _root)
			: cost{ _cost }, root{ _root }
		{
		}
	};

	if (elements.size() != frequency.size()) {
		throw std::runtime_error("OptimalBST construction error: elements and frequencies sizes mismatch.");
	}

	const size_t size = elements.size();

	if (size == 0) {
		return nullptr;
	}

	vector<Node*> nodes(size);
	vector<vector<CachedData>> cache(size);
	for (size_t i = 0; i < size; i++)
	{
		nodes[i] = new Node(elements[i].first, elements[i].second, frequency[i]);
		cache[i].resize(size);
		cache[i][i] = { frequency[i], (unsigned int) i };
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
					(root > i ? cache[i][root - 1].cost : 0) +
					(root < j ? cache[root + 1][j].cost : 0)
				);

				if (cost < cache[i][j].cost) {
					cache[i][j] = { cost, (unsigned int) root };
				}
			}
		}
	}
	function<Node*(int, int)> buildTree = [&](auto leftBound, auto rightBound) {
		Node* root = nullptr;
		if (leftBound <= rightBound)
		{
			auto rootIndex = cache[leftBound][rightBound].root;
			root = nodes[rootIndex];
			root->cost = cache[leftBound][rightBound].cost;
			root->left	= buildTree(leftBound, rootIndex - 1);
			root->right = buildTree(rootIndex + 1, rightBound);
			KeepParent(root);
		}
		return root;
	};

	return buildTree(0, size - 1);
}

template<typename K, typename T>
inline OptimalNode<K,T>* OptimalBST<K,T>::Find(const K& key)
{
	Node* result = Find(root, key);
	if (result && result->key != key) {
		result = nullptr;
	}
	return result;
}

template<typename K, typename T>
unsigned int OptimalBST<K,T>::Cost(Node* node)
{
	if (node != nullptr) {
		return node->cost;
	} else {
		return 0;
	}
}

