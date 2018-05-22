#ifndef INDEX_INDEX_H
#define INDEX_INDEX_H

#include <iostream>
#include <fstream>
#include <string>
#include <typeinfo>

#include <cstring>
#include <io.h>
#include <cstdlib>
using namespace std;

typedef int Index;
typedef enum { inner, leaf } NodeType;
typedef enum { ins, del } op;
#define defrank 6

template <class T>
class TreeNode {
public:
	Index PID;
	Index ID;
	int size;
	T key[10];
	Index last;   // 叶子
	Index next;
	int blockNum[10];
	Index child[10]; // 内部结点
	NodeType Type;

	TreeNode(NodeType type, int Rank, int size = 1) {
		this->size = size;
		this->ID = this->PID = -1;
		this->last = this->next = -1;
		Type = type;
		for (int i = 0; i < Rank + 1; i++)
			child[i] = -1;
		for (int i = 0; i < Rank; i++)
			blockNum[i] = -1;
		// cout << "TreeNode()" << endl;
	}
	~TreeNode() {
		// cout << "~Node()" << endl;
	}
	void setKey(T x, int i) {
		key[i] = x;
	}
	void setChild(Index t, int i) {
		child[i] = t;
	}
	// 叶子用函数
	Index getNext() const {
		return next;
	}
	void setNext(Index t) {
		next = t;
	}
	Index getLast() const {
		return last;
	}
	void setLast(Index t) {
		last = t;
	}
	void setBlock(int num, int i) {
		blockNum[i] = num;
	}
};

template <class T>
class BPlusTree {
public:
	string Type;
	string TableName;
	string DBName;
	string IndexName;
	string attrName;
	int Rank;
	int MinSize;

	TreeNode<T> *root;      // 出现treenode的地方都要有T类型
	TreeNode<T> *childNode;

	TreeNode<T> *queue[100000];
	int front, rear;

	TreeNode<T> *node[100000];
	int cnt;

public:
	BPlusTree(int rank, string dbname, string tablename, string index, string attr, string type) {
		Rank = rank;
		DBName = dbname;
		TableName = tablename;
		IndexName = index;
		attrName = attr;
		Type = type;
		MinSize = ((Rank + 1) >> 1);

		root = NULL;
		for (int i = 0; i < 100000; i++)
			node[i] = new TreeNode<T>(leaf, 1);

		childNode = node[0];
		cnt = 1;

		front = rear = 0;
		//cout << "BPlusTree()" << endl;
	}

	void reset() {
		root = NULL;
		for (int i = 0; i < 100000; i++) {
			delete node[i];
			node[i] = new TreeNode<T>(leaf, 1);
		}
		childNode = node[0];
		cnt = 1;
		front = rear = 0;
	}

	~BPlusTree() {
		for (int i = 0; i < 100000; i++)
			delete node[i];
		cout << "~BPlusTree()" << endl;
	}
	string getTableName() {
		return TableName;
	}

	string getDBName() {
		return DBName;
	}

	string getIndexName() {
		return IndexName;
	}

	string getAttrName() {
		return attrName;
	}

	void InsertIntoLeaf(TreeNode<T> *p, int i, T x, int y) { // 插入叶子
															 // set the position
		while (i >= 1 && x <= p->key[i - 1]) {
			p->setKey(p->key[i - 1], i);
			p->setBlock(p->blockNum[i - 1], i);
			i--;
		}
		p->setKey(x, i);
		p->setBlock(y, i);
		p->size++;
	}

	T FindKey(TreeNode<T> *t) {
		while (t->Type == inner) {
			if (node[t->child[0]]->size == 0)
				t = node[t->child[1]];
			else
				t = node[t->child[0]];
		}
		//   t = node[t->child[0]];
		return t->key[0];
	}

	T FindNextKey(TreeNode<T> *t) {
		while (t->Type == inner)
			t = node[t->child[0]];
		return t->key[1];
	}

	void UpdateIndex(TreeNode<T> *p, T x) {
		if (!p) return;
		if (p->PID == -1) return;
		TreeNode<T> *t = node[p->PID];
		//InnerNode *t = static_cast<InnerNode*>(node[p->ParentId]);
		while (t) {
			if (x < t->key[0]) {
				if (t->PID != -1)
					t = node[t->PID];
				else break;
			}
			else {
				bool flag = false;
				for (int i = 0; i < t->size; i++) {
					if (x == t->key[i]) {
						t->key[i] = FindKey(node[t->child[i + 1]]);
						flag = true;
						break;
					}
				}
				if (flag) break;
				else if (t->PID != -1)
					t = node[t->PID];
				else break;
			}
		}
	}

	void UpdateIndex(TreeNode<T> *p, T x, T y) {
		if (!p) return;
		Index index = p->PID;
		if (p->PID == -1) return;
		TreeNode<T> *t = node[p->PID];
		//InnerNode *t = static_cast<InnerNode*>(node[p->ParentId]);
		while (t) {
			if (y < t->key[0]) {
				index = t->PID;
				if (index != -1)
					t = node[index];
				else break;
			}
			else {
				bool flag = false;
				for (int i = 0; i < t->size; i++) {
					if (y == t->key[i]) {
						t->key[i] = x;
						flag = true;
						break;
					}
				}
				if (flag) break;
				index = t->PID;
				if (index != -1)
					t = node[index];
				else break;
			}
		}
	}

