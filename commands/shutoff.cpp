#include "../define.h"
#include "../datagrams.h"
#include "../comm.h"
#include "../global.h"
#include "shutoff.h"

void cShutoff::Execute( cDescriptor & d, cParam & param )
{
  // descriptor_list->Send_To_All(SERVER_SHUTOFF);
  server_shutoff = true;
}
