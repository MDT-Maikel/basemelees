/**
	Ruby Lake
	Scenario where two opposing teams each have their bases in a castle
	with a lake and ruby mountain in the middle.

	@author Maikel
*/


// List for storing whether a team has been initialized already.
static team_init;

public func Initialize()
{
	// Goals & Rules.
	CreateObject(Goal_Melee);
	CreateObject(Rule_Domination);
	CreateObject(Rule_BuyAtFlagpole);
	CreateObject(Rule_TeamAccount);
	CreateObject(Rule_BaseMeleeLogging);
	CreateObject(Rule_Medals);
	var construction_limit = CreateObject(Rule_ConstructionLimit);
	construction_limit->AddConstructionLimit(Flagpole, 3, true);
	var relaunch_rule = GetRelaunchRule();
	relaunch_rule->SetBaseRespawn(true);
	relaunch_rule->SetFreeCrew(false);
	relaunch_rule->SetLastClonkRespawn(true);
	relaunch_rule->SetRespawnDelay(0);
	
	// Scoreboard.
	Scoreboard_BaseMelee_Data->Init();
	
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
	SetPlayerZoomByViewRange(plr, 800, 600, PLRZOOM_LimitMax);
	SetPlayerViewLock(plr, true);
		
	// Position crew and give them a shovel.
	var i, crew;
	for (i = 0; crew = GetCrew(plr, i); ++i)
	{
		var x = 80 + RandomX(-20, 20);
		if (team == 2)
			x = LandscapeWidth() - x;
		var y = 616 - 11;
		crew->SetPosition(x, y);
		crew->CreateContents(Shovel);
	}
	
	// If team not yet initialize, take over the base.
	if (!team_init[team - 1])
	{
		team_init[team - 1] = true;
		SetWealth(plr, 250);
		var x = 0;
		if (team == 2)
			x = LandscapeWidth() / 2;
		for (var structure in FindObjects(Find_Or(Find_Category(C4D_Structure), Find_Func("IsFlagpole")), Find_InRect(x, 0, LandscapeWidth() / 2, LandscapeHeight())))
			structure->SetOwner(plr);
	}
	
	// Give player their knowledge and base materials.
	GivePlayerBaseMeleeKnowledge(plr);
	RemovePlayerSpecificKnowledge(plr, [WallKit]);
	// Give the player its base materials.
	GivePlayerBasicBaseMaterial(plr);
	if (SCENPAR_BaseMaterials >= 1)
		GivePlayerNormalBaseMaterial(plr, true);
	if (SCENPAR_BaseMaterials >= 2)
		GivePlayerAdvancedBaseMaterial(plr, true);
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
	var place_rect = Rectangle(wdt / 2 - 200, hgt / 2 - 200, 400, 400);
	Fish->Place(10, place_rect);
	Piranha->Place(4, place_rect);
	return;
}

private func InitVegetation()
{
	// Some grass is always nice.
	Grass->Place(85);
	
	// Some vegetation and mushrooms to regain health.
	Mushroom->Place(40);
	Fern->Place(30);
	Branch->Place(30);
	Trunk->Place(10);
	LargeCaveMushroom->Place(20, nil, { terraform = false });
	Seaweed->Place(16);
	Coral->Place(12);
	
	// Some objects in the earth.	
	PlaceObjects(Metal, 15 + Random(10), "Earth");
	PlaceObjects(Wood, 15 + Random(10), "Earth");
	PlaceObjects(Loam, 15 + Random(10), "Earth");
	PlaceObjects(Firestone, 30 + Random(5), "Earth");
	PlaceObjects(Dynamite, 10 + Random(5), "Earth");
	return;
}

private func InitEnvironment()
{
	// Set time to almost night and have stars.	
	var time = CreateObject(Time);
	time->SetTime(Random(24 * 60));
	time->SetCycleSpeed(120);

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
	// Create a blocking line at each of the castles for n minutes.
	AttackBarrier->BlockLine(wdt / 3, 0, wdt / 3, hgt, 36 * 60 * minutes);
	AttackBarrier->BlockLine(2 * wdt / 3, 0, 2 * wdt / 3, hgt, 36 * 60 * minutes);
	// Create a countdown clock for the players showing when the barriers come down.
	GUI_Clock->CreateCountdown(60 * minutes);
	return;
}