#include <cctype>
#include <cstdio>
#include "define.h"
#include "comm.h"
#include "string.h"

char *extract_cmd(const char *buffer, char *cmd)
{
  int i = 0;

  do {
    if (isspace(buffer[i]))
      cmd[i] = '\x0';
    else
      cmd[i] = tolower(buffer[i]);
  } while (cmd[i++] != '\x0');

  return ((char *)&buffer[i]);
}

const char *skip_spaces(char *str)
{
 int p = 0, s = 0;

 while (isspace(str[p]))
   p++;

 if (p == 0) return str;

 while (str[p]) {
   str[s] = str[p];
   s++;
   p++;
 }
 str[s] = '\x0';

 return str;
}

const char *stolower(const char *str, char *new_str)
{
 if (str == nullptr) return nullptr;

 while (*str)
   *new_str++ = tolower(*str++);
 *new_str = '\x0';

 return new_str;
}

const char *skip_crlf(char *str)
{
  if (str == nullptr) return nullptr;

  while (*str)
    if ((*str == '\r') || (*str == '\n')) {
      *str = '\x0';
      break;
    } else
	str++;
  return str;
}

bool isBufferValid( char *buffer, bool bSupLatin1 )
{
// const char *latin1 = "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞßàáâãäåæçèéêëìíîïğñòóôõö÷øùúûüışÿ";
// const char *normal = "AAAAAAACEEEEIIIIDNOOOOO*OUUUUYPBAAAAAAxceeeeiiiienooooo/ouuuuypy";

 for (unsigned int i = 0; i < strlen(buffer); i++) {
   if (isspace(buffer[i])) continue;
   if ((buffer[i] >= 32) && (buffer[i] <= 126)) continue;
   return false;
//   if (bSupLatin1) continue;
 }
 return true;
}

