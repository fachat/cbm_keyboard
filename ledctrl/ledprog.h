

// note LP_CONST is zero so it is the initialized state
enum {
	LP_CONST 	= 0,	// constant value, using the three parameters as RGB
	LP_LINDECAY 	= 1,	// linear decay (note may change hue while getting lower)
	LP_LINDECAYLOOP	= 2,	// linear decay with restart
} progs_t;

void prog_set(int chain, int led, int prog, char pars[COLSANDPARS]);

void prog_advance();

void prog_setup();

