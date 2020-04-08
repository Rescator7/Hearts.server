#ifndef _CSET_
#define _CSET_

#include "../commands.h"
#include "../comm.h"

class cSet : public cCommand
{
 public:
   cSet(void){};
   virtual ~cSet(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CSET_
