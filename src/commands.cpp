#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "define.h"
#include "comm.h"
#include "commands.h"
#include "string.h"
#include "includes.h"
#include "log.h"
#include "datagrams.h"
#include "player.h"

const int CMD_NOT_FOUND = -1;
const int CMD_MATCHES   = -2;

cCommandsStack::cCommandsStack(void)
{
  m_CountCmd = 0;

  Add( "admin",    DGI_HELP_ADMIN,    LVL_SUPERUSER, new cAdmin() );
  Add( "announce", DGI_HELP_ANNOUNCE, LVL_ADMIN,     new cAnnounce() );
  Add( "date",     DGI_HELP_DATE,     LVL_GUEST,     new cDate() );
  Add( "exit",     DGI_HELP_EXIT,     LVL_GUEST,     new cExit() );
  Add( "help",     DGI_HELP_HELP,     LVL_GUEST,     new cHelp() );
  Add( "join",     DGI_HELP_JOIN,     LVL_GUEST,     new cJoin() );
  Add( "leave",    DGI_HELP_LEAVE,    LVL_GUEST,     new cLeave() );
  Add( "moon",     DGI_HELP_MOON,     LVL_GUEST,     new cMoon() );
  Add( "mute",     DGI_HELP_MUTE,     LVL_GUEST,     new cMute() );
  Add( "new",      DGI_HELP_NEW,      LVL_GUEST,     new cNew() );
  Add( "pass",     DGI_HELP_PASS,     LVL_GUEST,     new cPass() );
  Add( "password", DGI_HELP_PASSWORD, LVL_GUEST,     new cPassword() );
  Add( "pause",    DGI_HELP_PAUSE,    LVL_SUPERUSER, new cPause() );
  Add( "play",     DGI_HELP_PLAY,     LVL_GUEST,     new cPlay() );
  Add( "say",      DGI_HELP_SAY,      LVL_GUEST,     new cSay() );
  Add( "set",      DGI_HELP_SET,      LVL_ADMIN,     new cSet() );
  Add( "shutdown", DGI_HELP_SHUTDOWN, LVL_ADMIN,     new cShutdown() );
  Add( "shutoff",  DGI_HELP_SHUTOFF,  LVL_ADMIN,     new cShutoff() );
  Add( "sit",      DGI_HELP_SIT,      LVL_GUEST,     new cSit() );
  Add( "stats",    DGI_HELP_STATS,    LVL_GUEST,     new cStats() );
  Add( "tables",   DGI_HELP_TABLES,   LVL_GUEST,     new cTables() );
  Add( "test",     DGI_HELP_TEST,     LVL_ADMIN,     new cTest() );
  Add( "uptime",   DGI_HELP_UPTIME,   LVL_GUEST,     new cUptime() );
  Add( "who",      DGI_HELP_WHO,      LVL_GUEST,     new cWho() );
  Add( "\xff",     nullptr,           0,             nullptr );
}

cCommandsStack::~cCommandsStack(void)
{
  for (int i=0; i<m_CountCmd - 1; i++) {
    free(m_commands[i].name);
    delete m_commands[i].func;
  }
}

cCommandsStack *cCommandsStack::GetInstance()
{
 static cCommandsStack S_CommandsStack;
 return &S_CommandsStack;
}

void cCommandsStack::Add(const char *name, const char *help, unsigned int level, cCommand *cmd )
{
  m_commands[m_CountCmd].name = strdup(name);
  m_commands[m_CountCmd].help = help;
  m_commands[m_CountCmd].level = level;
  m_commands[m_CountCmd].func = cmd;
  if (m_CountCmd++ >= MAX_SERVER_CMDS) {
    Log.Write("SYSERR: Maximum server commands reached.\n");
    exit(1);
  }
}

int cCommandsStack::Find(const char *cmd, int level, char *matches)
{
  int found = 0, cmd_id = 0, matches_count = 0, total = 0;  
  unsigned int len;
  const char *ptr;

  do {
    ptr = m_commands[cmd_id].name;
    len = strlen(ptr);
    if (m_commands[cmd_id].level <= level) {
      if (!strncmp(cmd, ptr, strlen(cmd))) {
        found = cmd_id;
        if (len == strlen(cmd)) {
          matches_count = 1;
	  break;
        }
        matches_count++;
        total += snprintf(matches + total, BUF_SIZE - total, "%s ", ptr);
      }
    }
    cmd_id++;
  } while (cmd_id < m_CountCmd - 1);

  if (matches_count == 0)
    return CMD_NOT_FOUND;

  if (matches_count == 1)
    return found;

  return CMD_MATCHES;
}

void cCommandsStack::Help(cDescriptor &d, const char *cmd)
{
  char matches[BUF_SIZE];
  int found;

  found = Find(cmd, d.player->Level(), (char *)&matches);

  if (found == CMD_NOT_FOUND) {
    d.Socket_Write(UNKNOWN_COMMAND);
    return;
  }

  if (found == CMD_MATCHES) {
    d.Socket_Write("%s %s", AMBIGOUS_COMMAND, matches);
    return;
  }

  d.Socket_Write("%s", m_commands[found].help);
}

bool cCommandsStack::Process_Command(cDescriptor *d, char *buffer)
{
  char command [SOCKET_BUFSIZE];
  char matches [BUF_SIZE] = ""; // 2k is enough to match the entire command list
  char *arguments;

  skip_spaces( buffer );
  arguments = extract_cmd( buffer, command );

  if (!*command) return false;

  int found;

  found = Find(command, d->player->Level(), (char *)&matches);

  if (found == CMD_NOT_FOUND) {
    d->Socket_Write(UNKNOWN_COMMAND);
    return false;
  }

  if (found == CMD_MATCHES) {
    d->Socket_Write("%s %s", AMBIGOUS_COMMAND, matches);
    return false;
  }

  skip_spaces( arguments );
  param.arguments = arguments;
  m_commands[found].func->Execute(*d, param);

  return true;
}
