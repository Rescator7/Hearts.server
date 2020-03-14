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
// const char *latin1 = "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";
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
// capital A grave                À     C0   &#192; --> À   &Agrave;   --> À
// capital A acute                Á     C1   &#193; --> Á   &Aacute;   --> Á
// capital A circumflex           Â     C2   &#194; --> Â   &Acirc;    --> Â
// capital A tilde                Ã     C3   &#195; --> Ã   &Atilde;   --> Ã
// capital A dieresis or umlaut   Ä     C4   &#196; --> Ä   &Auml;     --> Ä
// capital A ring                 Å     C5   &#197; --> Å   &Aring;    --> Å
// capital AE ligature            Æ     C6   &#198; --> Æ   &AElig;    --> Æ
// capital C cedilla              Ç     C7   &#199; --> Ç   &Ccedil;   --> Ç
// capital E grave                È     C8   &#200; --> È   &Egrave;   --> È
// capital E acute                É     C9   &#201; --> É   &Eacute;   --> É
// capital E circumflex           Ê     CA   &#202; --> Ê   &Ecirc;    --> Ê
// capital E dieresis or umlaut   Ë     CB   &#203; --> Ë   &Euml;     --> Ë
// capital I grave                Ì     CC   &#204; --> Ì   &Igrave;   --> Ì
// capital I acute                Í     CD   &#205; --> Í   &Iacute;   --> Í
// capital I circumflex           Î     CE   &#206; --> Î   &Icirc;    --> Î
// capital I dieresis or umlaut   Ï     CF   &#207; --> Ï   &Iuml;     --> Ï
// capital ETH                    Ð     D0   &#208; --> Ð   &ETH;      --> Ð
// capital N tilde                Ñ     D1   &#209; --> Ñ   &Ntilde;   --> Ñ
// capital O grave                Ò     D2   &#210; --> Ò   &Ograve;   --> Ò
// capital O acute                Ó     D3   &#211; --> Ó   &Oacute;   --> Ó
// capital O circumflex           Ô     D4   &#212; --> Ô   &Ocirc;    --> Ô
// capital O tilde                Õ     D5   &#213; --> Õ   &Otilde;   --> Õ
// capital O dieresis or umlaut   Ö     D6   &#214; --> Ö   &Ouml;     --> Ö
// multiplication sign            ×     D7   &#215; --> ×   &times;    --> ×
// capital O slash                Ø     D8   &#216; --> Ø   &Oslash;   --> Ø
// capital U grave                Ù     D9   &#217; --> Ù   &Ugrave;   --> Ù
// capital U acute                Ú     DA   &#218; --> Ú   &Uacute;   --> Ú
// capital U circumflex           Û     DB   &#219; --> Û   &Ucirc;    --> Û
// capital U dieresis or umlaut   Ü     DC   &#220; --> Ü   &Uuml;     --> Ü
// capital Y acute                Ý     DD   &#221; --> Ý   &Yacute;   --> Ý
// capital THORN                  Þ     DE   &#222; --> Þ   &THORN;    --> Þ
// small sharp s, sz ligature     ß     DF   &#223; --> ß   &szlig;    --> ß

// Lower Case Latin-1 Letters (sources: http://www.bbsinc.com/iso8859.html)
// -------------------------------------------------------------------------
// small a grave                  à     E0   &#224; --> à   &agrave;   --> à
// small a acute                  á     E1   &#225; --> á   &aacute;   --> á
// small a circumflex             â     E2   &#226; --> â   &acirc;    --> â
// small a tilde                  ã     E3   &#227; --> ã   &atilde;   --> ã
// small a dieresis or umlaut     ä     E4   &#228; --> ä   &auml;     --> ä
// small a ring                   å     E5   &#229; --> å   &aring;    --> å
// small ae ligature              æ     E6   &#230; --> æ   &aelig;    --> æ
// small c cedilla                ç     E7   &#231; --> ç   &ccedil;   --> ç
// small e grave                  è     E8   &#232; --> è   &egrave;   --> è
// small e acute                  é     E9   &#233; --> é   &eacute;   --> é
// small e circumflex             ê     EA   &#234; --> ê   &ecirc;    --> ê
// small e dieresis or umlaut     ë     EB   &#235; --> ë   &euml;     --> ë
// small i grave                  ì     EC   &#236; --> ì   &igrave;   --> ì
// small i acute                  í     ED   &#237; --> í   &iacute;   --> í
// small i circumflex             î     EE   &#238; --> î   &icirc;    --> î
// small i dieresis or umlaut     ï     EF   &#239; --> ï   &iuml;     --> ï
// small eth                      ð     F0   &#240; --> ð   &eth;      --> ð
// small n tilde                  ñ     F1   &#241; --> ñ   &ntilde;   --> ñ
// small o grave                  ò     F2   &#242; --> ò   &ograve;   --> ò
// small o acute                  ó     F3   &#243; --> ó   &oacute;   --> ó
// small o circumflex             ô     F4   &#244; --> ô   &ocirc;    --> ô
// small o tilde                  õ     F5   &#245; --> õ   &otilde;   --> õ
// small o dieresis or umlaut     ö     F6   &#246; --> ö   &ouml;     --> ö
// division sign                  ÷     F7   &#247; --> ÷   &divide;   --> ÷
// small o slash                  ø     F8   &#248; --> ø   &oslash;   --> ø
// small u grave                  ù     F9   &#249; --> ù   &ugrave;   --> ù
// small u acute                  ú     FA   &#250; --> ú   &uacute;   --> ú
// small u circumflex             û     FB   &#251; --> û   &ucirc;    --> û
// small u dieresis or umlaut     ü     FC   &#252; --> ü   &uuml;     --> ü
// small y acute                  ý     FD   &#253; --> ý   &yacute;   --> ý
// small thorn                    þ     FE   &#254; --> þ   &thorn;    --> þ
// small y dieresis or umlaut     ÿ     FF   &#255; --> ÿ   &yuml;     --> ÿ
