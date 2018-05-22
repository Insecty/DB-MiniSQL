#include"interpreter.h"
#include<stdlib.h>

char* CommError[] = {
	"Can't resolve this command! ",
	"Wrong name with keyword: ",
	"Can't resolve this command! ",
	"There left a symbol like '(' ",
	"There left a symbol like ')' ",
	"Table name can't include space like ' ' ",
	"Command need a legal table name! ",
	"There need a legal data type! ",
	"There is some mistakes in set Primary key",
	"Command need a end symbol like ';'",
	"There is some mistake value in the command like: "
};
char* CommSolution[] = {
	"Do you want to input 'create database' or 'create index' or 'create table' ",
	"Please input right name! ",
	"Do you want to input 'drop database' or 'drop index' or 'drop table' ",
	"Please add a left brace like '(' in command! ",
	"Please add a left brace like ')' in command! ",
	"Please input the normative table name! ",
	"Please input a legal table name!",
	"Please input a legal data type! like 'int' 'char(10)' 'float'",
	"Please input a legal primary key name! ",
	"Please add a symbol like ';' in the end! ",
	"Please check the data you inputed in the command! "
};
MsgType Resolution(char* command,cTable &nCreateTB,nIndex &nIndexData, base &nData, nRecord &nRecordData, nCondition &nConData, nUpdate &nUpdateData)
{
	try
	{
		Command cmd(command);
		if (cmd.getCmdType() == MSGERROR)
			return MSGERROR;
		switch (cmd.getCmdType())
		{
		case eMsgDEFAULT:
			break;
		case CREATEDB:
			cmd.CreateDatabase(nData);
			break;
		case CREATETABLE:
			cmd.CreateTable(nCreateTB);
			break;
		case CREATEINDEX:
			cmd.CreateIndex(nIndexData);
			break;
		case INSERT:
			cmd.InsertTable(nRecordData);
			break;
		case SELECT:
			cmd.SelectResult(nConData);
			break;
		case USE:
			cmd.UseDB(nData);
			break;
		case SOURCE:
			cmd.SourceFile(nData);
			break;
		case HELP:
			break;
		case update:
			cmd.UpdateResult(nUpdateData);
			break;
		case DELETE:
			cmd.DeleteResult(nConData);
			break;
		case DROPINDEX:
			cmd.DropIndex(nIndexData);
			break;
		case DROPDB:
			cmd.DropDB(nData);
			break;
		case DROPTABLE:
			cmd.DropTB(nData);
			break;
		case EXIT:
			break;
		default:
			break;
		}
		return cmd.getCmdType();
	}
	catch (CmdErrType ERROR)
	{
		cout << CommError[(int)ERROR] << endl;
		cout << CommSolution[(int)ERROR] << endl;
		return MSGERROR;
	}
	catch (string name)
	{
		cout << CommError[errName] << name << endl;
		cout << CommSolution[errName] << endl;
		return MSGERROR;
	}

}
MsgType Command::CmdType()
{
	string word;
	try {
		SkipCharacter(' ');
		word = ReadCommand(' ');
		if (word.compare("create") == 0)
		{
			SkipCharacter(' ');
			word = ReadCommand(' ');
			if (word.compare("database") == 0)
				CMDtype = CREATEDB;
			else if (word.compare("index") == 0)
				CMDtype = CREATEINDEX;
			else if (word.compare("table") == 0)
				CMDtype = CREATETABLE;
			else if (word.compare("database") != 0
				&& word.compare("index") != 0
				&& word.compare("table") != 0)
				throw errCreate;
		}
		else if (word.compare("drop") == 0)
		{
			SkipCharacter(' ');
			word = ReadCommand(' ');
			if (word.compare("database") == 0)
				CMDtype = DROPDB;
			else if (word.compare("index") == 0)
				CMDtype = DROPINDEX;
			else if (word.compare("table") == 0)
				CMDtype = DROPTABLE;
			else if (word.compare("database") != 0
				&& word.compare("index") != 0
				&& word.compare("table") != 0)
				throw errDrop;
		}
		else if (word.compare("insert") == 0)
		{

			SkipCharacter(' ');
			word = ReadCommand(' ');
			if (word.compare("into") == 0)
				CMDtype = INSERT;
			else throw word;
		}
		else if (word.compare("select") == 0)
			CMDtype = SELECT;
		else if (word.compare("use") == 0)
			CMDtype = USE;
		else if (word.compare("source") == 0)
			CMDtype = SOURCE;
		else if (word.compare("help") == 0)
			CMDtype = HELP;
		else if (word.compare("delete") == 0)
		{
			SkipCharacter(' ');
			word = ReadCommand(' ');
			if (word.compare("from") == 0)
				CMDtype = DELETE;
		}
		else if (word.compare("exit") == 0)
			CMDtype = EXIT;
		else return MSGERROR;
		return CMDtype;
	}
	catch (CmdErrType ERROR)
	{
		cout << CommError[(int)ERROR] << endl;
		cout << CommSolution[(int)ERROR] << endl;
		return MSGERROR;
	}
	catch (string name)
	{
		cout << CommError[errName] << name << endl;
		cout << CommSolution[errName] << endl;
		return MSGERROR;
	}
	return CMDtype;
}
Command::Command(char* cmd) :position(0), CMDtype(eMsgDEFAULT), length(0)
{
	strcpy(command, cmd);
	CalLength();
	CmdType();
}
Command::~Command()
{
	//	delete[] command;
}
int Command::SkipCharacter(char ch1, char ch2)
{
	while (command[position] == ch1 || command[position] == ch2)position++;
	return position;
}
string Command::ReadCommand(char EndSymbol, char dEndSymbol)
{
	string result;
	while (command[position] != EndSymbol&&command[position] != dEndSymbol)
	{
		if (command[position] != '\n')
			result += command[position];
		position++;
		if (position == length)
			break;
	}
	CheckWord(result, 1);
	return result;
}
void Command::CalLength()
{
	while (command[length] != '\0')length++;
	command[length++] = ';';
	command[length] = '\0';
}

