/**
 * (Very) Simple 2-3 tree implementation.
 */
#include <iostream>

class TwoThreeNode {
	public:
		int firstData, secondData;
		bool leaf;
		TwoThreeNode *first, *second, *third;
		TwoThreeNode *parent;
};

class TwoThreeTree {
	public:
		TwoThreeTree(void);
		~TwoThreeTree(void);

		void insert(int key);
		void split(TwoThreeNode *n, int x);
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

	TwoThreeNode *x = this.root;	
	if (*x == NULL) {
		this.root = x = new TwoThreeNode;
		x->firstData = key;
		return;
	}

	// Find proper leaf node
	while (!x.leaf) {
		if (key < x->firstData) {
			x = x->first;
		} else if (x->third == NULL || key < x->secondData) {
			x = x->second;
		} else {
			x = x->third;
		}
	}

	if (x->third == NULL) {
		if (key < x->firstData) {
			x->secondData = x->firstData;
			x->firstData = key;
		} else {
			x->secondData = key;
		}
	}
	
	// If space, insert
	// else split
	//
}


void TwoThreeTree::split(TwoThreeNode *n, int x) {
	TwoThreeNode *p;
	if (this.root == n) {
		p = new TwoThreeNode;
	} else {
		p = n->parent;
	}

	TwoThreeNode *n1 = new TwoThreeNode, *n2 = new TwoThreeNode;
	n1->parent = p;
	n2->parent = p;

	int min, max, mid;
	min = n->firstData;
	mid = n->secondData;
	if (n < min) {
		max = mid;
		mid = min;
		min = n;
	} else if (n < max) {
		max = mid;
		mid = n;
	} else {
		max = n;
	}

	n1.firstData = min;
	n2.firstData = max;



	if (!n.leaf) {
		n.
	}

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
