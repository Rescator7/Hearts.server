#include <fcntl.h>
#include <unistd.h> // close()
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <stdarg.h> // va_start(), etc.
#include <netdb.h>  // gethostbyaddr()
#include <time.h>   // time()
#include <ctype.h>  // isalnum()

#include "define.h"
#include "player.h"
#include "string.h"
#include "log.h"
#include "sql.h"
#include "global.h"
#include "comm.h"
#include "commands.h"
#include "errors.h"

//#define perror(a) fprintf(stderr, "%s\n", (a));
cCommandsStack cmd;

const char *prompt = "%%";
const char *login = "login:";
const char *password = "password:";
const char *handle = "handle:";
const char *confirm = "confirm:";

socket_t s;

socket_t init_socket(unsigned int port)
{
 struct sockaddr_in sa;
 int opt;

 if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
   Log.Write("SYSERR: Error creating socket");
   exit(1);
 }

#ifdef SO_REUSEADDR
  opt = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0){
    Log.Write("SYSERR: setsockopt REUSEADDR");
    exit(1);
  }
#endif

 opt = 12 * 1024;
 if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)) < 0) {
   Log.Write("SYSERR: setsockopt SNDBUF");
//   return ( -1 );
 }

#ifdef SO_LINGER
  struct linger ld;

  ld.l_onoff = 0;
  ld.l_linger = 0;
  if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld)) < 0) {
    Log.Write("SYSERR: setsockopt SO_LINGER");
  }
#endif

 memset((char *)&sa, 0, sizeof(sa));

 sa.sin_family = AF_INET;
 sa.sin_port = htons(port);
 memset((char *)&sa.sin_addr, 0, sizeof(sa.sin_addr));

 sa.sin_addr.s_addr = htonl(INADDR_ANY); // *(get_bind_addr());

 if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
   Log.Write("SYSERR: bind");
   close(s);
   exit(1);
 }

 fcntl(s, F_SETFL, O_NONBLOCK);
 listen(s, 5);

 return ( s );
}

cDescriptor::cDescriptor(socket_t mother_desc)
{
 player = NULL;
 bytes_read = 0;
 sit_time = 0;
 state = CON_LOGIN;
 len = sizeof(peer);
 if ((desc = accept(mother_desc, (struct sockaddr *) &peer, &len)) == INVALID_SOCKET) {
   Log.Write("SYSERR: accept");
   return;
 }
 player = new cPlayer;
 last_sockread = time(nullptr);
 fcntl(desc, F_SETFL, O_NONBLOCK);
 memset(ip, '\x0', sizeof(ip));
 strncpy(ip, inet_ntoa(peer.sin_addr), 15);
 player->Set_Ip(ip);
 from = gethostbyaddr((char *) &peer.sin_addr, sizeof(peer.sin_addr), AF_INET);
 printf("'%s' %d\r\n", ip, descriptor_list->Connection_Per_Ip((char *) &ip));
 if (descriptor_list->Connection_Per_Ip((char *)&ip) >= MAX_CONNECTION_PER_IP) {
   Socket_Write(SOCKET_MAX_CONN_IP);
   Disconnect();
 }
 if (server_shutoff) {
   Socket_Write(SERVER_SHUTOFF);
   Disconnect();
 }
 if (state == CON_LOGIN)
   Socket_Write(login);
}

cDescriptor::~cDescriptor()
{
 close( desc );
 if (player) {

   // this should be move into player destructor?
   // class player don't have a descriptor, and table->Stand() use a descriptor
   // maybe, modify it to use playerid instead...
   if (player->table)
     player->table->Stand(*this);

   delete player;
 }
}

void cDescriptor::Disconnect()
{
 state = CON_DISCONNECT;
}

bool cDescriptor::Socket_Write( const char * format, ... )
{
 va_list args;
 char buf [BUF_SIZE];

 va_start(args, format);
 vsnprintf(buf, BUF_SIZE, format, args);
// process_ansi( buf );
 printf("SOCKET_WRITE: '%s'\r\n", buf);
 send(desc, buf, strlen(buf)+1, 0); // +1 allow to send the \x0, and use it as a datagram end marker
 va_end(args);

 return ( true );
}

