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


void initLoadIndex(string dbname);// 在use database的时候加载所有存在的索引树
void createIndexForPrim(string tablename, string attr, string type, int mode);// create table时为主键/unique自动创建索引 // 参数有 dbname,tablename,primary类型
void Load(string path);
// 查找操作
void Select();
// 判断是否带条件查询的函数
bool hadIndex(string tablename, string attrName);

// getoffset
int getOffSet(string tablename, int colnum);
// 带索引
//void selectWithIndex(); // 参数(dbname),tablename,attrname
// 不带索引
//void selectWithoutIndex();
// 插入操作

// 删除操作


#endif //INDEX_RECORD_H