	void UpdateChild(TreeNode<T> *t, T z) {
		if (z < t->key[0]) {
			for (int i = 0; i < t->size; i++) {
				t->key[i] = t->key[i + 1];
				t->child[i] = t->child[i + 1];
			}
			t->child[t->size] = t->child[t->size + 1];
		}
		else {
			for (int i = 0; i < t->size + 1; i++) {
				if (z == t->key[i]) {
					for (int j = i; j < t->size; j++)
						t->key[j] = t->key[j + 1];
					for (int j = i + 1; j < t->size + 1; j++)
						t->child[j] = t->child[j + 1];
					return;
				}
			}
		}
	}

	TreeNode<T>* FindLeafNode(T x) {
		TreeNode<T> *p = root;
		Index j = 1;
		if (!p) return NULL;
		while (p->Type == inner) {
			TreeNode<T> *t = p;
			if (x < t->key[0]) {
				j = t->child[0];
				p = node[j];
			}
			else {
				for (int i = 0; i < t->size - 1; i++) {
					if (x >= t->key[i] && x < t->key[i + 1]) {
						j = t->child[i + 1];
						p = node[j];
						break;
					}
				}
				if (x >= t->key[t->size - 1]) {
					j = t->child[t->size];
					p = node[j];
				}
			}
		}
		return p;
	}

	Index FindLeafNode(T x, op type) {
		TreeNode<T> *p = root;
		Index j = 1;
		if (!p) return -1;
		while (p->Type == inner) {
			TreeNode<T> *t = p;
			if (x < t->key[0]) {
				j = t->child[0];
				p = node[j];
			}
			else {
				for (int i = 0; i < t->size - 1; i++) {
					if (x > t->key[i] && x < t->key[i + 1]) {
						j = t->child[i + 1];
						p = node[j];
						break;
					}
					else if (x == t->key[i]) {
						if (type == ins) return -1; // already exist
						else if (type == del) {
							j = t->child[i + 1];
							p = node[j];
							break;
						}
					}
				}
				if (x > t->key[t->size - 1]) {
					j = t->child[t->size];
					p = node[j];
				}
				else if (x == t->key[t->size - 1]) {
					if (type == ins) return -1;
					else if (type == del) {
						j = t->child[t->size];
						p = node[j];
					}
				}
			}
		}
		return j;
	}

	bool InsertTest(T x) {
		Index index = FindLeafNode(x, ins);
		if (index == -1) return false;
		TreeNode<T> *p = node[index];

		if (!p) return false;
		for (int j = 0; j < p->size; j++) {
			if (x == p->key[j])
				return false;
		}
		return true;
	}

	bool IsKey(int x) {
		if (!this) return false;
		if (!root) return false;
		Index index = FindLeafNode(x, ins);
		if (index == -1) return true;
		TreeNode<T> *p = node[index];
		//Leaf* p = static_cast<Leaf*>(node[index]);
		if (!p) return true;
		for (int j = 0; j < p->size; j++) {
			if (x == p->key[j])
				return true;
		}
		return false;
	}

