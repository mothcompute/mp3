#include <string.h>
#include <stdio.h>

unsigned long btoi(char* s) {
	unsigned long r = 0;
	int l = strlen(s) - 1;
	for(int i = l; i >= 0; i--) {
		r <<= 1;
		r |= (s[l-i]-'0');
	}
	return r;
}

int main(int argc, char** argv) {
	printf("\t\t{%s,%s,%s,%i},\n", argv[1], argv[2], argv[3], btoi(argv[4]));
}
