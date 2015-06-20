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
	//CreateObject(Goal_Melee);
	CreateObject(Rule_Domination);
	CreateObject(Rule_BaseRespawn);
	CreateObject(Rule_BuyAtFlagpole);
	CreateObject(Rule_TeamAccount);
	CreateObject(Rule_BaseMeleeLogging);
	CreateObject(Rule_Medals);
	
	// Scoreboard.
	Scoreboard_BaseMelee_Data->Init();
	
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
		{def = SteamEngine, amount = 1, contents = [[Coal, 8]]},
		{def = ToolsWorkshop, amount = 1, contents = [[Wood, 8], [Metal, 4]]},
		{def = ChemicalLab, amount = 1, contents = [[Firestone, 8], [Wood, 4], [Metal, 2]]},
		{def = Foundry, amount = 1, contents = [[Metal, 6]]},
		{def = Lorry, amount = 1, contents = [[Wood, 6], [Metal, 6], [Rock, 4], [Loam, 3], [Hammer, 2], [Axe, 2]]}
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
		var gate = CreateObjectAbove(StoneDoor, x + cave[2] - 20, y - 4, plr);
		gate->SetAutoControl(team);
		gate->CloseDoor();
		ClearFreeRect(x - cave[2] + 24, y - 72, 8, 72);
		var gate = CreateObjectAbove(StoneDoor, x - cave[2] + 28, y - 4, plr);
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
	GivePlayerBaseMeleeKnowledge(plr);
	// Give the player its base materials.
	GivePlayerBasicBaseMaterial(plr);
	if (SCENPAR_BaseMaterials >= 1)
		GivePlayerNormalBaseMaterial(plr, true);
	if (SCENPAR_BaseMaterials >= 2)
		GivePlayerAdvancedBaseMaterial(plr, true);
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
	LargeCaveMushroom->Place(100 + Random(30), nil, { terraform = false });
	for (var cave in cave_list)
		LargeCaveMushroom->Place(80 + Random(3), Rectangle(cave[0] - cave[2], cave[1] - cave[3], 2 * cave[2], 2 * cave[3]), { terraform = false });
	
	// Some mushrooms to regain health.
	Mushroom->Place(80);
	Fern->Place(60);
	
	// Some objects in the earth.	
	PlaceObjects(Metal, 15 + Random(10), "Earth");
	PlaceObjects(Wood, 15 + Random(10), "Earth");
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

private func InitBlocking(int minutes)
{
	if (!minutes)
		return;
	// Create a blocking rectangle around each cave for n minutes.
	for (var i = 0; i < 4; i++)
	{
		var cave = cave_list[i];
		var x = cave[0];
		var y = cave[1];
		var wdt = cave[2];
		var hgt = cave[3];
		AttackBarrier->BlockRectangle(x - wdt, y - hgt, 2 * wdt, 2 * hgt, 36 * 60 * minutes);
	}
}
