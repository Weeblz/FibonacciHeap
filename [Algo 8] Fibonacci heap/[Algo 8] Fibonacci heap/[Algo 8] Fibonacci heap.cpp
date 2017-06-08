#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>

class student;
class group {
public:
	std::string name;
	std::vector<student*> groupList;

	group(std::string a) {
		name = a;
	}
};

class student {
public:
	std::string name;
	std::string homeCountry;
	std::string homeCity;
	double rating;
	group* specialization;

	student(std::string n, std::string c, std::string c_, double r, group* s) : name(n), homeCountry(c), homeCity(c_), rating(r), specialization(s) {}

	void show() const {
		std::cout << "student Name: " << name << "\nStudent Country: " << homeCountry << "\nStudent City: " << homeCity << "\nStudent Grade: " << rating << std::endl;
	}
};

class FibonacciHeap;
class Node {
private:
	Node* prev;
	Node* next;
	Node* child;
	Node* parent;
	student* value;
	int degree;
	bool marked;
public:
	friend class FibonacciHeap;
	Node* getPrev() { return prev; }
	Node* getNext() { return next; }
	Node* getChild() { return child; }
	Node* getParent() { return parent; }
	student* getValue() { return value; }
	bool isMarked() { return marked; }

	bool hasChildren() { return child; }
	bool hasParent() { return parent; }
};

class FibonacciHeap {
protected:
	Node* heap;
public:

	FibonacciHeap() {
		heap = _empty();
	}
	virtual ~FibonacciHeap() {
		if (heap) {
			_deleteAll(heap);
		}
	}
	Node* insert(student* value) {
		Node* ret = _singleton(value);
		heap = _merge(heap, ret);
		return ret;
	}
	void merge(FibonacciHeap& other) {
		heap = _merge(heap, other.heap);
		other.heap = _empty();
	}

	bool isEmpty() {
		return heap == nullptr;
	}

	student* getMinimum() {
		return heap->value;
	}

	student* removeMinimum() {
		Node* old = heap;
		heap = _removeMinimum(heap);
		student* ret = old->value;
		delete old;
		return ret;
	}

	void decreaseKey(Node* n, student* value) {
		heap = _decreaseKey(heap, n, value);
	}

	Node* find(std::string value) {
		return _find(heap, value);
	}

	void show() {
		std::queue<std::pair<Node*, int>> traverse;
		Node* currentGlobal = heap;
		std::pair<Node*,int> current;
		do {
			std::cout << "TREE\n\n" << std::endl;
			traverse.push(std::make_pair(currentGlobal, 1));
			while (!traverse.empty()) {
				current = traverse.front();
				traverse.pop();
				putChildren(traverse, current.first, current.second);
				std::cout << "LEVEL " << current.second << "   " << current.first->getValue()->name << "   " << current.first->getValue()->rating << std::endl;
			}
			currentGlobal = currentGlobal->getNext();
		} while (currentGlobal != heap);
	}
private:
	Node* _empty() {
		return nullptr;
	}

	Node* _singleton(student* value) {
		Node* n = new Node();
		n->value = value;
		n->prev = n->next = n;
		n->degree = 0;
		n->marked = false;
		n->child = nullptr;
		n->parent = nullptr;
		return n;
	}

	Node* _merge(Node* a, Node* b) {
		if (a == nullptr)return b;
		if (b == nullptr)return a;
		if (a->value->rating > b->value->rating) {
			Node* temp = a;
			a = b;
			b = temp;
		}
		Node* an = a->next;
		Node* bp = b->prev;
		a->next = b;
		b->prev = a;
		an->prev = bp;
		bp->next = an;
		return a;
	}

	void _deleteAll(Node* n) {
		if (n != nullptr) {
			Node* c = n;
			do {
				Node* d = c;
				c = c->next;
				_deleteAll(d->child);
				delete d;
			} while (c != n);
		}
	}

	void _addChild(Node* parent, Node* child) {
		child->prev = child->next = child;
		child->parent = parent;
		parent->degree++;
		parent->child = _merge(parent->child, child);
	}

