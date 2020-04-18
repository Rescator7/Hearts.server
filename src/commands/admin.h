#ifndef _CADMIN_
#define _CADMIN_

#include "../commands.h"
#include "../comm.h"

class cAdmin : public cCommand
{
 public:
   cAdmin(void){};
   virtual ~cAdmin(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _ADMIN_