cTable::cTable() :TalColumn(0)
{
	head = new TableCol;
	head->colnum = 0;
	head->ColType = eColDEFAULT;
	head->isDiffer = 0;
	head->isPrimary = 0;
	head->length = 0;
	head->Next = NULL;
	end = head;
}
cTable::~cTable()
{
	pTableCol p = head, q = head->Next;
	while (q != NULL) {
		delete p;
		p = q;
		q = q->Next;
	}
	delete p;
}


int Command::CreateDatabase(DataBase &nCreateDB)
{
	return setName(nCreateDB);
}
int Command::CreateTable(cTable &nCreateTB)
{
	string Value_name;
	string Value_type;
	string Attribute;
	bool flag = 0;

	try
	{
		nCreateTB.TBname = ReadCommand('(');
		if (isEnd() == 1)
			throw errNoLBrace;
		else if (CheckWord(nCreateTB.TBname) == 1)
		{
			position++;
			while (1) {
				SkipCharacter(' ', '\n');
				Value_name = ReadCommand(' ');
				if (isKeyword(Value_name) == 1)
					throw errName;
				if (Value_name.compare("primary") == 0)
					break;
				SkipCharacter(' ');
				Value_type = ReadCommand(' ', ',');
				if (CheckWord(Value_type) == 0)
					throw errNoType;

				pTableCol node = new TableCol;
				node->Next = nCreateTB.end->Next;
				nCreateTB.end->Next = node;
				nCreateTB.end = node;
				nCreateTB.TalColumn++;
				node->colnum = nCreateTB.TalColumn;
				node->name = Value_name;
				node->length = 0;
				node->isDiffer = false;
				node->isPrimary = false;
				node->ColType = getColType(Value_type, node->length);
				if (command[position] != ',')
				{
					SkipCharacter(' ');
					Attribute = ReadCommand(',');
					if (CheckWord(Attribute, 1) == 1)
						if (Attribute.compare("unique") == 0)
							node->isDiffer = 1;
				}
				position++;
			}
			SkipCharacter(' ');
			Value_name = ReadCommand('(');
			if (isEnd() == 1)
				throw errNoLBrace;
			if (CheckWord(Value_name) == 1)
				if (Value_name.compare("key") == 0)
				{
					position++;
					SkipCharacter(' ');
					Value_name = ReadCommand(')');
					if (isEnd() == 1)
						throw errNoRBrace;
					Value_name += ')';
					if (setPrimary(nCreateTB, Value_name) == 0)
						throw errSetPrimary;
					else return 1;
				}
				else throw (string)"primary key";
			else throw errTableName;
		}
		return 0;
	}
	catch (CmdErrType ERROR)
	{
		cout << CommError[(int)ERROR] << endl;
		cout << CommSolution[(int)ERROR] << endl;
	}
	catch (string name)
	{
		cout << CommError[errName] << name << endl;
		cout << CommSolution[errName] << endl;
	}
	return 0;
}
int Command::CreateIndex(nIndex &nCreateIndex)
{
	string name = "#";
	SkipCharacter(' ');
	name = ReadCommand(' ');
	nCreateIndex.writeIndexName(name);
	SkipCharacter(' ');
	name = ReadCommand(' ');
	if (name == "on")
	{
		SkipCharacter(' ');
		name = ReadCommand(' ','(');
		nCreateIndex.writeTbName(name);
		position++;
		SkipCharacter(' ');
		name = ReadCommand(' ', ')');
		nCreateIndex.writeAttrName(name);
		return 1;
	}
	else throw name;
	return 0;
}
int Command::UseDB(DataBase &nUseDB)
{
	return setName(nUseDB);
}