	void Insert(T x, int y) {
		if (!root) {  // root为空
					  // root = new Leaf(1);
					  //root = new TreeNode<T>(leaf, 1);
			root = node[cnt];
			root->ID = cnt;
			//node[cnt] = root;

			childNode->next = cnt;
			childNode->last = -1;

			root->setKey(x, 0);
			root->setBlock(y, 0);
			root->setNext(-1);
			root->setLast(0);
			root->PID = -1;
			cnt++;
			return;
		}
		// 找到叶节点
		Index index = FindLeafNode(x, ins);
		if (index == -1) return;
		//Leaf* p = static_cast<Leaf*>(node[index]);
		TreeNode<T> *p = node[index];

		if (!p) return;
		for (int j = 0; j < p->size; j++) {
			if (x == p->key[j])
				return;
		}
		// 叶节点非满
		if (p->size != Rank) {
			InsertIntoLeaf(p, p->size, x, y);
		}
		// 叶节点满
		else {
			if (p->last != 0 && p->last != -1 && node[p->last]->size < Rank) {
				// 向左借儿子
				T w = p->key[0];
				int count = 1;
				node[p->last]->setKey(p->key[0], node[p->last]->size);
				node[p->last]->setBlock(p->blockNum[0], node[p->last]->size);
				node[p->last]->size++;
				bool flag = false;

				for (int i = 0; i < Rank; i++) {
					if (!flag && i == Rank - 1) {
						p->key[i] = x;
						p->blockNum[i] = y;
					}
					else if (x > p->key[count] || flag) {
						p->key[i] = p->key[count];
						p->blockNum[i] = p->blockNum[count++];
					}
					else {
						p->key[i] = x;
						p->blockNum[i] = y;
						flag = true;
					}
				}
				T m = p->key[0];
				UpdateIndex(p, m, w);
				return;
			}
			else if (p->next != -1 && node[p->next]->size < Rank) {
				// 向右借儿子
				T *num = new T[Rank + 1];
				int *Block = new int[Rank + 1];
				bool flag = false;
				int count = 0;
				T w = node[p->next]->key[0];

				for (int i = 0; i < Rank + 1; i++) {
					if (!flag && i == Rank) {
						num[i] = x;
						Block[i] = y;
					}
					else if (x > p->key[count] || flag) {
						num[i] = p->key[count];
						Block[i] = p->blockNum[count++];
					}
					else {
						num[i] = x;
						Block[i] = y;
						flag = true;
					}
				}// 把要插入的和原来满的rank个排序 然后取前rank个放进此叶子

				for (int i = 0; i < Rank; i++) {
					p->key[i] = num[i];
					p->blockNum[i] = Block[i];
				}
				for (int i = node[p->next]->size; i > 0; i--) {
					node[p->next]->setKey(node[p->next]->key[i - 1], i);
					node[p->next]->setBlock(node[p->next]->blockNum[i - 1], i);
				}
				node[p->next]->size++;
				node[p->next]->setKey(num[Rank], 0);
				node[p->next]->setBlock(Block[Rank], 0);

				T u = node[p->next]->key[0];
				UpdateIndex(node[p->next], u, w);
				delete[] Block;
				delete[] num;
				return;
			}
			// 左满右满 分裂p
			//TreeNode<T> *newNode = new TreeNode<T>(leaf, 1);
			TreeNode<T> *newNode = node[cnt];
			//node[cnt] = newNode;
			newNode->ID = cnt;
			cnt++;

			// 预存原数据
			T *num1 = new T[Rank + 1];
			int *Block1 = new int[Rank + 1];
			for (int i = 0; i < Rank; i++) {
				if (p->key[i] == x) {
					return; // already exists
				}
				num1[i] = p->key[i];
				Block1[i] = p->blockNum[i];
			}

			int j = Rank - 1;
			// 把新的数据插入预存的数据（有序）
			for (; x < num1[j] && j >= 0; j--) {
				num1[j + 1] = num1[j];
				Block1[j + 1] = Block1[j];
			}
			num1[j + 1] = x;
			Block1[j + 1] = y;

			// 把预存的数据复制到node1和node2
			for (int i = 0; i < MinSize; i++) {
				p->setKey(num1[i], i);
				p->setBlock(Block1[i], i);
			}
			p->size = MinSize;
			int count = 0;
			for (int i = MinSize; i < Rank + 1; i++) {
				newNode->setKey(num1[i], count);
				newNode->setBlock(Block1[i], count++);
			}
			newNode->size = Rank - MinSize + 1;

			delete[] num1;
			delete[] Block1;

			// 维护双向链表
			newNode->next = p->next;
			newNode->last = p->ID;
			if (p->next != -1)
				node[p->next]->last = newNode->ID;
			//static_cast<Leaf*>(node[p->next])->last = newNode->Id;
			p->next = newNode->ID;

			TreeNode<T>* parent = NULL; // 当前节点的父节点
			TreeNode<T>* newNode2 = NULL;
			//TreeNode<T>** Node = new TreeNode<T>*[Rank+2];
			//TreeNode<T>* Node[Rank + 2];
			TreeNode<T>* Node[defrank + 2];
			TreeNode<T>* t = p; // 当前节点
			while (t) {
				if (t->PID != -1)
					parent = node[t->PID];
				else parent = NULL;
				if (!newNode2) {// t是叶子
					if (parent) {
						//Leaf* tmp = static_cast<Leaf*>(node[parent->getChild(0)]);
						TreeNode<T> *tmp = node[parent->child[0]];
						for (int i = 0; i < parent->size + 2; i++) {
							Node[i] = tmp;
							if (tmp->next != -1)
								tmp = node[tmp->next];
						}
					}
					else {
						Node[0] = t;
						Node[1] = newNode;
					}
				}
				else { // t不是叶子
					if (parent) {
						count = 0;
						bool flag = false;
						TreeNode<T> *tmp = NULL;
						for (int i = 0; i < parent->size + 2; i++) {
							if (count < parent->size + 1)
								tmp = node[parent->child[count]];
							//tmp = static_cast<InnerNode*>(node[parent->child[count]]);
							if (tmp != NULL && !flag && ((newNode2->key[0] < tmp->key[0]) || (i == parent->size + 1))) {
								flag = true;
								Node[i] = newNode2;
							}
							else {
								Node[i] = node[parent->child[count++]];
							}
						}
					}
					else {
						Node[0] = t;
						Node[1] = newNode2;
					}
				}

				// case1 当前结点是根节点，根节点长高
				if (!parent) {
					//TreeNode<T> *r = new TreeNode<T>(inner,1);
					TreeNode<T> *r = node[cnt];
					//InnerNode* r = new InnerNode(Rank);
					//node[cnt] = r;
					r->ID = cnt;
					r->Type = inner;
					cnt++;

					for (int i = 0; i < 2; i++) {
						r->child[i] = Node[i]->ID;
						node[r->child[i]]->PID = r->ID;
					}

					r->key[0] = FindKey(node[r->child[1]]);
					root = r;
					root->PID = -1;
					//                    root->type = inner;
					break;
				}
				// case2 当前结点父节点不满
				else if (parent->size < Rank) {
					for (int i = 0; i < parent->size + 2; i++) {
						parent->child[i] = Node[i]->ID;
						node[parent->child[i]]->PID = parent->ID;
					}
					for (int i = 0; i < parent->size + 1; i++) {
						parent->key[i] = FindKey(node[parent->child[i + 1]]);
					}
					parent->size++;
					break;
				}
				// case3 父节点满 分裂父节点
				else if (parent->size == Rank) {
					newNode2 = node[cnt];
					newNode2->Type = inner;
					newNode2->size = Rank - MinSize;
					//newNode2 = new TreeNode<T>(inner, Rank - MinSize);
					//newNode2 = new InnerNode(Rank - MinSize);
					//node[cnt] = newNode2;
					newNode2->ID = cnt;
					cnt++;
					for (int i = 0; i < MinSize + 1; i++) {
						parent->child[i] = Node[i]->ID;
						node[parent->child[i]]->PID = parent->ID;
					}
					for (int i = 0; i < MinSize; i++) {
						parent->key[i] = FindKey(node[parent->child[i + 1]]);
					}
					parent->size = MinSize;
					count = 0;
					for (int i = MinSize + 1; i < Rank + 2; i++) {
						newNode2->child[count] = Node[i]->ID;
						node[newNode2->child[count++]]->PID = newNode2->ID;
					}
					count = 0;
					for (int i = MinSize + 1; i < Rank + 1; i++) {
						newNode2->key[count] = FindKey(node[newNode2->child[count + 1]]);
						count++;
					}
					newNode2->size = count;
					t = parent;
				}
			}
			//delete [] Node;
		}
		return;
	}

