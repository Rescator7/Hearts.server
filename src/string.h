#ifndef _STRING_
#define _STRING_

#include <cstring>

extern const char *stolower(const char *str, char *new_str);
extern const char *skip_crlf(char *str);
extern const char *skip_spaces(char *str);
extern bool isBufferValid(char *buffer, bool bSupLatin1);
extern char *extract_cmd(const char *buffer, char *cmd);

#endif // _STRING_
