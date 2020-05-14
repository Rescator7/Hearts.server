#ifndef _CPASSWORD_
#define _CPASSWORD_

#include "../commands.h"
#include "../comm.h"

class cPassword : public cCommand
{
 public:
   cPassword(void){};
   virtual ~cPassword(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _PASSWORD_
