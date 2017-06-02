//**************************************************************************************
//								< Red-Black Tree >		
//**************************************************************************************
// Type:		Balanced Binary-Search tree
// Purpose:		Map data structure
// Name:		Red-Black tree
// Implementation details:
//		> Implemented without "phantom" leaves.
//
//											Code written by NocturnalShadow.
//**************************************************************************************

#pragma once

template<typename K, typename T>
class RedBlackBST
{
public:
	class Node;
	enum class Color { RED, BLACK };

private:
	Node* root 	= nullptr;

public:
	RedBlackBST()	
		{	}
	~RedBlackBST()	
		{ delete root; }

public:
	RedBlackBST(const RedBlackBST& tree)			= delete;
	RedBlackBST& operator=(const RedBlackBST& tree) = delete;

public:
	void Insert(const K& key, const T& data);
	void Erase(const K& key);

	Node* Find(const K& key);

	Node* Root()
		{ return root; }

	template<typename Func>
	void InOrder(Node* _root, Func func);

private:
	Node* GrandFather(Node* node);
	Node* Uncle(Node* node);
	Node* Brother(Node* node);

	void RotateLeft(Node* node);
	void RotateRight(Node* node);

	void InsertNode(Node* node, Node*& _root, Node* root_parent = nullptr);
	
	void InsertCase1(Node* node);
	void InsertCase2(Node* node);
	void InsertCase3(Node* node);
	void InsertCase4(Node* node);
	void InsertCase5(Node* node);

	Node* MinNode(Node* _root);
	Node* MaxNode(Node* _root);
	Node* Find(const K& key, Node* _root);

	void DeleteCase1(Node* node);
	void DeleteCase2(Node* node);
	void DeleteCase3(Node* node);
	void DeleteCase4(Node* node);
	void DeleteCase5(Node* node);
	void DeleteCase6(Node* node);
};

template<typename K, typename T>
class RedBlackBST<K,T>::Node
{
	friend class RedBlackBST<K,T>;
private:
	K key;
	T* data			= nullptr;
	Color color		= Color::RED;

	Node* parent	= nullptr;
	Node* left		= nullptr;
	Node* right		= nullptr;

public:
	Node(const K& _key, const T& _data)
		: key{ _key }, data{ new T(_data) }
	{
	}
	~Node()
	{
		if (parent) 
		{
			if (this->isLeftChild()) {
				parent->left = nullptr;
			} else if (this->isRightChild()) {
				parent->right = nullptr;
			}
		}
		delete left;
		delete right;
		delete data;
	}

public:
	Node(const Node& node)				= delete;
	Node& operator=(const Node& node)	= delete;

public:
	bool isRoot() const 
		{ return parent == nullptr;	}
	bool isRed() const
		{ return color == Color::RED; }
	bool isBlack() const
		{ return color == Color::BLACK; }
	bool isLeaf() const
		{ return !left && !right; }
	bool isLeftChild() const
		{ return parent->left == this;	}
	bool isRightChild() const
		{ return parent->right == this; }
	bool hasRightChild() const
		{ return right != nullptr; }
	bool hasLeftChild() const
		{ return left != nullptr; }

	bool isGreaterThen(Node* node)
		{ return this->key > node->key; }
	bool isLessThen(Node* node) 
		{ return this->key < node->key;	}

	const K& Key() const 
		{ return key; }
	T& Data()
		{ return *data; }

private:
	void toRed()	{ color = Color::RED;	 }
	void toBlack()	{ color = Color::BLACK; }

	void PlaceTo(Node* node) {
		node->key = key;
	}
	void ReplaceIfNotNull(Node* node)
	{
		if (node == nullptr) { return; }
		if (this->isLeftChild()) {
			parent->left = node;
		} else {
			parent->right = node;
		}

		node->parent = parent;
		this->left = nullptr;
		this->right = nullptr;
		this->parent = nullptr;
	}
};


template<typename K, typename T> 
using RedBlackNode = typename RedBlackBST<K,T>::Node;

template<typename K, typename T>
inline RedBlackNode<K,T>* RedBlackBST<K,T>::GrandFather(Node* node)
{
	if (node && node->parent) {
		return node->parent->parent;
	} else {
		return nullptr;
	}
};

template<typename K, typename T>
inline RedBlackNode<K,T>* RedBlackBST<K,T>::Uncle(Node* node)
{
	Node* grandFather = GrandFather(node);
	if (grandFather == nullptr) { return nullptr; }
	if (node->parent == grandFather->left) {
		return grandFather->right;
	} else {
		return grandFather->left;
	}
}

