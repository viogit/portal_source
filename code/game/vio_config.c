#include "g_local.h"
#ifdef	VIOL_VM

/*
===========================================================================
Config System
===========================================================================
*/

qboolean vio_string(const char *token, const char *name, char *out, const char *value) {
	if (Q_stricmp(token, name) == 0) {
		if (value[0]) {
//			Com_Printf("%s = %s (size %i)\n", name, value, strlen(value));
			Com_sprintf(out, strlen(value)+1, "%s", value);
		} else {
			COM_ParseWarning("^1Missing value ^7\'%s\'", token);
		}
		return qtrue;
	}
	return qfalse;
}

qboolean vio_integer(const char *token, const char *name, int *out, const char *value) {
	if (Q_stricmp(token, name) == 0) {
		if (value[0]) {
			*out = atoi(value);
		} else {
			COM_ParseWarning("^1Missing value ^7\'%s\'", token);
		}
		return qtrue;
	}
	return qfalse;
}

qboolean vio_float(const char *token, const char *name, float *out, const char *value) {
	if (Q_stricmp(token, name) == 0) {
		if (value[0]) {
			*out = atof(value);
		} else {
			COM_ParseWarning("^1Missing value ^7\'%s\'", token);
		}
		return qtrue;
	}
	return qfalse;
}

qboolean vio_bool(const char *token, const char *name, int *out, const char *value) {
	if (Q_stricmp(token, name) == 0) {
		if (value[0]) {
			if (!Q_stricmp(value, "true")
			 || !Q_stricmp(value, "qtrue")
			 || !Q_stricmp(value, "1")
			 || !Q_stricmp(value, "yes")) {
				*out = qtrue;
			} else if (!Q_stricmp(value, "false")
			 || !Q_stricmp(value, "qfalse")
			 || !Q_stricmp(value, "0" )
			 || !Q_stricmp(value, "no")) {
				*out = qfalse;
			} else {
				COM_ParseWarning("^1Unknown value ^7%s", value);
				COM_ParseWarning("^1Valid options for ^7\'%s\' ^1true or false", name);
			}
		} else {
			COM_ParseWarning("^1Missing value ^7\'%s\'", token);
		}
		return qtrue;
	}
	return qfalse;
}

qboolean vio_notice(const char *token, const char *name, const char *value) {
	if (Q_stricmp(token, name) == 0) {
		if (value[0] == 0) {
			COM_ParseWarning("^1Missing value ^7\'%s\'", token);
		}
		return qtrue;
	}
	return qfalse;
}

/*
===============
Vio_ParseConfig_f
===============
*/
void Vio_ParseConfig_f(char *data) {
	char	com_token[MAX_TOKEN_CHARS];
	char	*value;

	while(1) {
		Q_strncpyz(com_token, COM_ParseExt(&data, qtrue), sizeof(com_token));

		// end data
		if (com_token[0] == 0) {
			break;
		}

		// skip
		if (Q_stricmp(com_token, ";") == 0) {
			continue;
		}

		// equal
		value = COM_ParseExt(&data, qfalse);
		if (Q_stricmp(value, "=") == 0) {
			value = COM_ParseExt(&data, qfalse);
		} else {
			COM_ParseWarning("^1Missing ^7\'=\'");
		}

		//Com_Printf("%s = %s\n", com_token, value);

		// notice
		if (vio_notice(com_token, "author", value)) {

		// string
		} else if (vio_string(com_token, "description", vio.description, value)) {
		} else if (vio_string(com_token, "vio_startweapon", vio.startweapon, value)) {

		// integer
		} else if (vio_integer(com_token, "vio_physic", &vio.physic, value)) {
		} else if (vio_integer(com_token, "vio_respawntime", &vio.respawntime, value)) {
		} else if (vio_integer(com_token, "vio_specspeed", &vio.specspeed, value)) {

		// bool
		} else if (vio_bool(com_token, "vio_selfdamage", &vio.selfdamage, value)) {
		} else if (vio_bool(com_token, "vio_falldamage", &vio.falldamage, value)) {
		} else if (vio_bool(com_token, "vio_dropweapon", &vio.dropweapon, value)) {
		} else if (vio_bool(com_token, "vio_teleportmissle", &vio.teleportmissle, value)) {
		} else if (vio_bool(com_token, "vio_specclip", &vio.specclip, value)) {

		// integer (weapon ammo)
		} else if (vio_integer(com_token, "vio_ammo_gauntlet", &vio.ammo_gauntlet, value)) {
		} else if (vio_integer(com_token, "vio_ammo_machinegun", &vio.ammo_machinegun, value)) {
		} else if (vio_integer(com_token, "vio_ammo_shotgun", &vio.ammo_shotgun, value)) {
		} else if (vio_integer(com_token, "vio_ammo_grenade", &vio.ammo_grenade, value)) {
		} else if (vio_integer(com_token, "vio_ammo_rocket", &vio.ammo_rocket, value)) {
		} else if (vio_integer(com_token, "vio_ammo_lightning", &vio.ammo_lightning, value)) {
		} else if (vio_integer(com_token, "vio_ammo_railgun", &vio.ammo_railgun, value)) {
		} else if (vio_integer(com_token, "vio_ammo_plasmagun", &vio.ammo_plasmagun, value)) {
		} else if (vio_integer(com_token, "vio_ammo_bfg", &vio.ammo_bfg, value)) {
		} else if (vio_integer(com_token, "vio_ammo_hook", &vio.ammo_hook, value)) {
		} else if (vio_integer(com_token, "vio_ammo_portal", &vio.ammo_portal, value)) {

		// temp tests
		} else if (vio_string(com_token, "vio_teststring", vio.teststring, value)) {
			Com_Printf("vio_teststring = %s\n", vio.teststring);
		} else if (vio_integer(com_token, "vio_testinteger", &vio.testinteger, value)) {
			Com_Printf("vio_testinteger = %i\n", vio.testinteger);
		} else if (vio_float(com_token, "vio_testfloat", &vio.testfloat, value)) {
			Com_Printf("vio_testfloat = %0.3f\n", vio.testfloat);
		} else if (vio_bool(com_token, "vio_testbool", &vio.testbool, value)) {
			Com_Printf("vio_testbool = %i\n", vio.testbool);

		} else {
			COM_ParseWarning("^1Unknown token ^7\'%s\'", com_token);
		}
	}
}

