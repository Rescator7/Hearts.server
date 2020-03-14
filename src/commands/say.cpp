#include "../define.h"
#include "../player.h"
#include "../errors.h"
#include "say.h"

void cSay::Execute( cDescriptor &d, cParam &param )
{
 if (!*param.arguments) return;

 if (!d.player) return;

 if (!d.player->table) {
   d.Socket_Write(PLAYER_NO_TABLE);
   return;
 }

 d.player->table->Say(d, param.arguments);
}
