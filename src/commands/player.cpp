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
 player_id = 0;
 level = 0;
 handle = nullptr;
 realname = nullptr;
 email = nullptr;
 password = nullptr;
 ip = nullptr;
 table = nullptr;
}

cPlayer::~cPlayer()
{
  if (ip)
    free(ip);
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

 srand(time(nullptr));
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

unsigned int cPlayer::SQL_ID()
{
 int id = 0;

 if (sql.query("select playerid from account where handle = '%s'", handle)) {
   id = atoi(sql.get_row(0));
 }
 return id;
}

bool cPlayer::save()
{
 printf ("%s %s %s %s %s\r\n", handle, password, ip, realname, email);

 return ( sql.query("insert into account values (0, '%s', '%s', '%s', '%s', '%s', now(), 0, 1 )", handle, password, ip, realname, email ));
}

bool cPlayer::load()
{
 if (sql.query("select playerid, ip, userlevel from account where handle = '%s'", handle)) {
   player_id = atoi(sql.get_row(0));
   if (sql.get_row(1))
     ip = strdup(sql.get_row(1));
   if (ip)
     printf("Connected from: %s\r\n", ip);
   level = atoi(sql.get_row(2));
 }
 printf("player id: %d, userlevel: %d\r\n", player_id, level);
 return ( true );
}

unsigned int cPlayer::Level()
{
  return level;
}

unsigned int cPlayer::ID()
{
  return player_id;
}

char *cPlayer::Handle()
{
  return handle;
}

char *cPlayer::Password()
{
  return password;
}

char *cPlayer::Ip()
{
  return ip;
}

void cPlayer::Set_Handle(char *h)
{
  handle = h;
}

void cPlayer::Set_Password(char *p)
{
  password = p;
}

void cPlayer::Set_Ip(char *_ip)
{
  ip = strdup(_ip);
}

void cPlayer::ULink_Table(unsigned int id)
{
  if (table && (table->TableID() == id))
    table = nullptr;
}
