#ifndef _LOG_
#define _LOG_

#include <stdio.h> // FILE 

class cLog {
public:
  cLog( const char * filename );
  ~cLog();

private:
  bool bLogging;
  FILE * logfile;

public:
  void Write (const char * format, ...);
};

// external variable
extern class cLog Log;

#endif // _LOG_
