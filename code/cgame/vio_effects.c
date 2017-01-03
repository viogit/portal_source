#include "cg_local.h"

#ifdef	VIOL_VM
// xDiloc - portalgun
/*
==================
CG_LaunchPortal
==================
*/
void CG_LaunchPortal(vec3_t origin, vec3_t velocity, qhandle_t hModel) {
	localEntity_t	*le;
	refEntity_t		*re;

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	le->leType = LE_FRAGMENT;
	le->startTime = cg.time;
	le->endTime = le->startTime + 500 + random() * 500;

	VectorCopy(origin, re->origin);
	AxisCopy(axisDefault, re->axis);
	re->hModel = hModel;

	le->pos.trType = TR_GRAVITY;
	VectorCopy(origin, le->pos.trBase);
	VectorCopy(velocity, le->pos.trDelta);
	le->pos.trTime = cg.time;

	le->bounceFactor = 0.5f;

	le->leBounceSoundType = LEBS_NONE;
	le->leMarkType = LEMT_NONE;
}

#define	PFX_VELOCITY	80
#define	PFX_JUMP	100
/*
===================
CG_PortalExplode
===================
*/
void CG_PortalExplode(vec3_t playerOrigin, qhandle_t hModel) {
	vec3_t	origin, velocity;
	int	i, cmon;

	cmon = 1;

	for(i = 0; i < 30; i++) {
		VectorCopy(playerOrigin, origin);
		velocity[0] = crandom()*PFX_VELOCITY*cmon;
		velocity[1] = crandom()*PFX_VELOCITY*cmon;
		velocity[2] = PFX_JUMP + crandom()*PFX_VELOCITY;
		CG_LaunchPortal(origin, velocity, hModel);

		cmon += 0.5;
	}
}

#endif