	void print() {
		if (!root) return;
		bool flag = false;
		TreeNode<T> *tmp;

		cout << Rank << " " << Type << endl;
		front = (front + 1) % 10000;
		queue[front] = root;
		int cur = 1;
		int curnum = 1;
		int nextnum = 0;
		while (front != rear) {
			rear = (rear + 1) % 10000;
			tmp = queue[rear];
			if (tmp->Type == inner) {
				cout << "[ 0 ";
				TreeNode<T> *t = tmp;
				//InnerNode *t = static_cast<InnerNode*>(tmp);
				curnum--;
				//                if(tmp->ParentId != -1){
				//                    cout << node[tmp->ParentId]->Id << " ";
				//                }else
				//                    cout << "-1 ";
				//                cout << t->Id << " " << tmp->size << " ";
				for (int i = 0; i < tmp->size - 1; i++)
					cout << tmp->key[i] << " ";
				cout << tmp->key[tmp->size - 1] << " ";

				for (int i = 0; i < tmp->size + 1; i++) {
					if (i == tmp->size) {
						if (t->child[i] != -1) {
							//cout << node[t->child[i]]->Id << "]" << endl;
							cout << "]" << endl;
							front = (front + 1) % 10000;
							queue[front] = node[t->child[i]];
							nextnum++;
						}
						else cout << "-1";
					}
					else {
						if (t->child[i] != -1) {
							//cout << node[t->child[i]]->Id << " ";
							front = (front + 1) % 10000;
							queue[front] = node[t->child[i]];
							nextnum++;
						}
						else cout << "-1";
					}
				}
				if (curnum == 0) {
					cur++;
					curnum = nextnum;
					nextnum = 0;
				}

			}
			else {
				cout << "[ 1 ";
				//                if(tmp->ParentId != -1)
				//                    cout << node[tmp->ParentId]->Id << " ";
				//                else
				//                    cout << "-1 ";
				//  cout << tmp->Id << " " << tmp->size << " " << tmp->getLast() << " " << tmp->getNext() << " ";
				//                for(int i = 0; i < tmp->size - 1; i++){
				//                    cout << tmp->getBlock(i) << " ";
				//                }
				//                cout << tmp->getBlock(tmp->size - 1) << " ";
				for (int i = 0; i < tmp->size - 1; i++) {
					cout << tmp->key[i] << " ";
				}
				cout << tmp->key[tmp->size - 1] << "]" << endl;
			}
		}
	}
	void Delete(T x) {
		//找到叶节点
		Index index = FindLeafNode(x, del);
		if (index == -1) return;
		TreeNode<T> *p = node[index];
		if (!p) return;
		T past; // 原索引值
		past = p->key[0];

		// 找到所在叶子节点下标
		int j = -1;
		for (int i = 0; i < p->size; i++) {
			if (x == p->key[i]) {
				j = i;
				break;
			}
		}

		// 用后面的值增补删除值的位置
		if (j == -1) return;
		for (int i = j + 1; i < p->size; i++) {
			p->setKey(p->key[i], i - 1);
			p->setBlock(p->blockNum[i], i - 1);
		}
		p->size--;

		// 删除后大小仍然大于最小值
		if (p->size >= MinSize) {
			if (p->PID != -1)
				UpdateIndex(p, x);
			return;
		}
		else if (p == root) {
			if (p->size == 0)
				root = NULL;
			return;
		}

		// 删除后变小 需要合并 向左借元素
		if (p->last != 0 && p->last != -1 && node[p->last]->size>MinSize) {
			if (p->PID != -1)
				UpdateIndex(p, x);
			for (int i = MinSize - 1; i > 0; i--) {
				p->key[i] = p->key[i - 1];
				p->blockNum[i] = p->blockNum[i - 1];
			}// 挪出一个位置
			p->key[0] = node[p->last]->key[node[p->last]->size - 1];
			p->blockNum[0] = node[p->last]->blockNum[node[p->last]->size - 1];

			T z = p->key[0];
			T y = p->key[1];
			node[p->last]->size--;
			p->size++;
			UpdateIndex(p, z, y);
			return;
		}
		else if (p->next != -1 && node[p->next]->size > MinSize) {
			if (p->PID != -1)
				UpdateIndex(p, x);
			T y = node[p->next]->key[0];
			T z = node[p->next]->key[1];

			p->key[MinSize - 1] = node[p->next]->key[0];
			p->blockNum[MinSize - 1] = node[p->next]->blockNum[0];
			p->size++;

			for (int i = 0; i < node[p->next]->size - 1; i++) {
				node[p->next]->setKey(node[p->next]->key[i + 1], i);
				node[p->next]->setBlock(node[p->next]->blockNum[i + 1], i);
			}
			node[p->next]->size--;
			UpdateIndex(node[p->next], z, y);
			return;
		}
		else {
			TreeNode<T> *t = NULL; // parent
			int count = 0;
			// 向左合并
			if (p->last != 0 && p->last != -1 && node[p->last]->size + p->size <= Rank) {
				t = node[p->PID];
				for (int i = node[p->last]->size; i < node[p->last]->size + p->size; i++) {
					node[p->last]->setKey(p->key[count], i);
					node[p->last]->setBlock(p->blockNum[count++], i);
				}

				node[p->last]->size = node[p->last]->size + p->size;

				// 删除p结点 维护双向链表
				node[p->last]->setNext(p->next);
				if (p->next != -1) node[p->next]->setLast(p->last);
				p->PID = -1;
				p->next = p->last = -1;
				//delete p;
			}
			// 向右合并
			else if (p->next != -1 && node[p->next]->size + p->size <= Rank) {
				count = 0;
				past = node[p->next]->key[0];
				t = node[node[p->next]->PID];
				for (int i = p->size; i < node[p->next]->size; i++) {
					p->setKey(node[p->next]->key[count], i);
					p->setBlock(node[p->next]->blockNum[count++], i);
				}

				p->size += node[p->next]->size;

				// 删除p next结点 维护双向链表
				if (node[p->next]->getNext() != -1)
					node[node[p->next]->getNext()]->setLast(p->ID);
				//Leaf *tmp = static_cast<Leaf*>(node[p->next]);
				TreeNode<T> *tmp = node[p->next];
				p->next = node[p->next]->getNext();
				tmp->PID = -1;
				tmp->last = tmp->next = -1;
				p->size = 0;
				//delete tmp;
			}

			t->size--;
			// UpdateIndex(t, past);
			UpdateChild(t, past);
			UpdateIndex(t, past);
			index = -1;

			while (t) {
				//InnerNode *parent;
				TreeNode<T> *parent = NULL;
				if (t->PID != -1)
					parent = node[t->PID];
				else
					parent = NULL;
				// case1 当前结点为根节点
				if (!parent) {
					if (t->size == 0) {
						root = node[t->child[0]];
						root->PID = -1;
					}
					break;
				}
				// case2 当前结点大于最小结点数
				else if (t->size >= MinSize - 1) {
					if (index != -1) {
						for (int i = index; i < t->size + 1; i++)
							t->child[i] = t->child[i + 1];
					}
					break;
				}

				// case3 当前结点小于最小结点数
				else if (t->size < MinSize - 1) {
					//InnerNode *sibling;
					TreeNode<T> *sibling;
					int last = -1, next = 1, now = 0;

					if (t->key[0] >= parent->key[0]) {
						for (int i = 0; i < parent->size; i++) {
							if (t->key[0] < parent->key[i]) {
								last = i - 1;
								now = i;
								next = i + 1;
								break;
							}
							else if (i == parent->size - 1) {
								last = parent->size - 1;
								now = parent->size;
								next = parent->size + 1;
							}
						}
					}
					if (last >= 0) {
						sibling = node[parent->child[last]];
						// 向左借节点
						if (sibling->size >= MinSize) {
							for (int i = t->size + 1; i >= 1; i--)
								t->child[i] = t->child[i - 1];
							for (int i = t->size; i >= 1; i--)
								t->key[i] = t->key[i - 1];
							t->child[0] = node[sibling->child[sibling->size]]->ID;
							node[t->child[0]]->PID = t->ID;

							t->key[0] = FindKey(node[t->child[1]]);

							// 更新父节点
							node[t->PID]->setKey(FindKey(node[node[t->PID]->child[now]]), last);
							sibling->child[sibling->size] = -1;
							sibling->size--;

							t->size++;
							break;
						}
						else if (t->size + sibling->size + 1 <= Rank) { // 和左节点合并
							count = 0;
							for (int i = sibling->size + 1; i < t->size + sibling->size + 2; i++) {
								sibling->child[i] = t->child[count++];
								node[sibling->child[i]]->PID = sibling->ID;
							}
							for (int i = sibling->size; i < t->size + sibling->size + 1; i++) {
								sibling->key[i] = FindKey(node[sibling->child[i + 1]]);
							}
							sibling->size = t->size + sibling->size + 1;

							t->PID = -1;
							t->last = t->next = -1;
							//  delete t;

							for (int i = now; i < parent->size; i++)
								parent->child[i] = parent->child[i + 1];
							for (int i = last; i < parent->size - 1; i++)
								parent->key[i] = FindKey(node[parent->child[i + 1]]);
							parent->size--;
							t = parent;
						}
					}
					else if (next <= Rank + 1) {
						sibling = node[parent->child[next]];
						// 向右借结点
						if (sibling->size >= MinSize) {
							t->child[t->size + 1] = sibling->child[0];
							node[t->child[t->size + 1]]->PID = t->ID;
							t->key[t->size] = FindKey(node[t->child[t->size + 1]]);
							for (int i = 0; i < sibling->size; i++)
								sibling->child[i] = sibling->child[i + 1];
							for (int i = 0; i < sibling->size - 1; i++)
								sibling->key[i] = FindKey(node[sibling->child[i + 1]]);

							node[t->PID]->setKey(FindKey(node[node[t->PID]->child[next]]), now);
							sibling->child[sibling->size] = -1;
							sibling->size--;
							t->size++;
							break;
						}
						// 与右结点合并
						else if (t->size + sibling->size + 1 <= Rank) {
							count = 0;
							for (int i = t->size + 1; i < t->size + sibling->size + 2; i++) {
								t->child[i] = sibling->child[count++];
								node[t->child[i]]->PID = t->ID;
							}
							for (int i = t->size; i < t->size + sibling->size + 1; i++) {
								t->key[i] = FindKey(node[t->child[i + 1]]);
							}
							t->size += sibling->size + 1;

							sibling->PID = -1;
							sibling->last = sibling->next = -1;
							//delete sibling;

							for (int i = next; i < parent->size; i++)
								parent->child[i] = parent->child[i + 1];
							for (int i = now; i < parent->size - 1; i++)
								parent->key[i] = FindKey(node[parent->child[i + 1]]);
							parent->size--;
							t = parent;
						}
					}
				}
			}
		}
	}

