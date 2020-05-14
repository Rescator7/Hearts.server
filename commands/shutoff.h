#ifndef _CSHUTOFF_
#define _CSHUTOFF_

#include "../commands.h"

class cShutoff : public cCommand
{
 public:
   cShutoff(void){};
   virtual ~cShutoff(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CSHUTOFF_
