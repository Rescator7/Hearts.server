#ifndef _CPURGE_
#define _CPURGE_

#include "../commands.h"
#include "../comm.h"

class cPurge : public cCommand
{
 public:
   cPurge(void){};
   virtual ~cPurge(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CPURGE_
