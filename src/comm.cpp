#include <fcntl.h>
#include <unistd.h> // close()
#include <netdb.h>  // gethostbyaddr()
#include <cstdlib> // exit()
#include <cstring> // memset()
#include <cstdarg> // va_start(), etc.
#include <cctype>  // isalnum()
#include <cerrno>

#include "define.h"
#include "config.h"
#include "player.h"
#include "string.h"
#include "log.h"
#include "sql.h"
#include "global.h"
#include "comm.h"
#include "commands.h"
#include "datagrams.h"

const char *login = "login:";
const char *password = "password:";
const char *handle = "handle:";
const char *confirm = "confirm:";

socket_t s;

socket_t init_socket(unsigned int port)
{
  struct sockaddr_in sa = {0};
  int reuse, sndbuf;

  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    Log.Write("SYSERR: socket SOCK_STREAM");
    exit(1);
  }

#ifdef SO_REUSEADDR
  reuse = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0){
    Log.Write("SYSERR: setsockopt SO_REUSEADDR");
    exit(1);
  }
  Log.Write("INFO: socket using SO_REUSEADDR");
#endif

  sndbuf = 12 * 1024;
  if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *) &sndbuf, sizeof(int)) < 0) {
    Log.Write("SYSERR: setsockopt SO_SNDBUF");
  }

#ifdef SO_LINGER
  struct linger l;

  l.l_onoff = 0;
  l.l_linger = 0;
  if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &l, sizeof(l)) < 0) {
    Log.Write("SYSERR: setsockopt SO_LINGER");
  }
  Log.Write("INFO: socket using SO_LINGER");
#endif

  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);

  if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
    Log.Write("SYSERR: bind");
    close(s);
    exit(1);
  }

  fcntl(s, F_SETFL, O_NONBLOCK);
  listen(s, SOMAXCONN);

  return s;
}

cDescriptor::cDescriptor(socket_t mother_desc)
{
  player = nullptr;
  bytes_read = 0;
  sit_time = 0;
  state = CON_LOGIN;
  len = sizeof(peer);
  if ((desc = accept(mother_desc, (struct sockaddr *) &peer, &len)) == INVALID_SOCKET) {
    Log.Write("SYSERR: accept");
    state = CON_DISCONNECT;
    return;
  }
  player = new cPlayer;
  last_sockread = time(nullptr);
  fcntl(desc, F_SETFL, O_NONBLOCK);
  memset(ip, '\x0', sizeof(ip));
  strncpy(ip, inet_ntoa(peer.sin_addr), 15);
  player->Set_Ip(ip);
  from = gethostbyaddr((char *) &peer.sin_addr, sizeof(peer.sin_addr), AF_INET);

#ifdef DEBUG
  printf("'%s' %d\r\n", ip, descriptor_list->Connection_Per_Ip((char *) &ip));
#endif

  if (descriptor_list->Connection_Per_Ip((char *)&ip) >= MAX_CONNECTION_PER_IP) {
    Socket_Write(DGE_SOCKET_MAX_CONN_IP);
    state = CON_DISCONNECT;
  }
  if (server_shutoff) {
    Socket_Write(DGE_SERVER_SHUTOFF);
    state = CON_DISCONNECT;
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
      player->table->Stand(*this, FLAG_TABLE_DISCONNECT);

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

#ifdef USE_ANSI
  process_ansi( buf );
#endif

#ifdef DEBUG
  printf("SOCKET_WRITE: '%s'\r\n", buf);
#endif

// strlen(buf) + 1 allow to send the \x0, and use it as a datagram end marker
  int bytes, left = strlen(buf) + 1, wrote = 0;

  while (left > 0) {
    bytes = send(desc, buf + wrote, left, 0);

    if (bytes == -1) { 
      Log.Write("SYSERR: socket write error: %s", strerror(errno));
      if (errno == ECONNRESET) 
        state = CON_DISCONNECT;
      break;
    }
    wrote += bytes;
    left -= bytes;

#ifdef DEBUG
    if (left > 0)
      printf("Socket_Write: left > 0\r\n");
#endif
  }

  va_end(args);

  return true;
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
      Socket_Write(DGE_SOCKET_FLOOD);
      return -1;
    }
    else
      bytes_read = 0;
  }

  if (ret > 0) {
    last_sockread = time(nullptr);
    skip_crlf( buffer );

    if ((state != CON_PASSWORD) && (state != CON_NEW_PASSWORD) && (state != CON_CONFIRM_PASSWORD))
      Log.Write("RCVD %d (%s): %s", desc, ip, buffer);

    if (!strcmp(buffer,"����")) {
      return -1; // ctrl-c received
    }
    return ret;
  }

  Log.Write("SYSERR: socket read error: %s", strerror(errno));
  return -1;
}