int Command::InsertTable(nRecord &nInsertTB)
{
	if (setName(nInsertTB, ' ') == 1)
	{
		string name = "#";
		SkipCharacter(' ');
		name = ReadCommand(' ', '(');
		if (isEnd() == 1)
			throw errNoEnd;
		if (name.compare("values") == 0)
		{
			ReadCommand('(');
			if (isEnd() == 1)
				throw errNoLBrace;
			position++;
			while (name != ")")
			{
				SkipCharacter(' ', ',');
				name = ReadCommand(',', ')');
				if (name == "")
					break;
				if (isEnd() == 1)
					throw errNoRBrace;
				if (SetRecordValue(name, nInsertTB) != 1)
					throw name;
			}
			//			if(checkRecord(nInsertTB)==1)
			return 1;
		}
		else throw name;
	}
	return 0;
}

int Command::SelectResult(nSelect &nSelectData)
{
	string name = "#";
	SkipCharacter(' ');
	name = ReadCommand(' ', ',');
	//CheckWord(name);
	if (name == "*")
	{
		nSelectData.setAll();		//标记
		SkipCharacter(' ');
		name = ReadCommand(' ');
	}
	else
	{
		while (name != "from")		//取出被选的条件
		{
			pTableCol tem = new TableCol;
			tem->colnum = 0;
			tem->ColType = eColDEFAULT;
			tem->isDiffer = 0;
			tem->isPrimary = 0;
			tem->length = 0;
			tem->name = "";
			tem->Next = NULL;

			tem->name = name;
			tem->Next = nSelectData.rend->Next;
			nSelectData.rend->Next = tem;
			nSelectData.rend = nSelectData.rend->Next;
			SkipCharacter(' ', ',');
			name = ReadCommand(' ', ',');
			if (isEnd() == 1)
				throw errNoEnd;
		}
	}
	if (name == "from")
	{
		SkipCharacter(' ');
		name = ReadCommand(' ');
		nSelectData.writeName(name);
		SkipCharacter(' ');
		name = ReadCommand(' ');
		if (name == "where")
		{
			nSelectData.setWhere();
			while (name != "") {
				pCondition tem = new Condition;
				tem->record = new RecordCol;
				tem->symbol = eSignDEFAULT;
				tem->record->colnum = 0;
				tem->record->ColType = eColDEFAULT;
				tem->record->length = 0;
				tem->record->Next = NULL;
				tem->record->nChar = "";
				tem->record->nFloat = 0;
				tem->record->nInt = 0;

				SkipCharacter(' ');
				name = ReadCommand(' ');
				if (name == "")
					break;
				tem->record->name = name;

				SkipCharacter(' ');
				name = ReadCommand(' ');
				tem->symbol = ReadSign(name);

				SkipCharacter(' ');
				name = ReadCommand(' ');
				if (SetRecordValue(name, tem->record) != 1)
					throw name;
				else {
					tem->Next = nSelectData.cend->Next;
					nSelectData.cend->Next = tem;
					nSelectData.cend = nSelectData.cend->Next;
				}
				SkipCharacter(' ');
				name = ReadCommand(' ');
				if (name == "and")
					continue;
				else if (name == "")
					break;
				else {
					throw  name;
					return 0;
				}
			}
		}
		return 1;
	}
	else throw name;
	return 0;
}
int Command::DeleteResult(nDelete &nDelData)
{
	string name = "#";
	SkipCharacter(' ');
	name = ReadCommand(' ');
	nDelData.writeName(name);
	SkipCharacter(' ');
	name = ReadCommand(' ');
	if (name == "where")
	{
		nDelData.setWhere();
		while (name != "") {
			pCondition tem = new Condition;
			tem->record = new RecordCol;
			tem->symbol = eSignDEFAULT;
			tem->record->colnum = 0;
			tem->record->ColType = eColDEFAULT;
			tem->record->length = 0;
			tem->record->Next = NULL;
			tem->record->nChar = "";
			tem->record->nFloat = 0;
			tem->record->nInt = 0;

			SkipCharacter(' ');
			name = ReadCommand(' ');
			if (name == "")
				break;
			tem->record->name = name;

			SkipCharacter(' ');
			name = ReadCommand(' ');
			tem->symbol = ReadSign(name);

			SkipCharacter(' ');
			name = ReadCommand(' ');
			if (SetRecordValue(name, tem->record) != 1)
				throw name;
			else {
				tem->Next = nDelData.cend->Next;
				nDelData.cend->Next = tem;
				nDelData.cend = nDelData.cend->Next;
			}
			SkipCharacter(' ');
			name = ReadCommand(' ');
			if (name == "and")
				continue;
			else if (name == "")
				break;
			else {
				throw  name;
				return 0;
			}
			return 1;
		}
	}
	else if (name == "")
	{
		nDelData.setAll();
		return 1;
	}
	else throw name;
	return 0;
}
int Command::UpdateResult(nUpdate &nUpdateData)
{
	string name = "#";
	SkipCharacter(' ');
	name = ReadCommand(' ');
	nUpdateData.writeName(name);
	SkipCharacter(' ');
	name = ReadCommand(' ');
	if (name == "set")
	{
		SkipCharacter(' ');
		name = ReadCommand(' ');
		while (name != "where") {
			pCondition tem = new Condition;
			tem->record = new RecordCol;
			tem->symbol = eSignDEFAULT;
			tem->record->colnum = 0;
			tem->record->ColType = eColDEFAULT;
			tem->record->length = 0;
			tem->record->Next = NULL;
			tem->record->nChar = "";
			tem->record->nFloat = 0;
			tem->record->nInt = 0;

			SkipCharacter(' ');
			name = ReadCommand(' ');
			if (name == "")
				break;
			tem->record->name = name;

			SkipCharacter(' ');
			name = ReadCommand(' ');
			tem->symbol = ReadSign(name);

			SkipCharacter(' ');
			name = ReadCommand(' ', ',');
			if (SetRecordValue(name, tem->record) != 1)
				throw name;
			else {
				tem->Next = nUpdateData.rend->Next;
				nUpdateData.rend->Next = tem;
				nUpdateData.rend = nUpdateData.rend->Next;
			}
			SkipCharacter(' ', ',');
			name = ReadCommand(' ');
		}
		if (name == "where")
		{
			while (name != "") {
				pCondition tem = new Condition;
				tem->record = new RecordCol;
				tem->symbol = eSignDEFAULT;
				tem->record->colnum = 0;
				tem->record->ColType = eColDEFAULT;
				tem->record->length = 0;
				tem->record->Next = NULL;
				tem->record->nChar = "";
				tem->record->nFloat = 0;
				tem->record->nInt = 0;

				SkipCharacter(' ');
				name = ReadCommand(' ');
				if (name == "")
					break;
				tem->record->name = name;

				SkipCharacter(' ');
				name = ReadCommand(' ');
				tem->symbol = ReadSign(name);

				SkipCharacter(' ');
				name = ReadCommand(' ');
				if (SetRecordValue(name, tem->record) != 1)
					throw name;
				else {
					tem->Next = nUpdateData.cend->Next;
					nUpdateData.cend->Next = tem;
					nUpdateData.cend = nUpdateData.cend->Next;
				}
				SkipCharacter(' ');
				name = ReadCommand(' ');
				if (name == "and")
					continue;
				else if (name == "")
					break;
				else {
					throw  name;
					return 0;
				}
				return 1;
			}
		}
	}
}
int Command::DropDB(nDropDB &nDropDbData)
{
	return setName(nDropDbData);
}
int Command::DropIndex(nDropIndex &nDropIndexData)
{
	string name = "";
	SkipCharacter(' ');
	name = ReadCommand(' ');
	nDropIndexData.writeIndexName(name);
	SkipCharacter(' ');
	name = ReadCommand(' ');
	if (name == "on")
	{
		SkipCharacter(' ');
		name = ReadCommand(' ', ';');
		nDropIndexData.writeTbName(name);
		return 1;
	}
	return 0;
}
int Command::DropTB(nDropTB &nDropTbData)
{
	return setName(nDropTbData);
}
int Command::SourceFile(nSourceFile &nSourceName)
{
	return setName(nSourceName,'\n');
}
template<typename T>
int Command::setName(T &nData, char ch1,char ch2)
{
	string name;
	try
	{
		SkipCharacter(' ');
		name = ReadCommand(ch1,ch2);
		if (isEnd() == 1)
			throw errNoEnd;
		if (isKeyword(name) == 1)
			throw name;
		else {
			nData.writeName(name);
			return 1;
		}
	}
	catch (CmdErrType ERROR)
	{
		cout << CommError[(int)ERROR] << endl;
		cout << CommSolution[(int)ERROR] << endl;
	}
	catch (string name)
	{
		cout << CommError[errName] << name << endl;
		cout << CommSolution[errName] << endl;
	}
	return 0;
}

