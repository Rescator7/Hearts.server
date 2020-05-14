#ifndef _CNEW_
#define _CNEW_

#include "../commands.h"
#include "../comm.h"

class cNew : public cCommand
{
 public:
   cNew(void){};
   virtual ~cNew(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CNEW_