	void notEqual(int *buffer, T x) {
		int count = 0;
		TreeNode<T> *p = FindLeafNode(x);
		while (1) {
			for (int i = 0; i < p->size; i++) {
				if (x < p->key[i])
					buffer[++count] = p->blockNum[i];
			}
			if (p->next != -1)
				p = node[p->next];
			else break;
		}
		p = FindLeafNode(x);
		while (1) {
			for (int i = 0; i < p->size; i++) {
				if (x > p->key[i])
					buffer[++count] = p->blockNum[i];
			}
			if (p->last != -1)
				p = node[p->last];
			else break;
		}
		buffer[0] = count;
	}

	void at(int index) {
		TreeNode<T> *tmp = childNode;
		for (int i = 0; i < index + 1; i++)
			tmp = node[tmp->next];
		for (int i = 0; i < tmp->size; i++)
			cout << tmp->key[i] << " ";
		cout << endl;
	}

	void save() { // 持久化存储
		bool flag = false;
		TreeNode<T> *tmp;

		char pathname[256] = "miniSQL\\";
		strcat(pathname, getDBName().c_str());
		strcat(pathname, "_index");
		if (_access(pathname, 00) == 0) {
			strcat(pathname, "\\");
			strcat(pathname, getIndexName().c_str());
			strcat(pathname, ".index");
		}
		else if (_mkdir(pathname) == 0) {
			strcat(pathname, "\\");
			strcat(pathname, getIndexName().c_str());
			strcat(pathname, ".index");
		}

		ofstream indexout(pathname, ios::trunc | ios::in | ios::out);
		indexout << Rank << " ";
		indexout << Type << " " << getDBName() << " " << getTableName() << " " << getIndexName() << " ";
		indexout << getAttrName() << endl;
		if (!root) return;
		front = (front + 1) % 10000;
		queue[front] = root;
		int cur = 1;
		int curnum = 1;
		int nextnum = 0;

		while (front != rear) {
			rear = (rear + 1) % 10000;
			tmp = queue[rear];
			if (tmp->Type == inner) {
				TreeNode<T> *t = tmp;
				curnum--;
				indexout << tmp->Type << " ";
				if (tmp->PID != -1)
					indexout << node[tmp->PID]->ID << " ";
				else
					indexout << "-1 ";
				indexout << t->ID << " " << tmp->size << " ";
				for (int i = 0; i < tmp->size - 1; i++)
					indexout << tmp->key[i] << " ";
				indexout << tmp->key[tmp->size - 1] << " ";
				for (int i = 0; i < tmp->size + 1; i++) {
					if (i == tmp->size) {
						if (t->child[i] != -1) {
							indexout << node[t->child[i]]->ID << endl;
							front = (front + 1) % 10000;
							queue[front] = node[t->child[i]];
							nextnum++;
						}
						else indexout << "-1" << endl;
					}
					else {
						if (t->child[i] != -1) {
							indexout << node[t->child[i]]->ID << " ";
							front = (front + 1) % 10000;
							queue[front] = node[t->child[i]];
							nextnum++;
						}
						else indexout << "-1 ";
					}
				}
				if (curnum == 0) {
					cur++;
					curnum = nextnum;
					nextnum = 0;
				}
			}
			else {
				indexout << tmp->Type << " ";
				if (tmp->PID != -1)
					indexout << node[tmp->PID]->ID << " ";
				else
					indexout << "-1 ";
				indexout << tmp->ID << " ";
				indexout << tmp->size << " ";
				indexout << tmp->getLast() << " " << tmp->getNext() << " ";
				for (int i = 0; i < tmp->size - 1; i++)
					indexout << tmp->blockNum[i] << " ";
				indexout << tmp->blockNum[tmp->size - 1] << " ";
				for (int i = 0; i < tmp->size - 1; i++)
					indexout << tmp->key[i] << " ";
				indexout << tmp->key[tmp->size - 1] << endl;
			}
		}
		indexout.close();
	}

