#include "SuffixTree.h"
#include <strings.h>

#include <iostream>
#include <cstring>
using std::cout;
using std::endl;

namespace suffixtree
{

Tree::Tree(
	std::string& input,
	std::string alphabet
	):
	input(input),
	alphabet(alphabet),
	B(input.length() + 1, 0),
	B_i(0),
	ranDFS(false),
	nIntNodes(1),
	nLeaves(0)
{
	input += '$';

	size_t size = sizeof(Node) * input.length() * 2;
	alloc_buf = (Node*)malloc(size);
	bzero(alloc_buf, size);

	alloc_i = 0;

	nextNodeId = input.length();
	root = newNode(assignId(), nullptr, 0, 0);
	lowestIntNode = root;
	root->suffixLink = root;
}

void Tree::Build() {
	Node *n = root;
	for (int i = 0; i < input.length(); i++) {
		n = insertSuffix(n, i);
	}
	DisplayChildren(root);
}

Node *
Tree::FindPath(
	Node *node,
	const char *s,
	int *extra
	)
{
	Node *child = getChildByLabelBeginning(node, s[0]);
	*extra = 0;

	//No child by that label
	if (child == nullptr) {
		return node;
	}

	int len = strlen(s);
	for (int i = 1; i < len; i++) {
		if (i >= child->len && child->child != nullptr) {
			return FindPath(child, s+i, extra);
		}
		if (i >= child->len && child->child == nullptr) {
			panic("Hit a leaf~\n");
		}
		else if (s[i] != input[child->beg + i] ||
			(i >= child->len && child->child == nullptr) ||
			(s[i] == 'N' && input[child->beg + i] == 'N')) {
			//Backtrack up to internal node most recently visited.
			*extra = i - 1;
			return node;
		}
	}

	//All suffix id's will belong to the child. We are just part way through the
	//edge leading up to that child.
	return child;
}

Node *
Tree::findPathAndInsert(
	Node *node,
	int suffix,
	int beg,
	int len
	)
{
	/*
	printf("= findPathAndInsert: ");
	PrintSegment(beg, len);
	printf(" -> ");
	PrintNodeLabel(node);
	printf("\n");
	*/

	if (len == 0) {
		printf("len is zero. why?\n");
	}

	Node *child = getChildByLabelBeginning(node, input[beg]);

	// No child by that label
	if (child == nullptr) {
		Node *nchild = newNode(suffix, node, beg, len);
		addChildToNode(node, nchild);

		return nchild;
	}

	for (int i = 1; i < len; i++) {
		if (i >= child->len) {
			return findPathAndInsert(child, suffix, beg + i, len - i);
		}
		if (input[beg + i] != input[child->beg + i]) {
			// SPLIT
			Node *mid = breakEdge(child, i);
			//printf("= splitting: ");
			//PrintNodeLabel(mid);
			//printf("\n");
			return findPathAndInsert(mid, suffix, (beg + i), (len - i));
		}
	}

	panic("shouldnt actually happen");
}

bool
Tree::areEqual(
	int i,
	int j
	)
{
	return input[i] == input[j];
}

int
Tree::assignId()
{
	return nextNodeId++;
}

Node*
Tree::insertSuffix(
	Node *prevLeaf,
	int suffix
	)
{
	Node *u = prevLeaf->parent;

	nLeaves++;

	if (prevLeaf == root) {
		return findPathAndInsert(root, suffix, 0, input.length());

	} else if (u->suffixLink != nullptr) {
		//Case 1: u already existed at the start of iteration (suffix-1).
		Node *v = u->suffixLink;
		//u->stringDepth is equivalent to the length of alpha
		return findPathAndInsert(v, suffix, suffix + v->stringDepth,
			input.length() - suffix - v->stringDepth);

	} else if (u->parent != root) {
		//Case 2A: u was created during iteration (suffix-1), and u' is not root
		Node *uprime = u->parent;
		Node *vprime = uprime->suffixLink;
		//Beta is the edge label between u and uprime.
		int beta = u->beg;
		int betalen = u->len;

		Node *v = nodeHopToV(vprime, beta, betalen);
		u->suffixLink = v;

		return findPathAndInsert(v, suffix, suffix + v->stringDepth,
			input.length() - suffix - v->stringDepth);

	} else {
		//Case 2B: u was created during iteration (suffix-1), and u' is the root
		Node *uprime = u->parent; // root
		Node *vprime = uprime->suffixLink; // also the root
		int betaprime = u->beg + 1; // beta = x betaprime
		int betaprimelen = u->len - 1;

		Node *v = nodeHopToV(vprime, betaprime, betaprimelen);
		u->suffixLink = v;

		return findPathAndInsert(v, suffix, suffix + v->stringDepth,
			input.length() - suffix - v->stringDepth);
	}
}

Node *
Tree::nodeHopToV(
	Node *start,
	int beta,
	int len
	)
{
	int i = 0, remaining;
	char c;
	Node *n1 = start, *n2;

	if (len == 0)
		return start;

	while (true) {
		remaining = len - i;
		c = input[beta + i];
		n2 = getChildByLabelBeginning(n1, c);


		if (n2->len > remaining) {
			//break edge right before index 'remaining' of the edge.
			return breakEdge(n2, remaining);

		} else if (n2->len == remaining) {
			//Beta is perfectly consumed, we have found V.
			return n2;

		} else {
			//Travel edge, consume part of beta
			i += n2->len;
			n1 = n2;
		}
	}
}

Node *
Tree::breakEdge(
	Node *n,
	int i
	)
{
	//printf("= breakEdge(%d)\n", i);
	Node *parent = nullptr, *nNode = nullptr;

	parent = n->parent;

	//Make new node
	nNode = newNode(assignId(), parent, n->beg, i);
	//Remove n from parent's children, replace with new node.
	replaceChild(parent, nNode);

	n->sibling = nullptr;

	//Insert n under new node
	addChildToNode(nNode, n);

	//Alter n
	n->beg = n->beg + i;
	n->parent = nNode;
	n->len = n->len - i;

	//We have a new internal node
	nIntNodes++;
	if (lowestIntNode == nullptr ||
		nNode->stringDepth > lowestIntNode->stringDepth) {
		lowestIntNode = nNode;
	}

	return nNode;
}

Node *
Tree::getChildByLabelBeginning(
	Node *n,
	char c
	)
{
	Node *cur = n->child;
	while (cur != nullptr)
	{
		if (input[cur->beg] == c)
			return cur;
		cur = cur->sibling;
	}
	return cur;
}

void 
Tree::addChildToNode(
	Node *parent,
	Node *child
	)
{
	Node *cur = parent->child;
	if (cur == nullptr) {
		parent->child = child;
		return;
	}

	char k = input[child->beg];
	if (k == '$') {
		if (cur != nullptr && input[cur->beg] == '$') {
			panic("shouldnt already have child with this label");
		}

		child->sibling = cur;
		parent->child = child;
		return;
	}

	Node *prev = nullptr;
	while(cur != nullptr) {
		if (k < input[cur->beg]) {
			child->sibling = cur;
			if (prev == nullptr) {
				parent->child = child;
			} else {
				prev->sibling = child;
			}
			return;
		}

		prev = cur;
		cur = cur->sibling;
	}
	prev->sibling = child;
}

void
Tree::DisplayChildren(
	Node *node
	)
{
	Node *child = node->child;

	while (child != nullptr) {
		std::cout << "Node id: " << child->id << "Edge label: '";
		for (int i = child->beg; i < child->len + child->beg; i++) {
			std::cout << input[i];
		}
		std::cout << "'" << std::endl;

		child = child->sibling;
	}
}

void
Tree::EnumerateNodesDFS()
{
	printCount = 0;
	recursiveEnumerateNodesDFS(root);
	std::cout << std::endl;
	printCount = 0;
}

void
Tree::recursiveEnumerateNodesDFS(
	Node *n
	)
{
	//After 10 enumerations, print a newline
	if (printCount == 10) {
		std::cout << std::endl;
		printCount = 0;
	}

	std::cout << n->stringDepth;

	if (printCount != 9)
		std::cout << " ";

	printCount++;
	Node *child = n->child;

	while (child != nullptr) {
		recursiveEnumerateNodesDFS(child);
		child = child->sibling;
	}
}

void Tree::recursiveEnumerateBWT(
	Node *n
	)
{
	//Must be a leaf node.
	if (n->child == nullptr) {
		if (n->id == 0) {
			B[B_i++] = input[input.length() - 1];
		} else {
			B[B_i++] = input[n->id - 1];
		}
		return;
	}

	Node *child = n->child;
	while (child != nullptr) {
		recursiveEnumerateBWT(child);
		child = child->sibling;
	}
}

void Tree::EnumerateBWT()
{
	recursiveEnumerateBWT(root);
	for (auto v : B) {
		std::cout << v << std::endl;
	}
	//printf("NNODES = %d\n", assignId());
}

void Tree::PrintTree() {
	printTreeRec(root, 0);
}

void writeTabs(int n) {
	for (int i = 0; i < n; i++) {printf("+");}
}

void Tree::printTreeRec(Node *nd, int depth) {
	writeTabs(depth);
	printf("[");
	PrintNodeLabel(nd);
	printf("]");
	printf("{");
	
	Node *cur = nd->child;
	if (cur != nullptr) {
		printf("\n");
	}

	while(cur != nullptr) {
		printTreeRec(cur, depth + 1);
		cur = cur->sibling;
	}
	if (nd->child != nullptr) {
		writeTabs(depth);
	}
	printf("}\n");
}

void Tree::replaceChild(Node *parent, Node *node) {
	Node *prev = nullptr;
	Node *cur = parent->child;
	char k = input[node->beg];

	while (cur != nullptr) {
		if (k == input[cur->beg]) {
			node->sibling = cur->sibling;
			if (prev == nullptr) {
				parent->child = node;
			} else {
				prev->sibling = node;
			}
			return;
		}

		prev = cur;
		cur = cur->sibling;
	}
}

void Tree::PrintNodeLabel(Node *n) {
	PrintSegment(n->beg, n->len);
}

void Tree::PrintSegment(int beg, int len) {
	for (int i = 0; i < len; i++) {
		printf("%c", input[beg + i]);
	}
}

Node *Tree::newNode(int id, Node *parent, int beg, int len) {
	Node *nxt = &alloc_buf[alloc_i++];
	nxt->id = id;
	nxt->parent = parent;
	nxt->beg = beg;
	nxt->len = len;
	nxt->EndLeafIndex = -1;
	nxt->StartLeafIndex = -1;
	if (parent != nullptr) {
		nxt->stringDepth = parent->stringDepth + len;
	}
	return nxt;
}

void
Tree::PrepareIndexArray() {
	int size = input.length() + 1;
	A = (int*) malloc(sizeof(int) * size);
	for (int i = 0; i < size; i++) {
		A[i] = -1;
	}

	nextIndex = 0;
	DFS_PrepareRoot(root);
}

void
Tree::DFS_PrepareRoot(Node *nd) {
	if (nd == nullptr) {
		return;
	}

	if (nd->child == nullptr) {
		A[nextIndex] = nd->id;
		if (nd->stringDepth >= XValue) {
			nd->StartLeafIndex = nextIndex;
			nd->EndLeafIndex = nextIndex;
		}
		nextIndex++;
		return;
	}

	Node *child = nd->child;
	Node *last = nullptr;
	while (child != nullptr) {
		DFS_PrepareRoot(child);
		last = child;
		child = child->sibling;
	}

	if (nd->stringDepth >= XValue) {
		nd->StartLeafIndex = nd->child->StartLeafIndex;
		nd->EndLeafIndex = last->EndLeafIndex;
	}
}

void
Tree::PrintTreeInfo() {
	std::cout << "Number of leaves: " << nLeaves << std::endl;
	std::cout << "Number of internal nodes: " << nIntNodes << std::endl;
	std::cout << "Total number of nodes: " << nLeaves + nIntNodes << std::endl;
}

void
Tree::PrintLongestRepeatSeqInfo() {
	std::cout << "Length of longest repeating sequence: " <<
	  lowestIntNode->stringDepth << std::endl;
	std::cout << "Starting indeces of longest repeating sequence: " << std::endl;

	Node *child = lowestIntNode->child;

	while (child != nullptr) {
		std::cout << child->id << " ";
		child = child->sibling;
	}
	std::cout << std::endl;
}

std::vector<int>
Tree::FindLoc(
	std::string& r
	)
{
	std::vector<int> vec;
	int read_ptr = 0;
	const char *s = r.c_str();
	int len = r.length();
	int extra;
	int maxdepth = 0;
	Node *t = root;
	Node *deepestNode = t;
	Node *temp;

	//Find deepest node/longest substring
	while (read_ptr < len) {
		temp = FindPath(t, s + read_ptr, &extra);
		if (temp == root) {
			read_ptr++;
			continue;
		}

		read_ptr += temp->stringDepth - t->stringDepth;
		if (temp->stringDepth + extra > maxdepth) {
			deepestNode = temp;
			maxdepth = temp->stringDepth + extra;
		}
		t = temp->suffixLink;
	}

	if (maxdepth >= XValue) {
		for (int i = deepestNode->StartLeafIndex;
			i <= deepestNode->EndLeafIndex; i++) {
			vec.push_back(A[i]);
		}
	}

	return vec;
}
}
