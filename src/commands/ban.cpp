#include <cstring>
#include <cstdio>
#include "../define.h"
#include "../global.h"
#include "../datagrams.h"
#include "../sql.h"
#include "../player.h"
#include "ban.h"

void cBan::Execute( cDescriptor &d, cParam &param )
{
  if (!*param.arguments) return;

  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  char query[QUERY_SIZE];

  int len = strlen(param.arguments);

  if ((len < MIN_HANDLE_LENGTH) || (len > MAX_HANDLE_LENGTH)) {
    d.Socket_Write("%s %s", DGE_PLAYER_NOT_FOUND, param.arguments);
    return;
  }

  snprintf(query, QUERY_SIZE, "select playerid, userlevel from account where handle = \"%s\";", param.arguments);
  if (sql.query(query)) {
   int playerid = atoi(sql.get_row(0));
   unsigned int level = atoi(sql.get_row(1));

   if (level >= player->Level()) {
     d.Socket_Write("%s %s", DGE_ADMIN_ABOVE, param.arguments);
     return;
   }

   snprintf(query, QUERY_SIZE, "update account set password = NULL, uuid = NULL where playerid = \"%d\";", playerid);
   sql.query(query);

   descriptor_list->DisconnectPlayerID(playerid);
   d.Socket_Write("%s %s", DGI_PLAYER_BANNED, param.arguments);
  } else
     d.Socket_Write("%s %s", DGE_PLAYER_NOT_FOUND, param.arguments);
}
