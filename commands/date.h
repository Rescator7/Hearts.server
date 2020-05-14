#ifndef _CDATE_
#define _CDATE_

#include "../commands.h"
#include "../comm.h"

class cDate : public cCommand
{
 public:
   cDate(void){};
   virtual ~cDate(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CDATE_
