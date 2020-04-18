#ifndef _CANNOUNCE_
#define _CANNOUNCE_

#include "../commands.h"
#include "../comm.h"

class cAnnounce : public cCommand
{
 public:
   cAnnounce(void){};
   virtual ~cAnnounce(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CANNOUNCE_