ssize_t cDescriptor::Socket_Read()
{
 int ret;

 memset(buffer, '\x0', sizeof(buffer));
 ret = read(desc, buffer, sizeof(buffer));
 if (ret == 0) {
   Log.Write("WARNING: EOF on socket read %d (connection broken by peer)", desc);
   return -1;
 }
 
 if (ret >= 1)
   bytes_read += ret;

 if (difftime(time(nullptr), last_sockread) >= 1) {
   if (bytes_read >= SOCKET_MAX_READ_BYTES) {
     Socket_Write(SOCKET_FLOOD);
     return -1;
   }
   else
     bytes_read = 0;
 }

 if (ret > 0) {
   last_sockread = time(nullptr);
   skip_crlf( buffer );
   Log.Write("RCVD %d (%s): %s", desc, ip, buffer);
   if (!strcmp(buffer,"ÿôÿý")) {
     return -1; // ctrl-c received
   }
   return ret;
 }

 // TODO: maybe catch more error here.
 Log.Write("SYSERR: unkown socket error");
 return -1;
}

char *cDescriptor::IP_Adress()
{
  return (char *)&ip;
}

bool cDescriptor::IsHandleValid( const char * handle, const char * message )
{
 if (strlen(handle) > MAX_HANDLE_LENGTH) {
   Socket_Write(HANDLE_TOO_LONG);
   return false;
 }
 if (strlen(handle) < MIN_HANDLE_LENGTH) {
   Socket_Write(HANDLE_TOO_SHORT);
   return false;
 }
 for (unsigned int i=0; i<strlen(handle); i++) {
   if (!isalnum(handle[i]) && (handle[i] != '_') && (handle[i] != '-')) {
     Socket_Write(HANDLE_ILLEGAL_CHAR);
     return false;
   }
 }
 return true;
}

void cDescList::DisconnectPlayerID(unsigned int pID)
{
 for (struct sList *Q = head; Q; Q = Q->next)
   if (Q->elem->player->ID() == pID) {
     Q->elem->Socket_Write(PLAYER_RECONNECT);
     Q->elem->Disconnect();
   }
}

