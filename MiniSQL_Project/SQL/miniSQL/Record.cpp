#include "Record.h"
#include "io.h"
using namespace std;

vector<BPlusTree<int>*> IndexInt(10);
vector<BPlusTree<float>*> IndexFloat(10);
vector<BPlusTree<string>*> IndexStr(10);
vector<string> index(10);

void initLoadIndex(string dbname) { // 在程序启动（use db)的时候加载所有存在的索引树

	char pathname[256] = "miniSQL\\";
	strcat(pathname, dbname.c_str());
	strcat(pathname, "_index");

	vector<string> filelist;
	string temp_name;

	long hFile = 0;
	_finddata_t fileinfo;
	char p[256];
	strcpy(p, pathname);
	strcat(p, "\\*");
	if ((hFile = _findfirst(p, &fileinfo)) != -1)
	{
		do {
			if ((fileinfo.attrib & _A_SUBDIR)) {
				continue;
			}
			else {
				temp_name.assign(pathname).append("\\").append(fileinfo.name);
				filelist.push_back(temp_name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}

	for (int i = 0; i < int(filelist.size()); i++) {
		Load(filelist[i]);
	}
}

void Load(string path) {
	//加载一棵B+树
	BPlusTree<int> *pi = NULL;
	BPlusTree<float> *pf = NULL;
	BPlusTree<string> *ps = NULL;
	ifstream in;
	in.open(path, ios::in);
	string str;
	string type;
	string dbname;
	string tablename;
	string indexname;
	string attrname;
	int rank;
	bool isroot = true;
	in >> rank >> type >> dbname >> tablename >> indexname >> attrname;
	if (type == "int") {
		pi = new BPlusTree<int>(rank, dbname, tablename, indexname, attrname, type);
	}
	else if (type == "float") {
		pf = new BPlusTree<float>(rank, dbname, tablename, indexname, attrname, type);
	}
	else if (type == "string") {
		ps = new BPlusTree<string>(rank, dbname, tablename, indexname, attrname, type);
	}
	getline(in, str);
	while (!in.eof()) {
		getline(in, str);
		if (str == "")
			break;
		if (type == "int") {
			pi->loadInt(str, isroot);
			isroot = false;
		}
		else if (type == "float") {
			pf->loadFloat(str, isroot);
			isroot = false;
		}
		else if (type == "string") {
			ps->loadString(str, isroot);
		}
	}
	if (type == "int")
		IndexInt.push_back(pi);
	else if (type == "float")
		IndexFloat.push_back(pf);
	else if (type == "string")
		IndexStr.push_back(ps);
}

/*
mode-1 primary
mode-2 unique
*/
void createIndexForPrim(string tablename, string attr, string type, int mode) {
	BPlusTree<int> *pi;
	BPlusTree<float> *	pf;
	BPlusTree<string> *ps;

	string indexname;
	string dbname = curDBname;
	switch (mode) {
	case 1:
		indexname = "Prim" + tablename;
		break;
	case 2:
		indexname = "Uniq" + tablename;
		break;
	}
	if (type == "int") {
		pi = new BPlusTree<int>(defrank, dbname, tablename, indexname, attr, type);
		IndexInt.push_back(pi);
		return;
	}
	else if (type == "float") {
		pf = new BPlusTree<float>(defrank, dbname, tablename, indexname, attr, type);
		IndexFloat.push_back(pf);
		return;
	}
	else if (type == "string") {
		ps = new BPlusTree<string>(defrank, dbname, tablename, indexname, attr, type);
		IndexStr.push_back(ps);
		return;
	}
}

void Select()
{
	bool indexflag = false;
	bool conditionflag = false;

	if (conditionflag == false) // 不带条件查询
	{
		// -->直接调用buffer 全输出
	}
	else { // 带条件查询
		   //indexflag = hadIndex();
		   // 属性type 
		string AttrType;

		BPlusTree<int> *pint = NULL;
		BPlusTree<float> *pfloat = NULL;
		BPlusTree<string> *pstr = NULL;

		if (indexflag == false) { // 不带索引
								  // -->直接调用bufefer 一个一个筛选
		}
		else { // 带索引
			   // 找到索引所在的b+树 调用写好的函数返回块号数组
			if (AttrType == "int") {
				for (int i = 0; i < (int)IndexInt.size(); i++) {
					if (IndexInt[i]->getTableName() == "xxx" && IndexInt[i]->getAttrName() == "xxx") {
						pint = IndexInt[i];
						break;
					}
				}
			}
			else if (AttrType == "float") {
				for (int i = 0; i < (int)IndexFloat.size(); i++) {
					if (IndexFloat[i]->getTableName() == "xxx" && IndexFloat[i]->getAttrName() == "xxx") {
						pfloat = IndexFloat[i];
						break;
					}
				}
			}
			else if (AttrType == "string") {
				for (int i = 0; i < (int)IndexStr.size(); i++) {
					if (IndexStr[i]->getTableName() == "xxx" && IndexStr[i]->getAttrName() == "xxx") {
						pstr = IndexStr[i];
						break;
					}
				}
			}


			int *blockID;
			blockID = new int[200];
			// 等于查找    // 参数都需要键值

			// 大于查找

			// 小于查找

			// 大于等于查找

			// 小于等于查找
		}
	}
	//delete p;
}

bool hadIndex(string tablename, string attrName)
{
	for (size_t i = 0; i < IndexInt.size(); i++) {
		if (IndexInt[i]->getAttrName() == attrName && IndexInt[i]->getTableName() == tablename)
			return true;
	}
	for (size_t i = 0; i < IndexFloat.size(); i++) {
		if (IndexFloat[i]->getAttrName() == attrName && IndexFloat[i]->getTableName() == tablename)
			return true;
	}
	for (size_t i = 0; i < IndexStr.size(); i++) {
		if (IndexStr[i]->getAttrName() == attrName && IndexStr[i]->getTableName() == tablename)
			return true;
	}
	return false;
}