#include "DBfiles.h"
#include"GlobalValue.h"
#include <cstring>
#include<iostream>

bool Available[MAXBLOCKNUM + 1] = {};  //false represents available

Block::Block()  //default constructor
{

}

Block::Block(int RecordSize)
{
	ifstream BlockInfo("BlockInfo", ios::binary | ios::in);
	BlockInfo.read((char*)Available, sizeof(Available));
	BlockInfo.close();
	for (int i = 1; i <= MAXBLOCKNUM; i++)
	{
		if (!Available[i])
		{
			ID = i;
			Available[i] = true;
			break;
		}
	}
	this->RecordSize = RecordSize;
	MaxNum = BLOCKSIZE / RecordSize;
	RecordNum = 0;
	time(&Time);
}

Block::Block(const Block& B)
{
	this->ID = B.ID;
	this->RecordSize = B.RecordSize;
	this->MaxNum = B.MaxNum;
	this->RecordNum = B.RecordNum;

	this->Time = B.Time;
	if(this->Data&&B.Data)
	memcpy(this->Data, B.Data, BLOCKSIZE);
}

void Block::WriteBlockToDisk()
{
	ofstream BlockFile;
	string BlockFileName = "#";
	char buffer[10];
	_itoa(ID, buffer, 10);
	string temp = buffer;
	BlockFileName += temp;
	BlockFileName += ".block";
	BlockFile.open(BlockFileName, ios::trunc | ios::binary);
	BlockFile.write((char*)this, sizeof(Block));
	BlockFile.close();

	ofstream BlockInfo("BlockInfo", ios::trunc | ios::binary);
	BlockInfo.write((char*)Available, sizeof(Available));
	BlockInfo.close();
}

Block Block::ReadBlockFromDisk(int id)
{
	string BlockFileName = "#";
	char buffer[10];
	_itoa(id, buffer, 10);
	string temp = buffer;
	BlockFileName += temp;
	BlockFileName += ".block";
	ifstream BlockFile(BlockFileName, ios::binary | ios::in);
	Block Result;
	BlockFile.read((char*)&Result, sizeof(Block));
	BlockFile.close();
	return Result;
}

void Block::InsertRecord(char* R)
{
	memcpy(Data + RecordSize*RecordNum, R, (size_t)RecordSize);
	RecordNum++;
}

void Block::RemoveRecord(int Offset)
{
	for (int i = Offset; i<RecordNum - 1; i++)
	{
		memcpy(Data + i*RecordSize, Data + (i + 1)*RecordSize, (size_t)RecordSize);
	}
	RecordNum--;
}


TableFile::TableFile()
{

}

TableFile::TableFile(string Name)
{
	int i;
	for (i = 0; i < Name.size(); i++)
	{
		TableName[i] = Name.at(i);
	}
	TableName[i] = '\0';
	for (int i = 1; i <= MAXBLOCKNUM; i++)
	{
		TableBlock[i] = false;
		NotFull[i] = false;
	}
	WriteTableToDisk();
}

TableFile::TableFile(const TableFile& T)
{
	strcpy(this->TableName, T.TableName);

	for (int i = 0; i <= MAXBLOCKNUM; i++)
	{
		this->TableBlock[i] = T.TableBlock[i];
		this->NotFull[i] = T.NotFull[i];
	}
}

TableFile::~TableFile()
{

}


void TableFile::AddBlockToTable(Block& B)
{
	TableBlock[B.ID] = true;
	if (B.RecordNum<B.MaxNum)
	{
		NotFull[B.ID] = true;
	}
	else
	{
		NotFull[B.ID] = false;
	}
}

void TableFile::RemoveBlockFromTable(int BlockID)
{
	TableBlock[BlockID] = false;
	NotFull[BlockID] = false;

	ifstream BlockInfo("BlockInfo", ios::binary | ios::in);
	BlockInfo.read((char*)Available, sizeof(Available));
	BlockInfo.close();
	Available[BlockID] = false;
	ofstream BlockInfo1("BlockInfo", ios::binary | ios::out);
	BlockInfo1.write((char*)Available, sizeof(Available));
	BlockInfo1.close();
}

void TableFile::WriteTableToDisk()
{
	string s = curDBname;
	string path = ".//miniSQL//" + s + "//" + TableName;
	ofstream Table(path, ios::trunc | ios::binary | ios::out);
	Table.write((char*)this, sizeof(TableFile));
	Table.close();
}

TableFile TableFile::ReadTableFromDisk(string Name)
{
	string s = curDBname;
	string path = ".//miniSQL//" + s + "//" + Name;
	ifstream File(path, ios::binary | ios::in);
	TableFile Result;
	File.read((char*)&Result, sizeof(Result));
	File.close();
	return Result;
}

int TableFile::GetNotFullBlock(int RecordSize)
{
	for (int i = 1; i <= MAXBLOCKNUM; i++)
	{
		if (TableBlock[i] && NotFull[i])
		{
			return i;
		}
	}
	Block temp(RecordSize);
	temp.WriteBlockToDisk();
	AddBlockToTable(temp);
	WriteTableToDisk();
	return temp.ID;
}