bool cDescriptor::process_input()
{
 char lcBuf [SOCKET_BUFSIZE] = "";

 stolower(buffer, lcBuf);

 printf("buf: '%s', lcbuf: '%s'\r\n", buffer, lcBuf);

 switch ( state ) {
   case CON_LOGIN : 
          Log.Write("PROCINP: CON_LOGIN");
          if (!strcmp(lcBuf, "new")) {
	    if (sql.query("select count(*) from account where ip = '%s'", player->Ip())) {
	      if (atoi(sql.get_row(0)) >= MAX_REGISTER_PER_IP) {
		Socket_Write(SOCKET_MAX_REGISTER_IP);
	        Disconnect();
		break;
	      }
	    }

            state = CON_NEW_HANDLE;
            Socket_Write(handle);
            break;
          }
          if (IsHandleValid( lcBuf, login)) {
            if (!sql.query("select handle, password from account where handle='%s'", lcBuf)) {
              Socket_Write(HANDLE_NOT_REGISTERED);
	      Disconnect();
              break;
            } else {
		Socket_Write(password);
                player->Set_Handle(strdup(lcBuf));
                player->Set_Password(strdup(sql.get_row(1)));
                state = CON_PASSWORD;
              }
          } else 
	      Disconnect();
          break;
   case CON_PASSWORD :
          if (!*lcBuf) {
//            Socket_Write(login);
//            state = CON_LOGIN;
            Disconnect();
            break;
          }
          if (!player->doesPasswordMatch(lcBuf)) {
            Socket_Write(WRONG_PASSWORD);
	    Disconnect();
            break;
          }
	  descriptor_list->DisconnectPlayerID(player->SQL_ID());
          if (!player->load()) {
            Socket_Write(PLAYER_LOAD_FAILED);
	    Disconnect();
            return ( false );
          }
          state = CON_MOTD;
          goto motd;
//   case CON_RETURN :
//          state = CON_MOTD;
//          break;
   case CON_NEW_HANDLE :
          Log.Write("PROCINP: CON_NEW_HANDLE");
          if (!strcmp(lcBuf, "new") ||
              !strncmp(lcBuf, "guest", 5)) {
            Socket_Write(HANDLE_RESERVED);
	    Disconnect();
            break;
          }
          if (descriptor_list->Find_Handle( lcBuf ) || 
              sql.query("select handle from account where handle = '%s'", lcBuf)) {
            Socket_Write(HANDLE_UNAVAILABLE);
	    Disconnect();
            break;
          }
          if (!IsHandleValid( lcBuf, handle)) {
	    Disconnect();
            break;
	  }
          player->Set_Handle(strdup(buffer));
	  Socket_Write(password);
	  state = CON_NEW_PASSWORD;
//          state = CON_NEW_REALNAME;
          break;
/*
   case CON_NEW_REALNAME :
          Log.Write("PROCINP: CON_NEW_REALNAME");
          if (strlen(lcBuf) > MAX_REALNAME_LENGTH) {
            Socket_Write("Your real name is too long. It should contains a maximum of %d characters.\n"
                         "Enter your real name: ", MAX_REALNAME_LENGTH);
            break;
          }
          Socket_Write("Enter your email address: ");
          player->realname = strdup(buffer);
          state = CON_NEW_EMAIL;
          break;
   case CON_NEW_EMAIL :
          Log.Write("PROCINP: CON_NEW_EMAIL");
          if (strlen(lcBuf) > MAX_EMAIL_LENGTH) {
            Socket_Write("Your email address is too long. It should contains a maximum of %d characters.\n"
                         "Enter your email address: ", MAX_EMAIL_LENGTH);
            break;
          }
          Socket_Write("Thanks for registering. You should receive your password via email soon.\n");
          return ( false );
          Socket_Write("Enter your password: ");
          player->email = strdup(buffer);
          state = CON_NEW_PASSWORD;
          break; 
*/
   case CON_NEW_PASSWORD :
          Log.Write("PROCINP: CON_NEW_PASSWORD");
          if (strlen(lcBuf) > MAX_PASSWORD_LENGTH) {
            Socket_Write(PASSWORD_TOO_LONG);
	    Disconnect();
            break;
          } 
          if (strlen(lcBuf) < MIN_PASSWORD_LENGTH) {
            Socket_Write(PASSWORD_TOO_SHORT);
	    Disconnect();
            break;
          } 
          player->setPassword( buffer, true );
          Socket_Write(confirm);
          state = CON_CONFIRM_PASSWORD;
          break;
   case CON_CONFIRM_PASSWORD :
          Log.Write("PROCINP: CON_CONFIRM_PASSWORD");
          if (!player->doesPasswordMatch( buffer )) {
            Socket_Write(PASSWORD_DONT_MATCH);
	    Disconnect();
            break;
          }
          if (!player->save()) {
            Socket_Write("Account creation failed");
            return ( false );
          } else {
	      descriptor_list->DisconnectPlayerID(player->SQL_ID()); // need to do this this way, to avoid to disconnect the new connection
	      player->load();                                        // load() we need the playerid now
	    }
          state = CON_MOTD;
   case CON_MOTD :
motd:     Log.Write("PROCINP: CON_MOTD");
	  Socket_Write("%s %d", PLAYER_UID, player->ID());
	  table_list->List(*this);
          state = CON_PROMPT;
          break;
   case CON_PROMPT :
          if (*buffer)
            cmd.Process_Command(this, (char *)&buffer);
          if (state != CON_DISCONNECT) 
            Send_Prompt();
          break;
   case CON_DISCONNECT :
          return false;
 }
  return true;
}

time_t cDescriptor::Get_Sit_Time()
{
  return sit_time;
}

int cDescriptor::State()
{
  return state;
}

void cDescriptor::Set_Sit_Time(time_t t)
{
  sit_time = t;
}

// unused for now
void cDescriptor::Send_Prompt()
{
// Socket_Write("%s ", player->prompt);
// don't send until i fix the system, it mess ECODE
// Socket_Write(prompt);
}

