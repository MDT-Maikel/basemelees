/**
	Labyrinth
	Up to four teams battle for gems and victory in an underground labyrinth. 

	@author Maikel
*/


// List for storing the different large caves.
static cave_list;
// List for storing whether a team has been initialized already.
static team_init;

protected func Initialize()
{
	// Goals & Rules.
	CreateObject(Goal_Melee);
	CreateObject(Rule_Domination);
	CreateObject(Rule_BaseRespawn);
	CreateObject(Rule_BuyAtFlagpole);
	CreateObject(Rule_TeamAccount);
	CreateObject(Rule_BaseMeleeLogging);
	CreateObject(Rule_Medals);
	
	// Rescale island coordinates with map zoom and shuffle them.
	var mapzoom = GetScenarioVal("MapZoom", "Landscape");
	for (var cave in cave_list)
	{
		cave[0] *= mapzoom;
		cave[1] *= mapzoom;	
		cave[2] *= mapzoom;		
		cave[3] *= mapzoom;	
	}
	ShuffleArray(cave_list);
	// Team initialized variable should be a list.
	team_init = [false, false, false, false];
	
	// Initialize different parts of the scenario.
	InitVegetation();
	InitEnvironment();
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
		{def = SteamEngine, amount = 1, contents = [[Chunk_Coal, 8]]},
		{def = ToolsWorkshop, amount = 1, contents = [[Chunk_Wood, 8], [Chunk_Metal, 4]]},
		{def = ChemicalLab, amount = 1, contents = [[Firestone, 8], [Chunk_Wood, 4], [Chunk_Metal, 2]]},
		{def = Foundry, amount = 1, contents = [[Chunk_Metal, 6]]},
		{def = Armory, amount = 1, contents = [[Planks, 5], [Chunk_Metal, 6]]},
		{def = Lorry, amount = 1, contents = [[Chunk_Wood, 6], [Chunk_Metal, 6], [Planks, 4], [Chunk_Rock, 4], [Loam, 3], [Hammer, 2], [Axe, 2]]}
	];
	
	// If team not yet initialize, do the startup objects and flagpole creation.
	if (!team_init[team - 1])
	{
		team_init[team - 1] = true;
		var cave = cave_list[team - 1];
		var x = cave[0];
		var y = FindHeight(x, cave[1]);
		SetWealth(plr, 50);
		CreateConstruction(Flagpole, x, y, plr, 100, true);		
		CreateBaseMenu(GetCrew(plr, 0), base_objects, Rectangle(x - 200, y - 160, 400, 320));
		
		// Give the team also gates to their cave.
		ClearFreeRect(x + cave[2] - 24, y - 72, 8, 72);
		var gate = CreateObject(StoneDoor, x + cave[2] - 20, y - 4, plr);
		gate->SetAutoControl(team);
		gate->CloseDoor();
		ClearFreeRect(x - cave[2] + 24, y - 72, 8, 72);
		var gate = CreateObject(StoneDoor, x - cave[2] + 28, y - 4, plr);
		gate->SetAutoControl(team);
		gate->CloseDoor();
	}
	
	// Position crew and give them a shovel.
	var i, crew;
	for (i = 0; crew = GetCrew(plr, i); ++i)
	{
		var cave = cave_list[team - 1];
		var x = cave[0] + RandomX(-50, 50);
		var y = FindHeight(x, cave[1]) - 11;
		crew->SetPosition(x, y);
		crew->CreateContents(Shovel);
		crew->CreateContents(Pickaxe);
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
		DoHomebaseMaterial(plr, mat[0], mat[1]);
	// Set the base production.
	var prods = [[Clonk, 2], [Shovel, 1], [Pickaxe, 1], [Axe, 1], [Hammer, 1]];
	for (var prod in prods)
		DoHomebaseMaterial(plr, prod[0], prod[1]);
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
	// Cave mushrooms provide wood, extra place them in the large caves.
	LargeCaveMushroom->Place(100 + Random(30), nil, { terrafom = false });
	for (var cave in cave_list)
		LargeCaveMushroom->Place(80 + Random(3), Rectangle(cave[0] - cave[2], cave[1] - cave[3], 2 * cave[2], 2 * cave[3]), { terrafom = false });
	
	// Some mushrooms to regain health.
	Mushroom->Place(80);
	Fern->Place(60);
	
	// Some objects in the earth.	
	PlaceObjects(Chunk_Metal, 15 + Random(10), "Earth");
	PlaceObjects(Chunk_Wood, 15 + Random(10), "Earth");
	PlaceObjects(Loam, 50 + Random(30), "Earth");
	PlaceObjects(Firestone, 50 + Random(30), "Earth");

	return;
}

private func InitEnvironment()
{
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
	// Set disasters to initial value.
	Earthquake->SetChance(4);
	return FX_OK;
}

global func FxIntDistasterControlTimer(object target, proplist effect, int time)
{
	// Increase counter.
	effect.count++;

	// Increase chance for Disasters.
	Earthquake->SetChance(2 * effect.count + 4);
	
	// Remove effect if above some maximum value.
	if (effect.count >= 20)
		return FX_Execute_Kill;
	return FX_OK;
}