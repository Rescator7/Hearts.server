#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "define.h"
#include "comm.h"
#include "commands.h"
#include "string.h"
#include "includes.h"
#include "log.h"
#include "datagrams.h"
#include "player.h"

cCommandsStack::cCommandsStack(void)
{
 m_CountCmd = 0;

 Add( "date",     LVL_GUEST, new cDate() );
 Add( "exit",     LVL_GUEST, new cExit() );
 Add( "join",     LVL_GUEST, new cJoin() );
 Add( "leave",    LVL_GUEST, new cLeave() );
 Add( "moon",     LVL_GUEST, new cMoon() );
 Add( "mute",     LVL_GUEST, new cMute() );
 Add( "new",      LVL_GUEST, new cNew() );
 Add( "pass",     LVL_GUEST, new cPass() );
 Add( "pause",    LVL_GUEST, new cPause() ); // set to level admin
 Add( "play",     LVL_GUEST, new cPlay() );
 Add( "say",      LVL_GUEST, new cSay() );
 Add( "set",      LVL_GUEST, new cSet() );   // set to level admin
 Add( "shutdown", LVL_ADMIN, new cShutdown() );
 Add( "shutoff",  LVL_ADMIN, new cShutoff() );
 Add( "sit",      LVL_GUEST, new cSit() );
 Add( "test",     LVL_GUEST, new cTest() );  // set to level admin
 Add( "who",      LVL_GUEST, new cWho() );
 Add( "\xff",     0,         nullptr );
}

cCommandsStack::~cCommandsStack(void)
{
}

cCommandsStack *cCommandsStack::GetInstance()
{
 static cCommandsStack S_CommandsStack;
 return &S_CommandsStack;
}

void cCommandsStack::Add(const char * name, unsigned int level, cCommand * cmd )
{
 m_commands[m_CountCmd].name = strdup(name);
 m_commands[m_CountCmd].level = level;
 m_commands[m_CountCmd].func = cmd;
 if (m_CountCmd++ >= MAX_SERVER_CMDS) {
   Log.Write("SYSERR: Maximum server commands reached.\n");
   exit(1);
 }
}

bool cCommandsStack::Process_Command(cDescriptor *d, char *buffer)
{
 char command [SOCKET_BUFSIZE];
 char matches [2048] = ""; // 2k is enough to match the entire command list
 int first = 0, command_id = 0, cmp, matches_count = 0;
 unsigned short int len;
 const char * ptr; 
 char * arguments;

 skip_spaces( buffer );
 arguments = extract_cmd( buffer, command );

 if (!*command) return false;

 do {
   ptr = m_commands[command_id].name;
   len = strlen(ptr);
   if (m_commands[command_id].level <= d->player->Level()) {
     cmp = strncmp(command, ptr, strlen(command));
     if (!cmp)  {
       matches_count++;
       strncat(matches, ptr, len);
       strcat(matches, " ");
       first = command_id;
     }
   }
   command_id++;
 } while (command_id < m_CountCmd);

 if (matches_count == 0) {
   d->Socket_Write(UNKNOWN_COMMAND);
   return false;
 }

 if (matches_count == 1) {
   skip_spaces( arguments );
   param.arguments = arguments;
   if (m_commands[first].level <= d->player->Level()) {
     m_commands[first].func->Execute(*d, param);
     return true;
   } else {
       d->Socket_Write(UNKNOWN_COMMAND);
       return false;
     }
 }

 d->Socket_Write("%s %s", AMBIGOUS_COMMAND, matches);

 return false;
}