bool Command::SetRecordValue(string name, pRecordCol &nSelectTB)
{
	try
	{
		pRecordCol &ndata = nSelectTB;
		if (name.compare(0, 1, "\"", 1) == 0 || name.compare(name.size() - 1, 1, "\"", 1) == 0)
		{
			if (name.compare(0, 1, "\"", 1) == 0 && name.compare(name.size() - 1, 1, "\"", 1) == 0)
			{
				ndata->ColType = CHAR;
				name = name.erase(name.size() - 1, 1);
				name = name.erase(0, 1);
				ndata->nChar = name;
			}
			else
			{
				throw name;
				return 0;
			}
		}
		else if (name.find('.') != string::npos)
		{
			if (name.find('.') == name.rfind('.'))
			{
				ndata->ColType = FLOAT;
				ndata->nFloat = stringTofloat(name);
			}
			else
			{
				throw name;
				return 0;
			}

		}
		else if (name.find(' ') == string::npos)
		{
			ndata->ColType = INT;
			ndata->nInt = stringToint(name);
		}
		return 1;
	}
	catch (string name)
	{
		cout << CommError[errWrongData] << name << endl;
		cout << CommSolution[errWrongData] << endl;
	}
	return 0;
}
bool Command::SetRecordValue(string name, nRecord &nInsertTB)
{
	try
	{
		pRecordCol ndata = new RecordCol;
		if (name.compare(0, 1, "\"", 1) == 0 || name.compare(name.size() - 1, 1, "\"", 1) == 0)
		{
			if (name.compare(0, 1, "\"", 1) == 0 && name.compare(name.size() - 1, 1, "\"", 1) == 0)
			{
				ndata->ColType = CHAR;
				name = name.erase(name.size() - 1, 1);
				name = name.erase(0, 1);
				ndata->nChar = name;
			}
			else
			{
				throw name;
				return 0;
			}
		}
		else if (name.find('.') != string::npos)
		{
			if (name.find('.') == name.rfind('.'))
			{
				ndata->ColType = FLOAT;
				ndata->nFloat = stringTofloat(name);
			}
			else
			{
				throw name;
				return 0;
			}

		}
		else if (name.find(' ') == string::npos)
		{
			ndata->ColType = INT;
			ndata->nInt = stringToint(name);
		}
		ndata->Next = nInsertTB.end->Next;
		nInsertTB.end->Next = ndata;
		nInsertTB.end = nInsertTB.end->Next;
		nInsertTB.NumPlus();
		ndata->colnum = nInsertTB.getNum();
		return 1;
	}
	catch (string name)
	{
		cout << CommError[errWrongData] << name << endl;
		cout << CommSolution[errWrongData] << endl;
	}
	return 0;
}

