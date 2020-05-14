#include <cstdio>
#include "../define.h"
#include "../datagrams.h"
#include "../global.h"
#include "uptime.h"

void cUptime::Execute( cDescriptor &d, cParam &param )
{
  int days, hours, minutes, seconds;
  char server_time[50];

  strftime(server_time, 50, "%a %b %e, %k:%M %Z %Y", localtime(&boot_time));

  seconds = difftime(time(nullptr), boot_time);
  days = seconds / 86400;
  hours = (seconds / 3600) % 24;
  minutes = (seconds / 60) % 60;

  d.Socket_Write("%s Hearts server version %s\r\nUp since %s: %d day%s, %d:%02d", DGI_TEXT, VERSION, server_time, days, days == 1 ? "" : "s", hours, minutes);
}
