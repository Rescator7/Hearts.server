#ifndef _CPASS_
#define _CPASS_

#include "../commands.h"
#include "../comm.h"

class cPass : public cCommand
{
 public:
   cPass(void){};
   virtual ~cPass(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CPASS_
