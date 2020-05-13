#include "../define.h"
#include "../datagrams.h"
#include "../global.h"
#include "help.h"

void cHelp::Execute( cDescriptor &d, cParam &param )
{
  cmd.Help(d, param.arguments);
}
