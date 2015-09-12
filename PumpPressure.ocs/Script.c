/**
	Pump Pressure
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
	InitLiquidControl();
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
	PlaceObjects(Rock, 30 + Random(5), "Earth");
	PlaceObjects(Loam, 30 + Random(5), "Earth");
	return;
}

private func InitEnvironment()
{
	// Make liquids boiling if possible.
	if (SCENPAR_LiquidMaterial == 1)
		BoilingAcid->Place();
	if (SCENPAR_LiquidMaterial == 2)
		BoilingLava->Place();
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
		AttackBarrier->BlockLine(x - 212, 70 * 8, x - 212, 120 * 8, time);
		AttackBarrier->BlockLine(x + 212, 70 * 8, x + 212, 120 * 8, time);
	}
	return;
}


/*-- Liquid Control --*/

private func InitLiquidControl()
{
	// Add an effect to control liquid flows.
	var effect = AddEffect("LiquidControl", nil, 100, 1);
	effect.rain_mat = "Water";
	if (SCENPAR_LiquidMaterial == 1)
		effect.rain_mat = "Acid";
	if (SCENPAR_LiquidMaterial == 2)
		effect.rain_mat = "DuroLava";
	effect.rain_amount = 10;

	// Add holes to the ceiling to allow for rain.
	var nr_areas = 2 * BoundBy(GetStartupTeamCount(), 2, 4);
	var exclude_width = 80;
	var include_width = LandscapeWidth() / nr_areas - exclude_width;

	for (var index = 0; index < nr_areas; index++)
	{
		var from_x = exclude_width / 2 + index * (include_width + exclude_width);
		var to_x = from_x + include_width;
		for (var x = from_x; x <= to_x; x += 8)
		{
			var y = 22;
			DrawMaterialQuad("Tunnel", x - 1, y, x + 1, y, x + 1, y + 12, x - 1, y + 12);
		}
	}
	
	// Add areas to liquid control effect.
	effect.areas = [];
	for (var index = 0; index < (nr_areas / 2); index++)
	{
		var set_from_x = exclude_width / 2 + index * 2 * (include_width + exclude_width);
		var set_to_x = set_from_x + 2 * include_width + exclude_width;
		effect.areas[index] = {from_x = set_from_x, to_x = set_to_x};
	}
	return;
}

global func FxLiquidControlStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	var wdt = LandscapeWidth();
	// Determine amount and location of liquid basins.
	effect.nr_basins = BoundBy(GetStartupTeamCount(), 2, 4);
	effect.basins = [];
	effect.basin_to_area = [];
	for (var index = 0; index < effect.nr_basins; index++)
	{
		effect.basins[index] = {x = (2 * index + 1) * wdt / (2 * effect.nr_basins), y = 68};
		CreateObject(Loam, effect.basins[index].x, effect.basins[index].y);
		effect.basin_to_area[index] = 0;
	}
	return FX_OK;
}

global func FxLiquidControlTimer(object target, proplist effect, int time)
{
	// Spread rain among the different area.
	if (Random(100) < effect.rain_amount)
	{
		for (var area in effect.areas)
		{
			var x = RandomX(area.from_x, area.to_x);
			var y = 20;
			InsertMaterial(Material(effect.rain_mat), x, y);
		}
	}

	// Transfer pumped liquids to enemy areas.
	var basin_nr = 0;
	for (var basin in effect.basins)
	{
		// Get all liquid in the basin.
		var mat;
		while ((mat = ExtractLiquid(basin.x, basin.y)) != -1)
		{
			// Determine area in which to insert the liquid.
			if (effect.basin_to_area[basin_nr] == basin_nr)
				effect.basin_to_area[basin_nr]++;
			if (effect.basin_to_area[basin_nr] >= effect.nr_basins)
				effect.basin_to_area[basin_nr] = 0;
			var area = effect.areas[effect.basin_to_area[basin_nr]];
			var x = RandomX(area.from_x, area.to_x);
			var y = 20;
			InsertMaterial(mat, x, y);
		}
		basin_nr++;
	}
	return FX_OK;
}

global func FxLiquidControlStop(target, proplist effect, int reason, bool temporary)
{
	return FX_OK;
}
