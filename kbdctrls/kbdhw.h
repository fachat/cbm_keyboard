
#define	ROWS	10
#define	COLS	8

extern unsigned char rowvals[16];

static inline int is_key_down(int idx) {
	return rowvals[idx/8] & (1 << (idx%8));
}

static inline void set_key_down(int idx) {
	rowvals[idx/8] |= (1<< (idx%8));
}

static inline void set_key_up(int idx) {
	rowvals[idx/8] &= 255 - (1<< (idx%8));
}

void kbd_scan();

void check_reset();

void joy_scan();

