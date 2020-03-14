#include "../define.h" 
#include "../player.h"
#include "../errors.h"
#include "mute.h"

void cMute::Execute( cDescriptor &d, cParam &param )
{
 if (d.player == nullptr) return;

 if (d.player->table == nullptr) {
   d.Socket_Write(PLAYER_NO_TABLE);
   return;
 }

 if (d.player->table->Muted()) return;

 if (d.player->table->PlayerSat(d))  {
   d.player->table->Mute();
   d.player->table->SendAll(TABLE_MUTED);
 } else
     d.Socket_Write(PLAYER_NOT_SAT);
}
