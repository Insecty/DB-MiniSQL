#include "GlobalValue.h"

bool isOpenDB; 
char curDBname[32];

char* Keyword[] = {
	"create","select","datebase","index","insert","Update",
	"delete","table","show","help","where","by","and","or",
	"drop","exit","source"
};

char* help[];