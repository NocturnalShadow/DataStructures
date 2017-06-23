#pragma once

#include <vector>
#include <memory>
#include <algorithm>

template<typename T>
class BinomialTree
{
public:
	class Node;

private:
	std::unique_ptr<Node> root;
	unsigned int order = 0;

public:
	BinomialTree() = default;
	BinomialTree(const T& value)
		: root{ std::make_unique<Node>(value) }
	{
	}
	BinomialTree(std::unique_ptr<Node>& _root, unsigned int _order)
		: root{ std::move(_root) }, order{ _order }
	{
	}

private:
	BinomialTree& append(BinomialTree& tree) 
	{
		root->append(tree.root);
		order++;
		return *this;
	}

public:
	Node* Root()					{ return root.get(); }
	const Node* Root()		const	{ return root.get(); }
	unsigned int Order()	const	{ return order; }

public:
	static BinomialTree Merge(BinomialTree& first, BinomialTree& second)
	{
		if (first.order != second.order) {
			throw std::runtime_error("Attempt to merge binomial trees of the different order.");
		}

		if (!first.root) {
			return std::move(second);
		} 
		if (!second.root) {
			return std::move(first);
		}

		if (first.root->value < second.root->value) {
			return std::move(first.append(second));
		} else {
			return std::move(second.append(first));
		}
	}

};

template<typename T>
class BinomialHeap;

template<typename T>
class BinomialTree<T>::Node
{
	friend class BinomialTree<T>;
	friend class BinomialHeap<T>;
private:
	T value;
	std::vector<
		std::unique_ptr<Node>> children;

public:
	Node(const T& _value)
		: value{ _value }
	{
	}

public:
	const T& Value() const {
		return value;
	}

private:
	void append(std::unique_ptr<Node>& root) {
		children.push_back(std::move(root));
	}
};

template<typename T>
class BinomialHeap
{
	typedef BinomialTree<T> Tree;
	typedef typename Tree::Node Node;

private:
	std::vector<Tree> trees;
	__int64 minTreeIndex = -1;

public:
	BinomialHeap() = default;
	BinomialHeap(const T& value) 
		: minTreeIndex{ 0 }
	{
		trees.emplace_back(value);
	}
	BinomialHeap(std::vector<std::unique_ptr<Node>>& roots)
	{
		unsigned int order = 0;
		for (auto& root : roots) {
			trees.emplace_back(root, order++);
		}
	}

private:
	void append(Tree& tree) 
	{
		auto treeIter = trees.insert(trees.end(), std::move(tree));
		if (minTreeIndex == -1 || treeIter->Root()->Value() < First()) {
			minTreeIndex = treeIter - trees.begin();
		}
	}
	auto appendOrMerge(Tree& tree) 
	{
		if (trees.empty()) {
			append(tree);
		}

		auto& lastInsertedTree = trees.back();
		if (tree.Order() > lastInsertedTree.Order()) {
			append(tree);
		} else { // tree.order == lastInsertedTree.order
			lastInsertedTree = Tree::Merge(tree, lastInsertedTree);
		}
	}
	
public:
	void Push(const T& value)
	{
		BinomialHeap heap{ value };
		*this = Merge(*this, heap);
	}
	void Pop()
	{
		if (minTreeIndex == -1) {
			throw std::runtime_error("BinomialHeap is empty.");
		} 
		auto minTree = std::move(trees[minTreeIndex]);
		trees.erase(trees.begin() + minTreeIndex);
		BinomialHeap heap{ minTree.Root()->children };
		*this = Merge(*this, heap);
	}
	const T& First() const {
		return trees[minTreeIndex].Root()->Value();
	}

public:
	static BinomialHeap Merge(BinomialHeap& first, BinomialHeap& second);
};

template<typename T>
inline BinomialHeap<T> BinomialHeap<T>::Merge(BinomialHeap& first, BinomialHeap& second)
{
	BinomialHeap result;
	auto firstIter = first.trees.begin();
	auto firstEnd = first.trees.end();
	auto secondIter = second.trees.begin();
	auto secondEnd = second.trees.end();
	while (firstIter != firstEnd && secondIter != secondEnd)
	{
		if (firstIter->Order() < secondIter->Order()) 
		{
			result.appendOrMerge(*firstIter);
			firstIter++;
		}
		else if (firstIter->Order() > secondIter->Order()) 
		{
			result.appendOrMerge(*secondIter);
			secondIter++;
		}
		else 
		{
			result.append(Tree::Merge(*firstIter, *secondIter));
			firstIter++;
			secondIter++;
		}
	}
	if (secondIter != secondEnd)
	{
		firstIter = secondIter;
		firstEnd = secondEnd;
	}
	if (firstIter != firstEnd)
	{
		std::for_each(firstIter, firstEnd, [&] (Tree& tree) {
			result.appendOrMerge(tree);
		});
	}

	return std::move(result);
}
