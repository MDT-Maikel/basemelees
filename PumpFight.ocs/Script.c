/**
	Pump Fight
	Two opposing teams have their bases in nearby caves, where from the top 
	acid rains into the caves. Teams can dispose of the acid by pumping it 
	to the other team, increasing the amount of acid rain on their side. The 
	only connection between the two teams is through a thick granite wall
	or a deep underground passage.

	@author Maikel
*/


// List for storing the different bases.
static base_list;
// List for storing whether a team has been initialized already.
static team_init;

protected func Initialize()
{
	// Goals & Rules.
	//CreateObject(Goal_Melee);
	CreateObject(Rule_NoBorders);
	//CreateObject(Rule_Domination);
	CreateObject(Rule_BaseRespawn);
	CreateObject(Rule_BuyAtFlagpole);
	CreateObject(Rule_TeamAccount);
	CreateObject(Rule_BaseMeleeLogging);
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
		
	// Check whether the island is available for this team.
	// It may not have been generated if this team was not available at startup.
	if (!base_list[team - 1])
		return EliminatePlayer(plr);
	
	// Set a strict zoom range.
	//SetPlayerZoomByViewRange(plr, 500, 350, PLRZOOM_LimitMax);
	//SetPlayerViewLock(plr, true);
	SetFoW(false, plr);
	
	// Position crew and give them a shovel.
	var i, crew;
	for (i = 0; crew = GetCrew(plr, i); ++i)
	{
		var base = base_list[team - 1];
		var x = base[0] + RandomX(-50, 50);
		var y = base[1] - 11;
		crew->SetPosition(x, y);
		crew->CreateContents(Shovel);
	}
	
	// Base startup objects.
	var base_objects = [
		{def = Flagpole, amount = 1},
		{def = WindGenerator, amount = 1},
		{def = ToolsWorkshop, amount = 1, contents = [[Wood, 8], [Metal, 4]]},
		{def = ChemicalLab, amount = 1, contents = [[Firestone, 8], [Wood, 4], [Metal, 2]]},
		{def = Foundry, amount = 1, contents = [[Metal, 6]]},
		{def = Elevator, amount = 1},
		{def = Pump, amount = 1},
		{def = Lorry, amount = 1, contents = [[Wood, 6], [Metal, 6], [Rock, 4], [Loam, 3], [Hammer, 2], [Axe, 2], [Pipe, 2]]}
	];
	
	// If team not yet initialize, do the startup objects and flagpole creation.
	if (!team_init[team - 1])
	{
		team_init[team - 1] = true;
		var base = base_list[team - 1];
		var x = base[0];
		var y = base[1];
		SetWealth(plr, 100);
		CreateConstruction(Flagpole, x, y, plr, 100, true);	
		CreateObjectAbove(StoneDoor, x - 196, y, plr)->SetAutoControl();
		CreateObjectAbove(StoneDoor, x + 196, y, plr)->SetAutoControl();
		CreateBaseMenu(GetCrew(plr, 0), base_objects, Rectangle(x - 240, y - 240, 480, 480));
	}	
	
	// Give player their knowledge.
	GivePlayerBaseMeleeKnowledge(plr);
	// Give the player its base materials.
	GivePlayerBasicBaseMaterial(plr);
	if (SCENPAR_BaseMaterials >= 1)
		GivePlayerNormalBaseMaterial(plr, true);
	if (SCENPAR_BaseMaterials >= 2)
		GivePlayerAdvancedBaseMaterial(plr, true);
	return;	
}


/*-- Scenario Initiliaztion --*/

private func InitVegetation()
{
	// Some grass is always nice.
	PlaceGrass(85);
	
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

	return;
}

private func InitBlocking(int minutes)
{
	if (!minutes)
		return;
	// Create a blocking rectangle around each base for n minutes.
	for (var i = 0; i < GetLength(base_list); i++)
	{
		var base = base_list[i];
		var x = base[0];
		var y = base[1];
		var time = 36 * 60 * minutes;
		AttackBarrier->BlockLine(x - 188, y - 72, x - 188, y - 24, time);
		AttackBarrier->BlockLine(x + 188, y - 72, x + 188, y - 24, time);
		AttackBarrier->BlockLine(x - 212, 560, x - 212, 800, time);
		AttackBarrier->BlockLine(x + 212, 560, x + 212, 800, time);
	}
	return;
}