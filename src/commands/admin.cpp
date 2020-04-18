#include <cstring>
#include <cstdio>
#include "../define.h"
#include "../datagrams.h"
#include "../sql.h"
#include "admin.h"

void cAdmin::Execute( cDescriptor &d, cParam &param )
{
  if (!*param.arguments) return;

  char query[80];

  int len = strlen(param.arguments);

  if ((len < MIN_HANDLE_LENGTH) || (len > MAX_HANDLE_LENGTH)) {
    d.Socket_Write("%s %s", ADMIN_NOT_FOUND, param.arguments);
    return;
  }

  sprintf(query, "select userlevel from account where handle = \"%s\";", param.arguments);
  if (sql.query(query)) {
   int level = atoi(sql.get_row(0));

   if (level >= 10) {
     d.Socket_Write("%s %s", ADMIN_ABOVE, param.arguments);
     return;
   }

   sprintf(query, "update account set userlevel = 10 where handle = \"%s\";", param.arguments);
   sql.query(query);
   d.Socket_Write("%s %s", ADMIN_SET, param.arguments);
  } else
     d.Socket_Write("%s %s", ADMIN_NOT_FOUND, param.arguments);
}
