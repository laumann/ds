/**
 * (Very) Simple 2-3 tree implementation.
 */
#include <iostream>

/**
 * 2-3 tree node
 */
class TwoThreeNode {
	public:
	TwoThreeNode(void);
	int firstData, secondData;
	bool leaf;
	bool full;
	TwoThreeNode *first, *second, *third, *fourth;
	TwoThreeNode *parent;
};

TwoThreeNode::TwoThreeNode(void) {
	this->leaf = true;
	this->full = false;
	this->firstData = this->secondData = 0;
}

/**
 * 2-3 tree
 */
class TwoThreeTree {
	public:
	TwoThreeTree(void);

	void insert(int key);
	void split(TwoThreeNode *n, int x);
	int size;
	TwoThreeNode *root;
};

TwoThreeTree::TwoThreeTree() {
	std::cout << "Init 2-3 tree" << std::endl;
	this->size = 0;
}

void printNode(TwoThreeNode *n);

/**
 * Print a .dot file
 */
void dotty(TwoThreeNode *x) {
	// Declare node
	std::cout << "    n" << x->firstData << " [label=\"" << x->firstData;
	if (x->full) {
		std::cout << " | " << x->secondData;
	}
	std::cout << "\"];" << std::endl;

	if (!x->leaf) {
		dotty(x->first);
		std::cout << "    n" << x->firstData << " -- n" << x->first->firstData << ";" << std::endl;

		dotty(x->second);
		std::cout << "    n" << x->firstData << " -- n" << x->second->firstData << ";" << std::endl;

		if (x->full) {
			dotty(x->third);
			std::cout << "    n" << x->firstData << " -- n" << x->third->firstData << ";" << std::endl;
		}
	}
}

int count_nodes(TwoThreeNode *n) {
	if (n->leaf) {
		return 1;
	}
	int i = 1 + count_nodes(n->first) + count_nodes(n->second);
	if (n->full)
		i += count_nodes(n->third);
	return i;
}

int count_keys(TwoThreeNode *n) {
	int i = n->full ? 2 : 1;
	if (n->leaf)
		return i;
	i += count_keys(n->first) + count_keys(n->second);
	if (n->full)
		i += count_keys(n->third);
	return i;
}

int count_keys(TwoThreeTree *t) {
	if (t->size == 0) {
		return 0;
	}
	return count_keys(t->root);
}

int count_nodes(TwoThreeTree *t) {
	if (t->size == 0) {
		return 0;
	}
	return count_nodes(t->root);
}

void dotty(TwoThreeTree *t) {
	std::cout << "graph G {" << std::endl;
	dotty(t->root);
	std::cout << "}" << std::endl;
}

/**
 * Theoretically we should be able to split the nodes on insert as we're
 * descending into the tree, but for starters let's just stick to a simpler
 * method that splits the nodes on the way up (this is only single-threaded, so
 * we don't (yet) risk dead-locking).
 */
void TwoThreeTree::insert(int key) {
	std::cout << "Inserting key " << key << std::endl;

	if (this->size == 0) {
		this->root = new TwoThreeNode;
		this->root->firstData = key;
		this->size++;
		dotty(this);
		printNode(this->root);
		return;
	}
	dotty(this);

	TwoThreeNode *n = this->root;
	// Find proper leaf node
	while (!n->leaf) {
		if (key < n->firstData) {
			n = n->first;
		} else if (!n->full || key < n->secondData) {
			n = n->second;
		} else {
			n = n->third;
		}
	}

	if (n->full)
		split(n, key);
	else {
		//std::cout << "Inserting " << key << " in " << n << std::endl;
		if (key < n->firstData) {
			n->secondData = n->firstData;
			n->firstData = key;
		} else {
			n->secondData = key;
		}
		n->full = true;
	}
	this->size++;
}

/**
Considering the various cases

    [ 2 . 6 ]
   /    |    \
 [1]  [3.4]  [7]

Insert 5

Case splitting leaf node
 */
