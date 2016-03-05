/**
	Pump Pressure
	Teams in different areas need to pump liquids to the other area to keep the pressure on.
	
	@authors Maikel
*/

#include Library_Map


// List for storing the different main islands.
static base_list;

// Called be the engine: draw the complete map here.
protected func InitializeMap(proplist map)
{
	// Number of bases and map size depends on the number of active teams.
	var nr_teams = GetStartupTeamCount();
	nr_teams = BoundBy(nr_teams, 2, 4);
	
	// Map settings.
	var base_width = 60;
	var base_height = 72;
	var map_height = 120;
	var ground_height = 40;
	var lake_height = 32;
	
	// Set the map size.
	map->Resize(base_width * nr_teams, map_height);
	
	// Construct the underground.
	DrawUnderground(map, ground_height, lake_height);
	
	// Construct the bases.
	base_list = [];
	var bases = ConstructBases(map, nr_teams, base_height);
	for (var base in bases)
	{
		PushBack(base_list, [base.X + base.Wdt / 2, ground_height]);
		DrawBase(map, base, ground_height);
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

public func DrawBase(proplist map, proplist base, int ground_height)
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
	Draw("Brick", border);
	var border_granite = {Algo = MAPALGO_Rect, X = 0, Y = ground_height + 3, Wdt = map.Wdt, Hgt = map.Hgt - ground_height - 10};
	border_granite = {Algo = MAPALGO_And, Op = [border_granite, border]};
	border_granite = {Algo = MAPALGO_Or, Op = [border_granite, {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = 4, Scale = 4, Iterations = 2, Op = border_granite}]};
	Draw("Everrock", border_granite);

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
	
	// Draw top liquid construction.
	var topline = {Algo = MAPALGO_Rect, X = base.X, Y = 3, Wdt = base.Wdt, Hgt = 1};
	Draw("Brick", topline);
	var basin = {Algo = MAPALGO_Rect, X = base.X + base.Wdt / 2 - 3, Y = 7, Wdt = 6, Hgt = 3};
	basin = {Algo = MAPALGO_And, Op = [basin, {Algo = MAPALGO_Not, Op = {Algo = MAPALGO_Rect, X = base.X + base.Wdt / 2 - 2, Y = 7, Wdt = 4, Hgt = 2}}]};
	Draw("Brick", basin);
	return;
}

public func DrawUnderground(proplist map, int ground_height, int lake_height)
{
	// Draw the standard materials.
	var underground = {Algo = MAPALGO_Rect, X = 0, Y = ground_height + 10, Wdt = map.Wdt, Hgt = map.Hgt - ground_height - 10};
	DrawGround(underground);
	
	// Draw a liquid lake from the bottom based on the scenario settings.
	var underground_area = {Algo = MAPALGO_Rect, X = 0, Y = map.Hgt - lake_height, Wdt = map.Wdt, Hgt = lake_height};
	underground_area = {Algo = MAPALGO_Or, Op = [underground_area, {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = 8, Scale = 8, Iterations = 4, Op = underground_area}]};
	Draw("Tunnel", underground_area);
	var lake = {Algo = MAPALGO_And, Op = [underground_area, {Algo = MAPALGO_Rect, X = 0, Y = map.Hgt - lake_height + 8, Wdt = map.Wdt, Hgt = lake_height - 8}]};
	Draw(["Water", "Acid", "DuroLava"][SCENPAR_LiquidType], lake);
	var lake_islands = {Algo = MAPALGO_RndChecker, Ratio = 28, Wdt = 4, Hgt = 4};
	lake_islands = {Algo = MAPALGO_Turbulence, Iterations = 4, Op = lake_islands};
	lake_islands = {Algo = MAPALGO_And, Op = [lake_islands, underground_area]};
	var lake_roof = {Algo = MAPALGO_Border, Wdt = [-2, 1], Op = underground_area};
	var granite_area = {Algo = MAPALGO_Or, Op = [lake_roof, lake_islands]};
	Draw("Granite", granite_area);
	DrawMaterial("Rock", granite_area);
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
	DrawMaterial(["Water", "Acid", "DuroLava"][SCENPAR_LiquidType], ground, [6,4], 2);
	DrawMaterial("Coal", ground, 3, 4);
	DrawMaterial("Firestone", ground, 4, 3);
	DrawMaterial("Ore", ground, 3, 3);
	return;
}
