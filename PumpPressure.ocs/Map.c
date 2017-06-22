/**
	Pump Pressure
	Teams in different areas need to pump liquids to the other area to keep the pressure on.
	
	@authors Maikel
*/

#include Library_Map


// List for storing the different main islands.
//static base_list; // defined in Script.c

// Called be the engine: draw the complete map here.
protected func InitializeMap(proplist map)
{
	// Number of bases and map size depends on the number of active teams.
	var nr_teams = GetStartupTeamCount();
	nr_teams = BoundBy(nr_teams, 2, 4);
	
	// Map settings.
	var base_width = 60;    // Width of the base.
	var base_height = 84;   // Height of the base including resources.
	var ground_height = 40;	// Height of the resources patch below the base.
	var lake_height = 32;	// Height of the underground lake.
	var caves_height = 28;  // Height of the resource caves.
	var map_height = base_height + lake_height + caves_height;
	var lake_bottom = map_height - caves_height;
	
	
	// Set the map size.
	map->Resize(base_width * nr_teams, map_height);
	
	// Construct the underground.
	DrawUnderground(map, ground_height, lake_height, lake_bottom);
	
	// Construct the material caves.
	DrawMaterialCaves(map, caves_height, base_width);
	
	// Construct the bases.
	base_list = [];
	var bases = ConstructBases(map, nr_teams, base_height);
	for (var base in bases)
	{
		PushBack(base_list, [base.X + base.Wdt / 2, ground_height]);
		DrawBase(map, base, ground_height, base_height);
	}
	
	// Fix the remaining open liquid borders.
	FixLiquidBorders();

	// Return true to tell the engine a map has been successfully created.
	return true;
}

public func ConstructBases(proplist map, int nr_teams, int base_height)
{
	var bases = [];
	var width = map.Wdt / nr_teams;
	for (var i = 1; i <= nr_teams; i++)
	{
		var base = {Algo = MAPALGO_Rect, X = (i - 1) * width, Y = 0, Wdt = width, Hgt = base_height};
		PushBack(bases, base);
	}
	return bases;
}

public func DrawBase(proplist map, proplist base, int ground_height, int base_height)
{
	// Draw ground and surface.
	var ground = {Algo = MAPALGO_Rect, X = 0, Y = ground_height, Wdt = map.Wdt, Hgt = map.Hgt - ground_height};
	ground = {Algo = MAPALGO_And, Op = [ground, base]};
	DrawGround(ground);
	var surface = {Algo = MAPALGO_Rect, X = 0, Y = ground_height, Wdt = map.Wdt, Hgt = 3};
	surface = {Algo = MAPALGO_Or, Op = [surface, {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [8, 8], Scale = [8, 8], Iterations = 2, Op = surface}]};
	surface = {Algo = MAPALGO_And, Op = [ground, surface]};
	Draw("Earth", surface);
	DrawMaterial("Earth-earth_root", surface, 5, 20);
	DrawMaterial("Earth-earth_spongy", surface, 5, 20);
	DrawMaterial("Sand", surface, 5, 20);

	// Draw brick border, which is everrock underground.
	var border = {Algo = MAPALGO_Border, Left = 4, Right = 4, Top = 2, Op = base};
	border = {Algo = MAPALGO_And, Op = [border, {Algo = MAPALGO_Rect, X = 0, Y = 0, Wdt = map.Wdt, Hgt = base_height - 10}]};
	Draw("Brick", border);
	var border_everrock = {Algo = MAPALGO_Rect, X = 0, Y = ground_height + 3, Wdt = map.Wdt, Hgt = base_height - ground_height - 10};
	border_everrock = {Algo = MAPALGO_And, Op = [border_everrock, border]};
	border_everrock = {Algo = MAPALGO_Or, Op = [border_everrock, {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = 4, Scale = 4, Iterations = 2, Op = border_everrock}]};
	Draw("Everrock", border_everrock);

	// Draw side entrances.
	var brick = {Algo = MAPALGO_Rect, X = base.X + 4, Y = ground_height - 10, Wdt = 3, Hgt = 12};
	brick = {Algo = MAPALGO_Or, Op = [brick, {Algo = MAPALGO_Rect, X = base.X + base.Wdt - 7, Y = ground_height - 10, Wdt = 3, Hgt = 12}]};
	Draw("Brick", brick);
	var tunnel = {Algo = MAPALGO_Rect, X = base.X + 2, Y = ground_height - 4, Wdt = 5, Hgt = 4};
	tunnel = {Algo = MAPALGO_Or, Op = [tunnel, {Algo = MAPALGO_Rect, X = base.X + base.Wdt - 7, Y = ground_height - 4, Wdt = 5, Hgt = 4}]};
	tunnel = {Algo = MAPALGO_Or, Op = [tunnel, {Algo = MAPALGO_Rect, X = base.X + 5, Y = ground_height - 9, Wdt = 1, Hgt = 5}]};
	tunnel = {Algo = MAPALGO_Or, Op = [tunnel, {Algo = MAPALGO_Rect, X = base.X + base.Wdt - 6, Y = ground_height - 9, Wdt = 1, Hgt = 5}]};
	Draw("Tunnel", tunnel);
	var granite = {Algo = MAPALGO_Rect, X = base.X, Y = ground_height - 4, Wdt = 2, Hgt = 4};
	granite = {Algo = MAPALGO_Or, Op = [granite, {Algo = MAPALGO_Rect, X = base.X + base.Wdt - 2, Y = ground_height - 4, Wdt = 2, Hgt = 4}]};
	Draw("Granite", granite);
	return;
}

