#include "g_local.h"
#ifdef	VIOL_VM

/*
===========================================================================
Portals Logic
===========================================================================
*/

// How big the bbox for the portals should be.
#define BBOX 15.0f

// How far to trace out.
#define SCAN_RANGE 50.0f

/*
===============
Vio_TeleportMissle
===============
*/
void Vio_TeleportMissle(gentity_t *missle, vec3_t origin, vec3_t angles) {
	float	speed;
	vec3_t	dir;

	// use tent for effects. or not; plasma would become a mess
	speed = sqrt(missle->s.pos.trDelta[0]*missle->s.pos.trDelta[0]
	+missle->s.pos.trDelta[1]*missle->s.pos.trDelta[1]
	+missle->s.pos.trDelta[2]*missle->s.pos.trDelta[2]);

	AngleVectors(angles, dir, NULL, NULL);

	missle->s.pos.trDelta[0] = dir[0] * speed;
	missle->s.pos.trDelta[1] = dir[1] * speed;
	missle->s.pos.trDelta[2] = dir[2] * speed;

	missle->s.pos.trTime = level.time;

	// Move
	VectorCopy(origin, missle->s.pos.trBase);
	VectorCopy(origin, missle->r.currentOrigin);
}

/*
===============
Vio_TriggersMissle
===============
*/
qboolean Vio_TriggersMissle(gentity_t *ent, vec3_t origin) {
	trace_t		tr;
	gentity_t	*traceEnt = NULL;

	trap_Trace(&tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, ent->r.ownerNum, ent->clipmask | CONTENTS_TRIGGER);

	if (tr.entityNum == ENTITYNUM_NONE) {
		return qfalse;
	}

	traceEnt = &g_entities[tr.entityNum];

	// xDiloc - trigger_teleport
	if (traceEnt && traceEnt->s.eType == ET_TELEPORT_TRIGGER) {
		gentity_t	*dest;

		dest = G_PickTarget(traceEnt->target);

		if (!dest) {
			G_Printf("Couldn't find teleporter destination\n");
			return qfalse;
		}

		Vio_TeleportMissle(ent, dest->s.origin, dest->s.angles);
		return qtrue;
	}

	// xDiloc - portals
	if (!Q_stricmp(traceEnt->classname, "portal_blue")
	 || !Q_stricmp(traceEnt->classname, "portal_orange")) {
		gentity_t	*portal = NULL;

		// Simple function to see which portal is which
		if (traceEnt->parent->portals[PORTAL_BLUE] == traceEnt) {
			portal = traceEnt->parent->portals[PORTAL_ORANGE];
		} else if (traceEnt->parent->portals[PORTAL_ORANGE] == traceEnt) {
			portal = traceEnt->parent->portals[PORTAL_BLUE];
		}

		if (!portal) {
//			G_Printf("Couldn't find teleporter destination\n");
			return qfalse;
		}

		Vio_TeleportMissle(ent, portal->r.currentOrigin, portal->s.angles);
		return qtrue;
	}

	return qfalse;
}

/*
===============
G_Portal_Think
===============
*/
void G_Portal_Think(gentity_t *portal) {
	// if it isn't connected, clear portals
/*	if (portal->parent->client->pers.connected != CON_CONNECTED) {
		G_Portal_Clear(portal->parent, 0);
		return;
	} */
}

