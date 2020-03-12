#include "../define.h"
#include "../errors.h"
#include "../comm.h"
#include "shutoff.h"

extern bool server_shutdown;
extern bool server_shutoff;
extern class cDescList * descriptor_list;

void cShutoff::Execute( cDescriptor & d, cParam & param )
{
  descriptor_list->Send_To_All(SERVER_SHUTOFF);
  server_shutoff = true;
}
