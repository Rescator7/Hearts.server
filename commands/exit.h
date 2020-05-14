#ifndef _CEXIT_
#define _CEXIT_

#include "../commands.h"
#include "../comm.h"

class cExit : public cCommand
{
 public:
   cExit(void){};
   virtual ~cExit(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CEXIT_
