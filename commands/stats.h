#ifndef _CSTATS_
#define _CSTATS_

#include "../commands.h"
#include "../comm.h"

class cStats : public cCommand
{
 public:
   cStats(void){};
   virtual ~cStats(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CSTATS_
