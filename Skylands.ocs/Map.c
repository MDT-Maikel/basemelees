/**
	Skylands
	Four dynamically generated large sky islands and some smaller islands with 
	valuable materials. The number of large island depends on the number of 
	active teams at the start.
	
	@authors Maikel
*/

#include Library_Map


// List for storing the different main islands.
//static island_list; // defined in Script.c

// Called be the engine: draw the complete map here.
protected func InitializeMap(proplist map)
{
	// Number of islands and map size depends on the number of active teams.
	var nr_teams = GetStartupTeamCount();
	nr_teams = BoundBy(nr_teams, 2, 4);
	
	// Set the map size.
	map->Resize(120 * nr_teams, 160);
	
	// Calculate the main island positions and return them in terms of rectangles.
	var main_islands = FindMainIslands(map, nr_teams, 56, 40);
	
	// Store island positions (middle top) in a static variable for the scenario script.
	island_list = [];
	for (var island in main_islands)
	{
		var x = island.X + island.Wdt / 2;
		var y = island.Y;
		PushBack(island_list, [x, y]);
	}	
	
	// Draw the first main island and then duplicate that onto the others.
	DrawMainIsland(main_islands[0]);
	for (var i = 1; i < GetLength(main_islands); i++)
		DuplicateMainIsland(main_islands[0], main_islands[i]);

	// Then draw some smaller islands which will have a certain distance from the main islands.
	var small_islands = FindSmallIslands(map, main_islands, 24);
	for (island in small_islands)
		DrawRandomSmallIsland(island);
	
	// Fix the liquid borders.
	FixLiquidBorders();

	// Return true to tell the engine a map has been successfully created.
	return true;
}


/*-- Main Sky Islands --*/

public func FindMainIslands(proplist map, int nr_teams, int island_wdt, int island_hgt)
{
	// Safety check for number of teams.
	nr_teams = BoundBy(nr_teams, 2, 4);
	
	var positions = [];
	for (var i = 1; i <= nr_teams; i++)
	{
		var x = (2 * i - 1) * map.Wdt / (2 * nr_teams) - island_wdt / 2 + RandomX(-4, 4);
		var y = map.Hgt / 2 - island_hgt / 2 + RandomX(0, 6);
		var mask = {Algo = MAPALGO_Rect, X = x,  Y = y, Wdt = island_wdt, Hgt = island_hgt};	
		PushBack(positions, mask);
	}
	return positions;
}

public func DrawMainIsland(proplist island_largerect)
{
	// Create a smaller island rect out of the large rect, the large rect is used for duplicating.
	var island_rect = {Algo = MAPALGO_Rect, X = island_largerect.X + 4, Y = island_largerect.Y + 4, Wdt = island_largerect.Wdt - 8, Hgt = island_largerect.Hgt - 8};
	
	// Create the shape of the island, derived from the island rectangle.
	var island = {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [16, 8], Scale = [12, 6], Op = island_rect};
	
	// Draw the island materials.
	Draw("Earth", island);
	DrawMaterial("Earth-earth_root", island, 4, 24);
	DrawMaterial("Earth-earth_spongy", island, 4, 24);
	DrawMaterial("Coal", island, 3, 8);
	DrawMaterial("Firestone", island, 4, 6);
	DrawMaterial("Ore", island, 3, 8);
	DrawMaterial("Gold", island, 3, 7);
	DrawMaterial("Tunnel", island, 4, 20);
	DrawMaterial("Water", island, 4, 6);
	DrawMaterial("Coal", island, 3, 8);
	DrawMaterial("Firestone", island, 4, 6);
	DrawMaterial("Ore", island, 3, 8);
	
	// Draw a larger cave in the middle of the island.
	var x = island_largerect.X + island_largerect.Wdt / 2;
	var y = island_largerect.Y + island_largerect.Hgt / 2 - RandomX(3, 5);
	var cave = {Algo = MAPALGO_Rect, X = x - 7, Y = y - 4, Wdt = 14, Hgt = 8};
	cave = {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [8, 8], Scale = [6, 6], Iterations = 3, Op = cave};
	Draw("Tunnel", cave);
	
	// Draw a granite border, with "holes" out of tunnel and rock.
	var island_border = {Algo = MAPALGO_Border, Left = [-1, 1], Right = [-1, 1], Top = 0, Bottom = [-1,2], Op = island};
	Draw("Granite", island_border);
	DrawMaterial("Tunnel", island_border, 4, 10);
	DrawMaterial("Rock", island_border, 4, 40);
	
	// Draw the top with earth using another border.
	var island_top = {Algo = MAPALGO_Border, Left = 0, Right = 0, Top = [-1, 1], Bottom = 0, Op = island};
	Draw("Earth", island_top);
	DrawMaterial("Earth-earth_root", island_top, 4, 24);
	DrawMaterial("Earth-earth_spongy", island_top, 2, 32);
	return;
}

public func DuplicateMainIsland(proplist orig, proplist onto)
{
	// Loop over all pixels in the original and duplicate onto the new island.
	for (var x = 0; x < orig.Wdt; x++)
		for (var y = 0; y < orig.Hgt; y++)
			SetPixel(x + onto.X, y + onto.Y, GetPixel(x + orig.X, y + orig.Y), GetBackPixel(x + orig.X, y + orig.Y));
	return;
}


/*-- Small Sky Islands --*/

public func FindSmallIslands(proplist map, array main_islands, int amount)
{
	// Prepare a mask out of the map and the large islands.
	var mask = map->CreateLayer();
	mask->Draw("Rock");
	for (var island in main_islands)
		mask->Draw("Tunnel", {Algo = MAPALGO_Rect, X = island.X - 15, Y = island.Y - 18, Wdt = island.Wdt + 30, Hgt = island.Hgt + 32});

	var small_islands = [];
	var border = 4;
	var island_dist = 24;
	var count = 0;
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
		var size = RandomX(5, 6);
		var small_island = {Algo = MAPALGO_Ellipsis, X = x, Y = y, Wdt = size + 3, Hgt = size};
		small_island = {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [6, 6], Scale = [6, 6], Iterations = 4, Op = small_island};
		small_islands[count++] = small_island;
	}
	return small_islands;
}

public func DrawRandomSmallIsland(proplist island)
{
	var rnd = Random(3);
	// Draw a gems core with granite borders.
	if (rnd == 0)
	{
		Draw(["Ruby", "Amethyst"][Random(2)], island);
		var border = {Algo = MAPALGO_Border, Wdt = 2, Op = island};
		Draw("Granite", border);
		DrawMaterial("Rock", border, 4, 20);
		return;
	}
	// Mainly firestones with some earth and rock, dangerous!
	if (rnd == 1)
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
	if (rnd == 2)
	{
		Draw("Rock", island);
		DrawMaterial("Ore", island, 3, 50);
		DrawMaterial("Coal", island, 3, 50);
		DrawMaterial("Gold", island, 3, 25);
		var border = {Algo = MAPALGO_Border, Top = 1, Op = island};
		Draw("Earth", border);
		DrawMaterial("Earth-earth_root", border, 4, 20);
		DrawMaterial("Earth-earth_spongy", border, 4, 20);
		return;
	}
	Draw("Earth", island);
	return;
}
