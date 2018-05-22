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
bool checkDropIndex(nDropIndex &nIndexData);//δ�깤����һ���ͣ�
bool checkDropDB(nDropDB &nDbData);
bool checkDropTB(nDropTB &nTbData);
bool checkSourceFile(nSourceFile &nSourceData);
int ReadCommandFromFile(char* cmd,char * fname,int fpositoin);

DatabaseInfo GlobalDB;
TableInfo GlobalTB;    // ��ǰ���ݿ�����б�
#endif // MINISQL_H

