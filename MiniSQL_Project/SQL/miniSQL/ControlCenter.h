#ifndef CONTROLCENTER
#define CONTROLCENTER
#include "interpreter.h"
#include "Catalog.h"
#include "Record.h"
#include "Buffer.h"
#include <string>
#define CloseDB ExitProgram

void CreateDatabase(DataBase&);
void CreateTable(cTable&);
void CreateIndex(nIndex&);
void UseDB(DataBase&);
void InsertData(nRecord&);
void DeleteData(nDelete&);
void SelectData(nSelect&,bool flag);
void DropDB(nDropDB&);
void DropTB(nDropTB&);
void DropIndex(nDropIndex&);
pTableCol getTableKey(string TBname, int mode);
int getOffSet(string tablename, int colnum);
eColType getAttrType(string tablename, string attr);
int  removeDir(const char*  dirPath);

extern TableInfo GlobalTB;
extern DatabaseInfo GlobalDB;
extern void ExitProgram();


#endif // !CONTROLCENTER