/*
===============
G_Portal_Clear

This is the portal clear function.
===============
*/
void G_Portal_Clear(gentity_t *parent, portal_t portalnum) {
	if (!parent->client) {
		return;
	}

	// xDiloc - sound close
	if (portalnum == PORTAL_ALL) {
		if (parent->portals[PORTAL_BLUE] && parent->portals[PORTAL_ORANGE]) {
			gentity_t	*te;

			te = G_TempEntity(parent->s.pos.trBase, EV_GENERAL_SOUND);
			te->s.eventParm = G_SoundIndex("sound/weapons/portalgun/portal_close2.wav");
			te->r.svFlags |= SVF_BROADCAST;

			// effects
			G_TempEntity(parent->portals[PORTAL_BLUE]->r.currentOrigin, EV_PORTAL_CLOSE);
			G_TempEntity(parent->portals[PORTAL_ORANGE]->r.currentOrigin, EV_PORTAL_CLOSE);
		} else if (parent->portals[PORTAL_BLUE] || parent->portals[PORTAL_ORANGE]) {
			gentity_t	*te;

			te = G_TempEntity(parent->s.pos.trBase, EV_GENERAL_SOUND);
			te->s.eventParm = G_SoundIndex("sound/weapons/portalgun/portal_close1.wav");
			te->r.svFlags |= SVF_BROADCAST;
		}
	}

	if ((portalnum == PORTAL_ALL || portalnum == PORTAL_BLUE) && parent->portals[PORTAL_BLUE]) {
		G_FreeEntity(parent->portals[PORTAL_BLUE]);
		parent->portals[PORTAL_BLUE] = NULL;
	}

	if ((portalnum == PORTAL_ALL || portalnum == PORTAL_ORANGE) && parent->portals[PORTAL_ORANGE]) {
		G_FreeEntity(parent->portals[PORTAL_ORANGE]);
		parent->portals[PORTAL_ORANGE] = NULL;
	}
}

/*
===============
G_Portal_Touch

The portal touch function. This is used to do traces and to teleport the clients using the portal!
===============
*/
void vec3_debug(char *name, vec3_t out) {
	/*
	angle indexes:
	PITCH	0	up / down
	YAW	1	left / right
	ROLL	2	fall over
	pattern:
	"%s: %.3f %.3f %.3f\n", name, out[PITCH], out[YAW], out[ROLL]
	*/
	G_Printf("%s = %s\n", name, vtos(out));
}

