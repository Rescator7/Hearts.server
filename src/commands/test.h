#ifndef _CTEST_
#define _CTEST_

#include "../commands.h"
#include "../comm.h"

class cTest : public cCommand
{
 public:
   cTest(void){};
   virtual ~cTest(void){};

   virtual void Execute( cDescriptor &, cParam & );
};

#endif // _CTEST_
