#include <time.h>
#include "../define.h"
#include "date.h"

void cDate::Execute( cDescriptor & d, cParam & param )
{
 const time_t now = time(0);
 char server_time[50], gm_time[50];

 strftime(server_time, 50, "%a %b %e, %k:%M %Z %Y", localtime(&now));
 strftime(gm_time, 50, "%a %b %e, %k:%M GMT %Y", gmtime(&now));
 d.Socket_Write("Local time     - %s\n"
                "Server time    - %s\n"
                "GMT            - %s\n",
                server_time, server_time, gm_time);
}
