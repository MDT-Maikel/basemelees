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
// List for storing whether a base has been initialized already.
static base_init;

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
	
	// Rescale base coordinates with map zoom and shuffle them.
	var mapzoom = GetScenarioVal("MapZoom", "Landscape");
	for (var base in base_list)
	{
		base[0] *= mapzoom;
		base[1] *= mapzoom;	
	}
	ShuffleArray(base_list);

	// Base initialized variable should be a list.
	base_init = [false, false, false, false];
	
	// Initialize different parts of the scenario.
	var nr_areas = GetLength(base_list);
	InitVegetation(nr_areas);
	InitEnvironment(nr_areas);
	InitAnimals(nr_areas);
	InitBlocking(SCENPAR_AttackBarrier);
	InitResourceCaves(nr_areas);
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
	
	// Get the team base.
	var base_nr = TeamToBase(team);
		
	// Check whether the island is available for this team.
	// It may not have been generated if this team was not available at startup.
	if (!base_list[base_nr])
		return EliminatePlayer(plr);
	
	// Set a strict zoom range.
	SetPlayerZoomByViewRange(plr, 500, 350, PLRZOOM_LimitMax);
	SetPlayerViewLock(plr, true);
	
	// Position crew and give them a shovel.
	var i, crew;
	for (i = 0; crew = GetCrew(plr, i); ++i)
	{
		var base = base_list[base_nr];
		var x = base[0] + RandomX(-50, 50);
		var y = base[1] - 11;
		crew->SetPosition(x, y);
		crew->CreateContents(Shovel);
	}
	
	// Create base drain.
	CreateObject(BaseDrain, base[0], 22, plr);
	
	// Base startup objects.
	var base_objects = [
		{def = Flagpole, amount = 1, basement = true},
		{def = WindGenerator, amount = 1, basement = true},
		{def = ToolsWorkshop, amount = 1, basement = true, contents = [[Wood, 8], [Metal, 8], [Rock, 8]]},
		{def = ChemicalLab, amount = 1, basement = true, contents = [[Firestone, 8], [Coal, 4], [Wood, 4]]},
		{def = Elevator, amount = 1, basement = true},
		{def = Pump, amount = 1, basement = true, scen_callback = "OnPumpPlacement"},
		{def = Lorry, amount = 1, contents = [[Wood, 6], [Metal, 4], [Rock, 4], [Loam, 3], [Hammer, 2], [Axe, 2], [Pipe, 2], [WallKit, 1]]},
		{def = Cannon, amount = 1, contents = [[PowderKeg, 1]]}
	];
	
	// If team not yet initialize, do the startup objects and flagpole creation.
	if (!base_init[base_nr])
	{
		base_init[base_nr] = true;
		var base = base_list[base_nr];
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
		CreateBaseMenu(GetCrew(plr, 0), base_objects, Rectangle(x - 240, y - 240, 480, 400));
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

public func OnPumpPlacement(object pump, int plr)
{
	// Place a source pipe at the base of the pump.
	var source = pump->CreateObjectAbove(Pipe, 0, pump->GetBottom());
	source->ConnectPipeTo(pump, PIPE_STATE_Source);
	// Place a drain pipe and connect it to base drain.
	var drain = pump->CreateObjectAbove(Pipe, 0, pump->GetBottom());
	drain->ConnectPipeTo(pump, PIPE_STATE_Drain);
	var base_drain = FindObject(Find_ID(BaseDrain), Find_Owner(plr));
	drain->ConnectPipeTo(base_drain);
	return;
}


/*-- Scenario Initiliaztion --*/

private func InitVegetation(int nr_areas)
{
	// Some grass is always nice.
	Grass->Place(85);
	
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
	PlaceObjects(Metal, 6 * nr_areas, "Earth");
	PlaceObjects(Wood, 6 * nr_areas, "Earth");
	PlaceObjects(Firestone, 12 * nr_areas, "Earth");
	PlaceObjects(Rock, 12 * nr_areas, "Earth");
	PlaceObjects(Loam, 12 * nr_areas, "Earth");
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
	var bottom_rect = Rectangle(0, LandscapeHeight() / 2, LandscapeWidth(), LandscapeHeight() / 2);
	// Fish if material is water.
	if (SCENPAR_LiquidType == 0)
	{
		Fish->Place(8 * nr_areas, bottom_rect);
		Piranha->Place(2 * nr_areas, bottom_rect);
	}
	// Chippies if material is acid.
	if (SCENPAR_LiquidType == 1)
	{
		Chippie->Place(10 * nr_areas, bottom_rect);
	}
	// Lava cores if material is lava.
	if (SCENPAR_LiquidType == 2)
	{
		LavaCore->Place(2 * nr_areas, bottom_rect, {size_range = [30, 45]});
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
		AttackBarrier->BlockLine(x - 212, 560, x - 212, LandscapeHeight(), time);
		AttackBarrier->BlockLine(x + 212, 560, x + 212, LandscapeHeight(), time);
	}
	// Create a countdown clock for the players showing when the barriers come down.
	GUI_Clock->CreateCountdown(time / 36);
	return;
}

private func InitResourceCaves(int nr_areas)
{
	// Lorries with valuable stuff in the resource caves.
	for (var cnt = 0; cnt < nr_areas; cnt++)
	{
		var loc = FindLocation(Loc_InRect(0, LandscapeHeight() - 28 * 8, LandscapeWidth(), 28 * 8), Loc_Tunnel(), Loc_Space(10));
		if (!loc)
			continue;
		var lorry = CreateObject(Lorry, loc.x, loc.y);
		lorry->CreateContents(DynamiteBox, 5);
		lorry->CreateContents(Wood, 30);
		lorry->CreateContents(GoldBar, 5);	
	}
	return;
}


/*-- Liquid Control --*/

private func InitLiquidControl()
{
	// Add an effect to control liquid flows.
	var fx = AddEffect("LiquidControl", nil, 100, 1);
	// Type of rain according to the scenario parameter.
	fx.rain_mat = "Water";
	if (SCENPAR_LiquidType == 1)
		fx.rain_mat = "Acid";
	if (SCENPAR_LiquidType == 2)
		fx.rain_mat = "DuroLava";
	if (SCENPAR_LiquidType == 3)
		fx.rain_mat = "Oil";
	// Amount of rain according to the scenario parameter.
	fx.rain_amount = SCENPAR_LiquidAmount;
	
	
	// Add effect to check flooding and take measures.
	var fx_flooding = AddEffect("FloodingControl", nil, 100, 5);

	// Add areas to liquid control and flooding control effect.
	var nr_areas = 2 * BoundBy(GetStartupTeamCount(), 2, 4);
	var exclude_width = 80;
	var include_width = LandscapeWidth() / nr_areas - exclude_width;
	fx.areas = [];
	fx_flooding.areas = [];
	for (var index = 0; index < (nr_areas / 2); index++)
	{
		var set_from = exclude_width / 2 + index * 2 * (include_width + exclude_width);
		var set_to = set_from + 2 * include_width + exclude_width;
		fx.areas[index] = {from = set_from, to = set_to};
		fx_flooding.areas[index] = {from = set_from, to = set_to};
	}	
	return;
}

global func FxLiquidControlStart(object target, effect fx, int temporary)
{
	if (temporary)
		return FX_OK;
	var wdt = LandscapeWidth();
	// Determine amount and location of liquid basins.
	fx.nr_drains = BoundBy(GetStartupTeamCount(), 2, 4);
	fx.drains = [];
	fx.drain_to_area = [];
	for (var index = 0; index < fx.nr_drains; index++)
	{
		fx.drains[index] = {x = (2 * index + 1) * wdt / (2 * fx.nr_drains), y = 20};
		fx.drain_to_area[index] = 0;
	}
	return FX_OK;
}

global func FxLiquidControlTimer(object target, effect fx, int time)
{
	// Spread rain among the different area.
	if (Random(100) < fx.rain_amount)
	{
		for (var area in fx.areas)
		{
			var x = RandomX(area.from, area.to);
			var y = 20;
			InsertMaterial(Material(fx.rain_mat), x, y, 0, 10);
		}
	}

	// Transfer pumped liquids to enemy areas.
	var drain_nr = 0;
	for (var drain_pos in fx.drains)
	{
		var base_drain = FindObject(Find_ID(BaseDrain), Find_AtPoint(drain_pos.x, drain_pos.y));
		if (!base_drain)
		{
			drain_nr++;
			continue;
		}
		// Get all liquid in the base drain.
		var mat;
		while ((mat = base_drain->RemoveLiquid(nil, 1)[0]))
		{
			// Determine area in which to insert the liquid. Cycle through the enemy basins.
			fx.drain_to_area[drain_nr]++;
			if (fx.drain_to_area[drain_nr] == drain_nr)
				fx.drain_to_area[drain_nr]++;
			if (fx.drain_to_area[drain_nr] >= fx.nr_drains)
				fx.drain_to_area[drain_nr] = 0;
			if (fx.drain_to_area[drain_nr] == drain_nr)
				fx.drain_to_area[drain_nr]++;
			var area = fx.areas[fx.drain_to_area[drain_nr]];
			var x = RandomX(area.from, area.to);
			var y = 20;
			InsertMaterial(Material(mat), x, y, 0, 10);
		}
		drain_nr++;
	}
	return FX_OK;
}

global func FxLiquidControlStop(target, effect fx, int reason, bool temporary)
{
	return FX_OK;
}

global func FxFloodingControlTimer(object target, effect fx, int time)
{
	for (var area in fx.areas)
	{
		// Check if area is flooded.
		var flooded = true;
		for (var x = area.from; x < area.to; x++)
		{
			if (!GBackSemiSolid(x, 17))
			{
				flooded = false;
				break;
			}
		}
		// Launch meteor as punishment, this also breaks any type of protection players build.
		if (flooded && !Random(20))
			LaunchMeteor(RandomX(area.from, area.to), 20 + Random(8), 50, RandomX(-2, 2), RandomX(30, 40));		
	}
	return FX_OK;
}
