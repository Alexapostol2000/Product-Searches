#ifndef TREEMAP_H_
#define TREEMAP_H_

#include <stdlib.h>

#define MAX(a, b) (((a) >= (b))?(a):(b))

//-------------------------------------------------------------------------

typedef struct node {
	void * elem; // Node key
	void * info; // Node satellite information
	struct node * pt; // Parent link
	struct node * lt; // left child link
	struct node * rt; // right child link
	struct node * next; // next link in the list of duplicates
	struct node * prev; // prev link in the list of duplicates
	struct node * end; // pointer to the end of the duplicates list
	long height;
} TreeNode;

typedef struct TTree {
	TreeNode * root; // pointer to the root of the tree
	void * (* createElement)(void *); // create element method
	void (* destroyElement)(void *); // destroy element method
	void * (* createInfo)(void *); // create info method
	void (* destroyInfo)(void *); // destroy info method
	int (* compare)(void *, void *); // comparison method between elements
	long size;
} TTree;


TTree * createTree(void * (* createElement)(void *), void (* destroyElement)(void *),
                   void * (* createInfo)(void *), void (* destroyInfo)(void *),
                   int compare(void *, void *)) {
	TTree * tree;
	tree = (TTree *) calloc(1, sizeof(TTree));
	tree->createElement = createElement;
	tree->destroyElement = destroyElement;
	tree->createInfo = createInfo;
	tree->destroyInfo = destroyInfo;
	tree->compare = compare;
	tree->size = 0;

	return tree;
}

int isEmpty(TTree * tree) {
	return tree->root == NULL;
}


TreeNode * search(TTree * tree, TreeNode * x, void * elem) {
	TreeNode * t = tree->root;
	while (t != NULL) {
		if (tree->compare(t->elem, elem) == 0)
			return t;
		else if (tree->compare(t->elem, elem) == 1)
			t = t->lt;
		else
			t = t->rt;
	}

	return NULL;

}


TreeNode * minimum(TreeNode * x) {
	if (x == NULL)
		return NULL;
	while (x->lt != NULL)
		x = x->lt;
	return x;

}

TreeNode * maximum(TreeNode * x) {
	if (x == NULL)
		return NULL;
	while (x->rt != NULL)
		x = x->rt;
	return x;
}

TreeNode * successor(TreeNode * x) {
	TreeNode * aux = x;
	while (aux->pt != NULL)
		aux = aux->pt;

	if (maximum(aux) == x)
		return NULL;
	if (x->rt != NULL)
		return minimum(aux);
	TreeNode * p = x->pt;
	while (p != aux && x == p->rt) {
		x = p;
		p = p->pt;
	}
	return p;


}

TreeNode * predecessor(TreeNode * x) {
	TreeNode * aux = x;
	while (aux->pt != NULL)
		aux = aux->pt;
	if (minimum(aux) == x)
		return NULL;
	if (x->lt != NULL)
		return maximum(x->lt);
	TreeNode * p = x->pt;
	while (p != NULL && x == p->lt) {
		x = p;
		p = p->pt;
	}
	return p;
}

void updateHeight(TreeNode * x) {
	int leftHeight = 0;
	int rightHeight = 0;
	if (x != NULL) {
		if (x->lt != NULL) leftHeight = x->lt->height;
		if (x->rt != NULL) rightHeight = x->rt->height;
		x->height = MAX(leftHeight, rightHeight) + 1;
	}
}

void avlRotateLeft(TTree * tree, TreeNode * x) {

	TreeNode * k2 = x->rt, * k1;
	if (x == NULL)
		return;
	if (x == tree->root) {
		tree->root = k2;
		k2->pt = NULL;
		k1 = NULL;
	} else k1 = x->pt;
	x->rt = k2->lt;
	k2->lt = x;
	if (k2->lt != NULL) k2->lt->pt = x;
	x->pt = k2;

	int hl = 0, hr = 0, hl1 = 0;
	if (x->rt != NULL)
		hr = x->rt->height;
	if (x->lt != NULL)
		hl = x->lt->height;

	x->height = MAX(hl, hr) + 1;

	if (k2->lt != NULL)
		hl1 = k2->lt->height;
	k2->height = MAX(hl1, x->height) + 1;

	if (k1 != NULL && tree->root != k2) {
		if (k1->lt == x) {
			k1->lt = k2;
		} else if (k1->rt == x) {
			k1->rt = k2;
		}
	}
	k2->pt = k1;

}


