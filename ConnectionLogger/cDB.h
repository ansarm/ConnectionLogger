#include "sqlite3.h"
#include <string>

class cDB
{
public:
	cDB(void);
	~cDB(void);
	
	int cDBOpen(char * szInitDatabaseName);
	int cDBClose(void);
	int cDBBeginTransaction(void);
	int cDBCommitTransaction(void);
	int cDBPrepare(void);
	int cDBInsert(int iIPAddress,int iProtocol, int iPort);
	int cDBInsert(int iIPAddress,int iProtocol, int iPort, time_t timeStamp);
	int iNumCapturedPackets;
private:
	char szDatabaseName[1024];
	sqlite3 *db;
	char * error;

};
