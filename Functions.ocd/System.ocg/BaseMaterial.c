/**
	Base Material 
	Provides global functions to give players base material for scenarios,
	related to base melees.
	
	@author Maikel
*/

// Gives the player specific base materials as given in the materials array.
global func GivePlayerSpecificBaseMaterial(int plr, array base_mats, bool add_mats)
{
	for (var mat in base_mats)
	{
		if (!add_mats)
		{
			SetBaseMaterial(plr, mat[0], mat[1]);
			SetBaseProduction(plr, mat[0], mat[2]);
		}
		else
		{
			DoBaseMaterial(plr, mat[0], mat[1]);
			DoBaseProduction(plr, mat[0], mat[2]);
		}
	}
	return;
}

// Gives the player the basic base materials.
global func GivePlayerBasicBaseMaterial(int plr, bool add_mats)
{
	var base_mats = [
		// Clonks & health.
		[Clonk,          10,  2],
		[Bread,           5,  1],
		// Materials & mining.
		[Wood,           10,  4],
		[Metal,          10,  4],
		[Rock,           10,  4],
		[Cloth,           2,  1],
		[Loam,            4,  2],
		[Firestone,       4,  2],
		[Dynamite,        4,  2],
		// Essential tools.
		[Shovel,          4,  2],
		[Hammer,          2,  1],
		[Axe,             2,  1],
		[Pickaxe,         2,  1]
	];
	return GivePlayerSpecificBaseMaterial(plr, base_mats, add_mats);
}

// Gives the player the normal base materials.
global func GivePlayerNormalBaseMaterial(int plr, bool add_mats)
{
	var base_mats = [
		// Additional: Clonks & health.
		[Clonk,           4,  1],
		[Bread,           2,  1],
		// Additional: Materials & mining.
		[Wood,            2,  1],
		[Metal,           2,  1],
		[Rock,            2,  1],
		[Cloth,           2,  0],
		[Firestone,       2,  2],
		[Dynamite,        2,  2],
		[DynamiteBox,     2,  1],
		// Additional tools.
		[Barrel,          2,  1],
		[MetalBarrel,     2,  1],
		[Bucket,          2,  1],
		[Torch,           2,  1],
		[Pipe,            2,  1],
		[WallKit,         1,  1],
		// Weapons & explosives.
		[Bow,             2,  1],
		[Arrow,           2,  1],
		[Shield,          2,  1],
		[Javelin,         2,  1],
		[Club,            1,  1],
		// Advanced tools.
		[Ropeladder,      2,  1],
		[GrappleBow,      2,  1],
		[Balloon,         2,  1]
	];
	return GivePlayerSpecificBaseMaterial(plr, base_mats, add_mats);
}

// Gives the player the advanced base materials.
global func GivePlayerAdvancedBaseMaterial(int plr, bool add_mats)
{
	var base_mats = [
		// Additional: Clonks & health.
		[Clonk,           2,  1],
		[Bread,           1,  1],
		// Additional: Materials & mining.
		[Wood,            4,  2],
		[Metal,           4,  2],
		[Rock,            4,  2],
		[Cloth,           2,  0],
		[Firestone,       2,  2],
		[Dynamite,        2,  2],
		// Additional: weapons.
		[Bow,             1,  1],
		[Arrow,           2,  1],
		[Club,            1,  1],
		// Advanced tools.
		[TeleGlove,       1,  1],
		[WindBag,         1,  1],
		[Boompack,        1,  1],
		// Advanced weapons.
		[FireArrow,       2,  1],
		[BombArrow,       2,  1],
		[Sword,           2,  1],
		[Blunderbuss,     1,  1],
		[LeadBullet,      1,  1],
		[GrenadeLauncher, 1,  1],
		[IronBomb,        2,  1],
		[SmokeBomb,       2,  1],
		[Lantern,         2,  1],
		[Catapult,        1,  1],
		[PowderKeg,       2,  1],
		[Cannon,          1,  1]
	];
	return GivePlayerSpecificBaseMaterial(plr, base_mats, add_mats);
}