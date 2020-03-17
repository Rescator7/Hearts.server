#ifndef _CPLAY_
#define _CPLAY_

#include "../commands.h"
#include "../comm.h"

class cPlay : public cCommand
{
 public:
   cPlay(void){};
   virtual ~cPlay(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CPLAY_