void avlRotateRight(TTree * tree, TreeNode * y) {
	TreeNode * k2 = y->lt, * k1;
	if (y == NULL)
		return;
	if (y == tree->root) {
		tree->root = k2;
		k1 = NULL;
	} else k1 = y->pt;

	y->lt = k2->rt;
	k2->rt = y;
	if (k2->rt) k2->rt->pt = y;
	y->pt = k2;

	int hl = 0, hr = 0, hl1 = 0;
	if (y->lt != NULL)
		hr = y->lt->height;
	if (y->rt != NULL)
		hl = y->rt->height;

	y->height = MAX(hr, hl) + 1;
	if (k2->lt != NULL)
		hl1 = k2->lt->height;

	k2->height = MAX(hl1, y->height) + 1;

	if (k1 != NULL && tree->root != k2) {
		if (k1->lt == y) {
			k1->lt = k2;
		} else {
			k1->rt = k2;
		}
	}
	k2->pt = k1;
}


/* Get AVL balance factor for node x */
int avlGetBalance(TreeNode * x) {
	if (x == NULL)
		return 0;
	int hl = 0, hr = 0;
	if (x->lt != NULL)
		hl = x->lt->height;
	if (x->rt != NULL)
		hr = x->rt->height;
	return hl - hr;

}


void avlFixUp(TTree * tree, TreeNode * y) {
	TreeNode * t = y->pt;
	while (t != NULL) {
		updateHeight(t);
		if (avlGetBalance(t) == 2) {
			if (tree->compare(y->elem, t->lt->elem) == -1) {

				avlRotateRight(tree, t);
			} else {

				avlRotateLeft(tree, t->lt);
				avlRotateRight(tree, t);
			}
		}

		if (avlGetBalance(t) == -2) {
			if (tree->compare(y->elem, t->rt->elem) == 1) {
				avlRotateLeft(tree, t);
			} else {
				avlRotateRight(tree, t->rt);
				avlRotateLeft(tree, t);
			}
		}

		t = t->pt;

	}

}


TreeNode * createTreeNode(TTree * tree, void * value, void * info) {
	// Allocate node
	TreeNode * newNode = (TreeNode *) malloc(sizeof(TreeNode));

	// Set its element field
	newNode->elem = tree->createElement(value);

	//Set its info
	newNode->info = tree->createInfo(info);

	// Set its tree links
	newNode->pt = newNode->rt = newNode->lt = NULL;

	// Set its list links
	newNode->next = newNode->prev = newNode->end = NULL;

	/*
	 *  The height of a new node is 1,
	 *  while the height of NULL is 0
	 */
	newNode->height = 1;

	return newNode;
}

void destroyTreeNode(TTree * tree, TreeNode * node) {
	// Check arguments
	if (tree == NULL || node == NULL) return;

	// Use object methods to de-alocate node fields
	tree->destroyElement(node->elem);
	tree->destroyInfo(node->info);

	// De-allocate the node
	free(node);
}


void insert(TTree * tree, void * elem, void * info) {
	TreeNode * node;
	node = createTreeNode(tree, elem, info);
	if (tree->root != NULL) {
		TreeNode * aux = tree->root;
		while (aux != NULL) {
			if (tree->compare(elem, aux->elem) == 0) {
				return;
			}

			if (tree->compare(elem, aux->elem) == -1) {
				if (aux->lt == NULL) {
					aux->lt = node;
					node->pt = aux;
					break;
				}
				aux = aux->lt;
			} else {
				if (tree->compare(elem, aux->elem) == 1)
					if (aux->rt == NULL) {
						aux->rt = node;
						node->pt = aux;
						break;
					}
				aux = aux->rt;
			}

		}
		tree->size++;
		avlFixUp(tree, node);

	} else {
		tree->root = node;
		node->pt = NULL;
		tree->size++;
	}


}

void delete(
TTree * tree,
void * elem
){
TreeNode * z;
TreeNode * y;
TreeNode * x;


if((z = search(tree, tree->root, elem)) == NULL)
return;


y = ((z->lt == NULL) || (z->rt == NULL)) ? z : successor(z);

x = (y->lt == NULL) ? y->rt : y->lt;

if (y->pt == NULL ){
tree->root = x;
}
else if (y == y->pt->lt){
y->pt->lt = x;
}
else{
y->pt->rt = x;
}

if (y != z){
y->lt = z->lt;
y->rt = z->rt;
y->pt = z->pt;
if(z -> lt ->pt != NULL )
z->lt->pt = y;
if(z -> rt -> pt != NULL)
z->rt->pt = y;
if(z -> pt != NULL){
if (z == z->pt->lt){
z->pt->lt = y;
}
else{
z->pt->rt = y;
}
}
else{

tree ->root = y;
}

}



tree->size--;
TreeNode * min, * max;
min = minimum(tree->root);
max = maximum(tree->root);
avlFixUp(tree, min);
avlFixUp(tree, max);

}

void destroyTree(TTree * tree) {
	// TODO: Cerinta 1
	/*
	 * Note: You can use the tree list to
	 * deallocate the hole tree.
	 */
}


#endif /* TREEMAP_H_ */
