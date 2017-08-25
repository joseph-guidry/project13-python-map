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
	fread(&iptest, 1, 20,  fp);
	if (  (iptest.ipv4.ver_header >> 4) == 6)
	{
		fread(&iptest, 1, 20,  fp);
	}
	pcap->pcapIp = iptest;
	fread(&udptest, 1, sizeof(struct udpHeader),  fp);
	pcap->pcapUdp = udptest;
	fread(&zergtest, 1, sizeof(struct zergHeader),  fp);
	pcap->pcapZerg = zergtest;
	return fp;
}
