#include <cstdlib>
#include "../define.h"
#include "../table.h"
#include "../global.h"
#include "../datagrams.h"
#include "purge.h"

void cPurge::Execute( cDescriptor &d, cParam &param )
{
  bool purged = table_list->Purge(atoi(param.arguments));

  d.Socket_Write("%s The table %s", DGI_TEXT, purged ? "has been purged." : "has not been found.");
}
