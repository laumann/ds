/**
 * (Very) Simple 2-3 tree implementation.
 */
#include <iostream>

class TwoThreeNode {
	public:
	TwoThreeNode(void);
	int firstData, secondData;
	bool leaf;
	TwoThreeNode *first, *second, *third;
	TwoThreeNode *parent;
};

TwoThreeNode::TwoThreeNode(void) {
	this->leaf = true;
}

class TwoThreeTree {
	public:
	TwoThreeTree(void);

	void insert(int key);
	void split(TwoThreeNode *n, int x);

	private:
	TwoThreeNode *root;
};


TwoThreeTree::TwoThreeTree()
{
	std::cout << "Init 2-3 tree" << std::endl;
}

#define full(n) n->third

/**
 * Theoretically we should be able to split the nodes on insert as we're
 * descending into the tree, but for starters let's just stick to a simpler
 * method that splits the nodes on the way up (this is only single-threaded, so
 * we don't (yet) risk dead-locking).
 */
void TwoThreeTree::insert(int key)
{
	std::cout << "Inserting key " << key << std::endl;

	TwoThreeNode *n = this->root;
	if (n == NULL) {
		this->root = n = new TwoThreeNode;
		n->firstData = key;
		return;
	}

	// Find proper leaf node
	while (!n->leaf) {
		if (key < n->firstData) {
			n = n->first;
		} else if (!full(n) || key < n->secondData) {
			n = n->second;
		} else {
			n = n->third;
		}
	}

	if (full(n))
		split(n, key);
	else
		if (key < n->firstData) {
			n->secondData = n->firstData;
			n->firstData = key;
		} else {
			n->secondData = key;
		}
}

/**
 *
 */
void TwoThreeTree::split(TwoThreeNode *n, int x) {
	TwoThreeNode *p = n == this->root ? new TwoThreeNode : n->parent;
	/*
	if (n == this->root) {
		p = new TwoThreeNode;
	} else {
		p = n->parent;
	}
	*/

	TwoThreeNode *n1 = new TwoThreeNode, *n2 = new TwoThreeNode;
	n1->parent = p;
	n2->parent = p;

	int min, max, mid;
	min = n->firstData;
	mid = n->secondData;
	if (x < min) {
		max = mid;
		mid = min;
		min = x;
	} else if (x < min) {
		max = mid;
		mid = x;
	} else {
		max = x; 
	}

	n1->firstData = min;
	n2->firstData = max;

	// Do we need to split the parent?
	if (full(p)) {
		// Split parent
		// We have an extra pointer to handle
	} else {
		// Parent has two children (and one key)
		// We push up mid, and find out which child we were originally
		if (n == p->first) {
			// arrange data
			p->secondData = p->firstData;
			p->firstData = mid;
			// arrange pointers
			p->third = p->second;
			p->second = n2;
			p->first = n1;
			delete n;
		}
	}


	/*
	if (!n.leaf) {
		n.
	}
	*/

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
