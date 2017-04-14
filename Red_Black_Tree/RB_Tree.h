//************************************************************************
//								<Red-Black Tree>		
//************************************************************************

#pragma once

template<typename T>
class RB_Tree
{
public:
	enum Color { RED, BLACK };
	struct Node;

private:
	Node* root;

public:
	RB_Tree()
	{
	}

	~RB_Tree()
	{
		delete root;
	}

public:
	void Insert(const T& key);
	void Erase(const T& key);

private:
	Node* GrandFather(Node* node);
	Node* Uncle(Node* node);
	Node* Brother(Node* node);

	void RotateLeft(Node* pivot);
	void RotateRight(Node* pivot);

	void InsertNode(Node* node, Node*& _root, Node* root_parent = nullptr);
	void EraseLeaf(Node* leaf);

	void InsertCase1(Node* node);
	void InsertCase2(Node* node);
	void InsertCase3(Node* node);
	void InsertCase4(Node* node);
	void InsertCase5(Node* node);

	Node* MinNode(Node* _root);
	Node* MaxNode(Node* _root);
	Node* Find(const T& key, Node* _root);

	void DeleteCase1(Node* node);
	void DeleteCase2(Node* node);
	void DeleteCase3(Node* node);
	void DeleteCase4(Node* node);
	void DeleteCase5(Node* node);
	void DeleteCase6(Node* node);
};

template<typename T>
struct RB_Tree<T>::Node
{
	T key;
	Color color = RED;

	Node* parent	= nullptr;
	Node* left		= nullptr;
	Node* right		= nullptr;

public:
	Node(const T& _key)
		: key{ _key }
	{
	}
	~Node()
	{
		delete left;
		delete right;
	}

public:
	bool isRoot() const 
		{ return parent == nullptr;	}
	bool isRed() const
		{ return color == RED; }
	bool isBlack() const
		{ return color == BLACK; }
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

	void toRed()	{ color = RED;	 }
	void toBlack()	{ color = BLACK; }

	void PlaceTo(Node* node)
	{
		node->key = key;
	}
	void ReplaceWith(Node* node)
	{
		if (this->isLeftChild()) {
			parent->left = node;
		} else {
			parent->right = node;
		}
		
		this->left = nullptr;
		this->right = nullptr;
	}

};

template<typename T> 
using NodePtr = typename RB_Tree<T>::Node*;

template<typename T>
inline NodePtr<T> RB_Tree<T>::GrandFather(Node* node)
{
	if (node && node->parent) {
		return node->parent->parent;
	} else {
		return nullptr;
	}
};

template<typename T>
inline NodePtr<T> RB_Tree<T>::Uncle(Node* node)
{
	Node* grandFather = GrandFather(node);
	if (grandFather == nullptr) { return nullptr; }
	if (node->parent == grandFather->left) {
		return grandFather->right;
	} else {
		return grandFather->left;
	}
}

template<typename T>
inline NodePtr<T> RB_Tree<T>::Brother(Node* node)
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

template<typename T>
inline void RB_Tree<T>::RotateLeft(Node* node)
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

template<typename T>
inline void RB_Tree<T>::RotateRight(Node* node)
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

template<typename T>
inline void RB_Tree<T>::Insert(const T& key)
{
	Node* node = new Node(key);
	InsertNode(node, root);
	InsertCase1(node);
}

