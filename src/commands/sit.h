#ifndef _CSIT_
#define _CSIT_

#include "../commands.h"
#include "../comm.h"

class cSit : public cCommand
{
 public:
   cSit(void){};
   virtual ~cSit(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CSIT_
