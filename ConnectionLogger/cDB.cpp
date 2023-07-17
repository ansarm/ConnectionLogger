#include "cDB.h"
using namespace std;

cDB::cDB(void)
{
	iNumCapturedPackets=0;

}

cDB::~cDB(void)
{
}

int cDB::cDBOpen(char * szInitDatabaseName)
{	
	return (sqlite3_open(szInitDatabaseName, &db));
}

int cDB::cDBClose(void)
{
	return (sqlite3_close(db));
}

int cDB::cDBBeginTransaction(void)
{
	return (sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &error));
}

int cDB::cDBCommitTransaction(void)
{
	return (sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &error)); 
}

int cDB::cDBPrepare(void)
{
	const char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS ConnectionLog(IPAddress INTEGER, Protocol INTEGER, Port INTEGER, TimeStamp INTEGER, PRIMARY KEY(IPAddress DESC,Protocol DESC, Port DESC));";
	return  (sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error));	
}

int cDB::cDBInsert(int iIPAddress,int iProtocol, int iPort)
{
	char *error;
	string szSQLInsert = "INSERT OR REPLACE INTO ConnectionLog VALUES(";
	string szIPAddress = to_string(iIPAddress);
	string szPort = to_string(iPort);
	string szProtocol = to_string(iProtocol);

	szSQLInsert.append(szIPAddress + "," + szProtocol + "," + szPort + "," + to_string(0) + ");");
	iNumCapturedPackets++;
	return  ( sqlite3_exec(db, szSQLInsert.c_str(), NULL, NULL, &error));
}


int  cDB::cDBInsert(int iIPAddress,int iProtocol, int iPort, time_t timeStamp)
{
	char *error;
	string szSQLInsert = "INSERT OR REPLACE INTO ConnectionLog VALUES(";
	string szIPAddress = to_string(iIPAddress);
	string szPort = to_string(iPort);
	string szProtocol = to_string(iProtocol);
	string szTimeStamp = to_string((int)timeStamp);

	szSQLInsert.append(szIPAddress + "," + szProtocol + "," + szPort + "," + szTimeStamp + ");");
	iNumCapturedPackets++;
	return  ( sqlite3_exec(db, szSQLInsert.c_str(), NULL, NULL, &error));
}