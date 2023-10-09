uint16_t l3_bitrates[16] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};
uint16_t l3_smprates[4] = {44100, 48000, 32000, 0};

// TODO possiblility block_type is undefined if blocksplit not set. read spec for blocksplit_flag and block_type
// table to get length of scalefactor bands
uint8_t sfcom[16][2] = {
	{0,0},
	{0,1},
	{0,2},
	{0,3},
	{3,0},
	{1,1},
	{1,2},
	{1,3},
	{2,1},
	{2,2},
	{2,3},
	{3,1},
	{3,2},
	{3,3},
	{4,2},
	{4,3},
};

uint8_t sf[3][33] = {
	{ // 32000hz band
		4,4,4,4,4,4,6,6,8,10,12,16,20,24,30,38,46,56,68,84,102,
		4,4,4,4,6,8,12,16,20,26,34,42
	}, { // 44100hz band
		4,4,4,4,4,4,6,6,8,8,10,12,16,20,24,28,34,42,50,54,76,
		4,4,4,4,6,8,10,12,14,18,22,30
	}, { // 48000hz band
		4,4,4,4,4,4,6,6,6,8,10,12,16,18,22,28,34,40,46,54,54,
		4,4,4,4,6,6,10,12,14,16,20,26
	}
};

uint8_t preemph[21] = {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,3,3,2};

long double alias[8] = {
	-.6,
	-.535,
	-.33,
	-.185,
	-.095,
	-.041,
	-.0142,
	-.0037
};

// Huffman code table for quadruples (A)
// search by value
uint8_t hquada[16] = {
	0b10000000,
	0b01010000,
	0b01000000,
	0b00101000,
	0b01100000,
	0b00010100,
	0b00100000,
	0b00010000,
	0b01110000,
	0b00011000,
	0b00110000,
	0b00000000,
	0b00111000,
	0b00001000,
	0b00001100,
	0b00000100,
};

// Huffman code table for quadruples (B)
// What The Hell
// search by index
uint8_t hquadb[16] = {
	15,
	14,
	13,
	12,
	11,
	10,
	9,
	8,
	7,
	6,
	5,
	4,
	3,
	2,
	1,
	0
};

// first ESC table is 16
char linbits[16] = {1, 2, 3, 4, 6, 8, 10, 13, 4, 5, 6, 7, 8, 9, 11, 13};

