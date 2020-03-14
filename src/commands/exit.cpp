#include "../define.h"
#include "exit.h"

void cExit::Execute( cDescriptor &d, cParam &param )
{
 d.Disconnect();
}
