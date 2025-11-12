
/*
 * map keys
 *
 * Has key index as param, as well as shift and ctrl flags
 * returns mapped key
 */
int map_key(int idx, int *shift, int *ctrl);

/*
 * available mappings
 */
enum {
	MAP_NONE	= 0,
	MAP_DIGITROW	= 1,
} map_t;

/*
 * enable or disable a mapping
 */
void map_enable(int enabled);

