#include "../define.h"
#include "../comm.h"
#include "../errors.h"
#include "shutdown.h"

extern bool server_shutdown;
extern class cDescList * descriptor_list;

void cShutdown::Execute( cDescriptor & d, cParam & param )
{
  descriptor_list->Send_To_All(SERVER_SHUTDOWN);
  server_shutdown = true;
}
