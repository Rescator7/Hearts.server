#ifndef _CSITC_
#define _CSITC_

#include "../commands.h"
#include "../comm.h"

class cSitc : public cCommand
{
 public:
   cSitc(void){};
   virtual ~cSitc(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CSITC_
