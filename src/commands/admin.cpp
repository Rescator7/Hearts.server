#include <cstring>
#include <cstdio>
#include "../define.h"
#include "../global.h"
#include "../datagrams.h"
#include "../sql.h"
#include "../player.h"
#include "admin.h"

void cAdmin::Execute( cDescriptor &d, cParam &param )
{
  if (!*param.arguments) return;

  char query[QUERY_SIZE];

  int len = strlen(param.arguments);

  if ((len < MIN_HANDLE_LENGTH) || (len > MAX_HANDLE_LENGTH)) {
    d.Socket_Write("%s %s", ADMIN_NOT_FOUND, param.arguments);
    return;
  }

  snprintf(query, QUERY_SIZE, "select userlevel from account where handle = \"%s\";", param.arguments);
  if (sql.query(query)) {
   int level = atoi(sql.get_row(0));

   if (level >= LVL_ADMIN) {
     d.Socket_Write("%s %s", ADMIN_ABOVE, param.arguments);
     return;
   }

   snprintf(query, QUERY_SIZE, "update account set userlevel = %d where handle = \"%s\";", LVL_ADMIN, param.arguments);
   sql.query(query);

   struct cPlayer *player;

   player = descriptor_list->Find_Username(param.arguments);
   if (player != nullptr)
     player->Set_Level(LVL_ADMIN);

   d.Socket_Write("%s %s", ADMIN_SET, param.arguments);
  } else
     d.Socket_Write("%s %s", ADMIN_NOT_FOUND, param.arguments);
}