template<typename T>
inline void RB_Tree<T>::Erase(const T& key)
{
	Node* target = Find(key, root);

	// key was not found
	if (!target) { return; }

	// complications
	// if target is a leaf 
	if (target->isLeaf())
	{
		if (target->isBlack()) {
			DeleteCase1(target);
		}
		if (target->isRoot()) {
			root = nullptr;
		} else if (target->isRightChild()) {
			target->parent->right = nullptr;
		} else {
			target->parent->left = nullptr;
		}
		delete target;
		return;
	}

	Node* node =
		target->hasRightChild() ? MinNode(target->right) : MaxNode(target->left);

	Node* child =
		node->hasLeftChild() ? node->left : node->right;

	node->PlaceTo(target);
	node->ReplaceWith(child);

	if (node->isBlack())
	{
		if (child == nullptr) 
		{
			// complications
			if (node->parent->hasRightChild()) 
			{
				node->parent->left = node;
				DeleteCase1(node);
				node->parent->left = nullptr;
			} 
			else {
				node->parent->right = node;
				DeleteCase1(node);
				node->parent->right = nullptr;
			}
		} 
		else if (child->isRed()) {
			child->toBlack();
		} else {
			DeleteCase1(child);
		}
	}
	delete node;
}

template<typename T>
inline void RB_Tree<T>::InsertNode(Node* node, Node*& _root, Node* root_parent)
{
	if (_root == nullptr) {
		_root = node;
		_root->parent = root_parent;
	} else {
		if (node->isGreaterThen(_root)) {
			InsertNode(node, _root->right, _root);
		} else {
			InsertNode(node, _root->left, _root);
		}
	}
}

template<typename T>
inline void RB_Tree<T>::EraseLeaf(Node* leaf)
{
	if (leaf->isRoot()) {
		root = nullptr;
	} else if (leaf->isLeftChild()) {
		leaf->parent->left = nullptr;
	} else {
		leaf->parent->right = nullptr;
	}
	delete leaf;
}

template<typename T>
inline void RB_Tree<T>::InsertCase1(Node* node)
{
	if (node->isRoot()) {
		node->color = BLACK;
	} else {
		InsertCase2(node);
	}
}

template<typename T>
inline void RB_Tree<T>::InsertCase2(Node* node)
{
	if (!node->parent->isBlack()) {
		InsertCase3(node);
	}
}

template<typename T>
inline void RB_Tree<T>::InsertCase3(Node* node)
{
	Node* uncle = Uncle(node);
	Node* grand_father = GrandFather(node);

	if (uncle && uncle->isRed()) 
	{
		node->parent->color = BLACK;
		uncle->color = BLACK;
		grand_father->color = RED;
		InsertCase1(grand_father);
	}
	else {
		InsertCase4(node);
	}
}

template<typename T>
inline void RB_Tree<T>::InsertCase4(Node* node)
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

template<typename T>
inline void RB_Tree<T>::InsertCase5(Node* node)
{
	Node* grand_father = GrandFather(node);
	Node* parent = node->parent;

	parent->color = BLACK;
	grand_father->color = RED;
	if (node->isLeftChild()) {
		RotateRight(grand_father);
	} else {
		RotateLeft(grand_father);
	}
}

template<typename T>
inline NodePtr<T> RB_Tree<T>::MinNode(Node* _root)
{
	if (_root != nullptr) 
	{
		while (_root->hasLeftChild()) {
			_root = _root->left;
		}
	}
	return _root;
}

template<typename T>
inline NodePtr<T> RB_Tree<T>::MaxNode(Node * _root)
{
	if (_root != nullptr)
	{
		while (_root->hasRightChild()) {
			_root = _root->right;
		}
	}
	return _root;
}

template<typename T>
inline NodePtr<T> RB_Tree<T>::Find(const T& key, Node* _root)
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

template<typename T>
inline void RB_Tree<T>::DeleteCase1(Node* node)
{
	if (!node->isRoot()) {
		DeleteCase2(node);
	}
}

template<typename T>
inline void RB_Tree<T>::DeleteCase2(Node* node)
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

template<typename T>
inline void RB_Tree<T>::DeleteCase3(Node* node)
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

template<typename T>
inline void RB_Tree<T>::DeleteCase4(Node * node)
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

template<typename T>
inline void RB_Tree<T>::DeleteCase5(Node* node)
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

template<typename T>
inline void RB_Tree<T>::DeleteCase6(Node* node)
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
