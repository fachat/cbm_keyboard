
#include "map.h"

static int map_enabled = 0;

void map_enable(int enabled) {
	map_enabled = enabled;
}

int map_key_slock(int idx, int *shift, int *ctrl) {

	if (idx == 67) {
		// new shift lock
		// return old sjg shift lock idx
		return 29; 
	}
	return idx;
}

/*
 * standard map
 */

int map_key_numbers(int idx, int *shift, int *ctrl) {

	if (map_enabled == MAP_DIGITROW) 
	switch (idx) {
	case 0:	// '!'
		if (*shift) {
			*shift = 0;
		} else 
		if (*ctrl) {
			*ctrl = 0;
			*shift = 1;
		} else {
			idx = 54;	// '1'
		}
		break;
	case 8:	// quote
		if (*shift) {
			*shift = 0;
		} else 
		if (*ctrl) {
			*ctrl = 0;
			*shift = 1;
		} else {
			idx = 62;	// '2'
		}
		break;
	case 1:	// '#'
		if (*shift) {
			*shift = 0;
		} else 
		if (*ctrl) {
			*ctrl = 0;
			*shift = 1;
		} else {
			idx = 55;	// '3'
		}
		break;
	case 9:	// '$'
		if (*shift) {
			*shift = 0;
		} else 
		if (*ctrl) {
			*ctrl = 0;
			*shift = 1;
		} else {
			idx = 38;	// '4'
		}
		break;
	case 2:	// '%'
		if (*shift) {
			*shift = 0;
		} else 
		if (*ctrl) {
			*ctrl = 0;
			*shift = 1;
		} else {
			idx = 46;	// '5'
		}
		break;
	case 10: // '''
		if (*shift) {
			*shift = 0;
		} else 
		if (*ctrl) {
			*ctrl = 0;
			*shift = 1;
		} else {
			idx = 39;	// '6'
		}
		break;
	case 3:	// '&'
		if (*shift) {
			*shift = 0;
		} else 
		if (*ctrl) {
			*ctrl = 0;
			*shift = 1;
		} else {
			idx = 22;	// '7'
		}
		break;
	case 11: // '\'
		if (*shift) {
			*shift = 0;
		} else 
		if (*ctrl) {
			*ctrl = 0;
			*shift = 1;
		} else {
			idx = 30;	// '8'
		}
		break;
	case 4:	// '('
		if (*shift) {
			*shift = 0;
		} else 
		if (*ctrl) {
			*ctrl = 0;
			*shift = 1;
		} else {
			idx = 23;	// '9'
		}
		break;
	case 12: // ')'
		if (*shift) {
			*shift = 0;
		} else 
		if (*ctrl) {
			*ctrl = 0;
			*shift = 1;
		} else {
			idx = 70;	// '0'
		}
		break;
	}
	return idx;
}


int map_key(int idx, int *shift, int *ctrl) {
	idx = map_key_slock(idx, shift, ctrl);
	idx = map_key_numbers(idx, shift, ctrl);
	return idx;
}

