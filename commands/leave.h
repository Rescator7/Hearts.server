#ifndef _CLEAVE_
#define _CLEAVE_

#include "../commands.h"
#include "../comm.h"

class cLeave : public cCommand
{
 public:
   cLeave(void){};
   virtual ~cLeave(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CLEAVE_