void G_Portal_Touch(gentity_t *self, gentity_t *other, trace_t *trace) {
//	gentity_t	*portal_in = NULL;
	gentity_t	*portal_out = NULL;
	int		i;
	vec3_t		origin, dir, end, mins, maxs;
	trace_t		tr, tr2;
	vec3_t		OriginDiff, OriginEnd;

	if (!other->client) {
		return;
	}

	// Simple function to see which portal is which
	if (self->parent->portals[PORTAL_BLUE] == self) {
//		portal_in = self->parent->portals[PORTAL_BLUE];
		portal_out = self->parent->portals[PORTAL_ORANGE];
	} else if (self->parent->portals[PORTAL_ORANGE] == self) {
//		portal_in = self->parent->portals[PORTAL_ORANGE];
		portal_out = self->parent->portals[PORTAL_BLUE];
	}

	// If there isn't a mate, don't go on!
	if (/* !portal_in || */ !portal_out) {
		return;
	}

	// Grab some portal info for tracing
	VectorCopy(portal_out->r.currentOrigin, origin);
	VectorCopy(portal_out->s.angles2, dir);

/* dev
	vec3_debug("s.angles", portal_out->s.angles);
	vec3_debug("s.angles2", portal_out->s.angles2);
	vec3_debug("r.currentOrigin", portal_out->r.currentOrigin);
*/

	for(i = BBOX; i < SCAN_RANGE; i++) {
		// Scale the trace to only go so far...
		VectorMA(origin, i, dir, end);

		// Two traces for anti-glitching reasons, these traces see how far forward I can go
		trap_Trace(&tr, origin, NULL, NULL, end, other->s.number, MASK_PLAYERSOLID);
		trap_Trace(&tr2, origin, other->r.mins, other->r.maxs, end, other->s.number, MASK_PLAYERSOLID);

		// Grab bbox info for EntityContact function
		VectorAdd(tr.endpos, other->r.mins, mins);
		VectorAdd(tr.endpos, other->r.maxs, maxs);

		// If they are both clear, move on the the client teleportation
		if (tr.fraction == 1.0f && tr2.fraction == 1.0f && !trap_EntityContact(mins, maxs, portal_out)) {
			// Make sure the client doesn't mess with any map objects
			trap_UnlinkEntity(other);

			// Set the client origin
			G_SetOrigin(other, tr.endpos);
			VectorCopy(tr.endpos, other->client->ps.origin);

			{
				float		speed;
				int		i;

				// no glitch (xyz)
				VectorCopy(other->client->oldVelocity, other->client->ps.velocity);
				speed = sqrt(((other->client->ps.velocity[0])*(other->client->ps.velocity[0]))
				+((other->client->ps.velocity[1])*(other->client->ps.velocity[1]))
				+((other->client->ps.velocity[2])*(other->client->ps.velocity[2])));

				for(i = 0; i < 3; i++) {
					other->client->ps.velocity[i] = (dir[i] * speed) / 1.002;
				}
				//vec3_debug("velocity", other->client->ps.velocity);

				// sound
				//G_Printf("speed %f\n", speed);

				if (speed < 600) {
					gentity_t	*te;

					te = G_TempEntity(other->s.pos.trBase, EV_GENERAL_SOUND);
					te->s.eventParm = G_SoundIndex("sound/weapons/portalgun/portal_enter2.wav");
					te->r.svFlags |= SVF_BROADCAST;
				} else if (other->client->portaltime < level.time) {
					gentity_t	*te;

					te = G_TempEntity(other->s.pos.trBase, EV_GENERAL_SOUND);
					te->s.eventParm = G_SoundIndex("sound/weapons/portalgun/portal_enter1.wav");
					te->r.svFlags |= SVF_BROADCAST;

					other->client->portaltime = level.time + 90;
				}
			}

			// Make sure we don't overdo the viewangle change
			if (dir[2] != 1 && dir[2] != -1) {
				vec3_t		angles;

				vectoangles(dir, angles);
				angles[PITCH] = 0;

				SetClientViewAngle(other, angles);
				//vec3_debug("angles", angles);
				//vec3_debug("viewangles", other->client->ps.viewangles);
			}

			// Set the teleportation flag, so the client doesn't look as if he/she is warping all over the map
			other->client->ps.eFlags ^= EF_TELEPORT_BIT;

			// save results of pmove
			BG_PlayerStateToEntityState(&other->client->ps, &other->s, qtrue);

			// use the precise origin for linking
			VectorCopy(other->client->ps.origin, other->r.currentOrigin);

			// Link the entity back to the world
			trap_LinkEntity(other);

			return;
		}
		{
			gentity_t	*traceEnt;

			// Set the entity picked up on the trace...
			traceEnt = &g_entities[tr.entityNum];

			// If the traceEnt is a client...
			if (traceEnt->client) {
				// Push the client out of the way!
				if (dir[2] != 1 && dir[2] != -1) {
					VectorScale(dir, 300, traceEnt->client->ps.velocity);
				}
			}
		}
	}
}

