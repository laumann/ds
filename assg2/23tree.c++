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
	~TwoThreeNode(void);
	int firstData, secondData;
	bool leaf;
	bool full;
	TwoThreeNode *first, *second, *third, *fourth;
	TwoThreeNode *parent;
};

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

TwoThreeNode::~TwoThreeNode(void) {
	std::cout << "Deleting node: "; dump_node(this);
}

/**
 * 2-3 tree
 */
class TwoThreeTree {
	public:
	TwoThreeTree(void);

	void insert(int key);
	void split(TwoThreeNode *n, int x);
	void split(TwoThreeNode *n, int key, TwoThreeNode *first, TwoThreeNode *second, TwoThreeNode *third, TwoThreeNode *fourth);
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
	std::cout << "Inserting key " << key << std::endl;

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
		std::cout << "Inserting " << key << " in " << n << std::endl;
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
	
	if (n == this->root) {
		std::cout << "Splitting root!" << std::endl;
		
		TwoThreeNode *r = new TwoThreeNode;
		TwoThreeNode *n1 = new TwoThreeNode, *n2 = new TwoThreeNode;
		
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
		std::cout << "min: " << min << ", mid: " << mid << ", max: " << max << std::endl;
		n1->firstData = min;
		n2->firstData = max;
		n1->parent = r;
		n2->parent = r;
		
		r->firstData = mid;
		r->first = n1;
		r->second = n2;
		r->leaf = false;

		if (!n->leaf) {
			n1->first = first;
			first->parent = n1;
			n1->second = second;
			second->parent = n1;
			n2->first = third;
			third->parent = n2;
			n2->second = fourth;
			fourth->parent = n2;
			n1->leaf = n2->leaf = false;
		}
		this->root = r;
		dump_node(n1);
		dump_node(n2);
		dump_node(r);
	} else {
		/**
		 * We're in the tree somewhere - could be a leaf or an internal,
		 * but we're not the root.
		 */
		dump_node(n->parent);
		TwoThreeNode *p = n->parent;
		TwoThreeNode *n1 = new TwoThreeNode;
		TwoThreeNode *n2 = new TwoThreeNode;
		std::cout << "n:      "; dump_node(n);
		std::cout << "parent: "; dump_node(p);
		
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
			std::cout << "pointer adjustment" << std::endl;
			std::cout << "first:  "; dump_node(first); 
			std::cout << "second: "; dump_node(second); 
			std::cout << "third:  "; dump_node(third); 
			std::cout << "fourth: "; dump_node(fourth); 

			n1->first = first;
			first->parent = n1;
			n1->second = second;
			second->parent = n1;
			n2->first = third;
			third->parent = n2;
			n2->second = fourth;
			fourth->parent = n2;
			n1->leaf = n2->leaf = false;
		}
		std::cout << "n1:     "; dump_node(n1);
		std::cout << "n2:     "; dump_node(n2);
		if (n2 == n2->parent)
			std::cout << "That's a whoopsie!" << std::endl;


		std::cout << "Pushing up " << mid << std::endl;
		if (p->full) {
			if (n == p->first) {
				split(p, mid, n1, n2, p->second, p->third);
			} else if (n == p->second) {
				split(p, mid, p->first, n1, n2, p->third);
			} else {
				split(p, mid, p->first, p->second, n1, n2);
			}
		} else {
			std::cout << "There's room... ";
			/**
			 * Add it in
			 */
			if (mid < p->firstData) {
				std::cout << "Added as first" << std::endl;
				p->secondData = p->firstData;
				p->firstData = mid;

				p->third = p->second;
				p->second = n2;
				p->first = n1;
			} else {
				std::cout << "Added as second" << std::endl;
				p->secondData = mid;

				p->second = n1;
				p->third = n2;
			}
			p->full = true;
			printNode(p);
		}
	}
	delete n;
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
		std::cout << "Splitting root!" << std::endl;
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
		n1->leaf = n->leaf;

		p->firstData = mid;
		p->first = n1;
		p->second = n2;
		p->leaf = false;

		n2->firstData = max;
		n2->first = n->third;
		n2->second = n->fourth;
		n2->leaf = n->leaf;
		
		delete n;

	} else {
		// We split node n (who is not the root)
		dump_node(n);
		p = n->parent;

		n1->parent = p;
		n2->parent = p;
		
		// sort the data (find out if x is min, mid or max)
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
		n1->firstData = min; // n1 receives smallest
		n2->firstData = max; // n2 receives largest

		if (!n->leaf) {
			/**
			 * If n is not a leaf it must have four children (by
			 * construction), 
			 */
			n1->first  = n->first;
			n1->second = n->second;
			n2->first  = n->third;
			n2->second = n->fourth;
			n1->leaf = n2->leaf = false;
		}
		std::cout << "Pushing up " << mid << " (parent " << (p->full ? "is full)" : "has room)") << std::endl;

		if (p->full) {
			// Give it four children
			if (n == p->first) {
				std::cout << "We're p->first" << std::endl;
				p->fourth = p->third;
				p->third = p->second;
				p->second = n2;
				p->first = n1;
			} else if (n == p->second) {
				std::cout << "We're p->second" << std::endl;
				p->fourth = p->third;
				p->third = n2;
				p->second = n1;
			} else if (n == p->third) {
				std::cout << "We're p->third" << std::endl;
				p->third = n1;
				p->fourth = n2;
			} else if (n == p->fourth) {	
				std::cout << "We're p->fourth. That's an error" << std::endl;
			}
			split(p, mid);
		} else {
			/**
			 * Add it in.
			 */
			dump_node(p);
			if (n == p->first) {
				std::cout << "We're p->first" << std::endl;
			} else if (n == p->second) {
				std::cout << "We're p->second" << std::endl;
			} else if (n == p->third) {
				std::cout << "We're p->third" << std::endl;
			} else if (n == p->fourth) {
				std::cout << "We're p->fourth" << std::endl;
			}
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