	void loadFloat(string s, bool isroot) { //加载
		float a[50];
		int i = 0;
		int j = 0;
		string str = "";
		while (s[i] != '\0') {
			if (s[i] != ' ')
				str += s[i];
			else if (s[i] == ' ') {
				a[j++] = (float)atof(str.c_str());
				str = "";
			}
			i++;
		}
		a[j++] = atof(str.c_str());
		node[(int)a[2]]->ID = (int)a[2];
		node[(int)a[2]]->PID = (int)a[1];
		if (isroot)
			root = node[(int)a[2]];
		if (a[0] == 1) {
			node[(int)a[2]]->Type = leaf;
			node[(int)a[2]]->last = (int)a[4];
			node[(int)a[2]]->next = (int)a[5];
			for (int k = 0; k < a[3]; k++) {
				node[(int)a[2]]->blockNum[k] = (int)a[k + 6];
				node[(int)a[2]]->key[k] = a[k + 6 + (int)a[3]];
			}
		}
		else {
			node[(int)a[2]]->Type = inner;
			for (int k = 0; k < a[3]; k++) {
				node[(int)a[2]]->key[k] = a[k + 4];
				node[(int)a[2]]->child[k] = (int)a[k + 4 + (int)a[3]];
			}
			node[(int)a[2]]->child[(int)a[3]] = (int)a[2 * (int)a[3] + 4];
		}
		node[(int)a[2]]->size = (int)a[3];
	}

