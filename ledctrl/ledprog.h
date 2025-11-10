

// note LP_CONST is zero so it is the initialized state
enum {
	// single LED programs. Followed by index, three colours, and a parameter
	LP_CONST 	= 0,	// constant value, using the three parameters as RGB
	LP_LINDECAY 	= 1,	// linear decay (note may change hue while getting lower)
	LP_LINDECAYLOOP	= 2,	// linear decay with restart

	// single command program
	SP_KEYPRESS	= 64,	// just followed by the index to indicate a pressed key
} progs_t;

void prog_set(int k, int prog, char pars[COLSANDPARS]);

void prog_advance();

void prog_setup();

