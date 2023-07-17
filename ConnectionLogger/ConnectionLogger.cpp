// ConnectionLogger.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "cDB.h"
#include <signal.h>
#include "winsock2.h"
#include <time.h>

#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include ".\iprout\iphdr.h"

#define MAXPACKETSIZE 65540
#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1)

using namespace std;
string szConnerctionLog;
cDB cdb;

int iMonitorInterval;
int iMinDriveSpaceMBAllowed =1024;

bool bFlush=false;
int iNumTCPPorts=0;
bool bAllTCPPorts=false;
bool bAllUDPPorts=false;
int iNumUDPPorts=0;
int iUDPPorts[256];
int iTCPPorts[256];

int iNumIgnoreSubnets=0;
int iIgnoreSubnets[256];
string szIPAddress="";
char * error;

time_t timeStamp;

int log_message(string message)
{
	cout << message;
	return 0;
}

void gracefull_close(int iExitCode )
{
	log_message("\nProcessed " + to_string(cdb.iNumCapturedPackets) + " packets.\n");
	log_message("Committing transactions\n");
	cdb.cDBCommitTransaction();
	log_message("Closing Database and exiting gracefully\n");
	cdb.cDBClose();
}

DWORD WINAPI MonitorThread( LPVOID lpParam ) 
{
	unsigned __int64 lpFreeBytesAvailable;
	unsigned __int64 lpTotalNumberOfBytes;
	unsigned __int64 lpTotalNumberOfFreeBytes;
	while (true) 
	{
		Sleep(iMonitorInterval);
		bFlush = true;
		GetDiskFreeSpaceEx(NULL,(PULARGE_INTEGER)&lpFreeBytesAvailable, (PULARGE_INTEGER)&lpTotalNumberOfBytes,(PULARGE_INTEGER)&lpTotalNumberOfFreeBytes);  
		if ((lpFreeBytesAvailable/1000000)<iMinDriveSpaceMBAllowed )
		{
			log_message("\nFree Drive Space Limit Reached. Exiting!\n");
			gracefull_close(0);
		}
	}
	return 0;
}

DWORD WINAPI TimerThread( LPVOID lpParam ) 
{	
	while (true) 
	{
		time(&timeStamp);
		Sleep(60*1000);		
	}
	return 0;
}


int open_db(string dbname )
{
	if (cdb.cDBOpen((char *)dbname.c_str()))
	{
		log_message("Error opening database\n");
		return 1;
	}	
	else
	{	
		log_message("Opened database file:\t\t\t"+ dbname + "\n");
	}
	return 0;
}

int prepare_db()
{	
	if (cdb.cDBPrepare())
	{
		log_message("Unable to prepare the ConnectionLog table\n");
		return 1;
	}
	return 0;
}

int insert_new_record( int iIPAddress,int iProtocol, int iPort)
{	
	if ( cdb.cDBInsert(iIPAddress, iProtocol, iPort, timeStamp))
	{
		log_message( "Error adding record to table\n" );
	}
	if (bFlush) //we have waited long enough to write commit.
	{
		log_message("\b!");
		cdb.cDBCommitTransaction();
		cdb.cDBBeginTransaction();
		bFlush = false;
	}
	return 0;
}

void process_tcp_header(void *buffer) 
{
	TCP_IP_HDR * tcpipHdr;
	tcpipHdr =(TCP_IP_HDR*)buffer;
	unsigned short usTCPPort= ntohs(tcpipHdr->tcpHdr.dst_portno);
	if (bAllTCPPorts)
	{
		if ((tcpipHdr->tcpHdr.control & ACK )== ACK) //only interested in ACK Packets
			insert_new_record(tcpipHdr->ipHdr.ip_srcaddr,6,usTCPPort);
	}
	else
	{
		for (int iLoop=0;iLoop<iNumTCPPorts;iLoop++)
		{
			if (usTCPPort==iTCPPorts[iLoop])
			{
				if ((tcpipHdr->tcpHdr.control & ACK )== ACK) //only interested in ACK Packets
					insert_new_record(tcpipHdr->ipHdr.ip_srcaddr,6,usTCPPort);
			}
		}
	}
}

