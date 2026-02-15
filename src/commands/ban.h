#ifndef _CBAN_
#define _CBAN_

#include "../commands.h"
#include "../comm.h"

class cBan : public cCommand
{
 public:
   cBan(void){};
   virtual ~cBan(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CBAN_
