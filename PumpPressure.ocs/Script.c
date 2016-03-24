/**
	Pump Pressure
	Opposing teams have their bases in nearby caves, where from the top 
	liquids rain into the caves. Teams can dispose of the liquids by pumping it 
	to the other team, increasing the amount of rain on the enemy's side. The 
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
	CreateObject(Goal_Melee);
	CreateObject(Rule_NoBorders);
	CreateObject(Rule_Domination);
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
	var nr_areas = GetLength(base_list);
	InitVegetation(nr_areas);
	InitEnvironment(nr_areas);
	InitAnimals(nr_areas);
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
	SetPlayerZoomByViewRange(plr, 500, 350, PLRZOOM_LimitMax);
	SetPlayerViewLock(plr, true);
	
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
		{def = Flagpole, amount = 1, basement = true},
		{def = WindGenerator, amount = 1, basement = true},
		{def = ToolsWorkshop, amount = 1, basement = true, contents = [[Wood, 8], [Metal, 8], [Rock, 8]]},
		{def = ChemicalLab, amount = 1, basement = true, contents = [[Firestone, 8], [Coal, 4], [Wood, 4]]},
		{def = Elevator, amount = 1, basement = true},
		{def = Pump, amount = 1, basement = true},
		{def = Lorry, amount = 1, contents = [[Wood, 6], [Metal, 4], [Rock, 4], [Loam, 3], [Hammer, 2], [Axe, 2], [Pipe, 2], [WallKit, 1]]},
		{def = Cannon, amount = 1, contents = [[PowderKeg, 1]]}
	];
	
	// If team not yet initialize, do the startup objects and flagpole creation.
	if (!team_init[team - 1])
	{
		team_init[team - 1] = true;
		var base = base_list[team - 1];
		var x = base[0];
		var y = base[1];
		SetWealth(plr, 100);
		// Give a flagpole to prevent elimination.
		var flag = CreateConstruction(Flagpole, x, y, plr, 100, true);
		var basement = CreateObject(Basement, x, flag->GetY() + flag->GetBottom() + 4, plr);
		basement->SetParent(flag);
		// Create doors which autmatically open.
		var door;
		door = CreateObjectAbove(StoneDoor, x - 196, y, plr);
		door->SetAutoControl();
		ClearFreeRect(door->GetX() - 4, door->GetY() - 52, 8, 72);
		door = CreateObjectAbove(StoneDoor, x + 196, y, plr);
		door->SetAutoControl();
		ClearFreeRect(door->GetX() - 4, door->GetY() - 52, 8, 72);
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

private func InitVegetation(int nr_areas)
{
	// Some grass is always nice.
	PlaceGrass(85);
	
	// Some mushrooms to regain health.
	Mushroom->Place(10 * nr_areas);
	Fern->Place(10 * nr_areas);
	Branch->Place(5 * nr_areas);
	Trunk->Place(2 * nr_areas);	
	
	// Trees for wood.
	LargeCaveMushroom->Place(10 * nr_areas, nil, { terraform = false });
	Tree_Coniferous->Place(6 * nr_areas);
	Tree_Coniferous2->Place(2 * nr_areas);
	Tree_Coniferous3->Place(2 * nr_areas);
	
	// If water, place some coral and seaweed.
	if (SCENPAR_LiquidType == 0)
	{
		Seaweed->Place(6 * nr_areas);
		Coral->Place(4 * nr_areas);
	}
	
	// Some objects in the earth.	
	PlaceObjects(Metal, 5 * nr_areas, "Earth");
	PlaceObjects(Wood, 5 * nr_areas, "Earth");
	PlaceObjects(Firestone, 10 * nr_areas, "Earth");
	PlaceObjects(Rock, 10 * nr_areas, "Earth");
	PlaceObjects(Loam, 10 * nr_areas, "Earth");
	return;
}

private func InitEnvironment(int nr_areas)
{
	// Make liquids boiling if possible.
	if (SCENPAR_LiquidType == 1)
		BoilingAcid->Place();
	if (SCENPAR_LiquidType == 2)
		BoilingLava->Place();
	// Adjust sky according to liquid type.
	if (SCENPAR_LiquidType == 1)
		SetSkyAdjust(RGBa(225, 255, 205, 191), RGB(63, 200, 0));
	if (SCENPAR_LiquidType == 2)
		SetSkyAdjust(RGBa(255, 185, 185, 191), RGB(220, 43, 0));
	return;
}

private func InitAnimals(int nr_areas)
{
	// Fish if material is liquid.
	if (SCENPAR_LiquidType == 0)
	{
		Fish->Place(8 * nr_areas);
		Piranha->Place(2 * nr_areas);	
	}
	// Chippies if material is acid.
	if (SCENPAR_LiquidType == 1)
	{
		//Chippie_Egg->Place(4 * nr_areas, Rectangle(0, LandscapeHeight() / 2, LandscapeWidth(), LandscapeHeight() / 2));
	}	
	// Zaps and mosquitos.
	Mosquito->Place(3 * nr_areas);
	Zaphive->Place(2 * nr_areas);
	return;
}

private func InitBlocking(int minutes)
{
	var nr_bases = GetLength(base_list);
	// Create areas where flagpoles can not claim ownership.
	var wdt = LandscapeWidth();
	Library_BlockOwnershipArea->BlockRectangle(Rectangle(0, 0, wdt, 80));
	for (var i = 1; i < nr_bases; i++)
		Library_BlockOwnershipArea->BlockRectangle(Rectangle(i * wdt / nr_bases - 32, 0, 64, 500));
	// Don't do blocking if time is zero.
	if (!minutes)
		return;
	var time = 36 * 60 * minutes;
	// Create a blocking rectangle around each base for n minutes.
	for (var i = 0; i < nr_bases; i++)
	{
		var base = base_list[i];
		var x = base[0];
		var y = base[1];
		AttackBarrier->BlockLine(x - 188, y - 72, x - 188, y - 24, time);
		AttackBarrier->BlockLine(x + 188, y - 72, x + 188, y - 24, time);
		AttackBarrier->BlockLine(x - 212, 70 * 8, x - 212, 120 * 8, time);
		AttackBarrier->BlockLine(x + 212, 70 * 8, x + 212, 120 * 8, time);
	}
	// Create a countdown clock for the players showing when the barriers come down.
	GUI_Clock->CreateCountdown(time / 36);
	return;
}


/*-- Liquid Control --*/

