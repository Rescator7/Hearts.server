#include "../define.h"
#include "../comm.h"
#include "../datagrams.h"
#include "../global.h"
#include "shutdown.h"

void cShutdown::Execute( cDescriptor & d, cParam & param )
{
  descriptor_list->Send_To_All(DGE_SERVER_SHUTDOWN);
  server_shutdown = true;
}
