#include "define.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h> // setitimer()
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#include "comm.h"
#include "log.h"
#include "sql.h"

// Global Variables
socket_t mother_desc;
unsigned long server_loop = 0, cpy_server_loop = 0;
unsigned int num_table = 0;
class cDescList * descriptor_list;
bool server_shutdown = false;
bool server_shutoff = false;
class cLog log ("server.log");
class cMYSQL sql ("localhost", "hearts", "75uVmTop", "hearts");
int sigint = 0;

// External Functions
extern socket_t init_socket(unsigned int port);

void game_loop( socket_t mother_desc )
{
 fd_set rfds, input_set, output_set, exc_set, null_set;
 struct timeval tv, null_time, opt_time;
 int retval;

 /* initialize various time values */
 null_time.tv_sec = 0;
 null_time.tv_usec = 0;
 opt_time.tv_usec = OPT_USEC;
 opt_time.tv_sec = 0;

 FD_ZERO(&input_set);
 FD_ZERO(&output_set);
 FD_ZERO(&exc_set);
 FD_ZERO(&null_set);

 while (!server_shutdown) {
   server_loop++;

   /* Poll (without blocking) for new input, output, and exceptions */
   FD_SET(mother_desc, &input_set);
   if (select(mother_desc + 1, &input_set, &output_set, &exc_set, &null_time) < 0) {
     log.Write("SYSERR: game_loop (select() pool) error");
     continue;
   }
    /* If there are new connections waiting, accept them. */
   if (FD_ISSET(mother_desc, &input_set)) {
     cDescriptor *desc = new cDescriptor( mother_desc );
     descriptor_list->Add( desc );
     log.Write("New connection");
   }

   descriptor_list->Check_Conns();

   FD_ZERO(&rfds);
   FD_SET(0, &rfds);
   tv.tv_sec = 0;
   tv.tv_usec = 1000;

   retval = select(1, &rfds, NULL, NULL, &tv);
//   if (retval == -1)
//     log.Write("SYSERR: wait select()");
 }
}

void handle_signals( int signo )
{
 switch ( signo ) {
   case SIGSEGV : 
          log.Write("SYSERR: got signal SIGSEGV");
          exit(1);
   case SIGILL :
          log.Write("SYSTEM: got signal SIGILL");
          exit(1);
   case SIGBUS :
          log.Write("SYSTEM: got signal SIGBUS");
          exit(1);
   case SIGINT :
          log.Write("Server interupted");
          if (++sigint >= 2) 
            exit(0); // if we fail to shutdown on the first ctrl-c, then we're probably in endless-loop so, let's quit
          else
            server_shutdown = true;
          return;
   case SIGPIPE :
          log.Write("SYSERR: got signal SIGPIPE");
          return;
   case SIGALRM :
          if (server_loop == cpy_server_loop) {
            log.Write("SYSERR: Endless-Loop detected");
            exit(1);
          }
          cpy_server_loop = server_loop;
          return;
 }
 game_loop( mother_desc ); 
 exit(1); // to avoid on shutdown to return on a crash
}

void set_signals()
{
 struct sigaction Action;

 int SIGNALS [9] = {SIGINT, SIGILL, SIGTRAP, SIGFPE, SIGBUS, SIGSEGV,
                    SIGPIPE, SIGALRM, SIGUSR2};
 for (int i = 0; i < 9; i++) {
   Action.sa_handler = handle_signals;
/* Action.sa_mask = SA_NOCLDSTOP; */
   Action.sa_flags = 0;
   sigaction (SIGNALS[i], &Action, NULL);
 }
 log.Write("Signals handler installed");

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
 int renice = 0, server_port = 4000;

 srand(time(0));

// log = new cLog("hackers.log");
 log.Write("Heart server is booting up");

 //sql = new cMYSQL("localhost", "hackers", "hartaxiome", "hackers");
 log.Write("Connected to the MYSQL database");

 if (sql.query("select server_port, nice from config")) {
   server_port = atoi(sql.get_row(0));
   renice = atoi(sql.get_row(1)); 
 } else {
     log.Write("SYSERR: Server configuration failed");
   }
// TODO: create a class cCONFIG and put all config in it

 nice(renice); 
 log.Write("Running in nice mode (priority = %d)", renice);

 descriptor_list = new cDescList;
 log.Write("The descriptor list has been created");

 set_signals();

 mother_desc = init_socket( server_port );
 log.Write("Listening on port: %d", server_port);

 log.Write("Entering the game_loop");
 game_loop( mother_desc );

 log.Write("Heart server is turning down");

 delete descriptor_list;
// delete log;

 return ( false );
}
