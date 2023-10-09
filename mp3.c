#include <mp3.h>

#include <huffman.h>
#include <dat.h>

// TODO autodetect endianness here
#define fromle(x) (x)

// get bit big endian
//#define bitb(x, y) ((((uint8_t*)(x))[(y) >> 3] >> (7 - ((y) & 7))) & 1)

uint8_t bitb(uint8_t* s, uint32_t b) {
	return (s[b >> 3] >> (7 - (b & 7))) & 1;
}

#define cast(x, y) (*((x*)&(y)))

#define cblimit 21
#define cblimit_short 12

uint32_t ibitrb(uint8_t* s, uint32_t* b, char e) { // bit range big endian
	uint32_t a = 0;
	for(int i = 0; i < e; i++) a = (a << 1) | bitb(s, (*b)+i);
	//printf("bitrb %i at %i: 0x%X, 0d%u\n", e, *b, a, a);
	*b += e;
	return a;
}

uint32_t verify_l3(uint32_t d) { // returns 0 if header is valid
	d = ntohl(d);
	printf("magic %08X\n", d);
	//return (d & 0xFFF00000) ^ 0xFFF00000;
	return (d & 0xFFFE0000) ^ 0xFFFA0000;
}


void rdl3(uint32_t* s, mp3_frame* m) {
	uint32_t d = ntohl(*(s++));
	m->hdr = d;
	m->prot = (d >> 16) & 1;
	m->bitrate = l3_bitrates[(d >> 12) & 0xF];
	m->smprate = l3_smprates[(d >> 10) & 3];
	m->smpbit = (d >> 10) & 3;
	m->len = 144000 * m->bitrate / m->smprate + ((d >> 9) & 1);
	m->priv = (d >> 8) & 1;
	m->mode = (d >> 6) & 3;
	m->jmss = (d >> 5) & 1;
	m->jint = (d >> 4) & 1;
	m->emph = d & 3;

	m->crc = htons(*((uint16_t*)s)); // only valid if m->crc
	
	uint32_t bp = m->prot << 4; // bitpointer
	
	uint16_t main_data_end = bitrb(s, &bp, 9);
	char private_bits = bitrb(s, &bp, 5 - ((m->mode != 3) << 1));
	char num_ch = 1 + (m->mode != 3);
	char scfsi[4][num_ch];
	uint16_t part2_3_length[2][num_ch];
	uint16_t big_values[2][num_ch];
	uint8_t global_gain[2][num_ch];
	uint8_t scalefac_compress[2][num_ch];
	uint8_t blocksplit_flag[2][num_ch];

	// used if blocksplit flag is set
	uint8_t block_type[2][num_ch];
	uint8_t switch_point[2][num_ch];
	uint8_t table_select[2][2][num_ch];
	uint8_t subblock_gain[3][2][num_ch];

	// used if NOT set
	uint8_t region_address1[2][num_ch];
	uint8_t region_address2[2][num_ch];

	uint8_t preflag[2][num_ch];
	uint8_t scalefac_scale[2][num_ch];
	uint8_t count1table_select[2][num_ch];

	for(int band = 0; band < 4; band++) for(int ch = 0; ch < num_ch; ch++) scfsi[band][ch] = bitrb(s, &bp, 1);
	
	for(int gr = 0; gr < 2; gr++) for(int ch = 0; ch < num_ch; ch++) {
		part2_3_length[gr][ch] = bitrb(s, &bp, 12);
		big_values[gr][ch] = bitrb(s, &bp, 9);
		global_gain[gr][ch] = bitrb(s, &bp, 8);
		scalefac_compress[gr][ch] = bitrb(s, &bp, 4);
		blocksplit_flag[gr][ch] = bitrb(s, &bp, 1);
		if(blocksplit_flag[gr][ch]) {
			block_type[gr][ch] = bitrb(s, &bp, 2);
			switch_point[gr][ch] = bitrb(s, &bp, 1);
			for(int reg = 0; reg < 2; reg++) table_select[reg][gr][ch] = bitrb(s, &bp, 5);
			for(int window = 0; window < 3; window++) subblock_gain[window][gr][ch] = bitrb(s, &bp, 3);
		} else {
			for(int reg = 0; reg < 3; reg++) table_select[reg][gr][ch] = bitrb(s, &bp, 5);
			region_address1[gr][ch] = bitrb(s, &bp, 4);
			region_address2[gr][ch] = bitrb(s, &bp, 3);
		}
		preflag[gr][ch] = bitrb(s, &bp, 1);
		scalefac_scale[gr][ch] = bitrb(s, &bp, 1);
		count1table_select[gr][ch] = bitrb(s, &bp, 1);
	}

	// begin to parse main_data
	// TODO completely unclear where it is supposed to start - comment from specification reproduced below
	/***
		The main_data follows. It does not follow the above side information in the
		bitstream. The main_data ends at a location in the main_data bitstream
		preceding the frame header of the following frame at an offset given by the
		value of main_data_end (see definition of main_data_end and 3-Annex Fig.3-
		A.7.1
	***/
	
	int bpsave = bp;

	for(int gr = 0; gr < 2; gr++) for(int ch = 0; ch < num_ch; ch++) {
		if(blocksplit_flag[gr][ch] && block_type[gr][ch] == 2) {
			// long window(?)
			for(int cb = 0; cb < (switch_point[gr][ch] ? 8 : 0); cb++) if(!scfsi[cb] | !gr) {
				// TODO scalefac[cb][gr][ch]
				// use scalefac_compress[gr][ch]
				int sfcd;
				if(block_type[gr][ch] != 2) sfcd = cb > 10;
				else if(switch_point[gr][ch]) {
					// TODO if block_type is 2 and switch_point is 1
				} else sfcd = cb > 5;
				//scalefac[cb][0/*window*/][gr][ch] = bitrb(s, &bp, sfcom[scalefac_compress[gr][ch]][sfcd]);

			}
			// short window(?)
			for(int cb = (switch_point[gr][ch] ? 3 : 0); cb < cblimit_short; cb++) for(int window = 0; window < 3; window++) if(!scfsi[cb] | !gr) {
				// TODO scalefac[cb][gr][ch]
				// use scalefac_compress[gr][ch]
			}
		} else {
			for(int cb = 0; cb < cblimit; cb++) if(!scfsi[cb] | !gr) {
				// TODO scalefac[cb][gr][ch]
				// use scalefac_compress[gr][ch]
			}
			// TODO huffman bits
			unsigned short csz = 16, ci = 0;
			coord* c = malloc(csz * sizeof(coord));
			while(bp < bpsave + part2_3_length[gr][ch]) {
				//bp += huffman(table_select[]);
			}
		}
	}
}
