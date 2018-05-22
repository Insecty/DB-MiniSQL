#include <ctime>
#include <string>
#include <stack>
#include <set>
#include <cstdlib>
#include <fstream>

#include "GlobalValue.h"
using namespace std;
#define BLOCKSIZE 4096
#define MAXBLOCKNUM 200

#ifndef DBFILES_H
#define DBFILES_H

class Block;
class TableFile;

// write through
class Block
{
public:
	int ID;
	int RecordSize;
	int MaxNum;
	int RecordNum;
	time_t Time;
	char Data[BLOCKSIZE];  //4096 Bytes    use memcpy to write data

	Block();
	Block(int RecordSize);
	Block(const Block& B);
	void WriteBlockToDisk();
	static Block ReadBlockFromDisk(int id);
	void InsertRecord(char* R);
	void RemoveRecord(int Offset);
};


class TableFile
{
public:
	char TableName[20];

	bool TableBlock[MAXBLOCKNUM + 1];
	bool NotFull[MAXBLOCKNUM + 1];

	TableFile();
	TableFile(string Name);
	TableFile(const TableFile& T);
	~TableFile();
	void AddBlockToTable(Block& B);
	void RemoveBlockFromTable(int BlockID);
	void WriteTableToDisk();
	static TableFile ReadTableFromDisk(string Name);
	int GetNotFullBlock(int RecordSize);
};

#endif