#include <cctype>
#include <cstring>
#include <cstdio>
#include "define.h"
#include "ansi.h"
#include "comm.h"
#include "log.h"
 
const char *cDescriptor::process_ansi( const char *outbuff )
{
 // a b c d e f g h i j k l m n o p q r s t u v w x y z
 const int colors [26] = {0, 34, 36, 30, 0, 0, 32, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 31, 0, 0, 0, 0, 37, 0, 33, 0};
 char ansi_code [20], color1, color2;
 int len = 0, len_ac;

 *buffer = '\x0';
 for (unsigned int i = 0; i < strlen(outbuff); i++) {
   *ansi_code = '\x0';
   if (outbuff[i] == '&') {
     color1 = tolower(outbuff[++i]);
     if ((color1 >= 'a') && (color1 <= 'z')) {
       sprintf(ansi_code, "\x1b[%d;%d", isupper(outbuff[i]) ? 1 : 0, colors[color1 - 'a']);
       if (outbuff[i + 1] == '/') {
         i += 2;
         color2 = tolower(outbuff[i]);
         if ((color2 >= 'a') && (color2 <= 'z'))
           sprintf(ansi_code + strlen(ansi_code), ";%d", colors[color2 - 'a'] + 10);
       } 
       strcat(ansi_code, "m");
     }
   }
   if (*ansi_code) {
     len_ac = strlen(ansi_code);
     if (len + len_ac > SOCKET_BUFSIZE) 
       Log.Write("process_ansi: **** Buffer Overflow ****");
     else {
       strncat(buffer, ansi_code, SOCKET_BUFSIZE - len);
       len += len_ac;
     }
   } else {
       strncat(buffer, (const char *) &outbuff[i], 1);
       len++;
     }
   if (len >= SOCKET_BUFSIZE) {
     Log.Write("process_ansi: **** Buffer Overflow ****");
     return ( (const char *) &buffer );
   }
   if (outbuff[i] == '\x0')
     return ( (const char *) &buffer );
 }
 return ( (const char *) &buffer );
}
