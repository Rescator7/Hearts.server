#include "../define.h"
#include "../table.h"
#include "../global.h"
#include "tables.h"

void cTables::Execute( cDescriptor &d, cParam &param )
{
  table_list->Show(d);
}
