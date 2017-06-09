#pragma once

#include <memory>
#include <functional>

template<typename K, typename T>
class PersistentBST
{
public:
	class Node;
	typedef std::shared_ptr<const Node> NodePtr;

private:
	NodePtr root = nullptr;

public:
	PersistentBST() = default;
	PersistentBST(NodePtr _root)
		: root{ _root }
	{
	}

public:
	PersistentBST Insert(const K& key, const T& data) const {
		return PersistentBST(Insert(root, key, data));
	}
	PersistentBST Erase(const K& key) const {
		return PersistentBST(Erase(root, key));
	}
	NodePtr Find(const K& key) const {
		return Find(root, key);
	}

	bool Empty() const { 
		return root == nullptr;
	}

private:
	NodePtr Insert(NodePtr root, const K& key, const T& data) const;
	NodePtr Erase(NodePtr root, const K& key) const;
	NodePtr Find(NodePtr root, const K& key) const;

	NodePtr MakeNode(NodePtr root, NodePtr left = nullptr, NodePtr right = nullptr) const {
		return std::make_shared<const Node>(root->key, root->data, left, right);
	}
};

template<typename K, typename T>
class PersistentBST<K,T>::Node 
{
	friend class PersistentBST<K,T>;
private:
	K key;
	T data;

	NodePtr left;
	NodePtr right;

public:
	Node(const K& _key, const T& _data, NodePtr _left = nullptr, NodePtr _right = nullptr)
		: key{ _key }, data{ _data }, left{ _left }, right{ _right }
	{
	}

public:
	const K& Key()	const { return key; }
	const T& Data() const { return data; }
};

template<typename K, typename T>
using PersistentNodePtr = typename PersistentBST<K, T>::NodePtr;

template<typename K, typename T>
inline PersistentNodePtr<K,T> PersistentBST<K, T>::Insert(NodePtr root, const K& key, const T& data) const
{
	if (root == nullptr) {
		return std::make_shared<const Node>(key, data);
	}
	if (root->key > key) {
		return MakeNode(root, Insert(root->left, key, data), root->right);
	}
	if (root->key < key) {
		return MakeNode(root, root->left, Insert(root->right, key, data));
	}
	return root;
}

template<typename K, typename T>
inline PersistentNodePtr<K,T> PersistentBST<K, T>::Erase(NodePtr root, const K& key) const
{
	if (root == nullptr) {
		return nullptr;
	}
	if (root->key > key) {
		return MakeNode(root, Erase(root->left, key), root->right);
	}
	if (root->key < key) {
		return MakeNode(root, root->left, Erase(root->right, key));
	}
	// root->key == key
	if (root->left == nullptr) {
		return root->right;
	}
	if (root->right == nullptr) {
			return root->left;
	} 
	// root has both children
	std::function<NodePtr (NodePtr, NodePtr&)> FindMinNode = [&](NodePtr node, NodePtr& minNode) {
		if (node->left == nullptr) {
			minNode = node;
			return node->right;
		} else {
			return MakeNode(node, FindMinNode(node->left, minNode), node->right);
		}
	};

	NodePtr minNode;
	NodePtr rightBranch = FindMinNode(root->right, minNode);
	return MakeNode(minNode, root->left, rightBranch);
}

template<typename K, typename T>
inline PersistentNodePtr<K,T> PersistentBST<K, T>::Find(NodePtr root, const K& key) const
{
	if (root == nullptr) {
		return nullptr;
	}
	if (root->key < key) {
		return Find(root->right, key);
	}
	if (root->key > key) {
		return Find(root->left, key);
	}
	return root;
}
