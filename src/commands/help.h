#ifndef _CHELP_
#define _CHELP_

#include "../commands.h"
#include "../comm.h"

class cHelp : public cCommand
{
 public:
   cHelp(void){};
   virtual ~cHelp(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CHELP_
