
/*
 * standard map
 */

int map_key(int idx, int shift, int ctrl) {

	if (idx == 67) {
		// new shift lock
		// return old sjg shift lock idx
		return 29; 
	}
	return idx;
}

