
#include <string.h>

#include "kbdhw.h"
#include "map.h"
#include "i2c.h"


extern unsigned char rowvals[16];


static unsigned char pot2[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

static inline int fix_row(int row) {
	return row ? row - 1 : 9;
}

static inline int fix_row_rev(int row) {
	return row != 9 ? row + 1 : 0;
}

void kbd_scan() {
	unsigned char rvals[16];
	memset(rvals, 0, 16);

	int idx = 0;
	unsigned char m;
	int c;
	int newidx;

	int shift = kbd_read(fix_row_rev(8)) & 0x21;
	int ctrl = kbd_read(fix_row_rev(1)) & 0x20;
	int origshift = shift;
	int origctrl = ctrl;

	for (int row = 0; row < 16; row++) {

		unsigned char v = kbd_read(fix_row_rev(row));

		idx = row << 3;

		for (c = 0; c < 8; c++) {

			m = pot2[c];

			if (v & m) {
				// key is set
				newidx = map_key(idx, &shift, &ctrl);
				rvals[newidx >> 3] |= pot2[newidx & 7];
			}
			idx++;
		}
	}
	// always disable ctrl
	rvals[1] &= 255 - 0x20;

	if (origshift && !shift) {
		rvals[8] &= 255 - 0x21;
	} else
	if (shift && !origshift) {
		rvals[8] |= 0x20;
	}

	if (origctrl && !ctrl) {
		rvals[1] &= 255 - 0x20;
	} else
	if (ctrl && !origctrl) {
		rvals[1] |= 0x20;
	}

	// find out which keys are new
	for (int row = 0; row < 10; row++) {
		if (rvals[row] != rowvals[row]) {
			m = rvals[row];
			if ((m & rowvals[row]) != m) {
				// newly set
				for (c = 0; c < 8; c++) {
					if ((m & pot2[c]) && !(rowvals[row] & pot2[c])) {
						i2c_send(row * 8 + c);
					} 
				}
			}
		}
	}

	memcpy(rowvals, rvals, 16);
}



