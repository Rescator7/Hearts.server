#include <fcntl.h>
//#include <netinet/in.h>
#include <unistd.h> // close()
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <stdarg.h> // va_start(), etc.
#include <netdb.h>  // gethostbyaddr()
//#include <sys/types.h>
//#include <sys/select.h>
//#include <sys/socket.h>
#include <time.h>   // time()

#include "define.h"
#include "player.h"
#include "string.h"
#include "log.h"
#include "sql.h"
#include "comm.h"
#include "commands.h"

//#define perror(a) fprintf(stderr, "%s\n", (a));
cCommandsStack cmd;

//External variables
extern class cDescList * descriptor_list;

const char *mesg1 = "login: ";
socket_t s;

socket_t init_socket(unsigned int port)
{
 struct sockaddr_in sa;
 int opt;

 if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
   log.Write("SYSERR: Error creating socket");
   exit(1);
 }

#ifdef SO_REUSEADDR
  opt = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0){
    log.Write("SYSERR: setsockopt REUSEADDR");
    exit(1);
  }
#endif

 opt = 12 * 1024;
 if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)) < 0) {
   log.Write("SYSERR: setsockopt SNDBUF");
//   return ( -1 );
 }

#ifdef SO_LINGER
  struct linger ld;

  ld.l_onoff = 0;
  ld.l_linger = 0;
  if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld)) < 0) {
    log.Write("SYSERR: setsockopt SO_LINGER");   /* Not fatal I suppose. */
  }
#endif

 memset((char *)&sa, 0, sizeof(sa));

 sa.sin_family = AF_INET;
 sa.sin_port = htons(port);
 memset((char *)&sa.sin_addr, 0, sizeof(sa.sin_addr));

 sa.sin_addr.s_addr = htonl(INADDR_ANY); // *(get_bind_addr());

 if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
   log.Write("SYSERR: bind");
   close(s);
   exit(1);
 }

 fcntl(s, F_SETFL, O_NONBLOCK);
 listen(s, 5);

 return ( s );
}

cDescriptor::cDescriptor(socket_t mother_desc)
{
 len = sizeof(peer);
 if ((desc = accept(mother_desc, (struct sockaddr *) &peer, &len)) == INVALID_SOCKET) {
   log.Write("SYSERR: accept");
   return;
 }
// cmd = new cCommandsStack();
 player = new cPlayer;
 state = CON_LOGIN;
 last_sockread = time(0);
 fcntl(desc, F_SETFL, O_NONBLOCK);
 send(desc, mesg1, strlen(mesg1), 0);
 memset(ip, '\x0', sizeof(ip));
 strncpy(ip, inet_ntoa(peer.sin_addr), 15);
 from = gethostbyaddr((char *) &peer.sin_addr, sizeof(peer.sin_addr), AF_INET);
// printf("from: %s\n", from->h_name);
// TODO: send() splash screen here.
}

cDescriptor::~cDescriptor()
{
 close( desc );
 delete player;
}

// Not used for now
void
cDescriptor::Disconnect()
{
 state = CON_DISCONNECT;
}

bool
cDescriptor::Socket_Write( const char * format, ... )
{
 va_list args;
 char buf [10 * 1024];

 va_start(args, format);
 vsprintf(buf, format, args); // FIXME: unsafe, no buffer overflow check on this
// process_ansi( buf );
 send(desc, buf, strlen(buf), 0);
 va_end(args);

 return ( true );
}

ssize_t
cDescriptor::Socket_Read()
{
 int ret;

 memset(buffer, '\x0', sizeof(buffer));
 ret = read(desc, buffer, sizeof(buffer));
 if (ret == 0) {
   log.Write("WARNING: EOF on socket read %d (connection broken by peer)", desc);
   return ( -1 );
 }
 
 if (ret > 0) {
   last_sockread = time(0);
   skip_crlf( buffer );
   log.Write("RCVD %d (%s): %s", desc, ip, buffer);
   if (!strncmp(buffer,"shutdown",8)) exit(0);
   if (!strncmp(buffer,"loop", 4)) for(;;);
   if (!strcmp(buffer,"ÿôÿý"))
     return ( -1 ); // ctrl-c received
   return ( ret );
 }

 // TODO: maybe catch more error here.
 log.Write("SYSERR: unkown socket error");
 return ( -1 );
}