char *cDescriptor::IP_Adress()
{
  return (char *)&ip;
}

bool cDescriptor::IsHandleValid( const char * handle, const char * message )
{
  if (strlen(handle) > MAX_HANDLE_LENGTH) {
    Socket_Write(DGE_HANDLE_TOO_LONG);
    return false;
  }
  if (strlen(handle) < MIN_HANDLE_LENGTH) {
    Socket_Write(DGE_HANDLE_TOO_SHORT);
    return false;
  }
  for (unsigned int i=0; i<strlen(handle); i++) {
    if (!isalnum(handle[i]) && (handle[i] != '_') && (handle[i] != '-')) {
      Socket_Write(DGE_HANDLE_ILLEGAL_CHAR);
      return false;
    }
  }
  return true;
}

void cDescList::DisconnectPlayerID(unsigned int pID)
{
  for (struct sList *Q = head; Q; Q = Q->next)
    if (Q->elem->player->ID() == pID) {
      Q->elem->Socket_Write(DGE_PLAYER_RECONNECT);
      Q->elem->Disconnect();
    }
}

// if return false, the descriptor will be disconnected
bool cDescriptor::process_input()
{
  char lcBuf [SOCKET_BUFSIZE] = "";

  stolower(buffer, lcBuf);

#ifdef DEBUG
  printf("buf: '%s', lcbuf: '%s'\r\n", buffer, lcBuf);
#endif

  switch ( state ) {
    case CON_LOGIN : 
           Log.Write("PROCINP: CON_LOGIN");
           if (!strcmp(lcBuf, "new")) {
             if (sql.query("select count(*) from account where ip = '%s'", player->Ip())) {
 	       if (atoi(sql.get_row(0)) >= MAX_REGISTER_PER_IP) {
 	         Socket_Write(DGE_SOCKET_MAX_REGISTER_IP);
		 return false;
	       }
	     }

             state = CON_NEW_HANDLE;
             Socket_Write(handle);
             break;
           }
           if (IsHandleValid(lcBuf, login)) {
             if (!sql.query("select handle, password from account where handle='%s'", lcBuf)) {
               Socket_Write(DGE_HANDLE_NOT_REGISTERED);
	       return false;
             } else {
                 Socket_Write(password);
                 player->Set_Handle(strdup(lcBuf));
                 player->Set_Password(strdup(sql.get_row(1)));
                 state = CON_PASSWORD;
               }
           } else 
	       return false;
           break;
    case CON_PASSWORD :
           if (!*lcBuf) 
             return false;
          
           if (!player->doesPasswordMatch(lcBuf)) {
             Socket_Write(DGE_WRONG_PASSWORD);
             return false;
           }

	   descriptor_list->DisconnectPlayerID(player->SQL_ID());
           if (!player->load()) {
             Socket_Write(DGE_PLAYER_LOAD_FAILED);
             return false;
           }
           state = CON_MOTD;
           goto motd;
    case CON_NEW_HANDLE :
           Log.Write("PROCINP: CON_NEW_HANDLE");
           if (!strcmp(lcBuf, "new") ||
               !strncmp(lcBuf, "guest", 5)) {
             Socket_Write(DGE_HANDLE_RESERVED);
           return false;
           }

           if (descriptor_list->Find_Username( lcBuf ) || 
               sql.query("select handle from account where handle = '%s'", lcBuf)) {
             Socket_Write(DGE_HANDLE_UNAVAILABLE);
	     return false;
           }

           if (!IsHandleValid(lcBuf, handle))
	     return false;
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
//          Log.Write("PROCINP: CON_NEW_PASSWORD");
           if (strlen(lcBuf) > MAX_PASSWORD_LENGTH) {
             Socket_Write(DGE_PASSWORD_TOO_LONG);
             return false;
           } 
           if (strlen(lcBuf) < MIN_PASSWORD_LENGTH) {
             Socket_Write(DGE_PASSWORD_TOO_SHORT);
	     return false;
           } 
           player->setPassword(buffer);
           Socket_Write(confirm);
           state = CON_CONFIRM_PASSWORD;
           break;
    case CON_CONFIRM_PASSWORD :
//          Log.Write("PROCINP: CON_CONFIRM_PASSWORD");
           if (!player->doesPasswordMatch(buffer)) {
             Socket_Write(DGE_PASSWORD_DONT_MATCH);
             return false;
           }
           if (!player->save()) {
             Socket_Write("Account creation failed");
             return false;
           } else {
               descriptor_list->DisconnectPlayerID(player->SQL_ID()); // need to do this this way, to avoid to disconnect the new connection
	       player->load();                                        // load() we need the playerid now
	     }
           state = CON_MOTD;
    case CON_MOTD :
motd:     Log.Write("PROCINP: CON_MOTD");
	  Socket_Write("%s %d", DGI_PLAYER_UID, player->ID());
	  table_list->List(*this);

	  player->update(CMD_LASTLOGIN);
          state = CON_PROMPT;
          break;
    case CON_PROMPT :
          if (*buffer)
            cmd.Process_Command(this, (char *)&buffer);
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
    Log.Write("SYSERR: Is_Connected(): socket select error: %s", strerror(errno));
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
    else {
      Socket_Write(DGE_SOCKET_ILLEGAL_INPUT);
      if (state < CON_PROMPT) 
	return false;
    }
  } else {
      int idleness = 0;
      if (state < CON_PROMPT) {
        if (time(nullptr) - last_sockread > MAX_LOGON_IDLE) idleness = MAX_LOGON_IDLE;
      } else {
          if (time(nullptr) - last_sockread > config.Idleness()) idleness = config.Idleness();
        }
      if (idleness) {
        Log.Write("WARNING: Idleness on socket %d (connection closed)", desc);
        Socket_Write("%s %d", DGE_AUTO_LOGOUT_IDLENESS, idleness);
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

  if (elem->player)
    elem->player->update(CMD_TOTALTIME);

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

struct cPlayer *cDescList::Find_Username( const char *handle )
{
  struct cPlayer *player;
  for (struct sList * Q = head; Q; Q = Q->next) {
    player = Q->elem->player;

    if (player == nullptr) continue;

    if (player->isHandle( handle ))
      return player;
  }
  return nullptr;
}

bool cDescList::Send_To_All( const char * format, ... )
{
  va_list args;
  char buffer [BUF_SIZE];

  va_start(args, format);
  vsnprintf(buffer, BUF_SIZE, format, args);
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
    else
      if (server_shutoff && Q->elem->player && !Q->elem->player->table) {
        Q->elem->Socket_Write(DGE_SERVER_SHUTOFF);
        Remove(Q->elem);
      }
    Q = N;
  }

  if (server_shutoff && (head == NULL))
    server_shutdown = true;

  return true;
}

void cDescList::Table_Kick_Unplaying(struct cTable *t)
{
  struct sList *Q = head;
  struct cTable *table;
  struct cDescriptor *d;

  for (Q = head; Q; Q=Q->next) {
    d = Q->elem;
    table = d->player->table;

    if (table != t) continue;

    if (table->Chair(*d) == PLAYER_NOWHERE) {
      d->player->table = nullptr;

      d->Socket_Write("%s %d", DGI_TABLE_LEAVE, table->TableID());
    }
  }
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

  total = snprintf(buf, BUFSIZE, "%s ", DGI_TEXT);

  while (Q) {
    if (Q->elem->State() >= CON_MOTD) {
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