float Command::stringTofloat(string name)
{
	float result = 0;
	for (int i = 0; i < name.size(); i++)
	{
		if (i != name.find('.'))
		{
			result *= 10;
			result += name[i] - '0' + 0;
		}
	}
	result /= pow(10, name.size() - name.find('.') - 1);
	return result;
}
int Command::stringToint(string name)
{
	int result = 0;
	for (int i = 0; i < name.size(); i++)
	{
		result *= 10;
		result += name[i] - '0' + 0;
	}
	return result;
}
eColType getColType(string a, int &len)
{
	if (a.compare("int") == 0)
		return INT;
	else if (a.compare("float") == 0)
		return FLOAT;
	else if (a.compare(0, 4, "char") == 0)
	{
		len = getLength(a);
		return CHAR;
	}
	else return eColDEFAULT;
}
int getLength(string a)
{
	int i = 0;
	int result = 0;
	char *word = new(char[12]);
	strcpy(word, a.c_str());
	while (word[i] != '(')i++;
	i++;
	while (word[i] != ')') {
		result *= 10;
		result += word[i] - '0' + 0;
		i++;
	}
	delete word;
	return result;
}
int setPrimary(cTable &nCreate, string a)
{
	string check;
	char *word = new(char[30]);
	strcpy(word, a.c_str());
	int i = 0;
	while (word[i] != ')')
	{
		check += word[i];
		i++;
	}
	CheckWord(check, 1);
	pTableCol tem;
	tem = nCreate.head;
	while (tem->Next != NULL) {
		tem = tem->Next;
		if (tem->name == check)
		{
			tem->isPrimary = 1;
			return 1;
		}
	}
	return 0;
}
//检查（引用）字符串中的空格或者换行，flag表示是否报错
bool CheckWord(string &a, bool flag)
{
	try
	{
		int i = 0, j = 0;
		char *name = new(char[100]);
		strcpy(name, a.c_str());
		while (name[i] == ' ')i++;
		while (name[i] != '\0')
		{
			if (name[i] == ' ' || name[i] == '\n')
				break;
			name[j++] = name[i++];
		}
		name[j] = '\0';
		while (name[i] != '\0')
		{
			if (name[i] == '\n')
				break;
			if (name[i] != ' ')
				throw errTableName;
			i++;
		}
		if (name[0] == '\0' && !flag)
			throw errNoName;
		a = name;
		delete name;
		return 1;
	}
	catch (CmdErrType ERROR)
	{
		cout << CommError[(int)ERROR] << endl;
		cout << CommSolution[(int)ERROR] << endl;
		return 0;
	}
}
bool isKeyword(string a)
{
	for (int i = 0; i < KeywordNum; i++)
		if (a.compare(Keyword[i]) == 0)
			return 1;
	return 0;
}

