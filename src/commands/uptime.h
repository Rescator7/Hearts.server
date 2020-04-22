#ifndef _CUPTIME_
#define _CUPTIME_

#include "../commands.h"
#include "../comm.h"

class cUptime : public cCommand
{
 public:
   cUptime(void){};
   virtual ~cUptime(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CUPTIME_
