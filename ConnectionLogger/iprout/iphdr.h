


#define		URG	32
#define		ACK 16
#define		PSH 8
#define		RST 4
#define		SYN 2
#define		FIN 1
typedef struct ip_hdr
{
	unsigned char ip_verlen;
	unsigned char ip_tos;
	unsigned short ip_totallength;
	unsigned short ip_id;
	unsigned short ip_offset;
	unsigned char ip_ttl;
	unsigned char ip_protocol;
	unsigned short ip_checksum;
	unsigned int ip_srcaddr;
	unsigned int ip_destaddr;
}
IP_HDR, * PIP_HDR;

//udp header
typedef struct udp_hdr
{
	unsigned short src_portno;
	unsigned short dst_portno;
	unsigned short udp_length;
	unsigned short udp_checksum;
} UDP_HDR, *PUDP_HDR;

//tcp header
typedef struct tcp_hdr
{
	unsigned short src_portno;
	unsigned short dst_portno;
	unsigned int seq;
	unsigned int ack_seq;
	unsigned char offset;
	unsigned char control;
	unsigned short window;
	unsigned short check;
	unsigned short urg_ptr;
	unsigned int tcpoptions1;
	unsigned int tcpoptions2;
} TCP_HDR, *PTCP_HDR;

typedef struct tcp_ip_hdr
{
	IP_HDR ipHdr;
	TCP_HDR tcpHdr;
}	TCP_IP_HDR, *PTCP_IP_HDR;


typedef struct udp_ip_hdr
{
	IP_HDR ipHdr;
	UDP_HDR udpHdr;
}	UDP_IP_HDR, *PUDP_IP_HDR;


typedef struct icmp_type_hdr
{
	IP_HDR ipHdr;
	unsigned char type;
} ICMP_TYPE_HDR, *PICMP_TYPE_HDR;


typedef struct icmp_destination_unreachable_hdr
{
	IP_HDR ipHdr;
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned int unused;
	IP_HDR original_datagram;
	double data_bits_original_datagram;
} ICMP_DESTINATION_UNREACHABLE_HDR, *pICMP_DESTINATION_UNREACHABLE_HDR;


typedef struct icmp_time_exeeded_hdr
{
	IP_HDR ipHdr;
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned int unused;
	IP_HDR original_datagram;
	double data_bits_original_datagram;
} ICMP_TIME_EXEEDED_HDR, *pICMP_TIME_EXEEDED_HDR;


typedef struct icmp_parameter_problem_hdr
{
	IP_HDR ipHdr;
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned char pointer;
	unsigned char unused1;
	unsigned short unused2;
	IP_HDR original_datagram;
	double data_bits_original_datagram;
} ICMP_PARAMETER_PROBLEM_HDR, *pICMP_PARAMETER_PROBLEM_HDR;


typedef struct icmp_source_quench_hdr
{
	IP_HDR ipHdr;
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned int unused;
	IP_HDR original_datagram;
	double data_bits_original_datagram;
} ICMP_SOURCE_QUENCH_HDR, *pICMP_SOURCE_QUENCH_HDR;


typedef struct icmp_redirect_hdr
{
	IP_HDR ipHdr;
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned int gateway_address;
	IP_HDR original_datagram;
	double data_bits_original_datagram;
} ICMP_REDIRECT_HDR, *pICMP_REDIRECT_HDR;


typedef struct icmp_echo_echoreply_hdr
{
	IP_HDR ipHdr;
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned short identifier;
	unsigned short seq_number;
} ICMP_ECHO_ECHOREPLY_HDR, *pICMP_ECHO_ECHOREPLY_HDR;

typedef struct icmp_information_requestreply_hdr
{
	IP_HDR ipHdr;
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned short identifier;
	unsigned short seq_number;
} ICMP_INFORMATION_REQUESTREPLY_HDR, *pICMP_INFORMATION_REQUESTREPLY_HDR;


typedef struct icmp_timestamp_timestampreply_hdr
{
	IP_HDR ipHdr;
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned short identifier;
	unsigned short seq_number;
	unsigned int origin_timestamp;
	unsigned int rec_timestamp;
	unsigned int transmit_timestamp;
} ICMP_TIMESTAMP_TIMESTAMPREPLY_HDR, *pICMP_TIMESTAMP_TIMESTAMPREPLY_HDR;


