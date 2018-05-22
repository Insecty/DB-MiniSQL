#include "miniSQL.h"
#include <direct.h>
#include <io.h>

int main()
{
	InitProgram();
	RunProgram();
	ExitProgram();
	return 1;
}
void InitProgram()
{
	cout << "   __  __ _       _ ____   ___  _" << endl;
	cout << "  |  \\/  (_)_ __ (_) ___| / _ \\| |" << endl;
	cout << "  | |\\/| | | __ \\| \\___ \\| | | | |" << endl;
	cout << "  | |  | | | | | | |___) | |_| | |___" << endl;
	cout << "  |_|  |_|_|_| |_|_|____/ \\__\\_\\_____|" << endl;
	cout << "===========================================" << endl;
	cout << "                 Authors: " << endl;
	cout << "        Qingpeng Cai  Chunyi Liu    " << endl;
	cout << "===========================================" << endl;
	if (_access("miniSQL", 00) == -1)		//File: miniSQL is not exist
		_mkdir("miniSQL");
	if (_access("miniSQL\\Database.catalog", 00) != 0)
	{
		FILE *fp = fopen("miniSQL\\Database.catalog","w");
		fwrite("0#", sizeof(char), 2, fp);
		fclose(fp);
	}
	GlobalDB.DBUpdate("miniSQL\\Database.catalog");
	//DatabaseInfo GlobalDB("miniSQL\\Database.catalog");

//	if (_access("Table.catalog", 00) == -1)
//		_mkdir("Table.catalog");
//	if (_access("Index.catalog", 00) == -1)
//		_mkdir("Index.catalog");
	
}
void RunProgram()
{
	Message msg = eMsgDEFAULT;
	isOpenDB = 0;
	char command[200];
	char file_name[100];
	int file_positoin = 0;
	bool source_flag = 0;
	memset(file_name, 0, sizeof(file_name));

	while (msg != EXIT) {
		cout << "MiniSQL>";
		memset(command, 0, sizeof(command));
		if (!source_flag)
			cin.get(command, 200, ';');
		else 
		{
			file_positoin = ReadCommandFromFile(command,file_name, file_positoin);
			if(file_positoin ==0)
				source_flag = 0;
			cout << command << endl;
		}
		cTable nCreateTB;
		nRecord nRecordData;
		nCondition nConData;
		base nData;
		DataBase &nCreateDB = nData;
		nIndex nIndexData;
		nCreateIndex &nCreateIndexData = nIndexData;
		DataBase &nUseDB = nData;
		nSelect &nSelectData = nConData;
		nDelete &nDeleteData = nConData;
		nDropDB &nDropDbData = nData;
		nDropTB &nDropTbData = nData;
		nDropIndex &nDropIndexData = nIndexData;
		nUpdate nUpdateData;

		msg = Resolution(command,nCreateTB, nIndexData, nData, nRecordData, nConData, nUpdateData);
		switch (msg)
		{
		case eMsgDEFAULT:
			break;
		case CREATEDB:
			CreateDatabase(nCreateDB);
			break;
		case CREATETABLE:
			CreateTable(nCreateTB);
			break;
		case CREATEINDEX:
			CreateIndex(nCreateIndexData);
			break;
		case INSERT:
			if(checkRecord(nRecordData)==1)
				InsertData(nRecordData);	
			break;
		case SELECT:
			if(checkSelect(nSelectData)==1)
				SelectData(nSelectData,source_flag);
			break;
		case USE:
			UseDB(nUseDB);
			break;
		case SOURCE:
			if (checkSourceFile(nData) == 1)
			{
				source_flag = 1;
				strcpy(file_name, nData.getName().c_str());
			}
			break;
		case HELP:
			break;
		case update:
			if (checkUpdate(nUpdateData) == 1)
				//updateData(nupdateData);
			break;
		case DELETE:
			if(checkDelete(nDeleteData)==1)
				DeleteData(nDeleteData);
			break;
		case DROPINDEX:
			if (checkDropIndex(nDropIndexData) == true)
				DropIndex(nDropIndexData);
			break;
		case DROPDB:
			if (checkDropDB(nDropDbData) == 1)
				DropDB(nDropDbData);
			break;
		case DROPTABLE:
			if (checkDropTB(nDropTbData) == 1)
				DropTB(nDropTbData);
			break;
		case EXIT:
			cout << "bye!bye!" << endl;
			return;
		case MSGERROR:
			break;
		default:
			break;
		}
		if (!source_flag)
		{
			cin.clear();		//clear the input cache
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}
}
void ExitProgram()
{
	FILE *fp = fopen("miniSQL\\Database.catalog", "w");
	pDBnode head = GlobalDB.getHead();
	while (head->Next != NULL)
	{
		head = head->Next;
		if (head->name == (string)curDBname)
		{
			head->TableNum = GlobalTB.getTbNum();
			head->IndexNum = GlobalTB.getIndexNum();
			break;
		}
	}
	WriteDBcat(GlobalDB, fp);
	fclose(fp);
	if (isOpenDB == 1)
	{
		char path[256]="miniSQL\\";
		strcat(path, curDBname);
		strcat(path, "_table.txt");
		fp = fopen(path, "w");
		WriteTBcat(GlobalTB, fp);
		fclose(fp);
	}
	for (size_t i = 0; i < IndexInt.size(); i++) {
		IndexInt[i]->save();
	}
	for (size_t i = 0; i < IndexFloat.size(); i++) {
		IndexFloat[i]->save();
	}
	for (size_t i = 0; i < IndexStr.size(); i++) {
		IndexStr[i]->save();
	}
}
void WriteDBcat(DatabaseInfo& sourceDB, FILE *fp)
{
	string data;
	data = intTostring(sourceDB.getNum());
	data += '#';
	fwrite(data.c_str(), sizeof(char), data.size(), fp);

	pDBnode tem;
	tem = sourceDB.getHead();
	string num;
	for (int i = 0; i < sourceDB.getNum(); i++)
	{
		tem = tem->Next;
		fwrite(tem->name.c_str(), sizeof(char), tem->name.size(), fp);
		fwrite(",", sizeof(char), 1, fp);
		num = intTostring(tem->TableNum);
		fwrite(num.c_str(), sizeof(char), num.size(), fp);
		fwrite(",", sizeof(char), 1, fp);
		num = intTostring(tem->IndexNum);
		fwrite(num.c_str(), sizeof(char), num.size(), fp);
		fwrite("|", sizeof(char), 1, fp);
	}
}
void WriteTBcat(TableInfo &sourceTB, FILE *fp)
{
	string data;
	data = intTostring(sourceTB.getTbNum());
	data += '#';
	fwrite(data.c_str(), sizeof(char), data.size(), fp);

	pTBnode tem=sourceTB.getHead();
	string num;
	pTableCol pCol;
	for (int i = 0; i < sourceTB.getTbNum();i++)
	{
		tem = tem->Next;
		fwrite(tem->TBname.c_str(), sizeof(char), tem->TBname.size(), fp);
		fwrite(",", sizeof(char), 1, fp);
		num = intTostring(tem->AttributeNum);
		fwrite(num.c_str(), sizeof(char), num.size(), fp);
		fwrite(",", sizeof(char), 1, fp);
		num = intTostring(tem->RecordNum);
		fwrite(num.c_str(), sizeof(char), num.size(), fp);
		fwrite(":", sizeof(char), 1, fp);
		pCol = tem->head;
		for (int j = 0; j < tem->AttributeNum; j++)
		{
			pCol = pCol->Next;
			num = intTostring(pCol->colnum);
			fwrite(num.c_str(), sizeof(char), num.size(), fp);
			fwrite(",", sizeof(char), 1, fp);
			fwrite(pCol->name.c_str(), sizeof(char), pCol->name.size(), fp);
			fwrite(",", sizeof(char), 1, fp);
			if (pCol->ColType == INT)
				fwrite("1", sizeof(char), 1, fp);
			else if (pCol->ColType == CHAR)
			{
				fwrite("2", sizeof(char), 1, fp);
				fwrite(",", sizeof(char), 1, fp);
				num = intTostring(pCol->length);
				fwrite(num.c_str(), sizeof(char), num.size(), fp);
			}
			else if (pCol->ColType == FLOAT)
				fwrite("3", sizeof(char), 1, fp);
			fwrite(",", sizeof(char), 1, fp);
			num = intTostring(pCol->isPrimary);
			fwrite(num.c_str(), sizeof(char), 1, fp);
			fwrite(",", sizeof(char), 1, fp);
			num = intTostring(pCol->isDiffer);
			fwrite(num.c_str(), sizeof(char), 1, fp);
			fwrite("|", sizeof(char), 1, fp);
		}
	}
}

string intTostring(int num)
{
	if (num == 0)
		return (string)"0";
	char result[10];
	sprintf(result, "%d", num);
	return result;
}
string floatTostring(float num)
{
	if (num == 0)
		return (string)"0";
	float check = num;
	int flg = 0;
	string result;
	while ((int)check != check)
	{
		check *= 10;
		flg++;
	}
	result = intTostring((int)check);
	result.insert(result.size() - flg, ".");
	return result;
}
bool checkRecord(nRecord &record)
{
	pTBnode check = GlobalTB.getHead();
	while (check->Next != NULL)
	{
		check = check->Next;
		if (check->TBname == record.getName())
			break;
	}
	if (check->Next == NULL&&check->TBname != record.getName())
	{
		cout << "Database: " << curDBname << " doesn't exist table: " << record.getName() << endl;
		return 0;
	}
	else if (check->AttributeNum != record.getNum())
	{
		cout << record.getName() << " has problem in number of atribute!" << endl;
		return 0;
	}
	pTableCol node = check->head;
	pRecordCol data = record.getHead();
	while (node->Next != NULL&&data->Next != NULL)
	{
		node = node->Next;
		data = data->Next;
		data->name = node->name;
		while (node->ColType != data->ColType)
		{
			if (node->ColType == FLOAT && data->ColType == INT)
			{
				data->ColType = FLOAT;
				data->nFloat = (float)data->nInt;
				break;
			}
			cout << "col: " << node->colnum << " is not corresponding! " << endl;
			return 0;
		}
		if (data->ColType == CHAR)
		{
			data->length = node->length;
			data->nChar.resize(data->length, ' ');
			record.addSize(sizeof(char)*data->length);
		}
		if (data->ColType == INT)
		{
			record.addSize(sizeof(int));
		}
		else if (data->ColType == FLOAT)
		{
			record.addSize(sizeof(float));
		}
	}
	if (node->colnum == check->AttributeNum)
		return 1;
	return 0;
}
bool checkSelect(nSelect &select)
{
	pTBnode check = GlobalTB.getHead();
	while (check->Next != NULL)
	{
		check = check->Next;
		if (check->TBname == select.getName())
			break;
	}
	if (check->Next == NULL&&check->TBname != select.getName())
	{
		cout << "Database: " << curDBname << " doesn't exist table: " << select.getName() << endl;
		return 0;
	}
	if (select.getAll() == 1)
	{

		pTableCol node = check->head;
		while (node->Next != NULL)
		{
			node = node->Next;
			pTableCol data = new TableCol;
			data->ColType = node->ColType;
			data->colnum = node->colnum;
			data->isDiffer = node->isDiffer;
			data->isPrimary = node->isPrimary;
			data->length = node->length;
			data->name = node->name;
			data->Next = select.rend->Next;
			select.rend->Next = data;
			select.rend = select.rend->Next;
		}
	}
	else
	{
		pTableCol data = select.getRecordHead();
		while (data->Next != NULL)
		{
			data = data->Next;
			pTableCol node = check->head;
			while (node->Next != NULL)
			{
				node = node->Next;
				if (data->name == node->name)
				{
					data->ColType = node->ColType;
					data->colnum = node->colnum;
					data->isDiffer = node->isDiffer;
					data->isPrimary = node->isPrimary;
					data->length = node->length;
					break;
				}
			}
			if (node->Next == NULL&&data->name != node->name)
				return 0;
		}
	}

	pCondition tem = select.getConditionHead();
	while (tem->Next != NULL)
	{
		tem = tem->Next;
		pTableCol node = check->head;
		while (node->Next != NULL)
		{
			node = node->Next;
			if (tem->record->name == node->name)
			{
				//tem->record->ColType = node->ColType;
				tem->record->colnum = node->colnum;
				tem->record->length = node->length;
				if (tem->record->ColType == CHAR)
					tem->record->nChar.resize(tem->record->length, ' ');
				if (node->ColType == FLOAT && tem->record->ColType == INT)
				{
					tem->record->ColType = FLOAT;
					tem->record->nFloat = (float)tem->record->nInt;
				}
				break;
			}
		}
		if (node->Next == NULL&&tem->record->name != node->name)
			return 0;
	}

	if (tem->Next == NULL)
		return 1;
	return 0;
}
bool checkDelete(nDelete &Delete)
{
	pTBnode check = GlobalTB.getHead();
	while (check->Next != NULL)
	{
		check = check->Next;
		if (check->TBname == Delete.getName())
			break;
	}
	if (check->Next == NULL&&check->TBname != Delete.getName())
	{
		cout << "Database: " << curDBname << " doesn't exist table: " << Delete.getName() << endl;
		return 0;
	}

	pCondition tem = Delete.getConditionHead();
	while (tem->Next != NULL)
	{
		tem = tem->Next;
		pTableCol node = check->head;
		while (node->Next != NULL)
		{
			node = node->Next;
			if (tem->record->name == node->name)
			{
				tem->record->colnum = node->colnum;
				tem->record->length = node->length;
				if (tem->record->ColType == CHAR)
					tem->record->nChar.resize(tem->record->length, ' ');
				if (node->ColType == FLOAT && tem->record->ColType == INT)
				{
					tem->record->ColType = FLOAT;
					tem->record->nFloat = (float)tem->record->nInt;
				}
				break;
			}
		}
		if (node->Next == NULL&&tem->record->name != node->name)
			return 0;
	}

	if (tem->Next == NULL)
		return 1;
	return 0;

}
bool checkUpdate(nUpdate &Update)
{
	pTBnode check = GlobalTB.getHead();
	while (check->Next != NULL)
	{
		check = check->Next;
		if (check->TBname == Update.getName())
			break;
	}
	if (check->Next == NULL&&check->TBname != Update.getName())
	{
		cout << "Database: " << curDBname << " doesn't exist table: " << Update.getName() << endl;
		return 0;
	}
	pCondition data = Update.getRecordHead();
	while (data->Next != NULL)
	{
		data = data->Next;
		pTableCol node = check->head;
		while (node->Next != NULL)
		{
			node = node->Next;
			if (data->record->name == node->name)
			{
				data->record->colnum = node->colnum;
				data->record->length = node->length;
				break;
			}
		}
		if (node->Next == NULL&&data->record->name != node->name)
			return 0;
	}
	pCondition tem = Update.getConditionHead();
	while (tem->Next != NULL)
	{
		tem = tem->Next;
		pTableCol node = check->head;
		while (node->Next != NULL)
		{
			node = node->Next;
			if (tem->record->name == node->name)
			{
				tem->record->colnum = node->colnum;
				tem->record->length = node->length;
				break;
			}
		}
		if (node->Next == NULL&&tem->record->name != node->name)
			return 0;
	}

	if (data->Next == NULL&&tem->Next == NULL)
		return 1;
	return 0;
}
bool checkDropIndex(nDropIndex &nIndexData)
{
	for (size_t i = 0; i < index.size(); i++) {
		if (index[i] == nIndexData.getIndexName()) {
			return true;
		}
	}
	cout << "Sorry! No such index. Please create first!" << endl;
	return false;
}
bool checkDropDB(nDropDB &nDbData)
{
	pDBnode check = GlobalDB.getHead();
	while (check->Next != NULL)
	{
		check = check->Next;
		if (check->name == nDbData.getName())
		{
			if (check->name == (string)curDBname)
			{
				cout << "You are using databse " << nDbData.getName() << " Please close it first!" << endl;
				return 0;
			}else return 1;
		}
	}
	cout << "Database: " << nDbData.getName() << "is not existed! " << endl;
	return 0;
}
bool checkDropTB(nDropTB &nTbData)
{
	pTBnode check = GlobalTB.getHead();
	while (check->Next != NULL)
	{
		check = check->Next;
		if (check->TBname == nTbData.getName())
			return 1;
	}
	cout << "Table: " << nTbData.getName() << "is not existed! " << endl;
	return 0;
}
int ReadCommandFromFile(char* cmd, char * fname,int position)
{
	int i = 0;
	FILE* fp = fopen(fname, "r+");
	if (!fp)
	{
		cout << "can't open the file" << endl;
		cout << "Please check the file existence!" << endl;
	}
	fseek(fp, position, SEEK_SET);
	char ch = fgetc(fp);
	while (ch == ' '||ch=='\n')ch = fgetc(fp);
	while (ch != EOF&&ch != ';')
	{
		cmd[i++] = ch;
		ch = fgetc(fp);
	}
	position = ftell(fp);
	if (ch == ';')
	{
		cmd[i] = ';';
		return position;
	}
	else if (ch == EOF)
		return 0;
}
bool checkSourceFile(nSourceFile &nSourceData)
{
	if (_access(nSourceData.getName().c_str(), 00) != 0)
	{
		cout << "Can't find the path: " << nSourceData.getName().c_str() << endl;
		cout << "Please check the path!" << endl;
		return 0;
	}
	else return 1;
}