void process_udp_header(void *buffer) 
{
	UDP_IP_HDR * udpIpHdr;
	udpIpHdr =(UDP_IP_HDR*)buffer;
	unsigned short usUDPPort= ntohs(udpIpHdr->udpHdr.dst_portno);
	if (bAllUDPPorts)
	{
		insert_new_record(udpIpHdr->ipHdr.ip_srcaddr,17,usUDPPort);
	}
	else
	{
		for (int iLoop=0;iLoop<iNumUDPPorts;iLoop++)
		{
			if (usUDPPort==iUDPPorts[iLoop])
			{
				insert_new_record(udpIpHdr->ipHdr.ip_srcaddr,17,usUDPPort);
			}
		}
	}
}

int ignore_subnet(unsigned int ulIPAddress)
{
	for (int iLoop=0; iLoop <iNumIgnoreSubnets; iLoop++)
	{
		if ((ulIPAddress & iIgnoreSubnets[iLoop]) == iIgnoreSubnets[iLoop])
			return 1;
	}
	return 0;
}

void listen_and_process_connections(void)
{
	IP_HDR *ipHdr;	
	SOCKET s;
	char  buffer[MAXPACKETSIZE];
	DWORD 	ioctl_in=1, outbytes;
	SOCKADDR_IN localaddr;	
	WSADATA wsaData;
	unsigned long ulLocalIP;
	ZeroMemory(buffer,MAXPACKETSIZE);

	if (WSAStartup(MAKEWORD(2,2), &wsaData) == 0)
	{
		s=WSASocket(AF_INET,SOCK_RAW, 0, 0,0,0);
		localaddr.sin_family=AF_INET;
		ulLocalIP = inet_addr (szIPAddress.c_str());
		localaddr.sin_addr.s_addr = ulLocalIP;
		localaddr.sin_port=7000;
		if (bind(s,(SOCKADDR*)&localaddr, sizeof(localaddr)) == 0)
		{
			if (WSAIoctl(s, SIO_RCVALL ,&ioctl_in,sizeof(ioctl_in), NULL, NULL, &outbytes, NULL, NULL)==0)
			{
				log_message("Logging Connections (Crtl-c to stop):\t ");
				while(true)
				{
					recvfrom(s, buffer, MAXPACKETSIZE, 0, NULL,NULL);
					ipHdr = (IP_HDR *)buffer;	
					if ((ipHdr->ip_destaddr  ==ulLocalIP) && //only interesting traffic coming into interface
						(!ignore_subnet(ipHdr->ip_srcaddr))) //make sure the source address is not on our ignore subnet list
					{
						log_message ("\b-");
						if (ipHdr->ip_protocol == 6) //tcp traffic
						{
							log_message("\b/");
							process_tcp_header(buffer);
						}
						else
						{
							if (ipHdr->ip_protocol == 17) //udp traffic
							{
								log_message("\b\\");
								process_udp_header(buffer);
							}
						}
					}
				}
			}
			else
			{
				log_message("Unable to set SIO_RCVALL IOCTL\n");
			}
		}
		else
		{
			log_message("Unable to bind to IP Address\n");
		}
	}
	else
	{
		log_message("Unable to start IP Stack\n");
	}
}