private func InitLiquidControl()
{
	// Add an effect to control liquid flows.
	var effect = AddEffect("LiquidControl", nil, 100, 1);
	// Type of rain according to the scenario parameter.
	effect.rain_mat = "Water";
	if (SCENPAR_LiquidType == 1)
		effect.rain_mat = "Acid";
	if (SCENPAR_LiquidType == 2)
		effect.rain_mat = "DuroLava";
	// Amount of rain according to the scenario parameter.
	effect.rain_amount = SCENPAR_LiquidAmount;

	// Add areas to liquid control effect.
	var nr_areas = 2 * BoundBy(GetStartupTeamCount(), 2, 4);
	var exclude_width = 80;
	var include_width = LandscapeWidth() / nr_areas - exclude_width;
	effect.areas = [];
	for (var index = 0; index < (nr_areas / 2); index++)
	{
		var set_from = exclude_width / 2 + index * 2 * (include_width + exclude_width);
		var set_to_middle = set_from + include_width + exclude_width / 2 - 32;
		var set_from_middle = set_from + include_width + exclude_width / 2 + 32;
		var set_to = set_from + 2 * include_width + exclude_width;
		effect.areas[index] = {from = set_from, to_middle = set_to_middle, from_middle = set_from_middle, to = set_to};
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
		effect.basins[index] = {x = (2 * index + 1) * wdt / (2 * effect.nr_basins), y = 52};
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
			var x = RandomX(area.from, area.to_middle);
			if (!Random(2))
				x = RandomX(area.from_middle, area.to);
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
			// Determine area in which to insert the liquid. Cycle through the enemy basins.
			effect.basin_to_area[basin_nr]++;
			if (effect.basin_to_area[basin_nr] == basin_nr)
				effect.basin_to_area[basin_nr]++;
			if (effect.basin_to_area[basin_nr] >= effect.nr_basins)
				effect.basin_to_area[basin_nr] = 0;
			if (effect.basin_to_area[basin_nr] == basin_nr)
				effect.basin_to_area[basin_nr]++;
			var area = effect.areas[effect.basin_to_area[basin_nr]];
			var x = RandomX(area.from, area.to_middle);
			if (!Random(2))
				x = RandomX(area.from_middle, area.to);
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
