#ifndef _CWHO_
#define _CWHO_

#include "../commands.h"
#include "../comm.h"

class cWho : public cCommand
{
 public:
   cWho(void){};
   virtual ~cWho(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CWHO_