int load_params()
{
	ifstream ConnectionLogConfig ("ConnectionLog.ini");
	string szInputLine;
	string szInputParam;
	char *nextToken, *nextTokenSubnet;
	char * szStrTok;
	char * szStrTokSubnet;
	int  iSubnetMask;
	unsigned long ulSubnetAddress;

	if(ConnectionLogConfig.is_open())
	{
		while(!(ConnectionLogConfig.eof()))
		{
			getline(ConnectionLogConfig, szInputLine,'=');
			if (szInputLine == "ListenOnIPAddress")
			{
				getline(ConnectionLogConfig, szInputParam,'\n');
				log_message("Listening on IP Address:\t\t" + szInputParam + "\n");
				szIPAddress = szInputParam;
			}		
			if (szInputLine == "MinDriveSpaceMBAllowed")
			{
				getline(ConnectionLogConfig, szInputParam,'\n');
				iMinDriveSpaceMBAllowed = atoi(szInputParam.c_str());
				log_message("Min Drive Free Space(MB):\t\t" + szInputParam + "\n");
			}
			if (szInputLine == "MonitorInterval")
			{
				getline(ConnectionLogConfig, szInputParam,'\n');
				iMonitorInterval = atoi(szInputParam.c_str());
				log_message("Monitor interval (ms):\t\t\t" + szInputParam + "\n");
			}
			if (szInputLine == "TCPPorts")
			{
				iNumTCPPorts = 0;
				getline(ConnectionLogConfig, szInputParam,'\n');
				log_message("Monitoring TCP Ports:\t\t\t" + szInputParam + "\n");
				if (szInputParam == "*")
				{
					bAllTCPPorts = true;
				}
				else
				{
					szStrTok  = strtok_s((char*)szInputParam.c_str(),",", &nextToken);
					while (szStrTok!=NULL)
					{
						iTCPPorts[iNumTCPPorts] = atoi(szStrTok);
						iNumTCPPorts++;
						szStrTok  = strtok_s(NULL,",",&nextToken);
					}
				}
			}			
			if (szInputLine == "IgnoreSubnets")
			{
				iNumIgnoreSubnets = 0;
				getline(ConnectionLogConfig, szInputParam,'\n');
				szStrTok  = strtok_s((char*)szInputParam.c_str(),",", &nextToken);
				while (szStrTok!=NULL)
				{
					szStrTokSubnet  = strtok_s(szStrTok,"/", &nextTokenSubnet);
					if (szStrTokSubnet!=NULL)
					{
						log_message("Ignoring Subnet: \t\t\t" + string(szStrTokSubnet) + ":");
						ulSubnetAddress = inet_addr(szStrTokSubnet);
						szStrTokSubnet = strtok_s(NULL,"/", &nextTokenSubnet);
						log_message(string(szStrTokSubnet) + "\n");
						iSubnetMask=atoi(szStrTokSubnet);
						iIgnoreSubnets[iNumIgnoreSubnets] = (int)ulSubnetAddress & htonl(((pow(2,iSubnetMask)) -1));
						iNumIgnoreSubnets++;
					}
					szStrTok  = strtok_s(NULL,",",&nextToken);
				}
			}
			if (szInputLine == "UDPPorts")
			{
				iNumUDPPorts = 0;
				getline(ConnectionLogConfig, szInputParam,'\n');
				log_message("Monitoring UDP Ports: \t\t\t" + szInputParam + "\n");
				if (szInputParam == "*")
				{
					bAllUDPPorts = true;
				}
				else
				{
					szStrTok  = strtok_s((char*)szInputParam.c_str(),",", &nextToken);
					while (szStrTok!=NULL)
					{
						iUDPPorts[iNumUDPPorts] = atoi(szStrTok);
						iNumUDPPorts++;
						szStrTok  = strtok_s(NULL,",",&nextToken);
					}
				}
			}
		}
	}
	else
	{
		log_message("No config file found or unable to load.\n");
		gracefull_close(1);
	}
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	string dbConnectionLog= "ConnectionLog.db";
	szConnerctionLog = "ConnectionLog.log";
	log_message("------------------------------------\n");
	log_message("-Connection Logging 1.0 Starting Up-\n");
	log_message("------------------------------------\n");

	//signal(SIGINT, gracefull_close);
	load_params();
	CreateThread(NULL,0,MonitorThread,NULL,0,NULL);
	CreateThread(NULL,0,TimerThread,NULL,0,NULL);
	if( open_db(dbConnectionLog.c_str()) != 0)
	{ 	
		gracefull_close(0); 
	}
	else
	{   
		if (prepare_db() != 0) 
			gracefull_close(0); 
		else 
		{ 	
			cdb.cDBBeginTransaction();		
			listen_and_process_connections();
		}
	}
	gracefull_close(0);
	return 0;
}