void TwoThreeTree::split(TwoThreeNode *n, int x) {

	TwoThreeNode *p;
	TwoThreeNode *n1 = new TwoThreeNode;
	TwoThreeNode *n2 = new TwoThreeNode;
	if (n == this->root) {
		// When splitting the root we short-circuit the entire deal
		p = new TwoThreeNode;
		this->root = p;
		n1->parent = p;
		n2->parent = p;

		int min, max, mid;
		min = n->firstData;
		mid = n->secondData;
		if (x < min) {
			max = mid;
			mid = min;
			min = x;
		} else if (x < mid) {
			max = mid;
			mid = x;
		} else {
			max = x;
		}
		n1->firstData = min;
		n1->first = n->first;
		n1->second = n->second;
		n1->leaf = n2->leaf = n->leaf;

		p->firstData = mid;
		p->first = n1;
		p->second = n2;
		p->leaf = false;

		n2->firstData = max;
		n2->first = n->third;
		n2->second = n->fourth;
		
		delete n;

	} else {
		p = n->parent;

		n1->parent = p;
		n2->parent = p;
		
		int min = n->firstData, mid = n->secondData, max;
		if (x < min) {
			max = mid;
			mid = min;
			min = x;
		} else if (x < mid) {
			max = mid;
			mid = x;
		} else {
			max = x;
		}
		n1->firstData = min;
		n2->firstData = max;

		if (!n->leaf) {
			n1->first  = n->first;
			n1->second = n->second;
			n2->first  = n->third;
			n2->second = n->fourth;
			n1->leaf = n2->leaf = false;
		}
		std::cout << "Pushing up " << mid << std::endl;

		if (p->full) {
			// Give it four children
			if (n == p->first) {
				p->fourth = p->third;
				p->third = p->second;
				p->second = n2;
				p->first = n1;
			} else if (n == p->second) {
				p->fourth = p->third;
				p->third = n2;
				p->second = n1;
			} else {
				p->third = n1;
				p->fourth = n2;
			}
			//delete n;
			split(p, mid);
		} else {
			/**
			 * Add it in.
			 */
			if (n == p->first) {
				p->third = p->second;
				p->second = n2;
				p->first = n1;
				p->secondData = p->firstData;
				p->firstData = mid;
			} else {
				p->second = n1;
				p->third = n2;
				p->secondData = mid;
			}
			p->full = true;
		}
		delete n;
	}
}

void printNode(TwoThreeNode *n) {
	std::cout << "### Node " << n << std::endl;
	std::cout << "    root? " << ((n->parent == NULL) ? "yes" : "no") << std::endl;
	if (n->parent)
		std::cout << "    parent: " << n->parent << std::endl;
	std::cout << "    key1  " << n->firstData << std::endl;
	//if (n->third == NULL)
	if (n->full)
		std::cout << "    key2  " << n->secondData << std::endl;

	if (n->leaf)
		std::cout << "    " << "Is leaf (no children)" << std::endl;
	else
		std::cout << "    Has " << (n->third == NULL ? "2" : "3") << " children." << std::endl;

}

#define ARRAYSIZE(x) sizeof(x)/sizeof(x[0])

/**
 * Test this
 */
int main(int argc, char *argv[]) {
	TwoThreeTree t;

	std::cout << "Size of TwoThreeTree: " << sizeof(TwoThreeTree) << std::endl;
	std::cout << "Size of TwoThreeNode: " << sizeof(TwoThreeNode) << std::endl;

	int vals[] = {50, 60, 70, 40, 30, 20, 10, 80, 90, 100};
	//int vals[] = {1,2,3,7,6,4};

	int nkeys = 0;

	for (int i = 0; i < ARRAYSIZE(vals); i++) {
		if (t.size > 0)
			dotty(&t);
		t.insert(vals[i]);
		dotty(&t);
		int n = count_keys(&t);
		if (n <= nkeys) {
			std::cout << "Inconsistency!!!" << std::endl;
			std::cout << "Number of keys:  " << count_keys(&t) << std::endl;
			std::cout << "Number of nodes: " << count_nodes(&t) << std::endl;
			return -1;
		}
		nkeys = n;
	}

	dotty(&t);

	std::cout << t.size << std::endl;
	std::cout << "Number of keys:  " << count_keys(&t) << std::endl;
	std::cout << "Number of nodes: " << count_nodes(&t) << std::endl;

	return 0;
}