eSign ReadSign(string code)
{
	if (code == "=")
		return EQUAL;
	else if (code == "<")
		return SMALLER;
	else if (code == ">")
		return BIGGER;
	else if (code == "<=")
		return ESMALLER;
	else if (code == ">=")
		return EBIGGER;
	else if (code == "<>")
		return NEQUAL;
}

nRecord::nRecord()
{
	head = new RecordCol;
	head->Next = NULL;
	head->ColType = eColDEFAULT;
	head->nInt = -1;
	head->nFloat = -1;
	head->nChar = '#';
	head->length = -1;
	end = head;
	num = 0;
	size = 0;
	//	data = "";
}
nRecord::~nRecord()
{
	pRecordCol a, b;
	a = head;
	while (a->Next != NULL)
	{
		b = a->Next;
		delete a;
		a = b;
	}
	delete a;
}


nCondition::nCondition()
{
	isWhere = 0;
	isAll = 0;

	rhead = new TableCol;
	rhead->colnum = 0;
	rhead->ColType = eColDEFAULT;
	rhead->isDiffer = 0;
	rhead->isPrimary = 0;
	rhead->name = '#';
	rhead->length = 0;
	rhead->Next = NULL;
	rend = rhead;

	chead = new Condition;
	chead->record = new RecordCol;
	chead->record->colnum = 0;
	chead->record->ColType = eColDEFAULT;
	chead->record->length = 0;
	chead->record->nChar = '#';
	chead->record->nFloat = 0;
	chead->record->nInt = 0;
	chead->Next = NULL;
	cend = chead;
}

