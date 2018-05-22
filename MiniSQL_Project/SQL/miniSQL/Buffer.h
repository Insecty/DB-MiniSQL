#ifndef BUFFER_H_
#define BUFFER_H_
#include "GlobalValue.h"
#include "DBfiles.h"
#define BUFFERBLOCKNUM 50


typedef enum eColType2
{
	eColDefault = 0, Int = 1, Char = 2, Float = 3
}eColType2;

class Buffer;
class TheSelectResult;

class Buffer
{
public:
	Block Cache[BUFFERBLOCKNUM];
	bool Valid[BUFFERBLOCKNUM];


	Buffer();
	bool Hit(int BlockID);
	int GetLRU();
	int Index(int BlockID);

	int Insert(string Name, int RecordSize, char* R);  //后两个参数可以用链表替换
	TheSelectResult Delete(string Name, int BlockID[], int Offset, int Size, char* Key, eColType2 type, eSign op);
	TheSelectResult Select(string Name, int BlockID[], int Offset, int Size, char* Key, eColType2 type, eSign op);
};

class TheSelectResult
{
public:
	int RecordSize;
	int RecordNum;
	char* Result;

	TheSelectResult();
	TheSelectResult(int RecordSize);
	~TheSelectResult();
	TheSelectResult(const TheSelectResult& R);
};


extern Buffer bufferMan;
#endif