	void loadString(string s, bool isroot) { //加载
		string a[50];
		int i = 0;
		int j = 0;
		string str = "";
		while (s[i] != '\0') {
			if (s[i] != ' ')
				str += s[i];
			else if (s[i] == ' ') {
				a[j++] = str;
				str = "";
			}
			i++;
		}
		a[j++] = str;
		node[atoi(a[2].c_str())]->ID = atoi(a[2].c_str());
		node[atoi(a[2].c_str())]->PID = atoi(a[1].c_str());
		if (isroot)
			root = node[atoi(a[2].c_str())];
		if (atoi(a[0].c_str()) == 1) {
			node[atoi(a[2].c_str())]->Type = leaf;
			node[atoi(a[2].c_str())]->last = atoi(a[4].c_str());
			node[atoi(a[2].c_str())]->next = atoi(a[5].c_str());
			for (int k = 0; k < atoi(a[3].c_str()); k++) {
				node[atoi(a[2].c_str())]->blockNum[k] = atoi(a[k + 6].c_str());
				node[atoi(a[2].c_str())]->key[k] = a[k + 6 + atoi(a[3].c_str())];
			}
		}
		else {
			node[atoi(a[2].c_str())]->Type = inner;
			for (int k = 0; k < atoi(a[3].c_str()); k++) {
				node[atoi(a[2].c_str())]->key[k] = a[k + 4];
				node[atoi(a[2].c_str())]->child[k] = atoi(a[k + 4 + atoi(a[3].c_str())].c_str());
			}
			node[atoi(a[2].c_str())]->child[atoi(a[3].c_str())] = atoi(a[2 * atoi(a[3].c_str()) + 4].c_str());
		}
		node[atoi(a[2].c_str())]->size = atoi(a[3].c_str());
	}

	void loadInt(string s, bool isroot) {
		int a[50];
		int i = 0;
		int j = 0;
		string str = "";
		while (s[i] != '\0') {
			if (s[i] != ' ')
				str += s[i];
			else if (s[i] == ' ') {
				a[j++] = atoi(str.c_str());
				str = "";
			}
			i++;
		}
		a[j++] = atoi(str.c_str());
		node[a[2]]->ID = a[2];
		node[a[2]]->PID = a[1];
		if (isroot)
			root = node[a[2]];
		if (a[0] == 1) {
			node[a[2]]->Type = leaf;
			node[a[2]]->last = a[4];
			node[a[2]]->next = a[5];
			for (int k = 0; k < a[3]; k++) {
				node[a[2]]->blockNum[k] = a[k + 6];
				node[a[2]]->key[k] = a[k + 6 + a[3]];
			}
		}
		else {
			node[a[2]]->Type = inner;
			for (int k = 0; k < a[3]; k++) {
				node[a[2]]->key[k] = a[k + 4];
				node[a[2]]->child[k] = a[k + 4 + a[3]];
			}
			node[a[2]]->child[a[3]] = a[2 * a[3] + 4];
		}
		node[a[2]]->size = a[3];
	}

