#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#define bitrb(x, y, z) ibitrb((uint8_t*)x, y, z)

typedef struct {
	uint8_t x;
	uint8_t y;
} coord;

// frame data
typedef struct {

	uint32_t hdr;

	// parsed out
	uint16_t bitrate;
	uint16_t smprate;
	uint16_t len;

	// bitflags
	uint8_t prot; // crc protection - TODO add to verify_l3
	uint8_t priv;
	uint8_t smpbit;
	uint8_t jint; // joint stereo - intensity stereo
	uint8_t jmss; // joint stereo - MS stereo (?)

	// indices
	uint8_t mode;
	uint8_t emph; // emphasis?

	// parsed data
	int32_t crc; // negative when not present

	uint8_t dat[1018];
} mp3_frame;

uint8_t bitb(uint8_t* s, uint32_t b);
uint32_t ibitrb(uint8_t* s, uint32_t* b, char e);
uint32_t verify_l3(uint32_t d);
void rdl3(uint32_t* s, mp3_frame* m);
