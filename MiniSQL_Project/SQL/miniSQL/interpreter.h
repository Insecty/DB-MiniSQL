#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include "GlobalValue.h"
#include <iostream>
#include <string>

using namespace std;
class base;
class Command;
class nRecord;
class nCondition;
class nUpdate;
class nIndex;

//����������ö�ٴ���
#define DataBase base
#define nTable base
#define nCreateIndex nIndex
#define nSelect nCondition
#define nDelete nCondition
#define nDropDB base
#define nDropTB base
#define nDropIndex nIndex
#define nSourceFile base
//#define nRecordType union DataType

enum InterError {
	errCreate, errName, errDrop,errNoLBrace,errNoRBrace,
	errTableName,errNoName, errNoType, errSetPrimary,
	errNoEnd,errWrongData
}; 
typedef struct TableCol *pTableCol;
typedef struct RecordCol *pRecordCol;
typedef struct Condition *pCondition;

class nIndex
{
	friend class Command;
public:
	nIndex() {};
	~nIndex() {};
	string getTbName() { return TbName; }
	void writeTbName(string name) { TbName = name; }
	string getIndexName() { return IndexName; }
	void writeIndexName(string name) { IndexName = name; }
	string getAttrName() { return AttributeName; }
	void writeAttrName(string name) { AttributeName = name; }
private:
	string TbName;
	string IndexName;
	string AttributeName;
};


class Command
{
	friend class cTable;
public:	
	char command[200];
	Command(char* cmd);
	~Command();
	string ReadCommand(char EndSymbol, char dEndSymbol = ';');
	MsgType getCmdType() { return CMDtype; }	
	bool isEnd() { return (length == position); }
	int CreateDatabase(DataBase &nCreateDB);
	int CreateTable(cTable &nCreateTB);
	int CreateIndex(nIndex &nCreateIndex);
	int UseDB(DataBase &nUseDB);
	int InsertTable(nRecord &nInsertTB);
	int SelectResult(nSelect &nSelectData);
	int DeleteResult(nDelete &nDelData);
	int UpdateResult(nUpdate &nUpdateData);
	int DropDB(nDropDB &nDropDbData);
	int DropIndex(nDropIndex &nDropIndexData);
	int DropTB(nDropTB &nDropTbData);
	int SourceFile(nSourceFile &nSourceName);
	template<typename T>
	int setName(T &nData, char ch1 = ';', char ch2 = ';');

private:
	int length;
	int position;
	MsgType CMDtype;
	MsgType CmdType();
	int SkipCharacter(char ch1, char ch2 = ';');
	void CalLength();
	bool SetRecordValue(string name, nRecord &nInsertTB);
	bool SetRecordValue(string name, pRecordCol &nSelectTB);
	float stringTofloat(string name);
	int stringToint(string name);
};

class base
{
	friend class Command;
public:
	base() {};
	~base() {};
	string getName() { return Name; }
	void writeName(string name) { Name = name; }
private:
	string Name;
};

class cTable {
	friend class Command;
public:
	string TBname;	//����
	int TalColumn;	//�ֶ�������
	pTableCol head;	//ͷָ��
	cTable();
	~cTable();
	string getName() { return TBname; }
	void writeName(string name) { TBname = name; }
	
private:	
	pTableCol end;
};

struct  TableCol {
	int		  colnum;		//�б��
	string    name;			//������
	eColType  ColType;		//���� DEFAULT - 0,int - 1,char - 2��float - 3
	int       length;		//char����
	bool      isDiffer;		//unique Ψһ��
	bool      isPrimary;	//primary key ����
	pTableCol Next;			//next attribute ��һ������
};
/*
union DataType {
	char nChar[255];
	int nInt;
	float nFloat;
};//ÿ�������Ĵ�С����255�ֽ�
*/

struct RecordCol {
	int colnum;
	string name;			//������
	eColType  ColType;		//���� DEFAULT - 0,int - 1,char - 2��float - 3
	//nRecordType nRecordData;
	int nInt;
	float nFloat;
	string nChar;
	int length;
	pRecordCol Next;
};

class nRecord
{
public:
	nRecord();
	~nRecord();
	inline void NumPlus() { num++; }
	inline int getNum() { return num; }
	inline string getName() { return TBname; }
	inline void writeName(string name) { TBname = name; }
	inline void writeSize(int data) { size = data; }
	inline int addSize(int num) { return size+num; }
	//inline void writeRecord(string record) { data = record; }
	//inline string getRecord() { return data; }
	inline pRecordCol getHead() { return head; }
	pRecordCol end;
private:
	int size;			//��¼record �ĳ���
	//string data;		//��¼record �����ݣ���ȥ�ո�-����-����
	int num;			//��¼record������
	pRecordCol head;	//��¼�е�head
	string TBname;		//��¼�������
};

struct Condition {
	SignType symbol;	//��¼��������
	pRecordCol record;	//��¼��ֵ������
	pCondition Next;	
};
class nCondition
{
public:
	nCondition();
	~nCondition();
	inline bool getWhere() { return isWhere; }
	inline bool getAll() { return isAll; }
	inline pCondition getConditionHead() { return chead; }
	inline pTableCol getRecordHead() { return rhead; }
	inline string getName() { return TBname; }
	inline void setAll() { isAll = 1; }
	inline void setWhere() { isWhere = 1; }
	inline void writeName(string name) { TBname = name; }
	pTableCol   rend;	//��¼ɸѡ���е�head
	pCondition  cend;	//��¼ɸѡ������head
private:
	bool isAll;			//��¼�Ƿ�Ϊ���м�¼
	bool isWhere;		//��¼�Ƿ���where�־�
	pTableCol  rhead;	//��¼ɸѡ���е�head
	pCondition chead;	//��¼ɸѡ������head
	string TBname;		//��¼�������

};
class nUpdate
{
public:
	nUpdate();
	~nUpdate();
	inline pCondition getConditionHead() { return chead; }
	inline pCondition getRecordHead() { return rhead; }
	inline string getName() { return TBname; }
	inline void writeName(string name) { TBname = name; }
	pCondition  rend;	//��¼ɸѡ���е�head
	pCondition  cend;	//��¼ɸѡ������head
private:
	pCondition rhead;	//��¼ɸѡ���е�head
	pCondition chead;	//��¼ɸѡ������head
	string TBname;		//��¼�������
};





MsgType Resolution(char* command,cTable &nCreateTB, nIndex &nIndexData, base &nData, nRecord &nRecordData, nCondition &nConData, nUpdate &nUpdateData);
bool isKeyword(string a);
bool CheckWord(string &a, bool flag = 0);
eColType getColType(string a,int &len);
int getLength(string a);
int setPrimary(cTable &nCreate,string a);
bool checkRecord(nRecord &record);
eSign ReadSign(string code);
#endif // INTERPRETER_H_
