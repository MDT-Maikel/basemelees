/**
	Cliff Fight
	Scenario where two opposing teams each have their bases on a cliff.

	@author Maikel
*/


// List for storing whether a team has been initialized already.
static team_init;

public func Initialize()
{
	// Goals & Rules.
	CreateObject(Goal_Melee);
	CreateObject(Rule_Domination);
	CreateObject(Rule_BaseRespawn);
	CreateObject(Rule_BuyAtFlagpole);
	CreateObject(Rule_TeamAccount);
	CreateObject(Rule_BaseMeleeLogging);
	CreateObject(Rule_Medals);
	
	// Team initialized variable should be a list.
	team_init = [false, false];
	
	// Initialize different parts of the scenario.
	InitAnimals();
	InitVegetation();
	InitEnvironment();
	InitBlocking(SCENPAR_AttackBarrier);
	return;
}


/*-- Player Initiliaztion --*/

protected func InitializePlayer(int plr)
{
	// Get player team.
	var team = GetPlayerTeam(plr);
	if (team == 0)
		return;
	
	// Set a strict zoom range.
	SetPlayerZoomByViewRange(plr, 500, 350, PLRZOOM_LimitMax);
	SetPlayerViewLock(plr, true);
	
	// Base startup objects.
	var base_objects = [
		{def = Flagpole, amount = 1},
		{def = WindGenerator, amount = 1},
		{def = ToolsWorkshop, amount = 1, contents = [[Wood, 8], [Metal, 4]]},
		{def = ChemicalLab, amount = 1, contents = [[Firestone, 8], [Wood, 4], [Metal, 2]]},
		{def = Sawmill, amount = 1},
		{def = Foundry, amount = 1, contents = [[Metal, 6]]},
		{def = Lorry, amount = 1, contents = [[Wood, 6], [Metal, 6], [Rock, 4], [Loam, 3], [Hammer, 2], [Axe, 2]]},
		{def = Cannon, amount = 1, contents = [[PowderKeg, 1]]}
	];
	
	// If team not yet initialize, do the startup objects and flagpole creation.
	if (!team_init[team - 1])
	{
		team_init[team - 1] = true;
		var x = 304;
		if (team == 2)
			x = LandscapeWidth() - x;
		var y = FindHeight(x);
		SetWealth(plr, 100);
		CreateConstruction(Flagpole, x, y, plr, 100, true);		
		CreateBaseMenu(GetCrew(plr, 0), base_objects, Rectangle(x - 300, y - 200, 640, 400));
	}
	
	// Position crew and give them a shovel.
	var i, crew;
	for (i = 0; crew = GetCrew(plr, i); ++i)
	{
		var x = 304 + RandomX(-50, 50);
		if (team == 2)
			x = LandscapeWidth() - x;
		var y = FindHeight(x) - 11;
		crew->SetPosition(x, y);
		crew->CreateContents(Shovel);
	}
	
	// Give player their knowledge and base materials.
	GivePlayerKnowledge(plr);
	GivePlayerMaterials(plr);
	return;
}

// Give the relevant knowledge to each player.
private func GivePlayerKnowledge(int plr)
{
	var structures = [Flagpole, WindGenerator, SteamEngine, Compensator, Foundry, Sawmill, Elevator, Pump, ToolsWorkshop, ChemicalLab, Armory, Chest, Windmill, Kitchen, InventorsLab, Shipyard];
	var items = [Loam, GoldBar, Metal, Shovel, Axe, Hammer, Pickaxe, Barrel, Bucket, Dynamite, DynamiteBox, PowderKeg, Pipe, WallKit, TeleGlove, WindBag, GrappleBow, Boompack, Balloon];
	var weapons = [Bow, Arrow, Club, Sword, Javelin, Shield, Musket, LeadShot, IronBomb, GrenadeLauncher];
	var vehicles = [Lorry, Catapult, Cannon, Airship, Plane];
	var total = Concatenate(structures, items);
	total = Concatenate(total, weapons);
	total = Concatenate(total, vehicles);
	for (var def in total)
		SetPlrKnowledge(plr, def);
	return;
}

// Give the relevant materials to each player.
private func GivePlayerMaterials(int plr)
{
	// Set the base material.
	var mats = [[Clonk, 10], [Shovel, 2], [Pickaxe, 2], [Axe, 2], [Hammer, 2]];
	for (var mat in mats)
		DoBaseMaterial(plr, mat[0], mat[1]);
	// Set the base production.
	var prods = [[Clonk, 2], [Shovel, 1], [Pickaxe, 1], [Axe, 1], [Hammer, 1]];
	for (var prod in prods)
		DoBaseMaterial(plr, prod[0], prod[1]);
	return;
}

private func FindHeight(int x)
{
	var y = 0;
	while (!GBackSemiSolid(x, y) && y < LandscapeHeight())
		y += 5;
	return y;
}


/*-- Scenario Initiliaztion --*/

private func InitAnimals()
{
	var wdt = LandscapeWidth();
	var hgt = LandscapeHeight();
	var place_rect = Rectangle(wdt / 2 - 600, 0, 1200, hgt);
	Fish->Place(20, place_rect);
	return;
}

private func InitVegetation()
{
	// Some grass is always nice.
	PlaceGrass(85);
	
	// Pine trees provide wood.
	Tree_Coniferous->Place(80);
	
	// Some berry bushes at hard to reach locations.
	for (var i = 0; i < 3 + Random(3); i++)
	{
		PlaceVegetation(SproutBerryBush, 800, 830, 100, 100, 100000);
		PlaceVegetation(SproutBerryBush, LandscapeWidth() - 900, 830, 100, 100, 100000);
	}
	
	// Some mushrooms to regain health.
	Mushroom->Place(40);
	Fern->Place(30);
	
	// Some objects in the earth.	
	PlaceObjects(Metal, 15 + Random(10), "Earth");
	PlaceObjects(Wood, 15 + Random(10), "Earth");
	PlaceObjects(Firestone, 30 + Random(5), "Earth");

	return;
}

private func InitEnvironment()
{
	// Some objects in the sky, start in the evening.
	Cloud->Place(30);
	Cloud->SetPrecipitation("Water", 5);
	Cloud->SetLightning(5);
	// Set a certain parallax.
	SetSkyParallax(0, 20, 20);
	return;
}

private func InitBlocking(int minutes)
{
	if (!minutes)
		return;
	var wdt = LandscapeWidth();
	var hgt = LandscapeHeight();
	// Create a blocking line at each of the sides for n minutes.
	AttackBarrier->BlockLine(wdt / 3, 0, wdt / 3, hgt, 36 * 60 * minutes);
	AttackBarrier->BlockLine(2 * wdt / 3, 0, 2 * wdt / 3, hgt, 36 * 60 * minutes);
	return;
}