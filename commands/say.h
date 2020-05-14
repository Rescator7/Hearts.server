#ifndef _CSAY_
#define _CSAY_

#include "../commands.h"
#include "../comm.h"

class cSay : public cCommand
{
 public:
   cSay(void){};
   virtual ~cSay(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CSAY_
