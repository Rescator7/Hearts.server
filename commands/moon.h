#ifndef _CMOON_
#define _CMOON_

#include "../commands.h"
#include "../comm.h"

class cMoon : public cCommand
{
 public:
   cMoon(void){};
   virtual ~cMoon(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CMOON_