bool 
cDescriptor::IsHandleValid( const char * handle, const char * message )
{
 if (strlen(handle) > MAX_HANDLE_LENGTH) {
   Socket_Write("Your handle is too long. It should contains a maximum of %d characters.\n%s",
                MAX_HANDLE_LENGTH, message);
   return ( false );
 }
 if (strlen(handle) < MIN_HANDLE_LENGTH) {
   Socket_Write("Your handle is too short. It should contains atlease %d characters.\n%s",
                MIN_HANDLE_LENGTH, message);
   return ( false );
 }
 return ( true );
}

bool
cDescriptor::process_input()
{
 char lcBuf [SOCKET_BUFSIZE] = "";

 stolower(buffer, lcBuf);

printf("buf: '%s', lcbuf: '%s'\r\n", buffer, lcBuf);

 switch ( state ) {
   case CON_LOGIN : 
          log.Write("PROCINP: CON_LOGIN");
          if (!strcmp(lcBuf, "new")) {
            state = CON_NEW_HANDLE;
            Socket_Write("Choose your handle: ");
            break;
          }
          if (IsHandleValid( lcBuf, mesg1)) {
            if (!sql.query("select handle, password from account where handle='%s'", lcBuf)) {
              Socket_Write("\n\"%s\" is not a registered name. Try again.\n%s", lcBuf, mesg1);
              break;
            } else {
                Socket_Write("\n\"%s\" is a registered name. If it is yours, type the password.\n"
                             "If not, just hit return to try another name.\n\n"
                             "password: ", lcBuf);
                player->handle = strdup(lcBuf);
                player->password = strdup(sql.get_row(1));
                state = CON_PASSWORD;
// TODO: il semble qu'on peu envoye un code ainsi au client pour cache le password
// trouve ce code, et l'envoyer
              }
          } else break;
          break;
   case CON_PASSWORD :
          if (!*lcBuf) {
            Socket_Write(mesg1);
            state = CON_LOGIN;
            break;
          }
          if (!player->doesPasswordMatch(lcBuf)) {
            Socket_Write("\n**** Invalid password! ****\n%s", mesg1);
            state = CON_LOGIN;
            break;
          }
          if (!player->load()) {
            Socket_Write("\nFailed to load your player file.\n");
            return ( false );
          }
          state = CON_MOTD;
          goto motd;
//   case CON_RETURN :
//          state = CON_MOTD;
//          break;
   case CON_NEW_HANDLE :
          log.Write("PROCINP: CON_NEW_HANDLE");
          if (!strcmp(lcBuf, "new") ||
              !strncmp(lcBuf, "guest", 5)) {
            Socket_Write("You can't register as '%s' it's reserved.\n"
                         "Choose your handle: ", buffer);
            break;
          }
          if (descriptor_list->Find_Handle( lcBuf ) || 
              sql.query("select handle from account where handle = '%s'", lcBuf)) {
            Socket_Write("The handle '%s' is already registered.\n"
                         "Choose your handle: ", lcBuf);
            break;
          }
          if (!IsHandleValid( lcBuf, "Choose your handle: "))
            break;
          Socket_Write("Enter your real name: ");
          player->handle = strdup(buffer);
          state = CON_NEW_REALNAME;
          break;
   case CON_NEW_REALNAME :
          log.Write("PROCINP: CON_NEW_REALNAME");
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
          log.Write("PROCINP: CON_NEW_EMAIL");
          if (strlen(lcBuf) > MAX_EMAIL_LENGTH) {
            Socket_Write("Your email address is too long. It should contains a maximum of %d characters.\n"
                         "Enter your email address: ", MAX_EMAIL_LENGTH);
            break;
          }
//          Socket_Write("Thanks for registering. You should receive your password via email soon.\n");
//          return ( false );
          Socket_Write("Enter your password: ");
          player->email = strdup(buffer);
          state = CON_NEW_PASSWORD;
          break; 
   case CON_NEW_PASSWORD :
          log.Write("PROCINP: CON_NEW_PASSWORD");
          if (strlen(lcBuf) > MAX_PASSWORD_LENGTH) {
            Socket_Write("Your password is too long. It should contains a maximum of %d characters.\n"
                         "Enter your password: ", MAX_PASSWORD_LENGTH);
            break;
          } 
          if (strlen(lcBuf) < MIN_PASSWORD_LENGTH) {
            Socket_Write("Your password is too short. It should contains atlease %d characters.\n"
                         "Enter your password: ", MIN_PASSWORD_LENGTH);
            break;
          } 
          player->setPassword( buffer, true );
          Socket_Write("Confirm your password: ");
          state = CON_CONFIRM_PASSWORD;
          break;
   case CON_CONFIRM_PASSWORD :
          log.Write("PROCINP: CON_CONFIRM_PASSWORD");
          if (!player->doesPasswordMatch( buffer )) {
            Socket_Write("The passwords doesn't match. Try again.\n"
                         "Enter your password: ");
            state = CON_NEW_PASSWORD;
            break;
          }
          if (!player->save()) {
            Socket_Write("Account creation failed");
            return ( false );
          }
          state = CON_MOTD;
   case CON_MOTD :
motd:     log.Write("PROCINP: CON_MOTD");
//          Socket_Write("---- &yM&b/wO&n&GT&rD&n ----&n\n");
          state = CON_PROMPT;
          break;
   case CON_PROMPT :
          if (*buffer)
            cmd.Process_Command(this, (char *)&buffer);
          if (state != CON_DISCONNECT) 
            Send_Prompt();
          break;
   case CON_DISCONNECT :
          return ( false );
 }
 return ( true );
}

