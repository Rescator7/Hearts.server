#ifndef _CTABLES_
#define _CTABLES_

#include "../commands.h"
#include "../comm.h"

class cTables : public cCommand
{
 public:
   cTables(void){};
   virtual ~cTables(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CTABLES_