/*
===============
G_Portal_Create

This is used to create a portal. Here we will spawn it's entity, define its model, and give it a bbox!
===============
*/
void G_Portal_Create(gentity_t *parent, vec3_t origin, vec3_t normal, portal_t portalnum) {
	gentity_t	*portal;
	vec3_t		end, maxs, mins, tmins, tmaxs, angles;
	trace_t		tr;

	// If the ent isn't a client...return
	if (!parent->client) {
		return;
	}

	// Clear any existing portals!
	G_Portal_Clear(parent, portalnum);

	// Trace 1 game unit forward, to not mix in walls!
	VectorMA(origin, 1, normal, end);
	trap_Trace(&tr, origin, NULL, NULL, end, -1, MASK_SHOT);

	// Set the bbox...
	VectorSet(mins, -BBOX, -BBOX, -BBOX);
	VectorSet(maxs,  BBOX,  BBOX,  BBOX);
// how rotate this?
//	VectorSet(mins, 0, -15, -28);
//	VectorSet(maxs, 1,  15, 28);

	// Get the location + bbox for EntityContact info
	VectorAdd(tr.endpos, mins, tmins);
	VectorAdd(tr.endpos, maxs, tmaxs);

/* dev
	vec3_debug("tr.endpos", tr.endpos);
	vec3_debug("tmins", tmins);
	vec3_debug("tmaxs", tmaxs);
*/

	// If we're trying to make the first portal, and we have a second...
	if (portalnum == PORTAL_BLUE && parent->portals[PORTAL_ORANGE]) {
		// This is only to help EntityContact
		parent->portals[PORTAL_ORANGE]->r.contents = CONTENTS_SOLID;

		// If we're in contact with the mate, return
		if (trap_EntityContact(tmins, tmaxs, parent->portals[PORTAL_ORANGE])) {
			// Set the mates contents back to normal
			parent->portals[PORTAL_ORANGE]->r.contents = CONTENTS_TRIGGER;
			return;
		}

		// If we fail, set it back to normal
		parent->portals[PORTAL_ORANGE]->r.contents = CONTENTS_TRIGGER;
	}

	// If we're trying to make the second portal, and we have a first...
	if (portalnum == PORTAL_ORANGE && parent->portals[PORTAL_BLUE]) {
		// This is only to help EntityContact
		parent->portals[PORTAL_BLUE]->r.contents = CONTENTS_SOLID;

		// If we're in contact with the mate, return
		if (trap_EntityContact(tmins, tmaxs, parent->portals[PORTAL_BLUE])) {
			// Set the mates contents back to normal
			parent->portals[PORTAL_BLUE]->r.contents = CONTENTS_TRIGGER;
			return;
		}

		// If we fail, set it back to normal
		parent->portals[PORTAL_BLUE]->r.contents = CONTENTS_TRIGGER;
	}

	// xDiloc - sound open
	if (parent->portals[PORTAL_BLUE] || parent->portals[PORTAL_ORANGE]) {
		gentity_t	*te;

		te = G_TempEntity(parent->s.pos.trBase, EV_GENERAL_SOUND);
		te->s.eventParm = G_SoundIndex("sound/weapons/portalgun/portal_open.wav");
		te->r.svFlags |= SVF_BROADCAST;
	}

	// Create the portal entity
	portal = G_Spawn();
	// Set the ent, this will be use to track other portals made by this client
	portal->parent = parent;
	// Set entity type as general so we can define a model
	portal->s.eType = ET_GENERAL;
	// Help with ET_GENERAL stuff
	portal->s.pos.trType = TR_STATIONARY;
	portal->s.pos.trTime = level.time;
	// Set the portals contents, this is used to help with bbox info
	portal->r.contents = CONTENTS_TRIGGER;
	// Define the touch, and think function, when a portal is touched, teleportation time!
	portal->touch = G_Portal_Touch;
	portal->think = G_Portal_Think;

	// Define model...
	if (portalnum == PORTAL_BLUE) {
		portal->classname = "portal_blue";
		portal->s.modelindex = G_ModelIndex("models/portals/portal_blue.md3");
	}
	if (portalnum == PORTAL_ORANGE) {
		portal->classname = "portal_orange";
		portal->s.modelindex = G_ModelIndex("models/portals/portal_orange.md3");
	}

	// Set the surface face, this is also used to check which direction we will be tracing
	vectoangles(normal, portal->s.angles);
	VectorCopy(normal, portal->s.angles2);

	// xDiloc - omg
	vectoangles(normal, angles);
	VectorCopy(angles, portal->s.apos.trBase);
//	vec3_debug("angles", portal->s.apos.trBase);

	// Set the portals origin
	G_SetOrigin(portal, tr.endpos);

	// Add bbox
	VectorCopy(mins, portal->r.mins);
	VectorCopy(maxs, portal->r.maxs);

	// Link entity to the world!
	trap_LinkEntity(portal);

	parent->portals[portalnum] = portal;
}

#endif

