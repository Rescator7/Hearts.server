#ifndef _CJOIN_
#define _CJOIN_

#include "../commands.h"
#include "../comm.h"

class cJoin : public cCommand
{
 public:
   cJoin(void){};
   virtual ~cJoin(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CJOIN_
