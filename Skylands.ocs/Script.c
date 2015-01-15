/**
	Skylands
	Four teams battle for the domination of the sky, each of them start on a different
	large sky island and need to destroy the enemy bases. 

	@author Maikel
*/


// List for storing the different main islands.
static island_list;
// List for storing whether a team has been initialized already.
static team_init;

protected func Initialize()
{
	// Goals & Rules.
	CreateObject(Goal_Melee);
	CreateObject(Rule_NoBorders);
	CreateObject(Rule_Domination);
	CreateObject(Rule_BaseRespawn);
	CreateObject(Rule_BuyAtFlagpole);
	CreateObject(Rule_TeamAccount);
	CreateObject(Rule_BaseMeleeLogging);
	CreateObject(Rule_Medals);
	
	// Rescale island coordinates with map zoom and shuffle them.
	var mapzoom = GetScenarioVal("MapZoom", "Landscape");
	for (var island in island_list)
	{
		island[0] *= mapzoom;
		island[1] *= mapzoom;	
	}
	ShuffleArray(island_list);
	// Team initialized variable should be a list.
	team_init = [false, false, false, false];

	// Initialize different parts of the scenario.
	InitVegetation();
	InitEnvironment();
	InitBlocking();
	return;
}

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
		{def = Elevator, amount = 1},
		{def = Lorry, amount = 1, contents = [[Wood, 6], [Metal, 6], [Rock, 4], [Loam, 3], [Hammer, 2], [Axe, 2]]}
	];
	
	// If team not yet initialize, do the startup objects and flagpole creation.
	if (!team_init[team - 1])
	{
		team_init[team - 1] = true;
		var island = island_list[team - 1];
		var x = island[0];
		var y = FindHeight(x, island[1]);
		SetWealth(plr, 200);
		CreateConstruction(Flagpole, x, y, plr, 100, true);		
		CreateBaseMenu(GetCrew(plr, 0), base_objects, Rectangle(x - 300, y - 120, 600, 440));
	}
	
	// Position crew and give them a shovel.
	var i, crew;
	for (i = 0; crew = GetCrew(plr, i); ++i)
	{
		var island = island_list[team - 1];
		var x = island[0] + RandomX(-50, 50);
		var y = FindHeight(x, island[1]) - 11;
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

private func FindHeight(int x, int y)
{
	while (!GBackSemiSolid(x, y) && y < LandscapeHeight())
		y++;
	return y;
}


/*-- Scenario Initiliaztion --*/

private func InitVegetation()
{
	// Some grass is always nice.
	PlaceGrass(85);
	
	// Cave mushrooms and coconut trees provide wood.
	LargeCaveMushroom->Place(30, nil, { terraform = false });
	Tree_Coconut->Place(80);
	
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
	// Set time to almost night and have stars.	
	CreateObject(Environment_Celestial);
	var time = CreateObject(Environment_Time);
	time->SetTime(20 * 60 + 15);
	time->SetCycleSpeed(0);
	
	// Clouds and rain.
	Cloud->Place(15);
	Cloud->SetPrecipitation("Water", 50);
	for (var cloud in FindObjects(Find_ID(Cloud)))
	{
		while (cloud->RemoveVertex())
			/* Empty */;
		// Make some clouds appear in the foreground with high alpha.
		if (Random(3) == 0)
		{
			cloud.Plane = 600;
			cloud->SetCloudAlpha(40);
		}			
	}
	
	// Set a certain parallax.
	SetSkyParallax(0, 20, 20);
	
	// Disasters: meteors, lightning and earthquakes are controlled by an effect.
	AddEffect("IntDistasterControl", nil, 100);
	return;
}

global func FxIntDistasterControlStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	
	effect.count = 0;
	// Call the timer every five minutes.
	effect.Interval = 36 * 60 * 5;
	// Set disasters to zero.
	Meteor->SetChance(0);
	Cloud->SetLightning(0);
	Earthquake->SetChance(0);
	return FX_OK;
}

global func FxIntDistasterControlTimer(object target, proplist effect, int time)
{
	// Increase counter.
	effect.count++;

	// Increase chance for Disasters.
	Meteor->SetChance(2 * effect.count);
	Cloud->SetLightning(2 * effect.count);
	Earthquake->SetChance(2 * effect.count);
	
	// Remove effect if above some maximum value.
	if (effect.count >= 20)
		return FX_Execute_Kill;
	return FX_OK;
}

private func InitBlocking()
{
	// Create a blocking rectangle around each island for 4 minutes.
	for (var i = 0; i < 4; i++)
	{
		var island = island_list[i];
		var x = island[0];
		var y = FindHeight(x, island[1]);
		AttackBarrier->BlockRectangle(x - 300, y - 120, 600, 440, 36 * 60 * 4);
	}
	return;
}