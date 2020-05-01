#include "../define.h"
#include "../datagrams.h"
#include "../global.h"
#include "help.h"

void cHelp::Execute( cDescriptor &d, cParam &param )
{
  if (!*param.arguments) {
    d.Socket_Write("%s Commands list:\r\n"
	           "date	exit	help	join	leave\r\n"
		   "moon	mute	new	pass	password\r\n"
		   "play	say	sit	stats	tables\r\n"
		   "uptime	who\r\n", 
		   DG_TEXT);
    return;
  }

  cmd.Help(d, param.arguments);
}