template<typename K, typename T>
inline RedBlackNode<K,T>* RedBlackBST<K,T>::Brother(Node* node)
{
	if (!node || !node->parent) {
		return nullptr;
	}
	if (node == node->parent->left) {
		return node->parent->right;
	} else {
		return node->parent->left;
	}
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::RotateLeft(Node* node)
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
inline void RedBlackBST<K,T>::RotateRight(Node* node)
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
inline void RedBlackBST<K,T>::Insert(const K& key, const T& data)
{
	Node* node = new Node(key, data);
	InsertNode(node, root);
	InsertCase1(node);
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::Erase(const K& key)
{
	Node* target = Find(key, root);

	// key was not found
	if (!target) { return; }

	// case if target is a leaf node
	if (target->isLeaf())
	{
		if (target->isBlack()) {
			DeleteCase1(target);
		}
		if (target->isRoot()) {
			root = nullptr;
		} 
		delete target;
		return;
	}

	// a node to replace target with
	Node* node =
		target->hasRightChild() ? MinNode(target->right) : MaxNode(target->left);
	// the only child of the replacement node (may be nullptr if no children)
	Node* child =
		node->hasLeftChild() ? node->left : node->right;
	
	node->PlaceTo(target);
	node->ReplaceIfNotNull(child);

	if (node->isBlack())
	{
		if (child == nullptr) {
			DeleteCase1(node);
		} else if (child->isRed()) {
			child->toBlack();
		} else {
			DeleteCase1(child);
		}
	}
	delete node;
}

template<typename K, typename T>
inline RedBlackNode<K,T>* RedBlackBST<K,T>::Find(const K& key)
{
	return Find(key, root);
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::InsertNode(Node* node, Node*& _root, Node* root_parent)
{
	if (_root == nullptr) 
	{
		_root = node;
		_root->parent = root_parent;
	} 
	else
	{
		if (node->isGreaterThen(_root)) {
			InsertNode(node, _root->right, _root);
		} else {
			InsertNode(node, _root->left, _root);
		}
	}
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::InsertCase1(Node* node)
{
	if (node->isRoot()) {
		node->toBlack();
	} else {
		InsertCase2(node);
	}
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::InsertCase2(Node* node)
{
	if (!node->parent->isBlack()) {
		InsertCase3(node);
	}
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::InsertCase3(Node* node)
{
	Node* uncle = Uncle(node);
	Node* grand_father = GrandFather(node);

	if (uncle && uncle->isRed()) 
	{
		node->parent->toBlack();
		uncle->toBlack();
		grand_father->toRed();
		InsertCase1(grand_father);
	}
	else {
		InsertCase4(node);
	}
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::InsertCase4(Node* node)
{
	Node* grand_father = GrandFather(node);
	Node* parent = node->parent;

	if (node->isRightChild() && parent->isLeftChild()) 
	{
		RotateLeft(parent);
		node = node->left;
	}
	else if (node->isLeftChild() && parent->isRightChild()) 
	{
		RotateRight(parent);
		node = node->right;
	}
	InsertCase5(node);
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::InsertCase5(Node* node)
{
	Node* grand_father = GrandFather(node);
	Node* parent = node->parent;

	parent->toBlack();
	grand_father->toRed();
	if (node->isLeftChild()) {
		RotateRight(grand_father);
	} else {
		RotateLeft(grand_father);
	}
}

template<typename K, typename T>
inline RedBlackNode<K,T>* RedBlackBST<K,T>::MinNode(Node* _root)
{
	if (_root != nullptr) 
	{
		while (_root->hasLeftChild()) {
			_root = _root->left;
		}
	}
	return _root;
}

template<typename K, typename T>
inline RedBlackNode<K,T>* RedBlackBST<K,T>::MaxNode(Node* _root)
{
	if (_root != nullptr)
	{
		while (_root->hasRightChild()) {
			_root = _root->right;
		}
	}
	return _root;
}

template<typename K, typename T>
inline RedBlackNode<K,T>* RedBlackBST<K,T>::Find(const K& key, Node* _root)
{
	while (_root && _root->key != key)
	{
		if (_root->key < key) {
			_root = _root->right;
		} else {
			_root = _root->left;
		}
	}
	return _root;
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::DeleteCase1(Node* node)
{
	if (!node->isRoot()) {
		DeleteCase2(node);
	}
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::DeleteCase2(Node* node)
{
	Node* brother = Brother(node);

	if (brother->isRed())
	{
		node->parent->toRed();
		brother->toBlack();
		if (node->isLeftChild()) {
			RotateLeft(node->parent);
		} else {
			RotateRight(node->parent);
		}
	}
	DeleteCase3(node);
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::DeleteCase3(Node* node)
{
	Node* brother = Brother(node);

	bool repaint = 
		node->parent->isBlack() && brother->isBlack()	&&
		(!brother->left	 || brother->left->isBlack())	&&
		(!brother->right || brother->right->isBlack());

	if (repaint)
	{
		brother->toRed();
		DeleteCase1(node->parent);
	} else {
		DeleteCase4(node);
	}
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::DeleteCase4(Node* node)
{
	Node* brother = Brother(node);

	bool repaint =
		node->parent->isRed() &&
		brother->isBlack() &&
		(!brother->left || brother->left->isBlack()) &&
		(!brother->right || brother->right->isBlack());

	if (repaint)
	{
		brother->toRed();
		node->parent->toBlack();
	} else {
		DeleteCase5(node);
	}
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::DeleteCase5(Node* node)
{
	Node* brother = Brother(node);

	if (brother->isBlack()) 
	{ 
		bool left_child = 
			node->isLeftChild()			&&
			brother->right->isBlack()	&&
			brother->left->isRed();

		bool rigth_child = 
			node->isRightChild()		&&
			brother->left->isBlack()	&&
			brother->right->isRed();

		if (left_child)
		{ 
			brother->toRed();
			brother->left->toBlack();
			RotateRight(brother);
		}
		else if (rigth_child)
		{
			brother->toRed();
			brother->right->toBlack();
			RotateLeft(brother);
		}
	}
	DeleteCase6(node);
}

template<typename K, typename T>
inline void RedBlackBST<K,T>::DeleteCase6(Node* node)
{
	Node* brother = Brother(node);

	brother->color = node->parent->color;
	node->parent->toBlack();

	if (node->isLeftChild()) 
	{
		brother->right->toBlack();
		RotateLeft(node->parent);
	}
	else 
	{
		brother->left->toBlack();
		RotateRight(node->parent);
	}
}

template<typename K, typename T>
template<typename Func>
inline void RedBlackBST<K, T>::InOrder(Node* _root, Func func)
{
	if (_root != nullptr)
	{
		InOrder(_root->left, func);
		func(_root);
		InOrder(_root->right, func);
	}
}
