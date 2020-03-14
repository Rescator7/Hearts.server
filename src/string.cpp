#include <ctype.h>
#include <stdio.h>
#include "define.h"
#include "comm.h"
#include "string.h"

// #include <stdlib.h>
char *extract_cmd(const char *buffer, char *cmd)
{
  int i = 0;

  do {
    if ((buffer[i] == ' ') || (buffer[i] == '\x0') || (buffer[i] == '\r'))
      cmd[i] = '\x0';
    else
      cmd[i] = tolower(buffer[i]);
  } while (cmd[i++] != '\x0');

// printf("extract: '%s' '%s'\r\n", cmd, (char *)&buffer[i]);

  return ((char *)&buffer[i]);
}

const char *skip_spaces(char *str)
{
 int p = 0, s = 0, l = strlen(str);

// printf("in: '%s'\r\n", str);

 while (str[p] == ' ') if (++p >= l) break;

 if (p == 0) return str;

 while (p <= l) {
   str[s] = str[p];
   s++;
   p++;
 }

 // printf("str: '%s'\r\n", str);
 return str;
}

const char *stolower(const char *str, char *new_str)
{
 if (!str || !*str) return ( NULL );

 unsigned int i;
 for (i = 0; i < strlen(str); i++)
   new_str[i] = tolower(str[i]);
 new_str[i] = '\x0';

 return ( new_str );
}

const char *skip_crlf(char *str)
{
 if (!str || !*str) return ( NULL );

 int len = strlen(str);
 char * ptr = str + len - 1;

 if ((*ptr == '\n') || (*ptr == '\r')) *ptr = '\x0';
 if (len == 1) return ( (const char *)str );
 ptr--;
 if ((*ptr == '\n') || (*ptr == '\r')) *ptr = '\x0';
 return ( (const char *)str );
}

bool isBufferValid( char *buffer, bool bSupLatin1 )
{
// const char *latin1 = "����������������������������������������������������������������";
   const char *normal = "AAAAAAACEEEEIIIIDNOOOOO*OUUUUYPBAAAAAAxceeeeiiiienooooo/ouuuuypy";

 for (unsigned int i = 0; i < strlen(buffer); i++) {
   if ((buffer[i] == '\n') || (buffer[i] == '\r')) continue;
   if (buffer[i] < ' ') return ( false );
   if ((buffer[i] >= '\x7f') && (buffer[i] <= '\xbf')) return ( false ); // 127 - 191
   if (buffer[i] >= '\xc0') { // 192
     if (bSupLatin1) continue;
     buffer[i] = *(normal + buffer[i] - 192);
   }
 }
 return ( true );
}

// Upper Case Latin-1 Letters (sources: http://www.bbsinc.com/iso8859.html)
// -------------------------------------------------------------------------
// capital A grave                �     C0   &#192; --> �   &Agrave;   --> �
// capital A acute                �     C1   &#193; --> �   &Aacute;   --> �
// capital A circumflex           �     C2   &#194; --> �   &Acirc;    --> �
// capital A tilde                �     C3   &#195; --> �   &Atilde;   --> �
// capital A dieresis or umlaut   �     C4   &#196; --> �   &Auml;     --> �
// capital A ring                 �     C5   &#197; --> �   &Aring;    --> �
// capital AE ligature            �     C6   &#198; --> �   &AElig;    --> �
// capital C cedilla              �     C7   &#199; --> �   &Ccedil;   --> �
// capital E grave                �     C8   &#200; --> �   &Egrave;   --> �
// capital E acute                �     C9   &#201; --> �   &Eacute;   --> �
// capital E circumflex           �     CA   &#202; --> �   &Ecirc;    --> �
// capital E dieresis or umlaut   �     CB   &#203; --> �   &Euml;     --> �
// capital I grave                �     CC   &#204; --> �   &Igrave;   --> �
// capital I acute                �     CD   &#205; --> �   &Iacute;   --> �
// capital I circumflex           �     CE   &#206; --> �   &Icirc;    --> �
// capital I dieresis or umlaut   �     CF   &#207; --> �   &Iuml;     --> �
// capital ETH                    �     D0   &#208; --> �   &ETH;      --> �
// capital N tilde                �     D1   &#209; --> �   &Ntilde;   --> �
// capital O grave                �     D2   &#210; --> �   &Ograve;   --> �
// capital O acute                �     D3   &#211; --> �   &Oacute;   --> �
// capital O circumflex           �     D4   &#212; --> �   &Ocirc;    --> �
// capital O tilde                �     D5   &#213; --> �   &Otilde;   --> �
// capital O dieresis or umlaut   �     D6   &#214; --> �   &Ouml;     --> �
// multiplication sign            �     D7   &#215; --> �   &times;    --> �
// capital O slash                �     D8   &#216; --> �   &Oslash;   --> �
// capital U grave                �     D9   &#217; --> �   &Ugrave;   --> �
// capital U acute                �     DA   &#218; --> �   &Uacute;   --> �
// capital U circumflex           �     DB   &#219; --> �   &Ucirc;    --> �
// capital U dieresis or umlaut   �     DC   &#220; --> �   &Uuml;     --> �
// capital Y acute                �     DD   &#221; --> �   &Yacute;   --> �
// capital THORN                  �     DE   &#222; --> �   &THORN;    --> �
// small sharp s, sz ligature     �     DF   &#223; --> �   &szlig;    --> �

