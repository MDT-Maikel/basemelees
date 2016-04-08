/**
	Golden Skies
	Teams have their bases in on small islands below the golden skies.
	
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
	var base_width = 128;
	var map_height = 92;
	var gold_height = 16;
	
	// Set the map size.
	map->Resize(base_width * nr_teams, map_height);
	
	// Construct the bases.
	base_list = [];
	for (var index = 0; index < nr_teams; index++)
	{
		var x = (2 * index + 1) * base_width / 2;
		var y = 3 * map.Hgt / 5;
		PushBack(base_list, [x, y]);
		CreateBase(map, x, y);
	}
	
	// Construct the golden sky.
	CreateGoldenSky(map, gold_height);
	
	// Construct some small islands.
	CreateIslands(map, base_list, nr_teams * 6, gold_height);

	// Return true to tell the engine a map has been successfully created.
	return true;
}

public func CreateBase(proplist map, int x, int y)
{
	// Draw area for flagpole and elevator.
	var area = {Algo = MAPALGO_Rect, X = x - 10, Y = y - 10, Wdt = 20, Hgt = 12};
	var area_bkg = {Algo = MAPALGO_Rect, X = x - 6, Y = y - 8, Wdt = 12, Hgt = 8};
	var area_brick = {Algo = MAPALGO_And, Op = [area, {Algo = MAPALGO_Not, Op = area_bkg}]};
	Draw("BrickSoft:Tunnel-brickback", area_brick);
	Draw("Tunnel-brickback", area_bkg);
	
	// Draw a platform for launching attacks from.
	var platform = {Algo = MAPALGO_Rect, X = x - 16, Y = y + 6, Wdt = 32, Hgt = 1};
	platform = {Algo = MAPALGO_Or, Op = [platform, {Algo = MAPALGO_Rect, X = platform.X + 1, Y = platform.Y + 1, Wdt = platform.Wdt - 2, Hgt = 1}]};
	var platform_bkg = {Algo = MAPALGO_Rect, X = x - 10, Y = y + 2, Wdt = 20, Hgt = 4};
	Draw("BrickSoft:Tunnel-brickback", platform);
	Draw("Tunnel-brickback", platform_bkg);
	
	// Draw some everrock around the base.
	var cover = {Algo = MAPALGO_Rect, X = x - 11, Y = y - 12, Wdt = 22, Hgt = 14};
	cover = {Algo = MAPALGO_Or, Op = [cover, {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = 10, Scale = 8, Iterations = 2, Op = cover}]};
	var block_area = {Algo = MAPALGO_Rect, X = area.X + 1, Y = area.Y + 1, Wdt = area.Wdt - 2, Hgt = area.Hgt - 1};
	block_area = {Algo = MAPALGO_Or, Op = [block_area, {Algo = MAPALGO_Rect, X = x - 16, Y = y + 1, Wdt = 32, Hgt = 8}]};
	cover = {Algo = MAPALGO_And, Op = [cover, {Algo = MAPALGO_Not, Op = block_area}]};
	var border = {Algo = MAPALGO_Border, Top = 1, Op = cover};
	Draw("Everrock:Tunnel", cover);
	Draw("Earth", border);
	return;
}

public func CreateGoldenSky(proplist map, int height)
{
	var base = {Algo = MAPALGO_Rect, X = 0, Y = 0, Wdt = map.Wdt, Hgt = height / 2};
	base = {Algo = MAPALGO_Or, Op = [base, {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = 12, Scale = 8, Iterations = 4, Op = base}]};
	var lines = {Algo = MAPALGO_Lines, X = 8, Y = 0, Wdt = 4};
	lines = {Algo = MAPALGO_And, Op = [lines, {Algo = MAPALGO_Rect, X = 0, Y = 0, Wdt = map.Wdt, Hgt = height}]};
	lines = {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = 12, Scale = 8, Iterations = 4, Op = lines};
	var total = {Algo = MAPALGO_Or, Op = [base, lines]};
	Draw("Gold", total);
	return;
}

public func CreateIslands(proplist map, array base_list, int amount, int gold_height)
{
	// Prepare a mask out of the map and the bases.
	var mask = map->CreateLayer();
	mask->Draw("Rock");
	for (var base in base_list)
		mask->Draw("Tunnel", {Algo = MAPALGO_Rect, X = base[0] - 25, Y = base[1] - 30, Wdt = 50, Hgt = 54});
	mask->Draw("Tunnel", {Algo = MAPALGO_Rect, X = 0, Y = 0, Wdt = map.Wdt, Hgt = gold_height + 12});	
	
	var border = 6;
	var island_dist = 26;
	// Loop over the amount of islands to find.
	for (var i = 0; i < amount; i++)
	{
		// Find new island position.
		var island_mask = {};
		if (!mask->FindPosition(island_mask, "Rock", [border, border * 3, map.Wdt - border * 2, map.Hgt - border * 4]))
			continue;
		var x = island_mask.X;
		var y = island_mask.Y;
		// Add new island to the mask.
		mask->Draw("Tunnel", {Algo = MAPALGO_Ellipsis, X = x, Y = y, Wdt = island_dist, Hgt = island_dist});
		// Create algorithm for new island.
		var size = RandomX(3, 4);
		var island = {Algo = MAPALGO_Ellipsis, X = x, Y = y, Wdt = size + 3, Hgt = size};
		island = {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [6, 6], Scale = [6, 6], Iterations = 4, Op = island};
		DrawRandomIsland(island);
	}
	return;
}

public func DrawRandomIsland(proplist island)
{
	var rnd = Random(2);
	// Mainly firestones with some earth and rock, dangerous!
	if (rnd == 0)
	{
		Draw("Firestone", island);
		DrawMaterial("Rock", island, 2, 40);
		DrawMaterial("Earth", island, 2, 32);
		var border = {Algo = MAPALGO_Border, Top = 1, Op = island};
		Draw("Earth", border);
		DrawMaterial("Sand", border, 4, 20);
		return;
	}
	// Island with rock, ore, coal and gold.
	if (rnd == 1)
	{
		Draw("Rock", island);
		DrawMaterial("Ore", island, 3, 50);
		DrawMaterial("Coal", island, 3, 50);
		DrawMaterial("Earth", island, 3, 25);
		var border = {Algo = MAPALGO_Border, Top = 1, Op = island};
		Draw("Earth", border);
		DrawMaterial("Earth-earth_root", border, 4, 20);
		DrawMaterial("Earth-earth_spongy", border, 4, 20);
		return;
	}
	Draw("Earth", island);
	return;
}

