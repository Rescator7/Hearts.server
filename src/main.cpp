#include "define.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h> // setitimer()
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <ctime>
#include <cstring>
#include <cerrno>

#include "comm.h"
#include "log.h"
#include "sql.h"
#include "commands.h"
#include "table.h"
#include "config.h"

// Global Variables
class cCommandsStack cmd;
class cLog Log ("server.log");
class cMYSQL sql;
class cConfig config;
class cDescList *descriptor_list;
class cTabList *table_list;
socket_t mother_desc;
time_t boot_time;
unsigned long server_loop = 0, cpy_server_loop = 0;
unsigned int num_table = 0;
int sigint = 0;
bool server_shutdown = false;
bool server_shutoff = false;

// External Functions
extern socket_t init_socket(unsigned int port);

void game_loop( socket_t mother_desc )
{
  fd_set input_set, output_set, exc_set;
  struct timeval tv, null_time;

  null_time.tv_sec = 0;
  null_time.tv_usec = 0;

  while (!server_shutdown) {
    server_loop++;

    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    if (select(0, nullptr, nullptr, nullptr, &tv) < 0) {
#ifdef DEBUG
      Log.Write("SYSERR: wait select()");
#endif
    }

    FD_ZERO(&input_set);
    FD_ZERO(&output_set);
    FD_ZERO(&exc_set);

    FD_SET(mother_desc, &input_set);
    if (select(mother_desc + 1, &input_set, &output_set, &exc_set, &null_time) < 0) {
      Log.Write("SYSERR: game_loop (select() pool) error: %s", strerror(errno));
      continue;
    }

    if (FD_ISSET(mother_desc, &input_set)) {
      cDescriptor *desc = new cDescriptor( mother_desc );
      descriptor_list->Add( desc );
      Log.Write("New connection");
    }

    descriptor_list->Check_Conns();

    table_list->Remove_Expired();

    if (!table_list->Available())
      table_list->Create_Default_Table();

    table_list->Play();
  }
}

void handle_signals( int signo )
{
 switch ( signo ) {
   case SIGSEGV : 
          Log.Write("SYSERR: got signal SIGSEGV");
          exit(1);
   case SIGILL :
          Log.Write("SYSTEM: got signal SIGILL");
          exit(1);
   case SIGBUS :
          Log.Write("SYSTEM: got signal SIGBUS");
          exit(1);
   case SIGINT :
          Log.Write("Server interupted");
          if (++sigint >= 2) 
            exit(0); // if we fail to shutdown on the first ctrl-c, then we're probably in endless-loop so, let's quit
          else
            server_shutdown = true;
          return;
   case SIGPIPE :
          Log.Write("SYSERR: got signal SIGPIPE");
          return;
   case SIGFPE:
	  Log.Write("SYSERR: got signal SIGFPE");
          exit(1);
   case SIGALRM :
#ifdef DEBUG
	  Log.Write("INFO: got signal SIGALRM");
#endif
          if (server_loop == cpy_server_loop) {
            Log.Write("SYSERR: Endless-Loop detected");
            exit(1);
          }
          cpy_server_loop = server_loop;
          return;
   default: Log.Write("SYSERR: got signal (%d)", signo);
 }
 game_loop( mother_desc ); 
 Log.Write("WARNING: handle_signals exit(1)");
 exit(1); // to avoid on shutdown to return on a crash
}

void set_signals()
{
 const int num_signals = 7;
 struct sigaction Action;

 int SIGNALS [num_signals] = {SIGINT, SIGILL, SIGFPE, SIGBUS, SIGSEGV, SIGPIPE, SIGALRM};
 for (int i = 0; i < num_signals; i++) {
   Action.sa_handler = handle_signals;
/* Action.sa_mask = SA_NOCLDSTOP; */
   Action.sa_flags = 0;
   sigaction (SIGNALS[i], &Action, NULL);
 }
 Log.Write("Signals handler installed");

 struct itimerval itime;

// TODO: maybe use a config database value to set endless loop ( 2 mins )
// 2 mins seem fine, it means it could take upto 2*2 mins to detect the endless loop
 itime.it_interval.tv_sec = 60 * 2;
 itime.it_interval.tv_usec = 0;
 itime.it_value.tv_sec = 60 * 2;
 itime.it_value.tv_usec = 0;
 setitimer(ITIMER_REAL, &itime, NULL);
}

int main() 
{
 boot_time = time(nullptr);

 srand(boot_time);

 Log.Write("Heart server version: %s is booting up", VERSION);

 Log.Write("Connected to the MYSQL database");

 if (nice(config.Nice()) == -1)
   Log.Write("Setting nice mode error: %s", strerror(errno));
 else 
   Log.Write("Running in nice mode (priority = %d)", config.Nice());

 descriptor_list = new cDescList;
 Log.Write("The descriptor list has been created");

 table_list = new cTabList;
 Log.Write("The table list has been created");

 set_signals();

 mother_desc = init_socket( config.Port() );
 Log.Write("Listening on port: %d", config.Port());

 Log.Write("Entering the game_loop");
 game_loop( mother_desc );

 Log.Write("Heart server is shuting down");

 delete descriptor_list;
 delete table_list;

 return false;
}
