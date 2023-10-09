#include "mp3.h"

int main(int argc, char** argv) {
	
	struct stat s;
	mp3_frame m;
	int fd;
	
	for(int f = 1; f < argc; f++) {
		
		uint32_t sp = 0, fc = 0; // stream ptr., frame ctr.
		
		if((fd = open(argv[f], O_RDWR)) < 0) continue;
		fstat(fd, &s);
		uint8_t* stream = malloc(s.st_size);
		read(fd, stream, s.st_size);


		static char* yn[2] = {"no", "yes"};
		static char* chtyp[4] = {"stereo", "joint stereo", "dual mono (stereo)", "mono"};
		static char* emph[4] = {"none", "50/15 ms", "reserved", "CCIT J.17"};

		printf("begin reading %s (%lu bytes at %p)\n", argv[f], s.st_size, stream);

		if(!memcmp(stream, "ID3", 3)) {
			int d = htonl(*((uint32_t*)(stream + 6)));
			sp += d + 10 + ((stream[5] & 0x10) ? 10 : 0);
			printf("ID3v2 detected, skipping %08X bytes so as not to deal with it\n", sp);
		}

		if(!memcmp(stream + s.st_size - 128, "TAG", 3)) printf("TAG detected :)\n");

		while(!verify_l3(*(uint32_t*)(stream + sp))) {
			printf("frame %u sync at %u (%X)!\n", fc, sp, sp);
			rdl3((uint32_t*)(stream + sp), &m);
			printf(
				"\tprotected:\t%s\n"
				"\tbitrate:\t%ukbps\n"
				"\tsamplerate:\t%uhz\n"
				"\tframe length:\t%u bytes\n"
				"\tprivate bit:\t%u\n"
				"\tchannel type:\t%s\n"
				"\tMS stereo:\t%s\n"
				"\tint. stereo:\t%s\n"
				"\temphasis:\t%s\n",
				yn[m.prot],
				m.bitrate,
				m.smprate,
				m.len,
				m.priv,
				chtyp[m.mode],
				yn[m.jmss],
				yn[m.jint],
				emph[m.emph]
			);
			sp += m.len;
			fc++;
		}

		free(stream);
	}
}
