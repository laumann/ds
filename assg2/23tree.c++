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
	TwoThreeNode *first, *second, *third;
	TwoThreeNode *parent;
};

/**
 * Useful for debugging
 */
void dump_node(TwoThreeNode *n) {
	std::cout << n << " *TwoThreeNode{ first: " << n->first
		<< ", second: " << n->second
		<< ", third: " << n->third
		<< ", firstData: " << n->firstData;
	if (n->full) {
		std::cout << ", secondData: " << n->secondData;
	}
	std::cout << ", parent: " << n->parent;
	std::cout << ", leaf: " << (n->leaf ? "true" : "false");
	std::cout << ", full: " << (n->full ? "true" : "false");
	
	std::cout << "}" <<  std::endl;
}

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
	//void split(TwoThreeNode *n, int x);
	void split(TwoThreeNode *n, int key, TwoThreeNode *first, TwoThreeNode *second, TwoThreeNode *third, TwoThreeNode *fourth);
	int size;
	TwoThreeNode *root;
};

TwoThreeTree::TwoThreeTree() {
	std::cout << "Init 2-3 tree" << std::endl;
	this->size = 0;
}


/**
 * Print a .dot file
 */
void dotty(TwoThreeNode *x) {
	// Declare node
	std::cout << "    n" << x << " [label=\"" << x->firstData;
	if (x->full) {
		std::cout << " | " << x->secondData;
	}
	std::cout << "\"];" << std::endl;

	if (!x->leaf) {
		dotty(x->first);
		std::cout << "    n" << x << " -- n" << x->first << ";" << std::endl;

		dotty(x->second);
		std::cout << "    n" << x << " -- n" << x->second << ";" << std::endl;

		if (x->full) {
			dotty(x->third);
			std::cout << "    n" << x << " -- n" << x->third << ";" << std::endl;
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

	if (this->size == 0) {
		this->root = new TwoThreeNode;
		this->root->firstData = key;
		this->size++;
		return;
	}

	TwoThreeNode *n = this->root;
	// Find proper leaf node
	while (!n->leaf) {
		std::cout << "Descending from "; dump_node(n);
		if (key < n->firstData) {
			n = n->first;
		} else if (!n->full || key < n->secondData) {
			n = n->second;
		} else {
			n = n->third;
		}
	}

	if (n->full)
		split(n, key, NULL, NULL, NULL, NULL);
	else {
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
 * Splitting a 2-3 tree node, the addition of the key would cause the node to
 * have three keys, thus it must be split. The last four arguments are in case
 * we're dealing with an internal node, then we must specify its children
 * (because it has technically has four, but our structure doesn't allow it).
 */
void TwoThreeTree::split(TwoThreeNode *n, int key, TwoThreeNode *first, TwoThreeNode *second, TwoThreeNode *third, TwoThreeNode *fourth) {
	
	TwoThreeNode *n1 = new TwoThreeNode, *n2 = new TwoThreeNode;
	TwoThreeNode *p = n == this->root ? new TwoThreeNode : n->parent;

	// Get the keys in order
	int min = n->firstData, mid = n->secondData, max;
	if (key < min) {
		max = mid;
		mid = min;
		min = key;
	} else if (key < mid) {
		max = mid;
		mid = key;
	} else {
		max = key;
	}

	n1->firstData = min;
	n2->firstData = max;
	n1->parent = p;
	n2->parent = p;

	if (!n->leaf) {
		n1->first = first;
		n1->second = second;
		n2->first = third;
		n2->second = fourth;

		first->parent = n1;
		second->parent = n1;
		third->parent = n2;
		fourth->parent = n2;

		n1->leaf = n2->leaf = false;
	}

	if (n == this->root) {
		p->firstData = mid;
		p->first = n1;
		p->second = n2;
		p->leaf = false;
		this->root = p;
	} else if (p->full) {
		if (n == p->first) {
			split(p, mid, n1, n2, p->second, p->third);
		} else if (n == p->second) {
			split(p, mid, p->first, n1, n2, p->third);
		} else {
			split(p, mid, p->first, p->second, n1, n2);
		}
	} else {
		/**
		 * Simply add it in
		 */
		if (mid < p->firstData) {
			p->secondData = p->firstData;
			p->firstData = mid;

			p->third = p->second;
			p->second = n2;
			p->first = n1;
		} else {
			p->secondData = mid;

			p->second = n1;
			p->third = n2;
		}
		p->full = true;
	}
	delete n;
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
		t.insert(vals[i]);
		dotty(&t);
		int n = count_keys(&t);
		if (n <= nkeys) {
			std::cout << "Inconsistency!!! Had " << nkeys << " but now only " << n << std::endl;
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
