// ipcmd.cpp : Defines the entry point for the console application.
//






int main(int argc, char* argv[])
{
	
	
				
	if ((log_data_in_hex==0) && (	log_data_in_char ==0))
	{
		if (got_time==1)
            printf("SrcIP\t\tDestIP\t\tsize\tProt\tSrcport\tDstport\tControl\tTime\n");
		else
			printf("SrcIP\t\tDestIP\t\tsize\tProt\tSrcport\tDstport\tControl\n");
	}
	while(true)
	{
		recvfrom(s, buffer, MAXPACKETSIZE, 0, NULL,NULL);
		ipHdr = (IP_HDR *)buffer;
		if (ipHdr->ip_protocol ==6)
		{
			if ((got_filter_tcp==1) ||
				(got_get_all_packets==1))
			{ 
				tcpipHdr =(TCP_IP_HDR*)buffer;
				if (((filter_ports==1)&&((ntohs(tcpipHdr->tcpHdr.source)==port)||
					(ntohs(tcpipHdr->tcpHdr.dest)==port)))|| (filter_ports==0))
				{
					if ((got_filter_syn==1) &&((tcpipHdr->tcpHdr.control & 0x02)==0x02) || (got_filter_syn==0))
					{
						sourceaddr = (in_addr*)(&tcpipHdr->ipHdr.ip_srcaddr);
						destaddr = (in_addr*)(&tcpipHdr->ipHdr.ip_destaddr);
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nSource Address: ");
						printf("%s\t", inet_ntoa(*sourceaddr));
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nDestiniation Address: ");
						printf("%s\t", inet_ntoa(*destaddr));
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nIP Packet Length: ");
						printf("%d\t",ntohs( tcpipHdr->ipHdr.ip_totallength));
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nIP Protocol: ");
						printf("%d\t",tcpipHdr->ipHdr.ip_protocol);
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nSource Port: ");
						printf("%d\t", htons(tcpipHdr->tcpHdr.source));
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nDestination Port: ");
						printf("%d\t", htons(tcpipHdr->tcpHdr.dest));
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nControl: ");
						if ((tcpipHdr->tcpHdr.control & FIN )== FIN)
							printf(" FIN");
						if ((tcpipHdr->tcpHdr.control & SYN )== SYN)
							printf(" SYN");
						if ((tcpipHdr->tcpHdr.control & ACK )== ACK)
							printf(" ACK");
						if ((tcpipHdr->tcpHdr.control & PSH )== PSH)
							printf(" PSH");
						if ((tcpipHdr->tcpHdr.control & URG )== URG)
							printf(" URG");
						if ((tcpipHdr->tcpHdr.control & RST )== RST)
							printf(" RST");
						printf("\t");
						if (got_time==1)
						{
							if ((log_data_in_hex==1) || (	log_data_in_char ==1))
								printf("\nTime: ");
							_time64(&thetime);
							printf("%s ", _ctime64(&thetime));
						}
						else
							printf("\n");
						numpackets++;
					}
				}
			}
		}
		else
			if (ipHdr->ip_protocol ==17)
			{	
				if ((got_filter_udp==1) ||
				(got_get_all_packets==1))
				{
					udpipHdr = (UDP_IP_HDR*)buffer;	
					if (((filter_ports==1)&&((ntohs(udpipHdr->udpHdr.src_portno)==port)||
						(ntohs(udpipHdr->udpHdr.dst_portno)==port)))|| (filter_ports==0))
					{
						sourceaddr = (in_addr*)(&udpipHdr->ipHdr.ip_srcaddr);
						destaddr = (in_addr*)(&udpipHdr->ipHdr.ip_destaddr);
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nSource Address: ");
						printf("%s\t", inet_ntoa(*sourceaddr));
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nDestination Address: ");
						printf("%s\t", inet_ntoa(*destaddr));
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nIP Packet Length: ");
						printf("%d\t",ntohs( udpipHdr->ipHdr.ip_totallength));
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nIP Protocol: ");
						printf("%d\t",udpipHdr->ipHdr.ip_protocol);
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nSource Port: ");
						printf("%d\t", htons(udpipHdr->udpHdr.src_portno));
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nDestination Port: ");
						printf("%d\t", htons(udpipHdr->udpHdr.dst_portno));
						if (got_time==1)
						{
							if ((log_data_in_hex==1) || (	log_data_in_char ==1))
								printf("\nTime: ");
							_time64(&thetime);
							printf("\t%s ", _ctime64(&thetime));
						}
						else
							printf("\n");
						numpackets++;
		
					}
				}
			}
			else
				if ((got_get_all_packets==1))
				{
					ipHdr = (IP_HDR *)buffer;
					sourceaddr = (in_addr*)(&ipHdr->ip_srcaddr);
					destaddr = (in_addr*)(&ipHdr->ip_destaddr);
					if ((log_data_in_hex==1) || (	log_data_in_char ==1))
						printf("\nSource Address: ");
					printf("%s\t", inet_ntoa(*sourceaddr) );
					if ((log_data_in_hex==1) || (	log_data_in_char ==1))
						printf("\nDestination Address: ");
					printf("%s\t", inet_ntoa(*destaddr) );
					if ((log_data_in_hex==1) || (	log_data_in_char ==1))
						printf("\nIP Packet Length: ");
					printf("%d\t",ntohs( ipHdr->ip_totallength));
					if ((log_data_in_hex==1) || (	log_data_in_char ==1))
						printf("\nIP Protocol: ");
					printf("%d\t",ipHdr->ip_protocol);
					if (got_time==1)
					{
						if ((log_data_in_hex==1) || (	log_data_in_char ==1))
							printf("\nTime: ");
						_time64(&thetime);
						printf("\t\t\t%s ", _ctime64(&thetime));
					}
					else
						printf("\n");
					numpackets++;
				}
		if (log_data_in_hex==1) 
		{
			printf("Packet Data:\n");
			for (loop=(sizeof(IP_HDR)); loop<(ntohs(ipHdr->ip_totallength)); loop++)
				printf("%02x ",(unsigned char)buffer[loop]);			
			printf("\n");		
		}
		if (log_data_in_char ==1) 
		{
			printf("Packet Data:\n");
			for (loop=(sizeof(IP_HDR)); loop<(ntohs(ipHdr->ip_totallength)); loop++)
				printf("%c",(unsigned char)buffer[loop]);			
			printf("\n");
		}
		ZeroMemory(buffer,MAXPACKETSIZE);
	}
	return 0;
}