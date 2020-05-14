#ifndef _CSTART_
#define _CSTART_

#include "../commands.h"
#include "../comm.h"

class cStart : public cCommand
{
 public:
   cStart(void){};
   virtual ~cStart(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CSTART_