void
cDescriptor::Send_Prompt()
{
 Socket_Write("%s ", player->prompt);
}

bool
cDescriptor::Is_Connected()
{
 if (state == CON_DISCONNECT) 
   return ( false );

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
   log.Write("SYSERR: Is_Connected() select pool");
   return ( false );
 }
 if (FD_ISSET(desc, &exc_set)) {
   FD_CLR(desc, &input_set);
   FD_CLR(desc, &output_set);
   log.Write("select() exception: connection closed");
   return ( false );
 } 
 if (FD_ISSET(desc, &input_set)) {
   if (Socket_Read() == -1) 
     return ( false );
   if (isBufferValid(buffer, true)) {
     if (!process_input()) 
       return ( false );
   }
   else
     Socket_Write("Your input has been dropped it contains illegal characters.\n");
//   if (!isBufferValid(buffer, true))
//     *buffer = '\x0';
//   process_input();
 } else {
     int idleness = 0;
     if (state < CON_PROMPT) {
       if (time(0) - last_sockread > MAX_LOGON_IDLE) idleness = MAX_LOGON_IDLE;
     } else {
         if (time(0) - last_sockread > MAX_IDLE) idleness = MAX_IDLE;
       }
     if (idleness) {
       log.Write("WARNING: Idleness on socket %d (connection closed)", desc);
       if (idleness < 120)
         Socket_Write("\n\n**** Auto-logout because you were idle more than %d secondes. ****\n\n", idleness);
       else
         Socket_Write("\n\n**** Auto-logout because you were idle more than %d minutes. ****\n\n", idleness / 60);
       return ( false );
     }
   }
// if (FD_ISSET(desc, &output_set));
 return ( true );
}

cDescList::cDescList()
{
 head = NULL;
 num_elem = 0;
}

cDescList::~cDescList()
{
 Empty();
}

bool
cDescList::Add( cDescriptor *elem )
{
 struct sList * Q;

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
 log.Write("SOCKETS: %d", num_elem);
 return ( true );
}

bool
cDescList::Remove( cDescriptor *elem )
{
 struct sList * Q, * prev;

 prev = NULL;
 for (Q = head; Q; Q = Q->next) {
   if (Q->elem == elem) {
     if ( prev ) 
       prev->next = Q->next;
     else
       head = Q->next;
     delete Q->elem;
     delete Q;
     num_elem--;
     return ( true );
   }
   prev = Q;
 }
 return ( false );
}

bool
cDescList::Empty()
{
 struct sList * Q, * prev;

 Q = head;
 while ( Q ) {
   prev = Q;
   Q = Q->next;
   num_elem--;
   delete prev;
 }
 return ( true );
}

bool
cDescList::Find_Handle( const char * handle )
{
 for (struct sList * Q = head; Q; Q = Q->next) {
   if (Q->elem->player->isHandle( handle ))
     return ( true );
//     return ( Q->elem );
 }
 return ( false );
}

bool
cDescList::Send_To_All( const char * format, ... )
{
 va_list args;
 char buffer [10 * 1024];

 va_start(args, format);
 vsprintf(buffer, format, args); // FIXME: unsafe, no buffer overflow check on this
 for (struct sList * Q = head; Q; Q = Q->next)
   Q->elem->Socket_Write( (const char *)&buffer );
 va_end(args);
 return ( true );
}

bool
cDescList::Check_Conns()
{
 for (struct sList * Q = head; Q; Q = Q->next)
   if (!Q->elem->Is_Connected()) Remove( Q->elem );
 return ( true );
}
