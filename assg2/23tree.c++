/**
 * (Very) Simple 2-3 tree implementation.
 *
 * The key (haha) thing to observe about 2-3 trees is that they are special
 * cases of B-trees (see Cormen et al) when t = 2.
 *
 */
#include <iostream>

class TwoThreeNode {
	public:
		int firstData, secondData;
		TwoThreeNode *first, *second, *third;
		TwoThreeNode *parent;
};

class TwoThreeTree {
	public:
		TwoThreeTree(void);
		~TwoThreeTree(void);

		void insert(int key);
	private:
		TwoThreeNode *root;
};


TwoThreeTree::TwoThreeTree()
{
	std::cout << "Init 2-3 tree" << std::endl;
}

TwoThreeTree::~TwoThreeTree()
{
}

/**
 * Theoretically we should be able to split the nodes on insert as we're
 * descending into the tree, but for starters let's just stick to a simpler
 * method that splits the nodes on the way up (this is only single-threaded, so
 * we don't (yet) risk dead-locking).
 */
void TwoThreeTree::insert(int key)
{
	std::cout << "Inserting key " << key << std::endl;

	// Find proper leaf node
	
}

/**
 * Test this
 */
int main(int argc, char *argv[])
{
	TwoThreeTree t;

	std::cout << "Size of TwoThreeTree: " << sizeof(TwoThreeTree) << std::endl;
	std::cout << "Size of TwoThreeNode: " << sizeof(TwoThreeNode) << std::endl;

	t.insert(10);
	t.insert(50);

	return 0;
}
