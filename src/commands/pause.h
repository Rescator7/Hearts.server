#ifndef _CPAUSE_
#define _CPAUSE_

#include "../commands.h"
#include "../comm.h"

class cPause : public cCommand
{
 public:
   cPause(void){};
   virtual ~cPause(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CPAUSE_
