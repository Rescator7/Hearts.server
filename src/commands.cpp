#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "define.h"
#include "comm.h"
#include "commands.h"
#include "string.h"
#include "includes.h"
#include "log.h"
#include "errors.h"

cCommandsStack::cCommandsStack(void)
{
 m_CountCmd = 0;

 Add( "date", new cDate() );
 Add( "exit", new cExit() );
 Add( "shutdown", new cShutdown() );
 Add( "\xff", NULL );
}

cCommandsStack::~cCommandsStack(void)
{
}

cCommandsStack *cCommandsStack::GetInstance()
{
 static cCommandsStack S_CommandsStack;
 return &S_CommandsStack;
}

void cCommandsStack::Add(const char * name, cCommand * cmd )
{
 m_commands[m_CountCmd].name = strdup(name);
 m_commands[m_CountCmd].func = cmd;
 if (m_CountCmd++ >= MAX_SERVER_CMDS) {
   log.Write("SYSERR: Maximum server commands reached.\n");
   exit(1);
 }
}

bool cCommandsStack::Process_Command(cDescriptor * d, char * buffer)
{
 char command [SOCKET_BUFSIZE];
 char matches [2048] = ""; // 2k is enough to match the entire command list
 int command_id = 0, cmp, matches_count = 0;
 unsigned short int len;
 const char * ptr; 
 char * arguments;

 skip_spaces( buffer );
 arguments = extract_cmd( buffer, command );

 if (!*command) return ( false );

 do {
   ptr = m_commands[command_id].name;
   len = strlen(ptr);
   cmp = strncmp(command, ptr, strlen(command));
   if (!cmp) {
     matches_count++;
     strncat(matches, ptr, len);
     strcat(matches, " ");
   }
   command_id++;
 } while ((cmp >= 0) && (command_id < m_CountCmd));

 if (matches_count == 0) {
//   d->Socket_Write("%s: command not found\n", buffer);
   d->Socket_Write(UNKNOWN_COMMAND);
   return ( false );
 }

 if (matches_count == 1) {
   skip_spaces( arguments );
   param.arguments = arguments;
   m_commands[command_id - 2].func->Execute(*d, param);

// TODO: the function should return a code, and should be handled here...
// change all (void) function to (int)

   return ( true );
 }

 d->Socket_Write("Ambiguous command \"%s\". Matches: %s\n", buffer, matches);

 return ( false );
}
