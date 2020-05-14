#include "../define.h"
#include "../global.h"
#include "who.h"

void cWho::Execute( cDescriptor &d, cParam &param )
{
  descriptor_list->Who(d); 
}
