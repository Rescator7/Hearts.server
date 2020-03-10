#include "../define.h"
#include "../comm.h"
#include "shutoff.h"

extern bool server_shutdown;
extern bool server_shutoff;
extern class cDescList * descriptor_list;

void cShutoff::Execute( cDescriptor & d, cParam & param )
{
  descriptor_list->send_to_all("%s", "Server is shutting off...");
  server_shutoff = true;
}