/*
===============
Vio_ConfigLoad_f
===============
*/
#define MEM_CONFIG	4096
qboolean Vio_ConfigLoad_f(const char *filename) {
	fileHandle_t	fileHandle;
	int		len;
	char		buf[MEM_CONFIG];

	len = trap_FS_FOpenFile(filename, &fileHandle, FS_READ);

	if (!fileHandle) {
		Com_Printf("cant load: %s\n", filename);
		return qfalse;
	}

	if (len <= 0) {
		trap_FS_FCloseFile(fileHandle);
		Com_Printf("file %s is empty\n", filename);
		return qfalse;
	}

	if (len > MEM_CONFIG) {
		trap_FS_FCloseFile(fileHandle);
		Com_Printf("file too large: %s is %i, max allowed is %i\n", filename, len, MEM_CONFIG);
		return qfalse;
	}

	// xDiloc - read file
	trap_FS_Read(buf, len, fileHandle);
	buf[len] = 0;
	trap_FS_FCloseFile(fileHandle);

	// xDiloc - parse
	COM_BeginParseSession(filename);
	Vio_ParseConfig_f(buf);

	return qtrue;
}

/*
===============
Vio_ConfigInit_f
===============
*/
void Vio_ConfigInit_f(const char *file) {
	char	filepath[MAX_QPATH];
	char	filename[50];

	// xDiloc - clear data
	memset(&vio, 0, sizeof(vio));

	/* xDiloc - vq3 settings */
	// string
	Com_sprintf(vio.description, sizeof(vio.description), "unnamed");
	Com_sprintf(vio.startweapon, sizeof(vio.startweapon), "WP_GAUNTLET | WP_MACHINEGUN");

	// integer
	vio.physic = 0;
	vio.respawntime = 1700;
	vio.specspeed = 400;
	// bool
	vio.selfdamage = 1;
	vio.falldamage = 1;
	vio.dropweapon = 1;
	vio.teleportmissle = 0;
	vio.specclip = 0;

	// integer (weapon ammo)
	vio.ammo_gauntlet = -1;
	if (g_gametype.integer == GT_TEAM) {
		vio.ammo_machinegun = 50;
	} else {
		vio.ammo_machinegun = 100;
	}
	vio.ammo_shotgun = 10;
	vio.ammo_grenade = 10;
	vio.ammo_rocket = 10;
	vio.ammo_lightning = 100;
	vio.ammo_railgun = 10;
	vio.ammo_plasmagun = 50;
	vio.ammo_bfg = 20;
	vio.ammo_hook = -1;
	vio.ammo_portal = -1;

	// check file extension
	if (!strrchr(file, '.')) {
		Com_sprintf(filename, sizeof(filename), "%s.cfg", file);
	} else {
		Com_sprintf(filename, sizeof(filename), "%s", file);
	}

	// load
	Com_sprintf(filepath, sizeof(filepath), "%s/%s", "config", filename);
	if (Vio_ConfigLoad_f(filepath)) {
		Com_Printf("loaded: %s (%s)\n", filename, vio.description);
	} else {
		Com_Printf("default setting.\n");
	}

	// xDiloc - call cgame (31 is free?)
	trap_SetConfigstring(31, va("%i %i", vio.physic, vio.falldamage));

/* dev
	{
		char	info[MAX_INFO_STRING];
		trap_GetConfigstring(31, info, MAX_INFO_STRING);
		Com_Printf("%s\n",info);
	} */
}

/*
===============
Vio_ConfigCmd_f
===============
*/
void Vio_ConfigCmd_f(void) {
	char	str[MAX_TOKEN_CHARS];

	if (trap_Argc() < 2 || trap_Argc() > 2) {
		G_Printf("Usage: load <config>\n");
		return;
	}
	trap_Argv(1, str, sizeof(str));
	trap_Cvar_Set("vio_config", str);
	Vio_ConfigInit_f(str);
}

/* dev
memset(vio.description, 0, sizeof(vio.description));
strncpy(vio.description, value, sizeof(vio.description)-1);
Q_strncpyz(value, vio.description, sizeof(value));
*/
#endif