	int equal(T x) { // 找到x所在的块号
		TreeNode<T> *p = root;
		Index j = 1;
		if (!p) return -1;
		while (p->Type == inner) {
			//已知p为内部结点，向下造型为内部结点
			TreeNode<T> *t = p;
			if (x < t->key[0]) {
				j = t->child[0];
				p = node[j];
			}
			else {
				for (int i = 0; i < t->size - 1; i++) {
					if (x >= t->key[i] && x < t->key[i + 1]) {
						j = t->child[i + 1];
						p = node[j];
						break;
					}
				}
				if (x >= t->key[t->size - 1]) {
					j = t->child[t->size];
					p = node[j];
				}
			}
		}

		for (int i = 0; i < p->size; i++) {
			if (p->key[i] == x)
				return p->blockNum[i];
		}
		return -1;
	}

	void larger(int *buffer, T x) {   // 返回比x键值大的键值所在块号数组
		int count = 0;
		TreeNode<T> *p = FindLeafNode(x);
		while (1) {
			for (int i = 0; i < p->size; i++) {
				if (x < p->key[i])
					buffer[++count] = p->blockNum[i];
			}
			if (p->next != -1)
				p = node[p->next];
			else break;
		}
		buffer[0] = count;
	}

	void smaller(int *buffer, T x) {
		int count = 0;
		TreeNode<T> *p = FindLeafNode(x);
		while (1) {
			for (int i = 0; i < p->size; i++) {
				if (x > p->key[i])
					buffer[++count] = p->blockNum[i];
			}
			if (p->last != 0)
				p = node[p->last];
			else break;
		}
		buffer[0] = count;
	}

	void largerAndEqual(int *buffer, T x) {
		int count = 0;
		TreeNode<T> *p = FindLeafNode(x);
		while (1) {
			for (int i = 0; i < p->size; i++) {
				if (x <= p->key[i])
					buffer[++count] = p->blockNum[i];
			}
			if (p->next != -1)
				p = node[p->next];
			else break;
		}
		buffer[0] = count;
	}

	void smallerAndEqual(int *buffer, T x) {
		int count = 0;
		TreeNode<T> *p = FindLeafNode(x);
		while (1) {
			for (int i = 0; i < p->size; i++) {
				if (x >= p->key[i])
					buffer[++count] = p->blockNum[i];
			}
			if (p->last != 0)
				p = node[p->last];
			else break;
		}
		buffer[0] = count;
	}

	void deleteLarger(int *buffer, T x) {
		int count = 0;
		int count2 = 0;
		T temp[100000];
		TreeNode<T> *p = FindLeafNode(x);
		while (1) {
			for (int i = 0; i < p->size; i++) {
				if (x < p->key[i]) {
					temp[++count] = p->key[i];
					buffer[++count2] = p->blockNum[i];
				}
			}
			if (p->next != -1)
				p = node[p->next];
			else break;
		}
		buffer[0] = count2;
		for (int i = 1; i <= count; i++)
			Delete(temp[i]);
	}

	void deleteSmaller(int *buffer, T x) {
		int count = 0;
		int count2 = 0;
		T temp[100000];
		TreeNode<T> *p = FindLeafNode(x);
		while (1) {
			for (int i = 0; i < p->size; i++) {
				if (x > p->key[i]) {
					temp[++count] = p->key[i];
					buffer[++count2] = p->blockNum[i];
				}
			}
			if (p->last != -1)
				p = node[p->last];
			else break;
		}
		buffer[0] = count2;
		for (int i = 1; i <= count; i++)
			Delete(temp[i]);
	}

	void deleteLargerAndEqual(int *buffer, T x) {
		int count = 0;
		int count2 = 0;
		T temp[100000];
		TreeNode<T> *p = FindLeafNode(x);
		while (1) {
			for (int i = 0; i < p->size; i++) {
				if (x <= p->key[i]) {
					temp[++count] = p->key[i];
					buffer[++count2] = p->blockNum[i];
				}
			}
			if (p->next != -1)
				p = node[p->next];
			else break;
		}
		buffer[0] = count2;
		for (int i = 1; i <= count; i++)
			Delete(temp[i]);
	}

	void deleteSmallerAndEqual(int *buffer, T x) {
		int count = 0;
		int count2 = 0;
		T temp[100000];
		TreeNode<T> *p = FindLeafNode(x);
		while (1) {
			for (int i = 0; i < p->size; i++) {
				if (x >= p->key[i]) {
					temp[++count] = p->key[i];
					buffer[++count2] = p->blockNum[i];
				}
			}
			if (p->last != -1)
				p = node[p->last];
			else break;
		}
		buffer[0] = count2;
		for (int i = 1; i <= count; i++)
			Delete(temp[i]);
	}

};

#endif //INDEX_INDEX_H
