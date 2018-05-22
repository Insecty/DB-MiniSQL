//#pragma once
#ifndef MINISQL_H_
#define MINISQL_H_

#include <iostream>
#include "interpreter.h"
#include "GlobalValue.h"
#include "ControlCenter.h"
#include "Catalog.h"

void InitProgram();
void RunProgram();
void ExitProgram();

void WriteDBcat(DatabaseInfo &sourceDB, FILE *fp);
void WriteTBcat(TableInfo &sourceTB, FILE *fp);

string intTostring(int num);
string floatTostring(float num);
bool checkRecord(nRecord &Record);
bool checkSelect(nSelect &Select);
bool checkDelete(nDelete &Delete);
bool checkUpdate(nUpdate &Update);
bool checkDropIndex(nDropIndex &nIndexData);//未完工，纯一加油！
bool checkDropDB(nDropDB &nDbData);
bool checkDropTB(nDropTB &nTbData);
bool checkSourceFile(nSourceFile &nSourceData);
int ReadCommandFromFile(char* cmd,char * fname,int fpositoin);

DatabaseInfo GlobalDB;
TableInfo GlobalTB;    // 当前数据库的所有表
#endif // MINISQL_H

