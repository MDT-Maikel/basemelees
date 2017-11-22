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
	CreateObject(Rule_BuyAtFlagpole);
	CreateObject(Rule_TeamAccount);
	CreateObject(Rule_BaseMeleeLogging);
	CreateObject(Rule_Medals);
	var relaunch_rule = GetRelaunchRule();
	relaunch_rule->SetBaseRespawn(true);
	relaunch_rule->SetFreeCrew(false);
	relaunch_rule->SetLastClonkRespawn(true);
	relaunch_rule->SetRespawnDelay(0);
	
	// Scoreboard.
	Scoreboard_BaseMelee_Data->Init();
	
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
	if (!island_list[team - 1])
		return EliminatePlayer(plr);
	
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
	Grass->Place(85);
	
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
	var time = CreateObject(Time);
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

private func InitBlocking(int minutes)
{
	if (!minutes)
		return;
	// Create a blocking rectangle around each island for n minutes.
	for (var i = 0; i < GetLength(island_list); i++)
	{
		var island = island_list[i];
		var x = island[0];
		var y = FindHeight(x, island[1]);
		AttackBarrier->BlockRectangle(x - 300, y - 120, 600, 440, 36 * 60 * minutes);
	}
	// Create a countdown clock for the players showing when the barriers come down.
	GUI_Clock->CreateCountdown(60 * minutes);
	return;
}