nCondition::~nCondition()
{
	pTableCol a, b;
	pCondition c, d;
	a = rhead;
	while (a->Next != NULL)
	{
		b = a->Next;
		delete a;
		a = b;
	}
	delete a;

	c = chead;
	while (c->Next != NULL)
	{
		d = c->Next;
		delete c;
		c = d;
	}
	delete c;
}

nUpdate::nUpdate()
{
	rhead = new Condition;
	rhead->record = new RecordCol;
	rhead->record->colnum = 0;
	rhead->record->ColType = eColDEFAULT;
	rhead->record->length = 0;
	rhead->record->nChar = '#';
	rhead->record->nFloat = 0;
	rhead->record->nInt = 0;
	rhead->Next = NULL;
	rend = rhead;

	chead = new Condition;
	chead->record = new RecordCol;
	chead->record->colnum = 0;
	chead->record->ColType = eColDEFAULT;
	chead->record->length = 0;
	chead->record->nChar = '#';
	chead->record->nFloat = 0;
	chead->record->nInt = 0;
	chead->Next = NULL;
	cend = chead;
}
nUpdate::~nUpdate()
{
	pCondition c, d;

	c = rhead;
	while (c->Next != NULL)
	{
		d = c->Next;
		delete c;
		c = d;
	}
	delete c;

	c = chead;
	while (c->Next != NULL)
	{
		d = c->Next;
		delete c;
		c = d;
	}
	delete c;
}