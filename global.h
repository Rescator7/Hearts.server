#ifndef _GLOBAL_
#define _GLOBAL_
#include <ctime>

extern bool server_shutdown;
extern bool server_shutoff;
extern unsigned int num_table;
extern class cDescList *descriptor_list;
extern class cTabList *table_list;
extern time_t boot_time;
extern class cCommandsStack cmd;
#endif
