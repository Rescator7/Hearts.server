#include "../define.h"
#include "../comm.h"
#include "../errors.h"
#include "../global.h"
#include "shutdown.h"

void cShutdown::Execute( cDescriptor & d, cParam & param )
{
  descriptor_list->Send_To_All(SERVER_SHUTDOWN);
  server_shutdown = true;
}
