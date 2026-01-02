
#include <string.h>

#include "scan.h"
#include "kbdhw.h"
#include "map.h"
#include "i2c.h"


// output to the interrupt routine
extern unsigned char scanvals[16];

// raw values before mapping, to signal the right LEDs
static unsigned char rawvals[16];

static unsigned char pot2[] = { 1, 2, 4, 8, 16, 32, 64, 128 };


static inline int fix_row_rev(int row) {
	return row != 9 ? row + 1 : 0;
}

static void check_res(int slock, int *reset) {
        static int count = 0;

        // check RES
        if (slock) {        // SHIFT LOCK key
                count ++;
                if (count > 200) {
                        // res low
                        *reset = 1;
                }
        } else {
                count = 0;
                *reset = 0;
        }
}

static void handle_slock(int slock, int *shift) {
	static int lock = 0;
	static int locklock = 0;

	if (slock && (locklock == 0)) {
		lock = 1-lock;
		locklock = 1;
		i2c_send_slock(67, lock);
	} else
	if (slock == 0) {
		locklock = 0;
	}
	*shift |= lock;
}

void kbd_scan(int *reset) {
	unsigned char rvals[16];
	unsigned char hvals[16];
	memset(rvals, 0, 16);

	int idx = 0;
	unsigned char m;
	int c;
	int newidx;

	int shift = kbd_read(fix_row_rev(8)) & 0x21;
	int ctrl = kbd_read(fix_row_rev(1)) & 0x20;
	int origshift = shift;
	int origctrl = ctrl;

	int slock = kbd_read(fix_row_rev(8)) & 0x08;
	check_res(slock, reset);
	handle_slock(slock, &shift);

	for (int row = 0; row < 16; row++) {

		unsigned char v = kbd_read(fix_row_rev(row));

		// hardware values - to signal to LED ctrl
		hvals[row] = v;

		idx = row << 3;

		for (c = 0; c < 8; c++) {

			m = pot2[c];

			if (v & m) {
				// key is set
				newidx = map_key(idx, &shift, &ctrl, reset);
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

	memcpy(scanvals, rvals, 16);

	// find out which keys are new
	for (int row = 0; row < 10; row++) {
		if (hvals[row] != rawvals[row]) {
			m = hvals[row];
			if ((m & rawvals[row]) != m) {
				// newly set
				for (c = 0; c < 8; c++) {
					if ((m & pot2[c]) && !(rawvals[row] & pot2[c])) {
						i2c_send(row * 8 + c);
					} 
				}
			}
		}
	}

	memcpy(rawvals, hvals, 16);
}



