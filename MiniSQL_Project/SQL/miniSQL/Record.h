//#pragma once
#ifndef INDEX_RECORD_H
#define INDEX_RECORD_H

#include <iostream>
#include "Index.h"
#include "Buffer.h"
#include <string>
#include <vector>
using namespace std;


extern vector<BPlusTree<int>*> IndexInt;
extern vector<BPlusTree<float>*> IndexFloat;
extern vector<BPlusTree<string>*> IndexStr;
extern vector<string> index;


void initLoadIndex(string dbname);// ��use database��ʱ��������д��ڵ�������
void createIndexForPrim(string tablename, string attr, string type, int mode);// create tableʱΪ����/unique�Զ��������� // ������ dbname,tablename,primary����
void Load(string path);
// ���Ҳ���
void Select();
// �ж��Ƿ��������ѯ�ĺ���
bool hadIndex(string tablename, string attrName);

// getoffset
int getOffSet(string tablename, int colnum);
// ������
//void selectWithIndex(); // ����(dbname),tablename,attrname
// ��������
//void selectWithoutIndex();
// �������

// ɾ������


#endif //INDEX_RECORD_H