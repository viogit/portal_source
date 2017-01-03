#include "cg_local.h"

#ifdef	VIOL_VM

void CG_ColorByEntityType(int eType, byte colors[4]) {
	switch(eType) {
	default:
		colors[0] = 220;
		colors[1] = 220;
		colors[2] = 100;
		colors[3] = 255;
		break;
	case ET_GENERAL:
		colors[0] = 0;
		colors[1] = 0;
		colors[2] = 128;
		colors[3] = 255;
		break;
	case ET_PLAYER:
		colors[0] = 128;
		colors[1] = 128;
		colors[2] = 128;
		colors[3] = 255;
		break;
	case ET_ITEM:
		colors[0] = 128;
		colors[1] = 0;
		colors[2] = 128;
		colors[3] = 255;
		break;
	case ET_MISSILE:
		colors[0] = 64;
		colors[1] = 0;
		colors[2] = 0;
		colors[3] = 255;
		break;
	case ET_MOVER:
		colors[0] = 0;
		colors[1] = 255;
		colors[2] = 0;
		colors[3] = 255;
		break;
	case ET_BEAM:
		colors[0] = 0;
		colors[1] = 192;
		colors[2] = 0;
		colors[3] = 255;
		break;
	case ET_PORTAL:
		colors[0] = 0;
		colors[1] = 128;
		colors[2] = 0;
		colors[3] = 255;
		break;
	case ET_SPEAKER:
		colors[0] = 0;
		colors[1] = 64;
		colors[2] = 0;
		colors[3] = 255;
		break;
	case ET_PUSH_TRIGGER:
		colors[0] = 0;
		colors[1] = 0;
		colors[2] = 255;
		colors[3] = 255;
		break;
	case ET_TELEPORT_TRIGGER:
		colors[0] = 0;
		colors[1] = 0;
		colors[2] = 192;
		colors[3] = 255;
		break;
	case ET_INVISIBLE:
		colors[0] = 0;
		colors[1] = 0;
		colors[2] = 128;
		colors[3] = 255;
		break;
	// ET_GRAPPLE
	// ET_TEAM
	// ET_EXPLOSION
	// ET_BALLOON
	// ET_BAMBAM
	// ET_BOOMIES
	}
}

/*
=================
TransposeMatrix
=================
*/
void TransposeMatrix(vec3_t matrix[3], vec3_t transpose[3]) {
	int i, j;

	for (i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			transpose[i][j] = matrix[j][i];
		}
	}
}

/*
=================
CG_EntityVectors

Get origin vector and bounding box vextex vectors from an entity.
If box_vertex is NULL, it will be ignored.
=> qfalse, if the entity has no bounds vertex.
The bounds index are in the following order.

    2+------+3     ^ z
    /|     /|      |  y
  1/ |   0/ |      |/
  +------+  |      +--> x
  | 6+---|--+7
  | /    | /
  |/     |/
 5+------+4
=================
*/
void CG_EntityVectors(centity_t *cent, vec3_t origin, vec3_t box_vertex[8]) {
	entityState_t	*ent;
	vec3_t	angles, median, ext_xyz;
	int	i;

	// Output parameter origin has to be present
	if (!origin) {
		return;
	}

	ent = &cent->currentState;

	if (ent->solid == SOLID_BMODEL) {
		float		*vmodel;
		clipHandle_t	cmodel;
		vec3_t		bmaxs, bmins;

		vmodel = cgs.inlineModelMidpoints[ent->modelindex];
		VectorAdd(cent->lerpOrigin, vmodel, origin);

		cmodel = cgs.inlineDrawModel[ent->modelindex];
		trap_R_ModelBounds(cmodel, bmins, bmaxs);

		for (i = 0; i < 3; i++) {
			// Store a vector for one box side
			ext_xyz[i] = (bmaxs[i] - bmins[i]);

			// And store a *positive* vector, from origin to one vertex corner.
			median[i] = (bmaxs[i] - (bmaxs[i] + bmins[i]) * 0.5);
		}
	} else if (ent->solid) {
		int	x, zd, zu;

		VectorCopy(cent->lerpOrigin, origin);

		// encoded bbox
		x = (ent->solid & 255);
		zd = ((ent->solid >> 8) & 255);
		zu = (((ent->solid >> 16) & 255) - 32);

		median[0] = x;
		median[1] = x;
		median[2] = zu;

		ext_xyz[0] = (2 * x);
		ext_xyz[1] = (2 * x);
		ext_xyz[2] = (zu + zd);
	} else {
		// Not a solid
		VectorCopy(cent->lerpOrigin, origin);

		switch(cent->currentState.eType) {
		case ET_PLAYER:
			median[0] = 15;
			median[1] = 15;
			median[2] = 32;

			ext_xyz[0] = 30;
			ext_xyz[1] = 30;
			ext_xyz[2] = 56;
			break;
		case ET_ITEM:
			median[0] = 22;
			median[1] = 22;
			median[2] = 22;

			ext_xyz[0] = 44;
			ext_xyz[1] = 44;
			ext_xyz[2] = 44;
			break;
		default:
			median[0] = 15;
			median[1] = 15;
			median[2] = 15;

			ext_xyz[0] = 30;
			ext_xyz[1] = 30;
			ext_xyz[2] = 30;

			/* dev portal draw idea
			median[0] = 1;
			median[1] = 15;
			median[2] = 28;

			ext_xyz[0] = 1;
			ext_xyz[1] = 30;
			ext_xyz[2] = 56;
			*/

			break;
		}
	}

	// Vertex bounds for the "horizontal" top box.
	// Top forward right
	VectorAdd(origin, median, box_vertex[3]);

	// Top forward left
	VectorCopy(box_vertex[3], box_vertex[2]);
	box_vertex[2][0] -= ext_xyz[0];

	// Top backward left
	VectorCopy(box_vertex[2], box_vertex[1]);
	box_vertex[1][1] -= ext_xyz[1];

	// Top backward right
	VectorCopy(box_vertex[1], box_vertex[0]);
	box_vertex[0][0] += ext_xyz[0];

	// Vertex bounds for the "horizontal" bottom box.
	for (i = 0; i < 4; i++) {
		VectorCopy(box_vertex[i], box_vertex[i + 4]);
		box_vertex[i + 4][2] -= ext_xyz[2];
	}

	if (cent->currentState.eType == ET_PLAYER || cent->currentState.eType == ET_ITEM) {
		return;
	}

	// Rotate all the bounds, if needed
	VectorCopy(cent->lerpAngles, angles);
	if (angles[0] || angles[1] || angles[2]) {
		vec3_t	vertex, r, matrix[3], transpose[3];
		vec3_t	rotate_origin = {0.0f, 0.0f, 0.0f};

		VectorCopy(cent->lerpOrigin, rotate_origin);

		// Get the entity axis's point of view.
		AnglesToAxis(angles, matrix);

		// Transpose matrix as it is the entity axis point of view, which rotate.
		// Consider that it is not each bounds that rotate, but the entity axis.
		// So, if the entity axis rotate in one direction, the bounds will be rotate in the opposite,
		// from the entity axis point of view.
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		TransposeMatrix(matrix, transpose);
		for(i = 0; i < 8; i++) {
			// Rotate should be done from the rotation origin, not the entity origin
			VectorSubtract(box_vertex[i], rotate_origin, vertex);
			VectorRotate(vertex, transpose, r);

			// Apply the rotation result.
			VectorAdd(rotate_origin, r, box_vertex[i]);
		}
	}
}

