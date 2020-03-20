#ifndef _CSHUTDOWN_
#define _CSHUTDOWN_

#include "../commands.h"

class cShutdown : public cCommand
{
 public:
   cShutdown(void){};
   virtual ~cShutdown(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CSHUTDOWN_
