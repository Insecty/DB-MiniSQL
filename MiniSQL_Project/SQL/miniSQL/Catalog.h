#ifndef CATALOG_H_
#define CATALOG_H_
#include"interpreter.h"
#include <string>
#define pCatDBnode pDBnode
#define pCatTBnode pTBnode
#define pCatIndexNode pIndexNode

typedef struct CatDBnode *pCatDBnode;
struct CatDBnode
{
	string name;
	int TableNum;
	int IndexNum;
	pCatDBnode Next;
};
class DatabaseInfo
{
public:
	DatabaseInfo(string path);
	DatabaseInfo() :DBnum(0) { head = new CatDBnode; head->Next = NULL; end = head; }
	~DatabaseInfo();
	void DBUpdate(string path);
	inline pDBnode getHead() { return head; }
	inline int getNum() { return DBnum; }
	inline void NumPlus() { DBnum++; }
	inline void NumSub() { DBnum--; }
	pDBnode end;
private:
	int DBnum;
	pDBnode head;
};

typedef struct CatTBnode *pCatTBnode;
struct CatTBnode
{
//	string DBname;
	string TBname;
	int AttributeNum;
	int RecordNum;
	pTableCol head;
	pCatTBnode Next;
};

class TableInfo
{
public:
	TableInfo(string path);
	TableInfo() :TBnum(0), IndexNum(0){ head = new CatTBnode; head->Next = NULL;
							head->head = new TableCol; head->head->Next = NULL; 
							end = head; }
	~TableInfo();
	void TableUpdate(string path);
	inline int getTbNum() { return TBnum; }
	inline int getIndexNum() { return IndexNum; }
	inline void TbNumPlus(int i = 1) { TBnum += i; }
	inline void TbNumSub(int i = 1) { TBnum -= i; }
	inline void IndexNumPlus(int i = 1) { IndexNum += i; }
	inline void IndexNumSub(int i = 1) { IndexNum -= i; }
	inline pTBnode getHead() { return head; }
	pTBnode end;
private:
	int TBnum;
	int IndexNum;
	pTBnode head;
};
typedef struct CatIndexNode *pCatIndexNode;
struct CatIndexNode
{
	string IndexName;
	string DBname;
	string TableName;
	pIndexNode Next;
};
/*
class IndexInfo
{
public:
	IndexInfo();
	~IndexInfo();

private:
	int IndexNum;
	IndexNode head;
	IndexNode end;
};

IndexInfo::IndexInfo()
{
}

IndexInfo::~IndexInfo()
{
}
*/


string sReadFile(FILE *fp,char ch1,char ch2=EOF);
int iReadFile(FILE *fp, char ch1, char ch2 = EOF);
float fReadFile(FILE *fp, char ch1, char ch2 = EOF);
pTBnode TableToNode(cTable& table);


#endif // !CATALOG_H_
