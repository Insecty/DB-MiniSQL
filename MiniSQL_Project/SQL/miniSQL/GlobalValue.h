#ifndef GLOBALVALUE_H_
#define GLOBALVALUE_H_
#define KeywordNum 17
#define MsgType enum Message 
#define CmdErrType enum InterError
#define SignType enum eSign
enum Message
{
	eMsgDEFAULT, CREATEDB, CREATETABLE, CREATEINDEX,
	INSERT, SELECT, USE, SOURCE, HELP, update,
	DELETE, DROPINDEX, DROPDB, DROPTABLE, EXIT, MSGERROR
};
enum eColType
{
	eColDEFAULT = 0, INT = 1, CHAR = 2, FLOAT = 3
};
enum eSign
{
	eSignDEFAULT,BIGGER,SMALLER,EQUAL,EBIGGER,ESMALLER,NEQUAL
};
extern char* Keyword[];
extern bool isOpenDB;
extern char curDBname[32];  // 当前数据库名
extern char* help[];

#endif // GLOBALVALUE_H_
