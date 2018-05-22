#include "Catalog.h"
#include<stdio.h>

string sReadFile(FILE *fp, char ch1, char ch2)
{
	char ch = 0;
	string result;
	ch = fgetc(fp);
	while (ch != ch1&&ch != ch2&&ch != EOF)
	{
		result += ch;
		ch = fgetc(fp);
	}
	return result;
}
int iReadFile(FILE *fp, char ch1, char ch2)
{
	char ch;
	int num = 0;
	ch = fgetc(fp);
	while (ch != ch1&&ch != ch2&&ch != EOF)
	{
		num *= 10;
		num += ch - '0' + 0;
		ch = fgetc(fp);
	}
	return num;
}
float fReadFile(FILE *fp, char ch1, char ch2)
{
	char ch;
	float num = 0;
	bool decFlag = 0;
	int record = 1;
	ch = fgetc(fp);
	while (ch != ch1&&ch != ch2&&ch != EOF)
	{
		if (ch != '.')
		{
			num *= 10;
			num += ch - '0' + 0;
			if (decFlag == 1) {
				record *= 10;
			}
		}
		else decFlag = 1;
		ch = fgetc(fp);
	}
	return num / record;
}
void DatabaseInfo::DBUpdate(string path)
{
	FILE *file = fopen(path.c_str(), "r");
	head = new struct CatDBnode;
	head->name = "miniSQL";
	head->Next = NULL;
	end = head;
	DBnum = iReadFile(file, '#');
	for (int i = 0; i < DBnum; i++)
	{
		pDBnode tem = new CatDBnode;
		tem->name = sReadFile(file, ',');
		tem->TableNum = iReadFile(file, ',');
		tem->IndexNum = iReadFile(file, '|');
		tem->Next = head->Next;
		head->Next = tem;
	}
	fclose(file);
}
DatabaseInfo::DatabaseInfo(string path)
{
	FILE *file = fopen(path.c_str(), "r");
	head = new struct CatDBnode;
	head->name = "miniSQL";
	head->Next = NULL;
	end = head;
	DBnum = iReadFile(file, '#');
	for (int i = 0; i < DBnum; i++)
	{
		pDBnode tem = new CatDBnode;
		tem->name = sReadFile(file, ',');
		tem->TableNum = iReadFile(file, ',');
		tem->IndexNum = iReadFile(file, '|');
		tem->Next = head->Next;
		head->Next = tem;
	}
	fclose(file);
}

DatabaseInfo::~DatabaseInfo()
{
	pDBnode p, q;
	p = head;
	while (p->Next != NULL)
	{
		q = p->Next;
		delete p;
		p = q;
	}
}
void TableInfo::TableUpdate(string path)
{
	FILE *file = fopen(path.c_str(), "r+");
	TBnum = iReadFile(file, '#');
	for (int i = 0; i < TBnum; i++)
	{
		pTBnode catNode = new struct CatTBnode;
		//catNode->DBname = sReadFile(file, ',');
		catNode->TBname = sReadFile(file, ',');
		catNode->AttributeNum = iReadFile(file, ',');
		catNode->RecordNum = iReadFile(file, ':');
		catNode->Next = end->Next;
		end->Next = catNode;
		end = end->Next;
		catNode->head = new TableCol;
		catNode->head->Next = NULL;
		pTableCol tHead = catNode->head;
		for (int j = 0; j < catNode->AttributeNum; j++)
		{
			pTableCol tem = new TableCol;
			tem->colnum = iReadFile(file, ',');
			tem->name = sReadFile(file, ',');
			tem->ColType = (eColType)iReadFile(file, ',');
			if (tem->ColType == CHAR)
				tem->length = iReadFile(file, ',');
			tem->isPrimary = (bool)iReadFile(file, ',');
			tem->isDiffer = (bool)iReadFile(file, '|');
			tem->Next = tHead->Next;
			tHead->Next = tem;
			tHead = tHead->Next;
		}
	}
	fclose(file);
}
TableInfo::TableInfo(string path)
{
	FILE *file = fopen(path.c_str(), "r");
	TBnum = iReadFile(file, '#');
	head = new struct CatTBnode;
	head->Next = NULL;
	end = head;
	for (int i = 0; i < TBnum; i++)
	{
		pTBnode catNode = new struct CatTBnode;
		//		catNode->DBname = sReadFile(file, ',');
		catNode->TBname = sReadFile(file, ',');
		catNode->AttributeNum = iReadFile(file, ',');
		catNode->RecordNum = iReadFile(file, '|');
		catNode->Next = end->Next;
		end->Next = catNode;
		end = end->Next;
		catNode->head = new TableCol;
		catNode->head->Next = NULL;
		pTableCol tHead = catNode->head;
		for (int j = 0; j < catNode->AttributeNum; j++)
		{
			pTableCol tem = new TableCol;
			tem->name = sReadFile(file, ',');
			tem->ColType = (eColType)iReadFile(file, ',');
			if (tem->ColType == CHAR)
				tem->length = iReadFile(file, ',');
			tem->isPrimary = (bool)iReadFile(file, ',');
			tem->isDiffer = (bool)iReadFile(file, '|');
			tem->Next = tHead->Next;
			tHead->Next = tem;
			tHead = tHead->Next;
		}
	}
	fclose(file);
}

TableInfo::~TableInfo()
{
	pTBnode a, b;
	pTableCol c, d;
	a = head;
	while (a->Next != NULL)
	{
		b = a->Next;
		c = head->head;
		while (c->Next != NULL)
		{
			d = c->Next;
			delete c;
			c = d;
		}
		delete c;
		delete a;
		a = b;
	}
	delete a;
}
pTBnode TableToNode(cTable& table)
{
	pTBnode data = new CatTBnode;
	data->TBname = table.getName();
	data->AttributeNum = table.TalColumn;
	data->RecordNum = 0;
	data->head = new TableCol;
	data->head->Next = NULL;
	data->Next = NULL;
	pTableCol ncreate = table.head;
	pTableCol p = data->head;

	for (int i = 0; i <data->AttributeNum; i++)
	{
		ncreate = ncreate->Next;
		pTableCol tem = new TableCol;
		tem->colnum = ncreate->colnum;
		tem->ColType = ncreate->ColType;
		tem->isDiffer = ncreate->isDiffer;
		tem->isPrimary = ncreate->isPrimary;
		tem->length = ncreate->length;
		tem->name = ncreate->name;
		tem->Next = p->Next;
		p->Next = tem;
		p = p->Next;
	}
	return data;
}