/*
===================
CG_PortalBoundingBox
===================
*/
void Vio_DrawBoundingBox(centity_t *cent) {
	polyVert_t	verts[4];
	int		i;
	vec3_t		corners[8];
	entityState_t	*ent;

	// set the polygon's texture coordinates
	verts[0].st[0] = 0;
	verts[0].st[1] = 0;
	verts[1].st[0] = 0;
	verts[1].st[1] = 1;
	verts[2].st[0] = 1;
	verts[2].st[1] = 1;
	verts[3].st[0] = 1;
	verts[3].st[1] = 0;

	ent = &cent->currentState;

	// Set the bounding box color, according to the entity type
	for (i = 0; i < 4; i++) {
		CG_ColorByEntityType(ent->eType, verts[i].modulate);
	}

	CG_EntityVectors(cent, cent->lerpOrigin, corners);

	// top
	VectorCopy(corners[0], verts[0].xyz);
	VectorCopy(corners[1], verts[1].xyz);
	VectorCopy(corners[2], verts[2].xyz);
	VectorCopy(corners[3], verts[3].xyz);
	trap_R_AddPolyToScene(cgs.media.bbox, 4, verts);
	// bottom
	VectorCopy(corners[7], verts[0].xyz);
	VectorCopy(corners[6], verts[1].xyz);
	VectorCopy(corners[5], verts[2].xyz);
	VectorCopy(corners[4], verts[3].xyz);
	trap_R_AddPolyToScene(cgs.media.bbox, 4, verts);
	// top side
	VectorCopy(corners[3], verts[0].xyz);
	VectorCopy(corners[2], verts[1].xyz);
	VectorCopy(corners[6], verts[2].xyz);
	VectorCopy(corners[7], verts[3].xyz);
	trap_R_AddPolyToScene(cgs.media.bbox_nocull, 4, verts);
	// left side
	VectorCopy(corners[2], verts[0].xyz);
	VectorCopy(corners[1], verts[1].xyz);
	VectorCopy(corners[5], verts[2].xyz);
	VectorCopy(corners[6], verts[3].xyz);
	trap_R_AddPolyToScene(cgs.media.bbox_nocull, 4, verts);
	// right side
	VectorCopy(corners[0], verts[0].xyz);
	VectorCopy(corners[3], verts[1].xyz);
	VectorCopy(corners[7], verts[2].xyz);
	VectorCopy(corners[4], verts[3].xyz);
	trap_R_AddPolyToScene(cgs.media.bbox_nocull, 4, verts);
	// bottom side
	VectorCopy(corners[1], verts[0].xyz);
	VectorCopy(corners[0], verts[1].xyz);
	VectorCopy(corners[4], verts[2].xyz);
	VectorCopy(corners[5], verts[3].xyz);
	trap_R_AddPolyToScene(cgs.media.bbox_nocull, 4, verts);
}

#endif

