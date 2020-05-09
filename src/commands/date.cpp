#include <ctime>
#include "../define.h"
#include "../datagrams.h"
#include "date.h"

void cDate::Execute( cDescriptor & d, cParam & param )
{
 const time_t now = time(nullptr);
 char server_time[50], gm_time[50];

 strftime(server_time, 50, "%a %b %e, %k:%M %Z %Y", localtime(&now));
 strftime(gm_time, 50, "%a %b %e, %k:%M GMT %Y", gmtime(&now));
 d.Socket_Write("%s Server time	 - %s\n"
                "GMT	 - %s",
                DGI_TEXT, server_time, gm_time);
}
