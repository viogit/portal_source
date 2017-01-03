// xDiloc - init
#define	VIOL_VM

#ifdef	VIOL_VM
// xDiloc - data
#define VIO_BSIZE 1024
typedef struct {
	char		description[VIO_BSIZE];
	char		startweapon[VIO_BSIZE];
	int		physic;
	int		selfdamage;
	int		falldamage;
	int		respawntime;
	int		dropweapon;
	int		teleportmissle;
	int		specclip;
	int		specspeed;

	// weapon ammo
	int		ammo_gauntlet;
	int		ammo_machinegun;
	int		ammo_shotgun;
	int		ammo_grenade;
	int		ammo_rocket;
	int		ammo_lightning;
	int		ammo_railgun;
	int		ammo_plasmagun;
	int		ammo_bfg;
	int		ammo_hook;
	int		ammo_portal;

	// temp tests
	char		teststring[VIO_BSIZE];
	int		testinteger;
	float		testfloat;
	int		testbool;
} init_vio;

extern	init_vio	vio;

// xDiloc - prefix
#define PREFIX		"^7\x1d\x1e\x1f "

// xDiloc - tools strtok
char	*strtok(char *s, const char *delim);
char	*strtok_r(char *s, const char *delim, char **last);

#endif
