#ifndef _LOG_
#define _LOG_

#include <cstdio> // FILE 

class cLog {
public:
  cLog(const char *fname);
  ~cLog();

private:
  bool bLogging;
  long int byteswritten;
  char *filename;
  FILE *logfile;

public:
  void Write (const char * format, ...);
  void Open();
  bool Check_Size();
};

// external variable
extern class cLog Log;

#endif // _LOG_
