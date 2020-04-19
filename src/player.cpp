#include <openssl/des.h>
#include <cstring>
#include <cstdio>
#include <ctime>

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
 first = 0;
 second = 0;
 third = 0;
 fourth = 0;
 login_time = time(nullptr);
}

cPlayer::~cPlayer()
{
  if (ip)
    free(ip);
}

bool cPlayer::doesPasswordMatch( const char * p )
{ 
#ifdef DEBUG
 printf("salt: %c%c, real: %s\r\ngave: %s\r\n", salt[0], salt[1], password, DES_crypt(p, salt));
#endif

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

#ifdef DEBUG
 printf("salt: %c%c, p: %s\n", salt[0], salt[1], password);
#endif
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
#ifdef DEBUG
 printf ("%s %s %s %s %s\r\n", handle, password, ip, realname, email);
#endif

 return ( sql.query("insert into account values (0, '%s', '%s', '%s', '%s', '%s', now(), now(), 0, 1, 0, 0, 0, 0 )", handle, password, ip, realname, email ));
}

bool cPlayer::load()
{
 if (sql.query("select playerid, ip, userlevel, first, second, third, fourth from account where handle = '%s'", handle)) {
   player_id = atoi(sql.get_row(0));

   if (sql.get_row(1)) // can't strdup empty string
     ip = strdup(sql.get_row(1));

   level = atoi(sql.get_row(2));
   first = atoi(sql.get_row(3));
   second = atoi(sql.get_row(4));
   third = atoi(sql.get_row(5));
   fourth = atoi(sql.get_row(6));

#ifdef DEBUG
   if (ip)
     printf("Connected from: %s\r\n", ip);
 }
 printf("player id: %d, userlevel: %d\r\n", player_id, level);
#else
 }
#endif

 return ( true );
}

void cPlayer::update(usINT cmd)
{
  long int diff;

  switch (cmd) {
    case CMD_LASTLOGIN: sql.query("update account set lastlogin = now() where playerid = %d;", player_id);
			break;
    case CMD_TOTALTIME: diff = difftime(time(nullptr), login_time);
			sql.query("update account set totaltime = totaltime + %d where playerid = %d;", diff, player_id);
			break;
    case CMD_FIRST:     sql.query("update account set first = first + 1 where playerid = %d;", player_id);
			first++;
			break;
    case CMD_SECOND:    sql.query("update account set second = second + 1 where playerid = %d;", player_id);
			second++;
			break;
    case CMD_THIRD:     sql.query("update account set third = third + 1 where playerid = %d;", player_id);
                        third++;
			break;
    case CMD_FOURTH:    sql.query("update account set fourth = fourth + 1 where playerid = %d;", player_id);
			fourth++;
			break;
  }
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

void cPlayer::Set_Level(usINT l)
{
  level = l;
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

long int cPlayer::First()
{
  return first;
}

long int cPlayer::Second()
{
  return second;
}

long int cPlayer::Third()
{
  return third;
}

long int cPlayer::Fourth()
{
  return fourth;
}