bool cDescriptor::Is_Connected()
{
  if (state == CON_DISCONNECT) 
    return false;

  null_time.tv_sec = 0;
  null_time.tv_usec = 0;
  FD_ZERO(&input_set);
  FD_ZERO(&output_set);
  FD_ZERO(&exc_set);
  FD_ZERO(&null_set);

  FD_SET(desc, &input_set);
  FD_SET(desc, &output_set);
  FD_SET(desc, &exc_set);
  if (select(desc + 1, &input_set, &output_set, &exc_set, &null_time) < 0) {
    Log.Write("SYSERR: Is_Connected() select pool");
    return false;
  }
  if (FD_ISSET(desc, &exc_set)) {
    FD_CLR(desc, &input_set);
    FD_CLR(desc, &output_set);
    Log.Write("select() exception: connection closed");
    return false;
  } 
  if (FD_ISSET(desc, &input_set)) {
    if (Socket_Read() == -1) {
      return false;
    }
    if (isBufferValid(buffer, true)) {
      if (!process_input()) {
        return false;
      }
    }
    else
      Socket_Write("Your input has been dropped it contains illegal characters.\n");
  } else {
      int idleness = 0;
      if (state < CON_PROMPT) {
        if (time(nullptr) - last_sockread > MAX_LOGON_IDLE) idleness = MAX_LOGON_IDLE;
      } else {
          if (time(nullptr) - last_sockread > MAX_IDLE) idleness = MAX_IDLE;
        }
      if (idleness) {
        Log.Write("WARNING: Idleness on socket %d (connection closed)", desc);
        Socket_Write("%s %d", AUTO_LOGOUT_IDLENESS, idleness);
        return false;
      }
    }
// if (FD_ISSET(desc, &output_set));
  return true;
}

cDescList::cDescList()
{
  num_elem = 0;
  head = nullptr;
}

cDescList::~cDescList()
{
  Empty();
}

bool cDescList::Add( cDescriptor *elem )
{
  struct sList *Q;

  Q = new (struct sList); 
  Q->elem = elem;

  if (head == NULL) {
    Q->next = NULL;
    head = Q;
  } else {
      Q->next = head;
      head = Q;
    }

  num_elem++;
  Log.Write("SOCKETS: %d", num_elem);
  return true;
}

bool cDescList::Remove( cDescriptor *elem )
{
  struct sList *Q, *prev = NULL;

  for (Q = head; Q; Q = Q->next) {
    if (Q->elem == elem) {
      if ( prev ) 
        prev->next = Q->next;
      else
        head = Q->next;
      delete Q->elem;
      delete Q;
      num_elem--;
      return true;
    }
    prev = Q;
  }
  return false;
}

bool cDescList::Empty()
{
  struct sList *Q, *prev;

  Q = head;
  while ( Q ) {
    prev = Q;
    Q = Q->next;
    num_elem--;
    delete prev;
  }
  return true;
}

bool cDescList::Find_Handle( const char * handle )
{
  for (struct sList * Q = head; Q; Q = Q->next) {
    if (Q->elem->player->isHandle( handle ))
      return true;
  }
  return false;
}

bool cDescList::Send_To_All( const char * format, ... )
{
  va_list args;
  char buffer [BUF_SIZE];

  va_start(args, format);
  vsnprintf(buffer, BUF_SIZE, format, args); // FIXME: unsafe, no buffer overflow check on this
  for (struct sList * Q = head; Q; Q = Q->next)
    if ((Q->elem->State() == CON_PROMPT) && Q->elem->Socket_Write((const char *)&buffer));
  va_end(args);
  return true;
}

unsigned int cDescList::Connection_Per_Ip(char *ip)
{
  int cpt = 0;
  struct sList *Q = head;

  while (Q) {
    if (!strcmp(Q->elem->IP_Adress(), ip))
      cpt++; 
    Q = Q->next;
  }
  return cpt;
}

bool cDescList::Check_Conns()
{
  struct sList *Q = head, *N = NULL;

  while (Q) {
    N = Q->next;
    if (!Q->elem->Is_Connected())
      Remove(Q->elem);
    Q = N;
  }

  if (server_shutoff && (head == NULL))
    server_shutdown = true;

  return true;
}

void cDescList::ULink_TableID(unsigned int id)
{
  struct sList *Q = head;

  while (Q) {
    if (Q->elem->player)
      Q->elem->player->ULink_Table(id);
    Q = Q->next;
  } 
}

void cDescList::Who(cDescriptor &desc)
{
  const int BUFSIZE = 10240;

  char buf[BUFSIZE];

  struct sList *Q = head;
  int len, total;

  total = snprintf(buf, BUFSIZE, "%s ", SERVER_WHO);

  while (Q) {
    if (Q->elem->player) {
      len = snprintf(buf + total, BUFSIZE - total, "%s ", Q->elem->player->Handle());      
      total += len;
      if (total >= BUFSIZE) {
	sprintf(buf + BUFSIZE - 17, "*** OVERFLOW ***");
	break;
      }
    }
    Q = Q->next;
  }
  desc.Socket_Write(buf);
}

