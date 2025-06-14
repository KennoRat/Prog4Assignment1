#pragma once
#include "Event.h"

const unsigned int EVENT_PLAYER_DIED = make_sdbm_hash("PlayerDied");
const unsigned int EVENT_PLAYER_RESPAWNED = make_sdbm_hash("PlayerRespawned");
const unsigned int EVENT_POWER_UP_ACTIVATED = make_sdbm_hash("PowerUpActivated");