#include "ControlCenter.h"
#include "DBfiles.h"
#include "Buffer.h"
#include <direct.h>
#include <string.h>
#include <io.h>
#include "process.h"
#include <iomanip>
#include <fstream>
#include <iostream>

void CreateDatabase(DataBase& nCreate)
{
	char pathname[256] = "miniSQL\\"; 
	strcat(pathname, nCreate.getName().c_str());
	if (_access(pathname, 00) == 0)
		cout << "This database: "<< nCreate.getName() <<" is already exist! " << endl;
	else if (_mkdir(pathname) == 0)
	{
		pDBnode node = new CatDBnode;
		node->TableNum = 0;
		node->IndexNum = 0;
		node->name = nCreate.getName();
		node->Next = GlobalDB.end->Next;
		GlobalDB.end->Next=node;
		GlobalDB.end = GlobalDB.end->Next;
		GlobalDB.NumPlus();
		cout << "Create database: " << nCreate.getName() << " succefully!" << endl;
	}
		
}
void CreateTable(cTable& nCreate)
{
	char pathname[256] = "miniSQL\\";
	if (isOpenDB == 0)
	{
		cout << "Please open a database!" << endl;
		return;
	}
	strcat(pathname, curDBname);
	strcat(pathname, "\\");
	strcat(pathname, nCreate.getName().c_str());
	strcat(pathname, "_record.txt");
	if (_access(pathname, 00) == 0)
	{
		cout << "This table: " << nCreate.getName() << " is already exist! " << endl;
		return;
	}
	FILE *fp;
	fp = fopen(pathname, "w");
	fwrite("0#",sizeof(char),2,fp);
	cout << "Create table record: " << nCreate.getName() << " succefully!" << endl;
	fclose(fp);
/*
	memset(pathname, 0, sizeof(pathname));
	strcat(pathname, "miniSQL\\");
	strcat(pathname, curDBname);
	strcat(pathname, "_index.txt");
	if (_access(pathname, 00) == 0)
	{
		cout << "This table: " << nCreate.getName() << " is already exist! " << endl;
		return;
	}
	fp = fopen(pathname, "w");
	fwrite("0#", sizeof(char), 2, fp);
	cout << "Create table index: " << nCreate.getName() << " succefully!" << endl;
	fclose(fp);
*/
	pTBnode data = TableToNode(nCreate);
	
	data->Next = GlobalTB.end->Next;
	GlobalTB.end->Next = data;
	GlobalTB.end = data;
	GlobalTB.TbNumPlus();

	// 为primkey 建立索引
	pTableCol prim = getTableKey(nCreate.getName(), 1);
	if (prim != NULL)
		GlobalTB.IndexNumPlus();
	string type;
	if (prim->ColType == INT)
		type = "int";
	else if (prim->ColType == FLOAT)
		type = "float";
	else if (prim->ColType == CHAR)
		type = "string";
	createIndexForPrim(nCreate.getName(), prim->name, type, 1);

	// 为unique 建立索引
	pTableCol uniq = getTableKey(nCreate.getName(), 2);
	if (uniq != NULL) {
		GlobalTB.IndexNumPlus();
		if (uniq->ColType == INT)
			type = "int";
		else if (uniq->ColType == FLOAT)
			type = "float";
		else if (uniq->ColType == CHAR)
			type = "string";
		createIndexForPrim(nCreate.getName(), uniq->name, type, 2);
	}

	// buffer init
	string s = nCreate.getName();
	TableFile T(s);
}
void CreateIndex(nIndex& nCreate)  // 创建索引
{
	//if create success  remember to add GlobalTB.IndexNumPlus;
	string dbname = curDBname;
	eColType type = getAttrType(nCreate.getTbName(), nCreate.getAttrName());
	if (type == INT) {
		BPlusTree<int> *tree;
		tree = new BPlusTree<int>(defrank, dbname, nCreate.getTbName(), nCreate.getIndexName(), nCreate.getAttrName(), "INT");
	}
	else if (type == FLOAT) {
		BPlusTree<float> *tree;
		tree = new BPlusTree<float>(defrank, dbname, nCreate.getTbName(), nCreate.getIndexName(), nCreate.getAttrName(), "FLOAT");
	}
	else if (type == CHAR) {
		BPlusTree<string> *tree;
		tree = new BPlusTree<string>(defrank, dbname, nCreate.getTbName(), nCreate.getIndexName(), nCreate.getAttrName(), "STRING");
	}
	cout << "Create index: " << nCreate.getIndexName() << " succefully" << endl;
	index.push_back(nCreate.getIndexName());
}
void UseDB(DataBase& nData)
{
	if (isOpenDB == 1)
	{
		if (strcmp(nData.getName().c_str(), curDBname) != 0)
		{
			char pathname[256] = "miniSQL\\";
			strcat(pathname, nData.getName().c_str());
			if(_access(pathname, 00) == 0)
				cout << "Database(" << curDBname <<") is closed!"<< endl;
			else {
				cout << "Database : " << nData.getName() << " is not exist" << endl;
				return;
			}
		}
		else
		{
			cout << "Database(" << curDBname << ") is using!" << endl;
			return;
		}
		CloseDB();
	}
	pDBnode node=GlobalDB.getHead();
	for (int i = 0; i < GlobalDB.getNum(); i++)
	{
		node = node->Next;
		if (node->name.c_str() == nData.getName())
		{
			cout << "Welcome to the database :" << nData.getName() << endl;
			isOpenDB = 1;
			strcpy(curDBname, nData.getName().c_str());
			char pathname[256] = "miniSQL\\";
			strcat(pathname, nData.getName().c_str());
			strcat(pathname, "_table.txt");
			if (_access(pathname, 00) != 0)
			{
				FILE *fp = fopen(pathname, "w");
				fwrite("0#", sizeof(char), 2, fp);
				fclose(fp);
			}
			GlobalTB.TableUpdate(pathname);
			pDBnode head = GlobalDB.getHead();
			while (head->Next != NULL)
			{
				head = head->Next;
				if (head->name == (string)curDBname)
				{
					GlobalTB.IndexNumPlus(head->IndexNum);
					break;
				}
			}
			// init index
			IndexInt.clear();
			IndexFloat.clear();
			IndexStr.clear();
			//initLoadIndex((string)curDBname);
			return;
		}
	}
	cout << "Database : " << nData.getName() << " is not found!" << endl;
	return;
	/*
	char pathname[256] = "miniSQL\\";
	strcat(pathname, nData.getName().c_str());
	if (_access(pathname, 0) != 0)
	{
		cout << "This database: " << nData.getName() << " is not exist! " << endl;
		return;	
	}
	cout << "Welcome to the database :" << nData.getName() << endl;
	isOpenDB = 1;
	strcpy(curDBname, nData.getName().c_str());
	return;
	*/

}
void InsertData(nRecord& nData)
{
	pCatTBnode table;
	table = GlobalTB.getHead();
	while (table->Next != NULL)
	{
		if (table->TBname == nData.getName())
			break;
		else table = table->Next;
	}// nData.getName() 就是表名

	 // 判断主键和unique是否冲突
		pTableCol primcolumn = getTableKey(nData.getName(), 1);
		pTableCol uniqcolumn = getTableKey(nData.getName(), 2);
	bool conflict = false;

	if (primcolumn->ColType == INT) {
		for (size_t i = 0; i < IndexInt.size(); i++) {
			if (IndexInt[i] && IndexInt[i]->getTableName() == nData.getName() && IndexInt[i]->getAttrName() == primcolumn->name) {
				int key;
				pRecordCol primhead = nData.getHead();
				primhead = primhead->Next;
				while (primhead != NULL) {
					if (primhead->colnum == primcolumn->colnum) {
						key = primhead->nInt;
						break;
					}
					primhead = primhead->Next;
				}
				bool index = IndexInt[i]->InsertTest(key);
				if (!index && IndexInt[i]->root != NULL)
					conflict = true;
				break;
			}
		}
	}
	else if (primcolumn->ColType == FLOAT) {
		for (size_t i = 0; i < IndexFloat.size(); i++) {
			if (IndexFloat[i] && IndexFloat[i]->getTableName() == nData.getName() && IndexFloat[i]->getAttrName() == primcolumn->name) {
				float key;
				pRecordCol primhead = nData.getHead();
				primhead = primhead->Next;
				while (primhead != NULL) {
					if (primhead->colnum == primcolumn->colnum) {
						key = primhead->nFloat;
						break;
					}
					primhead = primhead->Next;
				}
				bool index = IndexFloat[i]->InsertTest(key);
				if (!index && IndexInt[i]->root != NULL)
					conflict = true;
				break;
			}
		}
	}
	else if (primcolumn->ColType == CHAR) {
		for (size_t i = 0; i < IndexStr.size(); i++) {
			if (IndexStr[i]&&IndexStr[i]->getTableName() == nData.getName() && IndexStr[i]->getAttrName() == primcolumn->name) {
				string key;
				pRecordCol primhead = nData.getHead();
				primhead = primhead->Next;
				while (primhead != NULL) {
					if (primhead->colnum == primcolumn->colnum) {
						key = primhead->nChar;
						break;
					}
					primhead = primhead->Next;
				}
				bool index = IndexStr[i]->InsertTest(key);
				if (!index && IndexInt[i]->root != NULL)
					conflict = true;
				break;
			}
		}
	}
	if (uniqcolumn != NULL) {
		if (uniqcolumn->ColType == INT) {
			for (size_t i = 0; i < IndexInt.size(); i++) {
				if (IndexInt[i]&&IndexInt[i]->getTableName() == nData.getName() && IndexInt[i]->getAttrName() == uniqcolumn->name) {
					int key;
					pRecordCol uniqhead = nData.getHead();
					uniqhead = uniqhead->Next;
					while (uniqhead != NULL) {
						if (uniqhead->colnum == uniqcolumn->colnum) {
							key = uniqhead->nInt;
							break;
						}
						uniqhead = uniqhead->Next;
					}
					bool index = IndexInt[i]->InsertTest(key);
					if (!index && IndexInt[i]->root != NULL)
						conflict = true;
					break;
				}
			}
		}
		else if (uniqcolumn->ColType == FLOAT) {
			for (size_t i = 0; i < IndexFloat.size(); i++) {
				if (IndexFloat[i]&&IndexFloat[i]->getTableName() == nData.getName() && IndexFloat[i]->getAttrName() == uniqcolumn->name) {
					float key;
					pRecordCol uniqhead = nData.getHead();
					uniqhead = uniqhead->Next;
					while (uniqhead != NULL) {
						if (uniqhead->colnum == uniqcolumn->colnum) {
							key = uniqhead->nFloat;
							break;
						}
						uniqhead = uniqhead->Next;
					}
					bool index = IndexFloat[i]->InsertTest(key);
					if (!index && IndexInt[i]->root != NULL)
						conflict = true;
					break;
				}
			}
		}
		else if (uniqcolumn->ColType == CHAR) {
			for (size_t i = 0; i < IndexStr.size(); i++) {
				if (IndexStr[i]&&IndexStr[i]->getTableName() == nData.getName() && IndexStr[i]->getAttrName() == uniqcolumn->name) {
					string key;
					pRecordCol uniqhead = nData.getHead();
					uniqhead = uniqhead->Next;
					while (uniqhead != NULL) {
						if (uniqhead->colnum == uniqcolumn->colnum) {
							key = uniqhead->nChar;
							break;
						}
						uniqhead = uniqhead->Next;
					}
					bool index = IndexStr[i]->InsertTest(key);
					if (!index && IndexInt[i]->root != NULL)
						conflict = true;
					break;
				}
			}
		}
	}
	if (conflict == true) {
		cout << "Sorry! Primary key or unique key has already exists~ " << endl;
		return;
	}

	// 插入块中
	string record = "";
	int RecordSize = 0;
	pRecordCol Datahead = nData.getHead();
	while (Datahead->Next != NULL) {
		if (Datahead->Next->ColType == INT) {
			RecordSize += sizeof(int);
		}
		else if (Datahead->Next->ColType == FLOAT) {
			RecordSize += sizeof(float);
		}
		else if (Datahead->Next->ColType == CHAR) {
			RecordSize += Datahead->Next->length;
		}
		Datahead = Datahead->Next;
	}

	char R[BLOCKSIZE];
	Datahead = nData.getHead();
	int remove = 0;
	while (Datahead->Next != NULL) {
		if (Datahead->Next->ColType == INT) {
			int x = Datahead->Next->nInt;
			memcpy(R + remove, &x, sizeof(int));
			remove += sizeof(int);
		}
		else if (Datahead->Next->ColType == FLOAT) {
			float y = Datahead->Next->nFloat;
			memcpy(R + remove, &y, sizeof(float));
			remove += sizeof(float);
		}
		else if (Datahead->Next->ColType == CHAR) {
			char *z = new char[Datahead->Next->length];
			strcpy(z, Datahead->Next->nChar.c_str());
			memcpy(R + remove, z, Datahead->Next->nChar.size());
			remove += Datahead->Next->length;
		}
		Datahead = Datahead->Next;
	}

	// 更新索引
	int blockID;
	blockID = bufferMan.Insert(nData.getName(), RecordSize, R);
	// 对于所有有索引的属性都要插入键值更新索引
	Datahead = nData.getHead();
	Datahead = Datahead->Next;
	while (Datahead != NULL) {
		if (Datahead->ColType == INT) {
			for (size_t i = 0; i < IndexInt.size(); i++) {
				if (IndexInt[i]->getTableName() == nData.getName() && IndexInt[i]->getAttrName() == Datahead->name) {
					IndexInt[i]->Insert(Datahead->nInt, blockID);
				}
			}
		}
		else if (Datahead->ColType == FLOAT) {
			for (size_t i = 0; i < IndexFloat.size(); i++) {
				if (IndexFloat[i]->getTableName() == nData.getName() && IndexFloat[i]->getAttrName() == Datahead->name) {
					IndexFloat[i]->Insert(Datahead->nFloat, blockID);
				}
			}
		}
		else if (Datahead->ColType == CHAR) {
			for (size_t i = 0; i < IndexStr.size(); i++) {
				if (IndexStr[i]->getTableName() == nData.getName() && IndexStr[i]->getAttrName() == Datahead->name) {
					IndexStr[i]->Insert(Datahead->nChar, blockID);
				}
			}
		}
		Datahead = Datahead->Next;
	}
	pTBnode head = GlobalTB.getHead();
	while (head->Next != NULL) {
		head = head->Next;
		if (head->TBname == nData.getName())
		{
			head->RecordNum++;
			break;
		}
	}
	cout << "Insert 1 record into :" << nData.getName() << "  Successfully!" << endl;

	// GlobalTB updata
	pTBnode pr = GlobalTB.getHead();
	pr = pr->Next;
	while (pr != NULL) {
		if (pr->TBname == nData.getName()) {
			pr->RecordNum++;
			break;
		}
		else pr = pr->Next;
	}
	return;
}
void DeleteData(nDelete& nData) {
	int buffer[MAXBLOCKNUM + 1]; // 存块号
	string tablename = nData.getName(); // 表名
	if (nData.getWhere() == false) { // delete *
		TheSelectResult R = bufferMan.Delete(tablename, NULL, 0, 0, NULL, (eColType2)0, eSignDEFAULT);
		cout << "Delete all records successfully from : " << nData.getName() << endl;
		// 更新recordnum in globalTB
		pTBnode pr = GlobalTB.getHead();
		pr = pr->Next;
		while (pr != NULL) {
			if (pr->TBname == nData.getName()) {
				pr->RecordNum = 0;
				break;
			}
			else pr = pr->Next;
		}
		// 更新索引
		for (size_t i = 0; i < IndexInt.size(); i++) {
			if (IndexInt[i]->getTableName() == nData.getName())
				IndexInt[i]->reset();
		}
		for (size_t i = 0; i < IndexFloat.size(); i++) {
			if (IndexFloat[i]->getTableName() == nData.getName())
				IndexFloat[i]->reset();
		}
		for (size_t i = 0; i < IndexStr.size(); i++) {
			if (IndexStr[i]->getTableName() == nData.getName())
				IndexStr[i]->reset();
		}
		return;
	}
	else if (nData.getWhere() == true) { // delete from ... where ... 
		pCondition info = nData.getConditionHead()->Next;
		pRecordCol record = info->record;
		bool flag = hadIndex(tablename, record->name);

		pTableCol primcolumn = getTableKey(nData.getName(), 1);
		pTableCol uniqcolumn = getTableKey(nData.getName(), 2);
		int Prim_offset = getOffSet(nData.getName(), primcolumn->colnum);
		int Uniq_offset = getOffSet(nData.getName(), uniqcolumn->colnum);

		if (flag) {// has index 
			if (record->ColType == INT) {
				for (size_t i = 0; i < IndexInt.size(); i++) {
					if (IndexInt[i]->getAttrName() == record->name && IndexInt[i]->getTableName() == nData.getName()) {
						if (info->symbol == BIGGER)
							IndexInt[i]->larger(buffer, record->nInt);
						else if (info->symbol == SMALLER)
							IndexInt[i]->smaller(buffer, record->nInt);
						else if (info->symbol == EQUAL) {
							buffer[0] = 1;
							buffer[1] = IndexInt[i]->equal(record->nInt);
						}
						else if (info->symbol == EBIGGER)
							IndexInt[i]->largerAndEqual(buffer, record->nInt);
						else if (info->symbol == ESMALLER)
							IndexInt[i]->smallerAndEqual(buffer, record->nInt);
						else if (info->symbol == NEQUAL)
							IndexInt[i]->notEqual(buffer, record->nInt);

						int offset = getOffSet(tablename, record->colnum);
						char K[4];
						memcpy(K, &record->nInt, sizeof(int));
						TheSelectResult R = bufferMan.Delete(tablename, buffer, offset, sizeof(int), K, (eColType2)1, info->symbol);
						
						if (R.RecordNum == 0) {
							cout << "Oops! No corresponding record!" << endl;
							return;
						}
						// 更新recordnum in globalTB
						pTBnode pr = GlobalTB.getHead();
						pr = pr->Next;
						while (pr != NULL) {
							if (pr->TBname == nData.getName()) {
								pr->RecordNum -= R.RecordNum;
								break;
							}
							else pr = pr->Next;
						}
						// 更新索引
						if (primcolumn->ColType == INT) {
							for (size_t i = 0; i < IndexInt.size(); i++) {
								if (IndexInt[i]->getTableName() == tablename && IndexInt[i]->getAttrName() == primcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										int x = *(int*)(R.Result + j*R.RecordSize + Prim_offset);
										IndexInt[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (primcolumn->ColType == FLOAT) {
							for (size_t i = 0; i < IndexFloat.size(); i++) {
								if (IndexFloat[i]->getTableName() == tablename && IndexFloat[i]->getAttrName() == primcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										float x = *(float*)(R.Result + j*R.RecordSize + Prim_offset);
										IndexFloat[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (primcolumn->ColType == CHAR) {
							for (size_t i = 0; i < IndexStr.size(); i++) {
								if (IndexStr[i]->getTableName() == tablename && IndexStr[i]->getAttrName() == primcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										char *a = new char[primcolumn->length];
										for (int k = 0; k < primcolumn->length; k++) {
											a[k] = *(char*)(R.Result + j*R.RecordSize + Prim_offset + k);
										}
										string str = a;
										IndexStr[i]->Delete(str);
									}
									break;
								}
							}
						}

						if (uniqcolumn->ColType == INT) {
							for (size_t i = 0; i < IndexInt.size(); i++) {
								if (IndexInt[i]->getTableName() == tablename && IndexInt[i]->getAttrName() == uniqcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										int x = *(int*)(R.Result + j*R.RecordSize + Uniq_offset);
										IndexInt[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (uniqcolumn->ColType == FLOAT) {
							for (size_t i = 0; i < IndexFloat.size(); i++) {
								if (IndexFloat[i]->getTableName() == tablename && IndexFloat[i]->getAttrName() == uniqcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										float x = *(float*)(R.Result + j*R.RecordSize + Uniq_offset);
										IndexFloat[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (uniqcolumn->ColType == CHAR) {
							for (size_t i = 0; i < IndexStr.size(); i++) {
								if (IndexStr[i]->getTableName() == tablename && IndexStr[i]->getAttrName() == uniqcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										string str = "";
										for (int k = 0; k < uniqcolumn->length; k++) {
											str += *(char*)(R.Result + j*R.RecordSize + Uniq_offset + k);
										}
										IndexStr[i]->Delete(str);
									}
									break;
								}
							}
						}

						cout << "Delete from : " << tablename << "  Successfully!" << endl;
						return;
					}
				}
			}
			else if (record->ColType == FLOAT) {
				for (size_t i = 0; i < IndexFloat.size(); i++) {
					if (IndexFloat[i]->getAttrName() == record->name && IndexFloat[i]->getTableName() == nData.getName()) {
						if (info->symbol == BIGGER)
							IndexFloat[i]->larger(buffer, record->nFloat);
						else if (info->symbol == SMALLER)
							IndexFloat[i]->smaller(buffer, record->nFloat);
						else if (info->symbol == EQUAL) {
							buffer[0] = 1;
							buffer[1] = IndexFloat[i]->equal(record->nFloat);
						}
						else if (info->symbol == EBIGGER)
							IndexFloat[i]->largerAndEqual(buffer, record->nFloat);
						else if (info->symbol == ESMALLER)
							IndexFloat[i]->smallerAndEqual(buffer, record->nFloat);
						else if (info->symbol == NEQUAL)
							IndexFloat[i]->notEqual(buffer, record->nFloat);

						int offset = getOffSet(tablename, record->colnum);
						char K[4];
						memcpy(K, &record->nFloat, sizeof(float));
						TheSelectResult R = bufferMan.Delete(tablename, buffer, offset, sizeof(float), K, (eColType2)3, info->symbol);
						
						if (R.RecordNum == 0) {
							cout << "Oops! No corresponding record!" << endl;
							return;
						}
						// 更新recordnum in globalTB
						pTBnode pr = GlobalTB.getHead();
						pr = pr->Next;
						while (pr != NULL) {
							if (pr->TBname == nData.getName()) {
								pr->RecordNum -= R.RecordNum;
								break;
							}
							else pr = pr->Next;
						}
						// 更新索引
						if (primcolumn->ColType == INT) {
							for (size_t i = 0; i < IndexInt.size(); i++) {
								if (IndexInt[i]->getTableName() == tablename && IndexInt[i]->getAttrName() == primcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										int x = *(int*)(R.Result + j*R.RecordSize + Prim_offset);
										IndexInt[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (primcolumn->ColType == FLOAT) {
							for (size_t i = 0; i < IndexFloat.size(); i++) {
								if (IndexFloat[i]->getTableName() == tablename && IndexFloat[i]->getAttrName() == primcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										float x = *(float*)(R.Result + j*R.RecordSize + Prim_offset);
										IndexFloat[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (primcolumn->ColType == CHAR) {
							for (size_t i = 0; i < IndexStr.size(); i++) {
								if (IndexStr[i]->getTableName() == tablename && IndexStr[i]->getAttrName() == primcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										char *a = new char[primcolumn->length];
										for (int k = 0; k < primcolumn->length; k++) {
											a[k] = *(char*)(R.Result + j*R.RecordSize + Prim_offset + k);
										}
										string str = a;
										IndexStr[i]->Delete(str);
									}
									break;
								}
							}
						}

						if (uniqcolumn->ColType == INT) {
							for (size_t i = 0; i < IndexInt.size(); i++) {
								if (IndexInt[i]->getTableName() == tablename && IndexInt[i]->getAttrName() == uniqcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										int x = *(int*)(R.Result + j*R.RecordSize + Uniq_offset);
										IndexInt[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (uniqcolumn->ColType == FLOAT) {
							for (size_t i = 0; i < IndexFloat.size(); i++) {
								if (IndexFloat[i]->getTableName() == tablename && IndexFloat[i]->getAttrName() == uniqcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										float x = *(float*)(R.Result + j*R.RecordSize + Uniq_offset);
										IndexFloat[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (uniqcolumn->ColType == CHAR) {
							for (size_t i = 0; i < IndexStr.size(); i++) {
								if (IndexStr[i]->getTableName() == tablename && IndexStr[i]->getAttrName() == uniqcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										char *a = new char[uniqcolumn->length];
										for (int k = 0; k < uniqcolumn->length; k++) {
											a[k] = *(char*)(R.Result + j*R.RecordSize + Uniq_offset + k);
										}
										string str = a;
										IndexStr[i]->Delete(str);
									}
									break;
								}
							}
						}

						cout << "Delete from : " << tablename << "  Successfully!" << endl;
						return;
					}
				}
			}
			else if (record->ColType == CHAR) {
				for (size_t i = 0; i < IndexStr.size(); i++) {
					if (IndexStr[i]->getAttrName() == record->name && IndexStr[i]->getTableName() == nData.getName()) {
						if (info->symbol == BIGGER)
							IndexStr[i]->larger(buffer, record->nChar);
						else if (info->symbol == SMALLER)
							IndexStr[i]->smaller(buffer, record->nChar);
						else if (info->symbol == EQUAL) {
							buffer[0] = 1;
							buffer[1] = IndexStr[i]->equal(record->nChar);
						}
						else if (info->symbol == EBIGGER)
							IndexStr[i]->largerAndEqual(buffer, record->nChar);
						else if (info->symbol == ESMALLER)
							IndexStr[i]->smallerAndEqual(buffer, record->nChar);
						else if (info->symbol == NEQUAL)
							IndexStr[i]->notEqual(buffer, record->nChar);

						int offset = getOffSet(tablename, record->colnum);
						char *K = new char[record->length + 1];
						memcpy(K, record->nChar.c_str(), record->length);
						K[record->length] = '\0';
						TheSelectResult R = bufferMan.Delete(tablename, buffer, offset, record->length, K, (eColType2)2, info->symbol);
						
						if (R.RecordNum == 0) {
							cout << "Oops! No corresponding record!" << endl;
							return;
						}
						// 更新recordnum in globalTB
						pTBnode pr = GlobalTB.getHead();
						pr = pr->Next;
						while (pr != NULL) {
							if (pr->TBname == nData.getName()) {
								pr->RecordNum -= R.RecordNum;
								break;
							}
							else pr = pr->Next;
						}
						// 更新索引
						if (primcolumn->ColType == INT) {
							for (size_t i = 0; i < IndexInt.size(); i++) {
								if (IndexInt[i]->getTableName() == tablename && IndexInt[i]->getAttrName() == primcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										int x = *(int*)(R.Result + j*R.RecordSize + Prim_offset);
										IndexInt[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (primcolumn->ColType == FLOAT) {
							for (size_t i = 0; i < IndexFloat.size(); i++) {
								if (IndexFloat[i]->getTableName() == tablename && IndexFloat[i]->getAttrName() == primcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										float x = *(float*)(R.Result + j*R.RecordSize + Prim_offset);
										IndexFloat[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (primcolumn->ColType == CHAR) {
							for (size_t i = 0; i < IndexStr.size(); i++) {
								if (IndexStr[i]->getTableName() == tablename && IndexStr[i]->getAttrName() == primcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										char *a = new char[primcolumn->length];
										for (int k = 0; k < primcolumn->length; k++) {
											a[k] = *(char*)(R.Result + j*R.RecordSize + Prim_offset + k);
										}
										string str = a;
										IndexStr[i]->Delete(str);
									}
									break;
								}
							}
						}

						if (uniqcolumn->ColType == INT) {
							for (size_t i = 0; i < IndexInt.size(); i++) {
								if (IndexInt[i]->getTableName() == tablename && IndexInt[i]->getAttrName() == uniqcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										int x = *(int*)(R.Result + j*R.RecordSize + Uniq_offset);
										IndexInt[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (uniqcolumn->ColType == FLOAT) {
							for (size_t i = 0; i < IndexFloat.size(); i++) {
								if (IndexFloat[i]->getTableName() == tablename && IndexFloat[i]->getAttrName() == uniqcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										float x = *(float*)(R.Result + j*R.RecordSize + Uniq_offset);
										IndexFloat[i]->Delete(x);
									}
									break;
								}
							}
						}
						else if (uniqcolumn->ColType == CHAR) {
							for (size_t i = 0; i < IndexStr.size(); i++) {
								if (IndexStr[i]->getTableName() == tablename && IndexStr[i]->getAttrName() == uniqcolumn->name) {
									for (int j = 0; j < R.RecordNum; j++) {
										char *a = new char[uniqcolumn->length];
										for (int k = 0; k < uniqcolumn->length; k++) {
											a[k] = *(char*)(R.Result + j*R.RecordSize + Uniq_offset + k);
										}
										string str = a;
										IndexStr[i]->Delete(str);
									}
									break;
								}
							}
						}

						cout << "Delete from : " << tablename << "  Successfully!" << endl;
						return;
					}
				}
			}
		}
		else { // no index on attr
			   //get all blockid
			int num = 0;
			TableFile T = TableFile::ReadTableFromDisk(tablename);
			for (int i = 1; i < MAXBLOCKNUM + 1; i++) {
				if (T.TableBlock[i] == true) {
					buffer[++num] = i;
				}
			}
			buffer[0] = num;

			if (buffer[0] == 0) {
				cout << "Oops! No corresponding record!" << endl;
				return;
			}

			int offset = getOffSet(tablename, record->colnum);
			int size = 0;
			int type;
			char key[32];
			if (record->ColType == INT) {
				type = 1;
				size = sizeof(int);
				memcpy(key, &record->nInt, sizeof(int));
			}
			else if (record->ColType == CHAR) {
				size = record->length;
				type = 2;
				char *z = new char[size];
				strcpy(z, record->nChar.c_str());
				memcpy(key, z, size);
			}
			else if (record->ColType == FLOAT) {
				type = 3;
				size = sizeof(float);
				memcpy(key, &record->nFloat, sizeof(float));
			}

			TheSelectResult R = bufferMan.Delete(tablename, buffer, offset, size, key, (eColType2)type, info->symbol);
			
			if (R.RecordNum == 0) {
				cout << "Oops! No corresponding record!" << endl;
				return;
			}
			// 更新recordnum in globalTB
			pTBnode pr = GlobalTB.getHead();
			pr = pr->Next;
			while (pr != NULL) {
				if (pr->TBname == nData.getName()) {
					pr->RecordNum -= R.RecordNum;
					break;
				}
				else pr = pr->Next;
			}
			// 更新索引
			if (primcolumn->ColType == INT) {
				for (size_t i = 0; i < IndexInt.size(); i++) {
					if (IndexInt[i]->getTableName() == tablename && IndexInt[i]->getAttrName() == primcolumn->name) {
						for (int j = 0; j < R.RecordNum; j++) {
							int x = *(int*)(R.Result + j*R.RecordSize + Prim_offset);
							IndexInt[i]->Delete(x);
						}
						break;
					}
				}
			}
			else if (primcolumn->ColType == FLOAT) {
				for (size_t i = 0; i < IndexFloat.size(); i++) {
					if (IndexFloat[i]->getTableName() == tablename && IndexFloat[i]->getAttrName() == primcolumn->name) {
						for (int j = 0; j < R.RecordNum; j++) {
							float x = *(float*)(R.Result + j*R.RecordSize + Prim_offset);
							IndexFloat[i]->Delete(x);
						}
						break;
					}
				}
			}
			else if (primcolumn->ColType == CHAR) {
				for (size_t i = 0; i < IndexStr.size(); i++) {
					if (IndexStr[i]->getTableName() == tablename && IndexStr[i]->getAttrName() == primcolumn->name) {
						for (int j = 0; j < R.RecordNum; j++) {
							char *a = new char[primcolumn->length];
							for (int k = 0; k < primcolumn->length; k++) {
								a[k] = *(char*)(R.Result + j*R.RecordSize + Prim_offset + k);
							}
							string str = a;
							IndexStr[i]->Delete(str);
						}
						break;
					}
				}
			}

			if (uniqcolumn->ColType == INT) {
				for (size_t i = 0; i < IndexInt.size(); i++) {
					if (IndexInt[i]->getTableName() == tablename && IndexInt[i]->getAttrName() == uniqcolumn->name) {
						for (int j = 0; j < R.RecordNum; j++) {
							int x = *(int*)(R.Result + j*R.RecordSize + Uniq_offset);
							IndexInt[i]->Delete(x);
						}
						break;
					}
				}
			}
			else if (uniqcolumn->ColType == FLOAT) {
				for (size_t i = 0; i < IndexFloat.size(); i++) {
					if (IndexFloat[i]->getTableName() == tablename && IndexFloat[i]->getAttrName() == uniqcolumn->name) {
						for (int j = 0; j < R.RecordNum; j++) {
							float x = *(float*)(R.Result + j*R.RecordSize + Uniq_offset);
							IndexFloat[i]->Delete(x);
						}
						break;
					}
				}
			}
			else if (uniqcolumn->ColType == CHAR) {
				for (size_t i = 0; i < IndexStr.size(); i++) {
					if (IndexStr[i]->getTableName() == tablename && IndexStr[i]->getAttrName() == uniqcolumn->name) {
						for (int j = 0; j < R.RecordNum; j++) {
							char *a = new char[uniqcolumn->length];
							for (int k = 0; k < uniqcolumn->length; k++) {
								a[k] = *(char*)(R.Result + j*R.RecordSize + Uniq_offset + k);
							}
							string str = a;
							IndexStr[i]->Delete(str);
						}
						break;
					}
				}
			}
		}
	}
	cout << "Delete from : " << tablename << "  Successfully!" << endl;
	return;
}
void SelectData(nSelect& nData, bool flag) {
	int recordnum = 0;
	char resultpath[256] = "miniSQL\\result.txt";
	ofstream ofile;
	if(flag)
		ofile.open(resultpath, ios::out);
	if (nData.getAll() && !nData.getWhere())   //select * from ..
	{
		TableFile T = TableFile::ReadTableFromDisk(nData.getName());
		pTBnode p = GlobalTB.getHead();
		while (p->Next)
		{
			p = p->Next;
			if (p->TBname == nData.getName()) break;
		}
		int ColNum = p->AttributeNum;

		int BlockID[MAXBLOCKNUM + 1];
		int num = 0;
		for (int i = 1; i < MAXBLOCKNUM + 1; i++)
		{
			if (T.TableBlock[i])
			{
				BlockID[++num] = i;
			}
		}
		BlockID[0] = num;

		pTableCol pTC = nData.getRecordHead();
		for (int i = 1; i <= ColNum; i++)
		{
			pTC = pTC->Next;
			switch (pTC->ColType)
			{
			case INT:
				cout << setiosflags(ios::left) << setw(12) << pTC->name;
				break;
			case FLOAT:
				cout << setiosflags(ios::left) << setw(12) << pTC->name;
				break;
			case CHAR:
				cout << setiosflags(ios::left) << setw(pTC->length) << pTC->name;
				break;
			default:
				break;
			}
		}
		cout << endl;

		for (int i = 1; i <= BlockID[0]; i++)
		{
			Block Temp = Block::ReadBlockFromDisk(BlockID[i]);
			recordnum = Temp.RecordNum;
			for (int j = 0; j < Temp.RecordNum; j++)
			{
				pTC = nData.getRecordHead();
				
				for (int k = 1; k <= ColNum; k++)
				{
					int offset = getOffSet(nData.getName(), k);
					pTC = pTC->Next;
					switch (pTC->ColType)
					{
					case INT:
						printf("%-12d", *(int*)(Temp.Data + j*Temp.RecordSize + offset));
						if (flag)
							ofile << *(int*)(Temp.Data + j*Temp.RecordSize + offset) << " | " ;
						break;
					case FLOAT:
						printf("%-12f", *(float*)(Temp.Data + j*Temp.RecordSize + offset));
						if (flag)
							ofile << *(float*)(Temp.Data + j*Temp.RecordSize + offset) << " | ";
						break;
					case CHAR:
						for (int m = 0; m < pTC->length; m++)
						{
							printf("%c", *(char*)(Temp.Data + j*Temp.RecordSize + offset + m));
							if (flag)
								ofile << *(char*)(Temp.Data + j*Temp.RecordSize + offset + m);
						}
						if (flag)
							ofile << " | ";
						break;
					default:
						break;
					}
				}
				cout << endl;
				if (flag)
					ofile << endl;
			}
		}

		cout << "Search " << recordnum << " records totally!" << endl;
		if(flag)
			ofile << "Search " << recordnum << " records totally!" << endl;

	}
	else if (!nData.getAll() && !nData.getWhere()) // selct a,b.. from ..
	{
		TableFile T = TableFile::ReadTableFromDisk(nData.getName());

		int BlockID[MAXBLOCKNUM + 1];
		int num = 0;
		for (int i = 1; i < MAXBLOCKNUM + 1; i++)
		{
			if (T.TableBlock[i])
			{
				BlockID[++num] = i;
			}
		}
		BlockID[0] = num;

		pTableCol pTC = nData.getRecordHead();
		while (pTC->Next)
		{
			pTC = pTC->Next;
			switch (pTC->ColType)
			{
			case INT:
				cout << setiosflags(ios::left) << setw(12) << pTC->name;
				break;
			case FLOAT:
				cout << setiosflags(ios::left) << setw(12) << pTC->name;
				break;
			case CHAR:
				cout << setiosflags(ios::left) << setw(pTC->length) << pTC->name;
				break;
			default:
				break;
			}
		}
		cout << endl;


		for (int i = 1; i <= BlockID[0]; i++)
		{
			Block Temp = Block::ReadBlockFromDisk(BlockID[i]);
			recordnum = Temp.RecordNum;
			for (int j = 0; j < Temp.RecordNum; j++)
			{
				pTC = nData.getRecordHead();
				while (pTC->Next)
				{

					pTC = pTC->Next;
					int offset = getOffSet(nData.getName(), pTC->colnum);
					switch (pTC->ColType)
					{
					case INT:
						printf("%-12d", *(int*)(Temp.Data + j*Temp.RecordSize + offset));
						break;
					case FLOAT:
						printf("%-12f", *(float*)(Temp.Data + j*Temp.RecordSize + offset));
						break;
					case CHAR:
						for (int m = 0; m < pTC->length; m++)
						{
							printf("%c", *(char*)(Temp.Data + j*Temp.RecordSize + offset + m));
						}
						break;
					default:
						break;
					}
				}
				cout << endl;
			}
		}
		cout << "Search " << recordnum << " records totally!" << endl;
	}
	else if (nData.getAll() && nData.getWhere()) //  select * from ... where ...
	{
		TableFile T = TableFile::ReadTableFromDisk(nData.getName());
		int BlockID[MAXBLOCKNUM + 1];
		string tablename = nData.getName();
		pCondition info = nData.getConditionHead()->Next;
		pRecordCol record = info->record;// where子句内容

		bool flag = hadIndex(tablename, record->name);
		if (flag) { // has index
			if (record->ColType == INT) {
				for (size_t i = 0; i < IndexInt.size(); i++) {
					if (IndexInt[i]->getAttrName() == record->name && IndexInt[i]->getTableName() == nData.getName()) {
						if (info->symbol == BIGGER)
							IndexInt[i]->larger(BlockID, record->nInt);
						else if (info->symbol == SMALLER)
							IndexInt[i]->smaller(BlockID, record->nInt);
						else if (info->symbol == EQUAL) {
							BlockID[0] = 1;
							BlockID[1] = IndexInt[i]->equal(record->nInt);
						}
						else if (info->symbol == EBIGGER)
							IndexInt[i]->largerAndEqual(BlockID, record->nInt);
						else if (info->symbol == ESMALLER)
							IndexInt[i]->smallerAndEqual(BlockID, record->nInt);
						else if (info->symbol == NEQUAL)
							IndexInt[i]->notEqual(BlockID, record->nInt);

						int offset = getOffSet(tablename, record->colnum);
						char K[4];
						memcpy(K, &record->nInt, sizeof(int));
						
						TheSelectResult R = bufferMan.Select(tablename, BlockID, offset, sizeof(int), K, (eColType2)1, info->symbol);
						recordnum = R.RecordNum;
						pTBnode p = GlobalTB.getHead();
						while (p->Next)
						{
							p = p->Next;
							if (p->TBname == nData.getName()) break;
						}
						int ColNum = p->AttributeNum;

						pTableCol pTC = nData.getRecordHead();
						for (int i = 1; i <= ColNum; i++)
						{
							pTC = pTC->Next;
							switch (pTC->ColType)
							{
							case INT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case FLOAT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case CHAR:
								cout << setiosflags(ios::left) << setw(pTC->length) << pTC->name;
								break;
							default:
								break;
							}
						}
						cout << endl;
						for (int i = 0; i < R.RecordNum; i++)
						{
							pTC = nData.getRecordHead();
							for (int k = 1; k <= ColNum; k++)
							{
								int offset = getOffSet(nData.getName(), k);
								pTC = pTC->Next;
								switch (pTC->ColType)
								{
								case INT:
									printf("%-12d", *(int*)(R.Result + i*R.RecordSize + offset));
									break;
								case FLOAT:
									printf("%-12f", *(float*)(R.Result + i*R.RecordSize + offset));
									break;
								case CHAR:
									for (int m = 0; m < pTC->length; m++)
									{
										printf("%c", *(char*)(R.Result + i*R.RecordSize + offset + m));
									}
									break;
								default:
									break;
								}
							}
							cout << endl;
						}
						return;
					}
				}
			}
			else if (record->ColType == FLOAT) {
				for (size_t i = 0; i < IndexFloat.size(); i++) {
					if (IndexFloat[i]->getAttrName() == record->name && IndexFloat[i]->getTableName() == nData.getName()) {
						if (info->symbol == BIGGER)
							IndexFloat[i]->larger(BlockID, record->nFloat);
						else if (info->symbol == SMALLER)
							IndexFloat[i]->smaller(BlockID, record->nFloat);
						else if (info->symbol == EQUAL) {
							BlockID[0] = 1;
							BlockID[1] = IndexFloat[i]->equal(record->nFloat);
						}
						else if (info->symbol == EBIGGER)
							IndexFloat[i]->largerAndEqual(BlockID, record->nFloat);
						else if (info->symbol == ESMALLER)
							IndexFloat[i]->smallerAndEqual(BlockID, record->nFloat);
						else if (info->symbol == NEQUAL)
							IndexFloat[i]->notEqual(BlockID, record->nFloat);

						int offset = getOffSet(tablename, record->colnum);
						char K[4];
						memcpy(K, &record->nFloat, sizeof(float));

						TheSelectResult R = bufferMan.Select(tablename, BlockID, offset, sizeof(float), K, (eColType2)3, info->symbol);
						recordnum = R.RecordNum;
						pTBnode p = GlobalTB.getHead();
						while (p->Next)
						{
							p = p->Next;
							if (p->TBname == nData.getName()) break;
						}
						int ColNum = p->AttributeNum;

						pTableCol pTC = nData.getRecordHead();
						for (int i = 1; i <= ColNum; i++)
						{
							pTC = pTC->Next;
							switch (pTC->ColType)
							{
							case INT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case FLOAT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case CHAR:
								cout << setiosflags(ios::left) << setw(pTC->length) << pTC->name;
								break;
							default:
								break;
							}
						}
						cout << endl;
						for (int i = 0; i < R.RecordNum; i++)
						{
							pTC = nData.getRecordHead();
							for (int k = 1; k <= ColNum; k++)
							{
								int offset = getOffSet(nData.getName(), k);
								pTC = pTC->Next;
								switch (pTC->ColType)
								{
								case INT:
									printf("%-12d", *(int*)(R.Result + i*R.RecordSize + offset));
									break;
								case FLOAT:
									printf("%-12f", *(float*)(R.Result + i*R.RecordSize + offset));
									break;
								case CHAR:
									for (int m = 0; m < pTC->length; m++)
									{
										printf("%c", *(char*)(R.Result + i*R.RecordSize + offset + m));
									}
									break;
								default:
									break;
								}
							}
							cout << endl;
						}
						return;
					}
				}
			}
			else if (record->ColType == CHAR) {
				for (size_t i = 0; i < IndexStr.size(); i++) {
					if (IndexStr[i]->getAttrName() == record->name && IndexStr[i]->getTableName() == nData.getName()) {
						if (info->symbol == BIGGER)
							IndexStr[i]->larger(BlockID, record->nChar);
						else if (info->symbol == SMALLER)
							IndexStr[i]->smaller(BlockID, record->nChar);
						else if (info->symbol == EQUAL) {
							BlockID[0] = 1;
							BlockID[1] = IndexStr[i]->equal(record->nChar);
						}
						else if (info->symbol == EBIGGER)
							IndexStr[i]->largerAndEqual(BlockID, record->nChar);
						else if (info->symbol == ESMALLER)
							IndexStr[i]->smallerAndEqual(BlockID, record->nChar);
						else if (info->symbol == NEQUAL)
							IndexStr[i]->notEqual(BlockID, record->nChar);

						int offset = getOffSet(tablename, record->colnum);
						char *K = new char[record->length + 1];
						memcpy(K, record->nChar.c_str(), record->length);
						K[record->length] = '\0';

						TheSelectResult R = bufferMan.Select(tablename, BlockID, offset, record->length, K, (eColType2)2, info->symbol);
						recordnum = R.RecordNum;
						pTBnode p = GlobalTB.getHead();
						while (p->Next)
						{
							p = p->Next;
							if (p->TBname == nData.getName()) break;
						}
						int ColNum = p->AttributeNum;

						pTableCol pTC = nData.getRecordHead();
						for (int i = 1; i <= ColNum; i++)
						{
							pTC = pTC->Next;
							switch (pTC->ColType)
							{
							case INT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case FLOAT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case CHAR:
								cout << setiosflags(ios::left) << setw(pTC->length) << pTC->name;
								break;
							default:
								break;
							}
						}
						cout << endl;
						for (int i = 0; i < R.RecordNum; i++)
						{
							pTC = nData.getRecordHead();
							for (int k = 1; k <= ColNum; k++)
							{
								int offset = getOffSet(nData.getName(), k);
								pTC = pTC->Next;
								switch (pTC->ColType)
								{
								case INT:
									printf("%-12d", *(int*)(R.Result + i*R.RecordSize + offset));
									break;
								case FLOAT:
									printf("%-12f", *(float*)(R.Result + i*R.RecordSize + offset));
									break;
								case CHAR:
									for (int m = 0; m < pTC->length; m++)
									{
										printf("%c", *(char*)(R.Result + i*R.RecordSize + offset + m));
									}
									break;
								default:
									break;
								}
							}
							cout << endl;
						}
						return;
					}
				}
			}
		}
		else { // no index
			int num = 0;
			for (int i = 1; i < MAXBLOCKNUM + 1; i++) {
				if (T.TableBlock[i] == true) {
					BlockID[++num] = i;
				}
			}
			BlockID[0] = num;

			int offset = getOffSet(tablename, record->colnum);
			int size = 0;
			int type;
			char key[32];
			if (record->ColType == INT) {
				type = 1;
				size = sizeof(int);
				memcpy(key, &record->nInt, sizeof(int));
			}
			else if (record->ColType == CHAR) {
				size = record->length;
				type = 2;
				char *z = new char[size];
				strcpy(z, record->nChar.c_str());
				memcpy(key, z, size);
			}
			else if (record->ColType == FLOAT) {
				type = 3;
				size = sizeof(float);
				memcpy(key, &record->nFloat, sizeof(float));
			}
			TheSelectResult R = bufferMan.Select(tablename, BlockID, offset, size, key, (eColType2)type, nData.getConditionHead()->Next->symbol);
			recordnum = R.RecordNum;
			pTBnode p = GlobalTB.getHead();
			while (p->Next)
			{
				p = p->Next;
				if (p->TBname == nData.getName()) break;
			}
			int ColNum = p->AttributeNum;

			pTableCol pTC = nData.getRecordHead();
			for (int i = 1; i <= ColNum; i++)
			{
				pTC = pTC->Next;
				switch (pTC->ColType)
				{
				case INT:
					cout << setiosflags(ios::left) << setw(12) << pTC->name;
					break;
				case FLOAT:
					cout << setiosflags(ios::left) << setw(12) << pTC->name;
					break;
				case CHAR:
					cout << setiosflags(ios::left) << setw(pTC->length) << pTC->name;
					break;
				default:
					break;
				}
			}
			cout << endl;
			for (int i = 0; i < R.RecordNum; i++)
			{
				pTC = nData.getRecordHead();
				for (int k = 1; k <= ColNum; k++)
				{
					int offset = getOffSet(nData.getName(), k);
					pTC = pTC->Next;
					switch (pTC->ColType)
					{
						case INT:
							printf("%-12d", *(int*)(R.Result + i*R.RecordSize + offset));
							break;
						case FLOAT:
							printf("%-12f", *(float*)(R.Result + i*R.RecordSize + offset));
							break;
						case CHAR:
							for (int m = 0; m < pTC->length; m++)
							{
								printf("%c", *(char*)(R.Result + i*R.RecordSize + offset+m));
							}
							break;
						default:
							break;
					}
				}
				cout << endl;
			}
		}
		cout << "Search " << recordnum << " records totally!" << endl;
	}
	else //  select a1,a2 from... where ...
	{
		TableFile T = TableFile::ReadTableFromDisk(nData.getName());
		int BlockID[MAXBLOCKNUM + 1];
		string tablename = nData.getName();
		pCondition info = nData.getConditionHead()->Next;
		pRecordCol record = info->record;// where子句内容

		bool flag = hadIndex(tablename, record->name);
		if (flag) { // has index
			if (record->ColType == INT) {
				for (size_t i = 0; i < IndexInt.size(); i++) {
					if (IndexInt[i]->getAttrName() == record->name && IndexInt[i]->getTableName() == nData.getName()) {
						if (info->symbol == BIGGER)
							IndexInt[i]->larger(BlockID, record->nInt);
						else if (info->symbol == SMALLER)
							IndexInt[i]->smaller(BlockID, record->nInt);
						else if (info->symbol == EQUAL) {
							BlockID[0] = 1;
							BlockID[1] = IndexInt[i]->equal(record->nInt);
						}
						else if (info->symbol == EBIGGER)
							IndexInt[i]->largerAndEqual(BlockID, record->nInt);
						else if (info->symbol == ESMALLER)
							IndexInt[i]->smallerAndEqual(BlockID, record->nInt);
						else if (info->symbol == NEQUAL)
							IndexInt[i]->notEqual(BlockID, record->nInt);

						int offset = getOffSet(tablename, record->colnum);
						char K[4];
						memcpy(K, &record->nInt, sizeof(int));
						TheSelectResult R = bufferMan.Select(tablename, BlockID, offset, sizeof(int), K, (eColType2)1, info->symbol);
						recordnum = R.RecordNum;
						pTableCol pTC = nData.getRecordHead();
						while (pTC->Next)
						{
							pTC = pTC->Next;
							switch (pTC->ColType)
							{
							case INT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case FLOAT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case CHAR:
								cout << setiosflags(ios::left) << setw(pTC->length) << pTC->name;
								break;
							default:
								break;
							}
						}
						cout << endl;


						for (int i = 0; i < R.RecordNum; i++)
						{
							pTC = nData.getRecordHead();
							while (pTC->Next)
							{

								pTC = pTC->Next;
								int offset = getOffSet(nData.getName(), pTC->colnum);
								switch (pTC->ColType)
								{
								case INT:
									printf("%-12d", *(int*)(R.Result + i*R.RecordSize + offset));
									break;
								case FLOAT:
									printf("%-12f", *(float*)(R.Result + i*R.RecordSize + offset));
									break;
								case CHAR:
									for (int m = 0; m < pTC->length; m++)
									{
										printf("%c", *(char*)(R.Result + i*R.RecordSize + offset + m));
									}
									break;
								default:
									break;

								}

							}
							cout << endl;
						}
					}
				}
			}
			else if (record->ColType == FLOAT) {
				for (size_t i = 0; i < IndexFloat.size(); i++) {
					if (IndexFloat[i]->getAttrName() == record->name && IndexFloat[i]->getTableName() == nData.getName()) {
						if (info->symbol == BIGGER)
							IndexFloat[i]->larger(BlockID, record->nFloat);
						else if (info->symbol == SMALLER)
							IndexFloat[i]->smaller(BlockID, record->nFloat);
						else if (info->symbol == EQUAL) {
							BlockID[0] = 1;
							BlockID[1] = IndexFloat[i]->equal(record->nFloat);
						}
						else if (info->symbol == EBIGGER)
							IndexFloat[i]->largerAndEqual(BlockID, record->nFloat);
						else if (info->symbol == ESMALLER)
							IndexFloat[i]->smallerAndEqual(BlockID, record->nFloat);
						else if (info->symbol == NEQUAL)
							IndexFloat[i]->notEqual(BlockID, record->nFloat);

						int offset = getOffSet(tablename, record->colnum);
						char K[4];
						memcpy(K, &record->nFloat, sizeof(float));
						TheSelectResult R = bufferMan.Select(tablename, BlockID, offset, sizeof(float), K, (eColType2)3, info->symbol);
						recordnum = R.RecordNum;
						pTableCol pTC = nData.getRecordHead();
						while (pTC->Next)
						{
							pTC = pTC->Next;
							switch (pTC->ColType)
							{
							case INT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case FLOAT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case CHAR:
								cout << setiosflags(ios::left) << setw(pTC->length) << pTC->name;
								break;
							default:
								break;
							}
						}
						cout << endl;


						for (int i = 0; i < R.RecordNum; i++)
						{
							pTC = nData.getRecordHead();
							while (pTC->Next)
							{

								pTC = pTC->Next;
								int offset = getOffSet(nData.getName(), pTC->colnum);
								switch (pTC->ColType)
								{
								case INT:
									printf("%-12d", *(int*)(R.Result + i*R.RecordSize + offset));
									break;
								case FLOAT:
									printf("%-12f", *(float*)(R.Result + i*R.RecordSize + offset));
									break;
								case CHAR:
									for (int m = 0; m < pTC->length; m++)
									{
										printf("%c", *(char*)(R.Result + i*R.RecordSize + offset + m));
									}
									break;
								default:
									break;

								}

							}
							cout << endl;
						}
					}
				}
			}
			else if (record->ColType == CHAR) {
				for (size_t i = 0; i < IndexStr.size(); i++) {
					if (IndexStr[i]->getAttrName() == record->name && IndexStr[i]->getTableName() == nData.getName()) {
						if (info->symbol == BIGGER)
							IndexStr[i]->larger(BlockID, record->nChar);
						else if (info->symbol == SMALLER)
							IndexStr[i]->smaller(BlockID, record->nChar);
						else if (info->symbol == EQUAL) {
							BlockID[0] = 1;
							BlockID[1] = IndexStr[i]->equal(record->nChar);
						}
						else if (info->symbol == EBIGGER)
							IndexStr[i]->largerAndEqual(BlockID, record->nChar);
						else if (info->symbol == ESMALLER)
							IndexStr[i]->smallerAndEqual(BlockID, record->nChar);
						else if (info->symbol == NEQUAL)
							IndexStr[i]->notEqual(BlockID, record->nChar);

						int offset = getOffSet(tablename, record->colnum);
						char *K = new char[record->length+1];
						memcpy(K, record->nChar.c_str(), record->length);
						K[record->length] = '\0';
						TheSelectResult R = bufferMan.Select(tablename, BlockID, offset, record->length, K, (eColType2)2, info->symbol);
						recordnum = R.RecordNum;
						pTableCol pTC = nData.getRecordHead();
						while (pTC->Next)
						{
							pTC = pTC->Next;
							switch (pTC->ColType)
							{
							case INT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case FLOAT:
								cout << setiosflags(ios::left) << setw(12) << pTC->name;
								break;
							case CHAR:
								cout << setiosflags(ios::left) << setw(pTC->length) << pTC->name;
								break;
							default:
								break;
							}
						}
						cout << endl;


						for (int i = 0; i < R.RecordNum; i++)
						{
							pTC = nData.getRecordHead();
							while (pTC->Next)
							{

								pTC = pTC->Next;
								int offset = getOffSet(nData.getName(), pTC->colnum);
								switch (pTC->ColType)
								{
								case INT:
									printf("%-12d", *(int*)(R.Result + i*R.RecordSize + offset));
									break;
								case FLOAT:
									printf("%-12f", *(float*)(R.Result + i*R.RecordSize + offset));
									break;
								case CHAR:
									for (int m = 0; m < pTC->length; m++)
									{
										printf("%c", *(char*)(R.Result + i*R.RecordSize + offset + m));
									}
									break;
								default:
									break;

								}

							}
							cout << endl;
						}
					}
				}
			}
		}
		else { // no index
			int num = 0;
			for (int i = 1; i < MAXBLOCKNUM + 1; i++) {
				if (T.TableBlock[i] == true) {
					BlockID[++num] = i;
				}
			}
			BlockID[0] = num;

			int offset = getOffSet(tablename, record->colnum);
			int size = 0;
			int type;
			char key[32];
			if (record->ColType == INT) {
				type = 1;
				size = sizeof(int);
				memcpy(key, &record->nInt, sizeof(int));
			}
			else if (record->ColType == CHAR) {
				size = record->length;
				type = 2;
				char *z = new char[size];
				strcpy(z, record->nChar.c_str());
				memcpy(key, z, size);
			}
			else if (record->ColType == FLOAT) {
				type = 3;
				size = sizeof(float);
				memcpy(key, &record->nFloat, sizeof(float));
			}
			TheSelectResult R = bufferMan.Select(tablename, BlockID, offset, size, key, (eColType2)type, nData.getConditionHead()->Next->symbol);
			recordnum = R.RecordNum;
			pTableCol pTC = nData.getRecordHead();
			while (pTC->Next)
			{
				pTC = pTC->Next;
				switch (pTC->ColType)
				{
				case INT:
					cout << setiosflags(ios::left) << setw(12) << pTC->name;
					break;
				case FLOAT:
					cout << setiosflags(ios::left) << setw(12) << pTC->name;
					break;
				case CHAR:
					cout << setiosflags(ios::left) << setw(pTC->length) << pTC->name;
					break;
				default:
					break;
				}
			}
			cout << endl;


			for (int i = 0; i < R.RecordNum; i++)
			{
				pTC = nData.getRecordHead();
				while (pTC->Next)
				{

					pTC = pTC->Next;
					int offset = getOffSet(nData.getName(), pTC->colnum);
					switch (pTC->ColType)
					{
						case INT:
							printf("%-12d", *(int*)(R.Result + i*R.RecordSize + offset));
							break;
						case FLOAT:
							printf("%-12f", *(float*)(R.Result + i*R.RecordSize + offset));
							break;
						case CHAR:
							for (int m = 0; m < pTC->length; m++)
							{
								printf("%c", *(char*)(R.Result + i*R.RecordSize + offset + m));
							}
							break;
						default:
							break;
						
					}
					
				}
				cout << endl;
			}
		}
		cout << "Search " << recordnum << " records totally!" << endl;
	}
}

/* TBname - table name
* mode=1 -> primary key
* mode=2 -> unique key*/
pTableCol getTableKey(string TBname, int mode)
{
	pTableCol result = new TableCol;
	pTBnode tmp = GlobalTB.getHead();
	while (tmp->Next != NULL)
	{
		tmp = tmp->Next;
		if (tmp->TBname == TBname)
		{
			pTableCol check = tmp->head;
			while (check->Next != NULL)
			{
				check = check->Next;
				if (check->isPrimary == 1 && mode == 1)
				{
					result->colnum = check->colnum;
					result->ColType = check->ColType;
					result->isDiffer = check->isDiffer;
					result->isPrimary = check->isPrimary;
					result->length = check->length;
					result->name = check->name;
					return result;
				}
				else if (check->isDiffer == 1 && mode == 2)
				{
					result->colnum = check->colnum;
					result->ColType = check->ColType;
					result->isDiffer = check->isDiffer;
					result->isPrimary = check->isPrimary;
					result->length = check->length;
					result->name = check->name;
					return result;
				}
			}
		}
	}
	return NULL;
}

/*get offset before colnum i*/
eColType getAttrType(string tablename, string attr) {
	int size = 0;
	pTBnode check = GlobalTB.getHead();
	while (check->Next != NULL)
	{
		check = check->Next;
		if (check->TBname == tablename)
		{
			pTableCol tem = check->head;
			while (tem->Next != NULL)
			{
				tem = tem->Next;
				if (tem->name == attr)
					return tem->ColType;
			}
		}
	}
}

int getOffSet(string tablename, int colnum) {
	int size = 0;
	pTBnode check = GlobalTB.getHead();
	while (check->Next != NULL)
	{
		check = check->Next;
		if (check->TBname == tablename)
		{
			pTableCol tem = check->head;
			while (tem->Next != NULL)
			{
				tem = tem->Next;
				if (tem->colnum == colnum)
					return size;
				else if (tem->ColType == INT)
					size += sizeof(int);
				else if (tem->ColType == FLOAT)
					size += sizeof(float);
				else if (tem->ColType == CHAR)
					size += sizeof(char)*tem->length;

			}
		}
	}
	return -1;
}

void DropDB(nDropDB& data)
{
	pDBnode check = GlobalDB.getHead();
	pDBnode befCheck;
	while (check->Next != NULL)
	{
		befCheck = check;
		check = check->Next;
		if (check->name == data.getName())
		{
			befCheck->Next = check->Next;
			GlobalDB.NumSub();
		}
	}
	string path = "miniSQL\\";
	path += data.getName();
	removeDir(path.c_str());
	path += "_table.txt";
	remove(path.c_str());
	if (strcmp(data.getName().c_str(), curDBname) == 0)
	{
		memset(curDBname, 0, sizeof(curDBname));
		isOpenDB = 0;
	}

}
void DropTB(nDropTB& data)
{
	pTBnode check = GlobalTB.getHead();
	pTBnode befCheck;
	while (check->Next!=NULL)
	{
		befCheck = check;
		check = check->Next;
		if (check->TBname == data.getName())
		{
			befCheck->Next = check->Next;
			GlobalTB.TbNumSub();
			//一个删除表内所有index
			GlobalTB.IndexNumSub(IndexInt.size() + IndexFloat.size() + IndexStr.size());
			char path[256] = "miniSQL\\";
			strcat(path, curDBname);
			strcat(path, "_index\\");
			
			char path_prim[256];
			strcpy(path_prim, path);
			strcat(path_prim, "Prim");
			strcat(path_prim, data.getName().c_str());
			remove(path_prim);
			char path_uniq[256];
			strcpy(path_prim, path);
			strcat(path_prim, "Uniq");
			strcat(path_prim, data.getName().c_str());
			remove(path_prim);
			
			IndexInt.clear();
			IndexFloat.clear();
			IndexStr.clear();
			
			return;
		}
	}
}

void DropIndex(nDropIndex& ndata) {
	vector<string>::iterator k = index.begin();
	for (; k != index.end(); k++) {
		if (*k == ndata.getIndexName()) 
			index.erase(k);
	}
	cout << "Delete index : " << ndata.getIndexName() << "successfully!" << endl;
}

int  removeDir(const char*  dirPath)
{

	struct _finddata_t fb;   //查找相同属性文件的存储结构体  
	char  path[250];
	long    handle;
	int  resultone;
	int   noFile;            //对系统隐藏文件的处理标记  

	noFile = 0;
	handle = 0;


	//制作路径  
	strcpy(path, dirPath);
	strcat(path, "/*");

	handle = _findfirst(path, &fb);
	//找到第一个匹配的文件  
	if (handle != 0)
	{
		//当可以继续找到匹配的文件，继续执行  
		while (0 == _findnext(handle, &fb))
		{
			//windows下，常有个系统文件，名为“..”,对它不做处理  
			noFile = strcmp(fb.name, "..");

			if (0 != noFile)
			{
				//制作完整路径  
				memset(path, 0, sizeof(path));
				strcpy(path, dirPath);
				strcat(path, "/");
				strcat(path, fb.name);
				//属性值为16，则说明是文件夹，迭代  
				if (fb.attrib == 16)
				{
					removeDir(path);
				}
				//非文件夹的文件，直接删除。对文件属性值的情况没做详细调查，可能还有其他情况。  
				else
				{
					remove(path);
				}
			}
		}
		//关闭文件夹，只有关闭了才能删除。找这个函数找了很久，标准c中用的是closedir  
		//经验介绍：一般产生Handle的函数执行后，都要进行关闭的动作。  
		_findclose(handle);
	}
	//移除文件夹  
	resultone = _rmdir(dirPath);
	return  resultone;
}