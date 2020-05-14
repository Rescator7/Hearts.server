#ifndef _CMUTE_
#define _CMUTE_

#include "../commands.h"
#include "../comm.h"

class cMute : public cCommand
{
 public:
   cMute(void){};
   virtual ~cMute(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CMUTE_
