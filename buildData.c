#include "decode.h"

#define IP_Vr4 0x0800
#define IP_Vr6 0x86DD

FILE * buildPcapData(struct zergPacket * pcap, char *filename, int * filesize)
{
	FILE *fp;
	struct filepcap filetest;
	
	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		fprintf(stderr, "Could not open %s \n", filename);
		return NULL;;
	}
	//GET FILESIZE
	fseek(fp, 0, 2);
	*filesize = ftell(fp);
	fseek(fp, 0, 0);
	fread(&filetest, 1, sizeof(struct filepcap),  fp);
	if ( (filetest.fileTypeID - 0xa1b2c3d4) != 0)
	{
		fprintf(stderr, "%s: Invalid PCAP files\n", filename);
		exit(1);
	}
	pcap->fileHeader = filetest;
	return fp;
}

FILE * buildPacketData( struct zergPacket * pcap, FILE *fp )
{
	unsigned long n;
	struct headerpcap headertest;
	struct etherFrame ethertest;
	ipVersion iptest;
	struct udpHeader udptest;
	struct zergHeader zergtest;
	
	fread(&headertest, 1, sizeof(struct headerpcap),  fp);
	pcap->packetHeader = headertest;
	fread(&ethertest, 1, sizeof(struct etherFrame),  fp);
	pcap->pcapFrame = ethertest;
	if (htons(ethertest.etherType) == IP_Vr4)
	{
		fread(&iptest, 1, 20,  fp);
		//printf("here ip version: %u \n", iptest.ipv4.ver_header >> 4);
	}
	else if ( htons(ethertest.etherType) == IP_Vr6)
	{
		fread(&iptest, 1, 20,  fp);
		//printf("ipv6 ? %u \n", iptest.ipv6.ver_class_flowLabel >> 28);
			
		if( iptest.ipv6.ver_class_flowLabel >> 28 == 6 )
		{
			fread(&iptest, 1, 20,  fp);
		}
		//printf("error invalid IPV6\n");
	}
	
	pcap->pcapIp = iptest;
	fread(&udptest, 1, sizeof(struct udpHeader),  fp);
	pcap->pcapUdp = udptest;
	fread(&zergtest, 1, sizeof(struct zergHeader),  fp);
	pcap->pcapZerg = zergtest;
	return fp;
}
