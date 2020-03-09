#include <openssl/des.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "define.h"
#include "player.h"
#include "sql.h"
#include "string.h"

cPlayer::cPlayer()
{
 handle = NULL;
 realname = NULL;
 email = NULL;
 password = NULL;
}

cPlayer::~cPlayer()
{
}

bool cPlayer::doesPasswordMatch( const char * p )
{ 
 printf("salt: %c%c, real: %s\r\ngave: %s\r\n", salt[0], salt[1], password, DES_crypt(p, salt));
 salt[0] = password[0];
 salt[1] = password[1];
 salt[2] = '\x0';
 return ( !strcmp(password, DES_crypt(p, salt)) );
}

void cPlayer::setPassword( const char * p, bool encrypt )
{
 const char letters[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

 if (!encrypt) {
   password = strdup(p);
   salt[0] = password[0];
   salt[1] = password[1];
   salt[2] = '\x0';
   return;
 } // already encrypted

 srand(time(0));
 salt[0] = letters[rand() % 62];
 salt[1] = letters[rand() % 62];
 salt[2] = '\x0';
// TODO: find a better way to get a random salt

 password = strdup(DES_crypt(p, salt));
 printf("salt: %c%c, p: %s\n", salt[0], salt[1], password);
}

bool cPlayer::isHandle( const char * h ) 
{
 char buf1 [MAX_HANDLE_LENGTH], buf2 [MAX_HANDLE_LENGTH];

 if (!handle || !*h) return ( false );

 stolower(h, buf1);
 stolower(handle, buf2);
 return ( !strcmp(buf1, buf2) ); 
}

bool cPlayer::save()
{
 printf ("%s %s %s %s\r\n", handle, password, realname, email);

 return ( sql.query("insert into account values (0, '%s', '%s', '%s', '%s', now(), 0, 1 )", handle, password, realname, email ));
}

bool cPlayer::load()
{
/*
 if (!sql.query("select prompt from account where handle = '%s'", handle))
   return ( false );

 strncpy (prompt, sql.get_row(0), MAX_PROMPT_LENGTH);
*/
 return ( true );
}
