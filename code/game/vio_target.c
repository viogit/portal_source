#include "g_local.h"
#ifdef	VIOL_VM

/*
===========================================================================
Game Target
===========================================================================
*/

// xDiloc - portalgun
void G_removePortals(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	int	i;
	int	clientfound = -1;

	if (!activator->client) {
		return;
	}

	// find the client
	for(i = 0; i < MAX_CLIENTS; i++) {
		if (activator->client == &level.clients[i]) {
			clientfound = i;
			break;
		}
	}

	if (clientfound == -1) {
		G_Printf("couldn't find client\n");
		return;
	}

	G_Portal_Clear(activator, PORTAL_ALL);
}

void SP_target_removePortal(gentity_t *ent) {
	ent->use = G_removePortals;
}

// xDiloc - defrag
void G_DefragStartTimer(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	int	i;
	int	clientfound = -1;

	if (!activator->client) {
		return;
	}

	// find the client
	for(i = 0; i < MAX_CLIENTS; i++) {
		if (activator->client == &level.clients[i]) {
			clientfound = i;
			break;
		}
	}

	if (clientfound == -1) {
		G_Printf("couldn't find client\n");
		return;
	}

	trap_SendServerCommand(activator-g_entities, va("cp \"^0GO HUHU\""));
	level.player_defragstarttime[clientfound] = trap_Milliseconds();
}

void G_DefragStopTimer(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	int	i;
	int	clientfound = -1;
	int	timesec, timemin, timemsec;

	if (!activator->client) {
		return;
	}

	// find the client
	for(i = 0; i < MAX_CLIENTS; i++) {
		if (activator->client == &level.clients[i]) {
			clientfound = i;
			break;
		}
	}

	if (clientfound == -1) {
		G_Printf("couldn't find client\n");
		return;
	}

	if (!level.player_defragstarttime[clientfound]) {
		return;
	}

	timemsec = trap_Milliseconds() - level.player_defragstarttime[clientfound];
	timesec = timemsec / 1000;
	timemsec -= timesec * 1000;
	timemin = timesec / 60;
	timesec -= timemin * 60;
	trap_SendServerCommand(-1, va("print \""PREFIX"%s finish %02i:%02i:%03i\n\"", activator->client->pers.netname, timemin, timesec, timemsec));
	trap_SendServerCommand(activator-g_entities, va("cp \"^0ZALUPA\""));

	//reset time
	level.player_defragstarttime[clientfound] = 0;
}

void G_DefragCheckpoint(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	int	i;
	int	clientfound = -1;
	int	timesec, timemin, timemsec;

	if (!activator->client) {
		return;
	}

	// find the client
	for(i = 0; i < MAX_CLIENTS; i++) {
		if (activator->client == &level.clients[i]) {
			clientfound = i;
			break;
		}
	}

	if (clientfound == -1) {
		G_Printf("couldn't find client\n");
		return;
	}

	if (!level.player_defragstarttime[clientfound]) {
		return;
	}

	timemsec = trap_Milliseconds() - level.player_defragstarttime[clientfound];
	timesec = timemsec / 1000;
	timemsec -= timesec * 1000;
	timemin = timesec / 60;
	timesec -= timemin * 60;
	trap_SendServerCommand(activator-g_entities, va("cp \"^0%02i^7:^0%02i^7:^0%03i\"", timemin, timesec, timemsec));
}

void SP_target_startTimer(gentity_t *ent) {
	ent->use = G_DefragStartTimer;
}

void SP_target_stopTimer(gentity_t *ent) {
	ent->use = G_DefragStopTimer;
}

void SP_target_checkpoint(gentity_t *ent) {
	ent->use = G_DefragCheckpoint;
}

#endif

