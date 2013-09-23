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
	void split_leaf(TwoThreeNode *n, int x);
	void split_inner(TwoThreeNode *n, int x, TwoThreeNode *n1, TwoThreeNode *n2);
	int size;

	private:
	TwoThreeNode *root;
};


TwoThreeTree::TwoThreeTree()
{
	std::cout << "Init 2-3 tree" << std::endl;
	this->size = 0;
}

#define full(n) n->third
void printNode(TwoThreeNode *n);

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
	std::cout << "Found " << n << std::endl;

	if (full(n))
		split_leaf(n, key);
	else
		if (key < n->firstData) {
			n->secondData = n->firstData;
			n->firstData = key;
		} else {
			n->secondData = key;
		}
	this->size++;
}

/**
 *
 */
void TwoThreeTree::split_leaf(TwoThreeNode *n, int x) {
	printNode(n);
	TwoThreeNode *p;
	if (n == this->root) {
		p = new TwoThreeNode;
		this->root = p;
	} else {
		p = n->parent;
	}
	//TwoThreeNode *p = n == this->root ? new TwoThreeNode : n->parent;

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
	p->leaf = false;

	// Do we need to split the parent?
	if (full(p)) {
		// Split parent
		// We have an extra pointer to handle
		this->split_inner(p, mid, n1, n2);
	} else {
		// Parent has two children (and one key)
		// We push up mid, and find out which child we were originally
		if (n == p->first) {
			p->secondData = p->firstData;	// data
			p->firstData = mid;
			p->third = p->second;		// pointers...
			p->second = n2;
			p->first = n1;
		} else if (n == p->second) {
			p->secondData = mid;
			p->second = n1;
			p->third = n2;
		}
	}
	//delete n;
}

void TwoThreeTree::split_inner(TwoThreeNode *m, int x, TwoThreeNode *n1, TwoThreeNode *n2) {
	printNode(m);
	TwoThreeNode *p;
	if (m == this->root) {
		p = new TwoThreeNode;
		this->root = p;
	} else {
		p = m->parent;
	}
	//TwoThreeNode *p = m == this->root ? new TwoThreeNode : m->parent;

	TwoThreeNode *m1 = new TwoThreeNode, *m2 = new TwoThreeNode;
	m1->parent = p;
	m2->parent = p;

	int min, max, mid;
	min = m->firstData;
	mid = m->secondData;
	if (x < min) {
		max = mid;
		mid = min;
		min = x;
		m1->first = n1;
		m1->second = n2;
		m2->first = m->second;
		m2->second = m->third;
	} else if (x < min) {
		max = mid;
		mid = x;
		m1->first = m->first;
		m1->second = n1;
		m2->first = n2;
		m2->second = m->third;
	} else {
		max = x;
		m1->first = m->first;
		m1->second = m->second;
		m2->first = n1;
		m2->second = n2;
	}

	m1->firstData = min;
	m2->firstData = max;
	m1->leaf = false;
	m2->leaf = false;

	// Do we need to split the parent?
	if (full(p)) {
		// Split parent
		// We have an extra pointer to handle
		this->split_inner(p, mid, m1, m2);
	} else {
		// Parent has two children (and one key)
		// We push up mid, and find out which child we were originally
		if (m == p->first) {
			p->secondData = p->firstData;	// data
			p->firstData = mid;
			p->third = p->second;		// pointers...
			p->second = m2;
			p->first = m1;
		} else if (m == p->second) {
			p->secondData = mid;
			p->second = m1;
			p->third = m2;
		}
	}
	//delete m;
}

void printNode(TwoThreeNode *n) {
	std::cout << "### Node " << n << std::endl;
	std::cout << "    root? " << ((n->parent == NULL) ? "yes" : "no") << std::endl;
	if (n->parent)
		std::cout << "    parent: " << n->parent << std::endl;
	std::cout << "    key1  " << n->firstData << std::endl;
	if (n->third == NULL)
		std::cout << "    key2  " << n->secondData << std::endl;

	if (n->leaf)
		std::cout << "    " << "Is leaf (no children)" << std::endl;
	else
		std::cout << "    Has " << (n->third == NULL ? "2" : "3") << " children." << std::endl;

}

/**
 * Test this
 */
int main(int argc, char *argv[])
{
	TwoThreeTree t;

	std::cout << "Size of TwoThreeTree: " << sizeof(TwoThreeTree) << std::endl;
	std::cout << "Size of TwoThreeNode: " << sizeof(TwoThreeNode) << std::endl;

	int vals[] = {50, 60,70,40,30,20,10,80,90,100};

	//int *val_p = vals;
	for (int *val_p = vals; val_p; val_p++)
		t.insert(*val_p);

	/**
	t.insert(0);
	t.insert(20);
	t.insert(50);
	t.insert(50);
	*/

	std::cout << t.size << std::endl;

	return 0;
}