	void _unMarkAndUnParentAll(Node* n) {
		if (n == nullptr)return;
		Node* c = n;
		do {
			c->marked = false;
			c->parent = nullptr;
			c = c->next;
		} while (c != n);
	}

	Node* _removeMinimum(Node* n) {
		_unMarkAndUnParentAll(n->child);
		if (n->next == n) {
			n = n->child;
		}
		else {
			n->next->prev = n->prev;
			n->prev->next = n->next;
			n = _merge(n->next, n->child);
		}
		if (n == nullptr)return n;
		Node* trees[64] = { nullptr };

		while (true) {
			if (trees[n->degree] != nullptr) {
				Node* t = trees[n->degree];
				if (t == n)break;
				trees[n->degree] = nullptr;
				if (n->value<t->value) {
					t->prev->next = t->next;
					t->next->prev = t->prev;
					_addChild(n, t);
				}
				else {
					t->prev->next = t->next;
					t->next->prev = t->prev;
					if (n->next == n) {
						t->next = t->prev = t;
						_addChild(t, n);
						n = t;
					}
					else {
						n->prev->next = t;
						n->next->prev = t;
						t->next = n->next;
						t->prev = n->prev;
						_addChild(t, n);
						n = t;
					}
				}
				continue;
			}
			else {
				trees[n->degree] = n;
			}
			n = n->next;
		}
		Node* min = n;
		do {
			if (n->value->rating < min->value->rating)min = n;
			n = n->next;
		} while (n != n);
		return min;
	}

	Node* _cut(Node* heap, Node* n) {
		if (n->next == n) {
			n->parent->child = nullptr;
		}
		else {
			n->next->prev = n->prev;
			n->prev->next = n->next;
			n->parent->child = n->next;
		}
		n->next = n->prev = n;
		n->marked = false;
		return _merge(heap, n);
	}

	Node* _decreaseKey(Node* heap, Node* n, student* value) {
		if (n->value < value)return heap;
		n->value = value;
		if (n->value < n->parent->value) {
			heap = _cut(heap, n);
			Node* parent = n->parent;
			n->parent = nullptr;
			while (parent != nullptr && parent->marked) {
				heap = _cut(heap, parent);
				n = parent;
				parent = n->parent;
				n->parent = nullptr;
			}
			if (parent != nullptr && parent->parent != nullptr)parent->marked = true;
		}
		return heap;
	}

	Node* _find(Node* heap, std::string name) {
		Node* n = heap;
		if (n == nullptr)return nullptr;
		do {
			if (n->value->name == name)return n;
			Node* ret = _find(n->child, name);
			if (ret)return ret;
			n = n->next;
		} while (n != heap);
		return nullptr;
	}

	void putChildren(std::queue<std::pair<Node*, int>>& s, Node* parent, int lvl) {
		Node* c = parent->getChild();
		if (!c) return;
		Node* current = c;
		do {
			s.push(std::make_pair(current, lvl+1));
			current = current->getNext();
		} while (current != c);
	}

};

int main() {
	std::vector<group*> Groups(10);
	std::vector<student*> Students(10);

	FibonacciHeap* h = new FibonacciHeap();

	std::ifstream in("base.txt");
	std::string temp, tempCountry, tempCity, tempSpec, tempR;
	double tempRating;

	for (int i = 0; i < 10; i++) {
		getline(in, temp);
		Groups[i] = new group(temp);
	}

	getline(in, temp);

	for (int i = 0; i < 10; i++) {
		getline(in, temp);
		getline(in, tempR);
		getline(in, tempCity);
		getline(in, tempCountry);
		getline(in, tempSpec);
		tempRating = atof(tempR.c_str());
		Students[i] = new student(temp, tempCountry, tempCity, tempRating, new group(tempSpec));
		
		h->insert(Students[i]);

		for (int j = 0; j < 10; j++) {
			if (Groups[j]->name == temp) {
				Groups[j]->groupList.push_back(Students[i]);
			}
		}
		getline(in, temp);
	}

	h->removeMinimum();
	h->show();
	//h->find("Gus Holbrook")->getValue()->show();

	system("pause");
    return 0;
}