public func DrawUnderground(proplist map, int ground_height, int lake_height, int lake_bottom)
{
	// Draw the standard materials.
	var underground = {Algo = MAPALGO_Rect, X = 0, Y = ground_height + 10, Wdt = map.Wdt, Hgt = lake_bottom - ground_height - 10};
	DrawGround(underground);
	
	// Draw a liquid lake from the bottom based on the scenario settings.
	var underground_area = {Algo = MAPALGO_Rect, X = 0, Y = lake_bottom - lake_height, Wdt = map.Wdt, Hgt = lake_height};
	underground_area = {Algo = MAPALGO_Or, Op = [underground_area, {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = 8, Scale = 8, Iterations = 4, Op = underground_area}]};
	Draw("Tunnel", underground_area);
	var lake = {Algo = MAPALGO_And, Op = [underground_area, {Algo = MAPALGO_Rect, X = 0, Y = lake_bottom - lake_height + 8, Wdt = map.Wdt, Hgt = lake_height - 8}]};
	Draw(["Water", "Acid", "DuroLava", "Oil"][SCENPAR_LiquidType], lake);
	var lake_islands = {Algo = MAPALGO_RndChecker, Ratio = 28, Wdt = 4, Hgt = 4};
	lake_islands = {Algo = MAPALGO_Turbulence, Iterations = 4, Op = lake_islands};
	lake_islands = {Algo = MAPALGO_And, Op = [lake_islands, underground_area]};
	var lake_roof = {Algo = MAPALGO_Border, Wdt = [-2, 1], Op = underground_area};
	var granite_area = {Algo = MAPALGO_Or, Op = [lake_roof, lake_islands]};
	Draw("Granite", granite_area);
	DrawMaterial("Rock", granite_area);
	// Draw some gems to provide more clunkers to the players.
	var gems = {Algo = MAPALGO_Rect, X = 0, Y = lake_bottom - lake_height / 2, Wdt = map.Wdt, Hgt = lake_height / 2};
	gems = {Algo = MAPALGO_And, Op = [gems, granite_area]};
	DrawMaterial("Ruby", gems, [6, 4], 20);
	DrawMaterial("Amethyst", gems, [6, 4], 20);
	return;
}

public func DrawMaterialCaves(proplist map, int cave_height, int base_width)
{
	// Draw background earth.
	var ground = {Algo = MAPALGO_Rect, X = 0, Y = map.Hgt - cave_height, Wdt = map.Wdt, Hgt = cave_height};
	Draw("Earth", ground);
	DrawMaterial("Earth-earth_root", ground, 5, 20);
	DrawMaterial("Earth-earth_spongy", ground, 5, 20);
	
	// Draw permanent materials as an infinite source for battle.
	DrawMaterial("Coal:Coal", ground, 4, 3);
	DrawMaterial("Ore:Ore", ground, 3, 3);
	DrawMaterial("Gold:Gold", ground, 3, 3);
	DrawMaterial("Rock:Rock", ground, 3, 3);
	
	// Draw larger tunnels
	DrawMaterial("Tunnel", ground, [7, 4], 30);
	DrawMaterial("Tunnel", ground, [2, 8], 10);
	
	// The top and bottom parts are a patch of everrock with one entrance per team.
	var border = {Algo = MAPALGO_Rect, X = 0, Y = map.Hgt - cave_height, Wdt = map.Wdt, Hgt = 4};
	border = {Algo = MAPALGO_Or, Op = [border, {Algo = MAPALGO_Rect, X = 0, Y = map.Hgt - 2, Wdt = map.Wdt, Hgt = 2}]};
	var entrance_width = 8;
	var border_entrance = {Algo = MAPALGO_Lines, X = base_width - entrance_width, Y = 0, Distance = base_width, OffX = -base_width / 2 + entrance_width / 2};
	border_entrance = {Algo = MAPALGO_Or, Op = [border_entrance, {Algo = MAPALGO_Rect, X = 0, Y = map.Hgt - 6, Wdt = map.Wdt, Hgt = 6}]};
	border = {Algo = MAPALGO_And, Op = [border, border_entrance]};
	border = {Algo = MAPALGO_Or, Op = [border, {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [8, 12], Scale = [8, 24], Iterations = 4, Op = border}]};
	Draw("Everrock", border);
	
	// Infinite wells at the bottom with material dependent on type of rain.
	var wells = {Algo = MAPALGO_RndChecker, Seed = Random(65536), Ratio = 8, Wdt = 2, Hgt = 20};
	wells = {Algo = MAPALGO_Or, Op = [wells, {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = 10, Scale = 10, Iterations = 4, Op = wells}]};
	wells = {Algo = MAPALGO_And, Op = [wells, border, {Algo = MAPALGO_Rect, X = 0, Y = map.Hgt - 4, Wdt = map.Wdt, Hgt = 4}]};
	Draw(["Oil:Oil", "Oil:Oil", "Water:Water", "Water:Water"][SCENPAR_LiquidType], wells);
	return;
}

public func DrawGround(proplist ground)
{
	Draw("Earth", ground);
	DrawMaterial("Earth-earth_root", ground, [5, 2], 12);
	DrawMaterial("Earth-earth_spongy", ground, 2, 16);
	DrawMaterial("Coal", ground, 3, 5);
	DrawMaterial("Firestone", ground, 4, 3);
	DrawMaterial("Ore", ground, 3, 3);
	DrawMaterial("Gold", ground, 3, 3);
	DrawMaterial("Rock", ground, [5, 2], 4);
	DrawMaterial("Tunnel", ground, [7, 4], 20);
	DrawMaterial(["Water", "Acid", "DuroLava", "Oil"][SCENPAR_LiquidType], ground, [6,4], 2);
	DrawMaterial("Coal", ground, 3, 4);
	DrawMaterial("Firestone", ground, 4, 3);
	DrawMaterial("Ore", ground, 3, 3);
	return;
}
