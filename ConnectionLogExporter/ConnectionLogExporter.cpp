// ConnectionLogExporter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <signal.h>
#include <windows.h>
#include <time.h>

#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include "sqlite3.h"

using namespace std;
sqlite3 *db;
char *error;
string szExportFileName = "ConnectionLog.csv";
int iNumberSessions = 0;


int log_message(string szMessage)
{
	cout << szMessage;
	return 0;
}

int export_to_csv(string szOutPut)
{

	ofstream ofsExportFile(szExportFileName,fstream::app);
	ofsExportFile << szOutPut;
	iNumberSessions++;
	return 0;
}

int setup_csv(void)
{
	ofstream ofsExportFile(szExportFileName);
	ofsExportFile << "SourceIPAddress,DestinationProtocol,DestinationPort,TimeStamp\n";
	return 0;
}

int open_db(string dbname )
{
	if (sqlite3_open_v2(dbname.c_str(), &db, SQLITE_OPEN_READONLY, NULL))
	{
		log_message("Error opening database\n");
		log_message(sqlite3_errmsg(db));
		return 1;
	}	
	else
	{	
		log_message("Opened database file:\t\t"+ dbname + "\n");
	}
	return 0;
}

void gracefull_close(int iExitCode )
{
	log_message("\nClosing Database and exiting gracefully\n");
	sqlite3_close(db);
}

int export_connection_log(void)
{
	string szSQLQuery = "SELECT * FROM ConnectionLog";
	sqlite3_stmt *pStmt;
	int iIPAddress, iProtocol, iPort;
	time_t timeStamp;
	char szTime[26];

	in_addr iaSourceAddress;
	string szIPAddress, szTimeStamp;

	if (sqlite3_prepare_v2( db, szSQLQuery.c_str(),  szSQLQuery.length(), &pStmt, NULL)==SQLITE_OK  )
	{
		log_message("Exporting log entries to csv: \t");
		setup_csv();
		while (sqlite3_step(pStmt) ==SQLITE_ROW )
		{
			iIPAddress = sqlite3_column_int(pStmt, 0);
			iProtocol =sqlite3_column_int(pStmt, 1);
			iPort =sqlite3_column_int(pStmt, 2);
			timeStamp = (time_t)sqlite3_column_int(pStmt, 3);
			if (timeStamp == NULL)
			{
				szTimeStamp= string("\n");
			}
			else
			{
				ctime_s(szTime, 26, &timeStamp);
				szTimeStamp = string(szTime);
			}
			iaSourceAddress.S_un.S_addr = iIPAddress;
			szIPAddress = inet_ntoa(iaSourceAddress);
			if (iProtocol == 6) {
				export_to_csv( szIPAddress + ",tcp," + to_string(iPort)+ "," +  szTimeStamp);		
			}
			else
			{
				if (iProtocol == 17) {
					export_to_csv(szIPAddress + ",udp," + to_string(iPort)+ "," + szTimeStamp);					
				}
			}
			if ((iNumberSessions%100)==0)
			{
			string szOutputLength = to_string(iNumberSessions);
			int iOutputLength = szOutputLength.length();
			printf("%d",iNumberSessions);
			for (int iBackspace = 0; iBackspace <iOutputLength; iBackspace++)
				{ printf("\b"); }
			}
		}
	}
	else {
		log_message(sqlite3_errmsg(db));
	}

	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	printf("Entries Exported:\t\t%d\n",iNumberSessions);
	sqlite3_finalize(pStmt);
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	string dbConnectionLog= "ConnectionLog.db";
	log_message("-----------------------------------------\n");
	log_message("-Connection Log Exporter 1.0 Starting Up-\n");
	log_message("-----------------------------------------\n");

	//signal(SIGINT, gracefull_close);

	if( open_db(dbConnectionLog.c_str()) == 0)
	{
		export_connection_log();
	}
	gracefull_close(0); 
	return 0;
}

