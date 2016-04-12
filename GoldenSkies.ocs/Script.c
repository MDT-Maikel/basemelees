/**
	Golden Skies
	Teams have their bases in on small islands below the golden skies.

	@author Maikel
*/


// List for storing the different bases.
static base_list;
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
	CreateObject(Rule_NoPowerNeed);
	CreateObject(Rule_Medals);
	
	// Scoreboard.
	Scoreboard_BaseMelee_Data->Init();
	
	// Rescale base coordinates with map zoom and shuffle them.
	var mapzoom = GetScenarioVal("MapZoom", "Landscape");
	for (var base in base_list)
	{
		base[0] *= mapzoom;
		base[1] *= mapzoom;	
	}
	ShuffleArray(base_list);

	// Team initialized variable should be a list.
	team_init = [false, false, false, false];
	
	// Initialize different parts of the scenario.
	var nr_areas = GetLength(base_list);
	InitVegetation(nr_areas);
	InitEnvironment(nr_areas);
	InitAnimals(nr_areas);
	InitBlocking(SCENPAR_AttackBarrier, nr_areas);
	return;
}


/*-- Player Initiliaztion --*/

protected func InitializePlayer(int plr)
{
	// Get player team.
	var team = GetPlayerTeam(plr);
	if (team == 0)
		return;
		
	// Check whether the island is available for this team.
	// It may not have been generated if this team was not available at startup.
	if (!base_list[team - 1])
		return EliminatePlayer(plr);
	
	// Set a strict zoom range.
	SetPlayerZoomByViewRange(plr, 800, 600, PLRZOOM_LimitMax);
	SetPlayerViewLock(plr, true);
	
	// Position crew and give them a shovel.
	var i, crew;
	for (i = 0; crew = GetCrew(plr, i); ++i)
	{
		var base = base_list[team - 1];
		var x = base[0] + RandomX(-40, 40);
		var y = base[1] - 11;
		crew->SetPosition(x, y);
		crew->CreateContents(Shovel);
	}
	
	// If team not yet initialize, do the startup objects and flagpole creation.
	if (!team_init[team - 1])
	{
		team_init[team - 1] = true;
		var base = base_list[team - 1];
		var x = base[0];
		var y = base[1];
		SetWealth(plr, 100);
		// Give a flagpole to prevent elimination.
		CreateObjectAbove(Flagpole, x + 26, y, plr);
		// Give an elevator to use the base.
		var elevator = CreateObjectAbove(Elevator, x - 19, y, plr);
		elevator->CreateShaft(72);
		// Create doors which autmatically open.
		var door;
		door = CreateObjectAbove(StoneDoor, x - 68, y + 48, plr);
		door->SetAutoControl();
		ClearFreeRect(door->GetX() - 4, door->GetY() - 52, 8, 72);
		door = CreateObjectAbove(StoneDoor, x + 68, y + 48, plr);
		door->SetAutoControl();
		ClearFreeRect(door->GetX() - 4, door->GetY() - 52, 8, 72);
		// Place a catapult at the platform.
		CreateObjectAbove(Catapult, x + (2 * Random(2) - 1) * 100, y + 46, plr);
		// Place a lorry with materials.
		var lorry = CreateObjectAbove(Lorry, x - 19, y - 2, plr);
		lorry->CreateContents(Axe, 2);
		lorry->CreateContents(Shovel, 2);
		lorry->CreateContents(Hammer, 2);
		lorry->CreateContents(Pickaxe, 2);
		lorry->CreateContents(Dynamite, 4);
	}	
	
	// Give player their knowledge.
	GivePlayerBaseMeleeKnowledge(plr);
	// Give the player its base materials.
	for (var count = 0; count < 2; count++)
	{
		GivePlayerBasicBaseMaterial(plr, true);
		GivePlayerNormalBaseMaterial(plr, true);
		GivePlayerAdvancedBaseMaterial(plr, true);
	}
	GivePlayerSpecificBaseMaterial(plr, [[Airplane, 1, 1]], true);
	return;	
}

/*-- Scenario Initiliaztion --*/

private func InitVegetation(int nr_areas)
{
	// Some grass is always nice.
	Grass->Place(85);
	
	// Cave mushrooms and coconut trees provide wood.
	Tree_Deciduous->Place(16 * nr_areas);
	Tree_Coniferous2->Place(2 * nr_areas);
	
	// Some vegetation.
	Mushroom->Place(10 * nr_areas);
	Fern->Place(10 * nr_areas);
	Branch->Place(8 * nr_areas);
	Trunk->Place(2 * nr_areas);
	Flower->Place(4 * nr_areas);
	Cotton->Place(2 * nr_areas);
	Wheat->Place(6 * nr_areas);
	
	// Vines on the islands and the gold.
	Vine->Place(14 * nr_areas);
	Vine->Place(3 * nr_areas, nil, {min_dist = 80, attach_material = Loc_Material("Gold")});
	
	// Some objects in the earth.	
	PlaceObjects(Metal, 3 * nr_areas, "Earth");
	PlaceObjects(Wood, 3 * nr_areas, "Earth");
	PlaceObjects(Firestone, 8 * nr_areas, "Earth");
	PlaceObjects(Rock, 6 * nr_areas, "Earth");
	PlaceObjects(Loam, 6 * nr_areas, "Earth");
	
	// Place some diamonds as extra clunker source.
	Diamond->Place(2 * nr_areas, nil, {cluster_size = 1});
	return;
}

private func InitEnvironment(int nr_areas)
{
	// Adjust sky color.
	SetSkyAdjust(RGBa(255, 205, 135, 180), RGB(228, 145, 32));
	return;
}

private func InitAnimals(int nr_areas)
{
	// Some flying animals.
	Mosquito->Place(3 * nr_areas);
	Zaphive->Place(2 * nr_areas);
	Butterfly->Place(5 * nr_areas);
	return;
}

private func InitBlocking(int minutes, int nr_areas)
{
	// Don't do blocking if time is zero.
	if (!minutes)
		return;
	var wdt = LandscapeWidth();
	var hgt = LandscapeHeight();
	var time = 36 * 60 * minutes;
	// Create a blocking line between each base for n minutes.
	for (var i = 0; i < nr_areas; i++)
	{
		var x1 = i * wdt / nr_areas + 40;
		var x2 = (i + 1) * wdt / nr_areas - 40;
		AttackBarrier->BlockLine(x1, 0, x1, hgt, 36 * 60 * minutes);
		AttackBarrier->BlockLine(x2, 0, x2, hgt, 36 * 60 * minutes);
	}
	// Create a countdown clock for the players showing when the barriers come down.
	GUI_Clock->CreateCountdown(time / 36);
	return;
}
