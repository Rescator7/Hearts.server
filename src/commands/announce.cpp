#include "../define.h"
#include "../global.h"
#include "../player.h"
#include "../datagrams.h"
#include "announce.h"

void cAnnounce::Execute( cDescriptor &d, cParam &param )
{
  if (!*param.arguments) return;

  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  descriptor_list->Send_To_All("%s [%s]: %s", SERVER_ANNOUNCE, player->Handle(), param.arguments);
}
