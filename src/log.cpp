#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <experimental/filesystem>

#include "define.h"
#include "log.h"

cLog::cLog(const char *fname)
{
  file_size = 0;
  byteswritten = 0;
  bLogging = true;
  filename = strdup(fname);

  Open();
}

cLog::~cLog()
{
  free(filename);
  if (bLogging)
    fclose(logfile); 
}
 
void cLog::Open()
{
  if (!(logfile=fopen(filename, "a"))) {
    printf("SYSERR: failed to turn on logging.\n");
    bLogging = false;
  } else
      file_size = std::experimental::filesystem::file_size(filename);
}

void cLog::Check_Size()
{
  file_size += byteswritten;
  
  byteswritten = 0;
  if (file_size < MAX_LOG_SIZE) 
    return;
  else 
    file_size = 0;
  
  fclose(logfile);

  remove("server.bak");

  rename(filename, "server.bak");

  if (!(logfile=fopen(filename, "w"))) {
    printf("SYSERR: failed to turn on logging.\n");
    bLogging = false;
  }
}

void cLog::Write (const char *format, ...)
{
  if (!bLogging) return;

  int bytes;

  va_list args;
  time_t ctime = time(nullptr);
  char *time_s = asctime(localtime(&ctime));

  if (format == NULL)
    format = "SYSERR: log() received a NULL format.";

  time_s[strlen(time_s) - 1] = '\0';

  bytes = fprintf(logfile, "%-15.15s :: ", time_s + 4); // skip day of the week

  if (bytes > 0)
    byteswritten += bytes;

  va_start(args, format);
  bytes = vfprintf(logfile, format, args);

  if (bytes > 0)
    byteswritten += bytes;

  va_end(args);

  fprintf(logfile, "\n");
  byteswritten++;

  // It's possible to write more than MAX_LOG_SIZE bytes, but
  // on the next Write(), the backup creation will occur.
  Check_Size();

  fflush(logfile);
}