// Lower Case Latin-1 Letters (sources: http://www.bbsinc.com/iso8859.html)
// -------------------------------------------------------------------------
// small a grave                  �     E0   &#224; --> �   &agrave;   --> �
// small a acute                  �     E1   &#225; --> �   &aacute;   --> �
// small a circumflex             �     E2   &#226; --> �   &acirc;    --> �
// small a tilde                  �     E3   &#227; --> �   &atilde;   --> �
// small a dieresis or umlaut     �     E4   &#228; --> �   &auml;     --> �
// small a ring                   �     E5   &#229; --> �   &aring;    --> �
// small ae ligature              �     E6   &#230; --> �   &aelig;    --> �
// small c cedilla                �     E7   &#231; --> �   &ccedil;   --> �
// small e grave                  �     E8   &#232; --> �   &egrave;   --> �
// small e acute                  �     E9   &#233; --> �   &eacute;   --> �
// small e circumflex             �     EA   &#234; --> �   &ecirc;    --> �
// small e dieresis or umlaut     �     EB   &#235; --> �   &euml;     --> �
// small i grave                  �     EC   &#236; --> �   &igrave;   --> �
// small i acute                  �     ED   &#237; --> �   &iacute;   --> �
// small i circumflex             �     EE   &#238; --> �   &icirc;    --> �
// small i dieresis or umlaut     �     EF   &#239; --> �   &iuml;     --> �
// small eth                      �     F0   &#240; --> �   &eth;      --> �
// small n tilde                  �     F1   &#241; --> �   &ntilde;   --> �
// small o grave                  �     F2   &#242; --> �   &ograve;   --> �
// small o acute                  �     F3   &#243; --> �   &oacute;   --> �
// small o circumflex             �     F4   &#244; --> �   &ocirc;    --> �
// small o tilde                  �     F5   &#245; --> �   &otilde;   --> �
// small o dieresis or umlaut     �     F6   &#246; --> �   &ouml;     --> �
// division sign                  �     F7   &#247; --> �   &divide;   --> �
// small o slash                  �     F8   &#248; --> �   &oslash;   --> �
// small u grave                  �     F9   &#249; --> �   &ugrave;   --> �
// small u acute                  �     FA   &#250; --> �   &uacute;   --> �
// small u circumflex             �     FB   &#251; --> �   &ucirc;    --> �
// small u dieresis or umlaut     �     FC   &#252; --> �   &uuml;     --> �
// small y acute                  �     FD   &#253; --> �   &yacute;   --> �
// small thorn                    �     FE   &#254; --> �   &thorn;    --> �
// small y dieresis or umlaut     �     FF   &#255; --> �   &yuml;     --> �
