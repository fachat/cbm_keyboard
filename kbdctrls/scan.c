
#include <string.h>

#include "kbdhw.h"
#include "map.h"


extern unsigned char rowvals[16];


static unsigned char pot2[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

void kbd_scan() {
	unsigned char rvals[16];
	memset(rvals, 0, 16);

	int idx = 0;
	unsigned char m;
	int c;
	int newidx;

	for (unsigned char row = 0; row < 16; row++) {

		unsigned char v = kbd_read(row);

		int fixed = row ? row-1 : 9;
		idx = fixed << 3;

		for (c = 0; c < 8; c++) {

			m = pot2[c];

			if (v & m) {
				// key is set
				newidx = map_key(idx, 0, 0);
				rvals[newidx >> 3] |= pot2[newidx & 7];
			}
			idx++;
		}
		//rowvals[fixed] = v;
	}
	memcpy(rowvals, rvals, 16);
}



