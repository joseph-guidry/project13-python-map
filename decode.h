#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <inttypes.h>

typedef union {						//Store floats and convert to binary
	double value;
	uint64_t value64;
} convertDouble;

typedef union {						//Store floats and convert to binary 
	float value;
	uint32_t value32;
} convertFloat;

struct filepcap {
	uint32_t fileTypeID;			//*File Type ID
	uint16_t majorVersion;			//*Major Version
	uint16_t minorVersion;			//*Minor Version
	uint32_t gmtOffset;				//GMT Offset (Hours before/after GMT timezone)
	uint32_t accuracyDelta;			//Accuracy Delta
	uint32_t maxLengthCapture;		//Maximum length of a capture
	uint32_t linkLayerType;			//*Link Layer Type
};

struct headerpcap {
	uint32_t unixEpoch;				//UNIX Epoch -> start time of UNIX
	uint32_t microsec;				//microsecs from Epoch
	uint32_t dataCapture;			//Length of Caputured Data
	uint32_t linkLayerType;			//Truncated packet length
};

struct etherFrame {
	uint8_t destMac[6];			//Ethernet Destination MAC address
	uint8_t srcMac[6];					
	uint16_t etherType;
};
	
struct ipv4Header {
	uint8_t ver_header;
	uint8_t dscp_ecn;
	uint16_t totalIPhdrlen;
	uint16_t identification;
	uint16_t flags_frags;
	uint8_t ttl;
	uint8_t nextProtocol;
	uint16_t ipChecksum;
	uint32_t srcAddroct1;				//Source IP Address
	uint32_t srcAddroct2;
	
};

struct ipv6Header {
	uint32_t ver_class_flowLabel;
	uint32_t payloadLen_nxtHdr_HopLimit;
	uint8_t srcAddress[16];
	uint8_t destAddress[16];
};

typedef union {
	struct ipv4Header ipv4;
	struct ipv6Header ipv6;
} ipVersion;

struct udpHeader {
	uint16_t sport;					//UDP Source Port
	uint16_t dport;					//UDP Destination Port
	uint16_t udpLen;
	uint16_t udpChecksum;
};

struct zergHeader {
	uint32_t ver_type_totalLen;					//Psychic Format version = 1
	uint16_t destID;
	uint16_t sourceID;
	uint32_t seqID;
};

struct zergPacket {
	struct filepcap fileHeader;				//pcap File Header structure
	struct headerpcap packetHeader;  		//pcap Packet Header structure
	struct etherFrame pcapFrame;			//pcap Ethernet Frame
	ipVersion pcapIp;						//pcap IP header
	struct udpHeader  pcapUdp;				//pcap UDP header
	struct zergHeader pcapZerg;				//custom Zerg Packet Header
};

struct msgPayload {
	char * message;
};

struct statusPayload {
	int32_t hitPoints;
	uint32_t maxHitPoints;
	uint32_t speed;
	char * zergName;
};

struct commandPayload {
	uint16_t command;
	uint16_t parameter1;
	uint32_t parameter2;
};

struct gpsDataPayload {
	uint64_t longitude;
	uint64_t latitude;
	uint32_t altitude;
	uint32_t bearing;
	uint32_t speed;
	uint32_t accuracy;
};

typedef struct health 
{
	unsigned int hit_points;
	unsigned int max_points;
} zerg_health;


typedef struct position 
{
	convertDouble latitude;
	convertDouble longitude;
	convertFloat altitude;
} zerg_coordinates;

struct zerg {
	uint16_t srcID;
	int number;
	char * zerg_name;				
	zerg_coordinates position;
	zerg_health health;
};

FILE * buildPcapData (struct zergPacket * pcap, char * filename, int * filesize);
FILE * buildPacketData( struct zergPacket * pcapfile, FILE *fp ); 
FILE * printMsgPayload (struct zergPacket * pcap, FILE *fp);
FILE * fillStatusPayload (struct zergPacket * pcaps, FILE *fp, struct zerg * zerg_info);
FILE * fillGpsPayload (FILE *fp, struct zerg * zerg_info);
FILE * printCmdPayload (FILE *fp);
void getZergType(char * test, int x);
double convertBin32toDecimal (unsigned int speed);
double convertBin64toDecimal (unsigned long speed);
uint64_t swapLong( uint64_t x);
void degreesConvertDMS( double degrees);

struct node * decode(int argc, char **argv, int * nodes);
uint16_t get_root_srcID(void * data);

