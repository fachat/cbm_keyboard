
#include "kbdhw.h"
#include "print.h"
#include "map.h"
#include "menu.h"

enum {
	MENU_INIT	= 0,
	MENU_P1		= 1,
} menu_t;

static const char msg_p1[] = "\x8c"\
		"press 1 to enable upper row digits, " \
		"\x8c" \
		"0 to disable:" \
		"\x8c";

static const char msg_p1_enable[] = "digits enabled" "\x8c";
static const char msg_p1_disable[] = "digits disabled" "\x8c";

static int state = MENU_INIT;
static int cnt = 0;
static int last_key = -1;

void menu_setup() {

	state = MENU_INIT;
	cnt = 0;
}


int p1_enter() {
	print_set(msg_p1);
	return MENU_P1;
}

int p1_enabled() {
	map_enable(MAP_DIGITROW);
	print_set(msg_p1_enable);
	return MENU_INIT;
}

int p1_disabled() {
	map_enable(MAP_NONE);
	print_set(msg_p1_disable);
	return MENU_INIT;
}

// returns 1 if menu has taken over the scan
int menu_advance() {
	int new_key = 0;
	
	switch(state) {
	case MENU_INIT:
		if (is_key_down(77)) {
			cnt++;
			if (cnt > 100) {
				state = p1_enter();
				cnt = 0;
			}
		} else {
			cnt = 0;
		}
		break;
	case MENU_P1:
		if (is_key_down(70)) {
			new_key = 70;
		} else
		if (is_key_down(54)) {
			new_key = 54;
		}
		if (new_key == 0) {
			if (last_key == 70) {
				// '0' pressed - disable
				state = p1_disabled();
			} else
			if (last_key == 54) {
				// '1' pressed - enable
				state = p1_enabled();
			}
		}
		last_key = new_key;
		cnt = 0;
		return 1;
	default:
		break;
	}
	return 0;
}

