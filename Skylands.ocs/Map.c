/**
	Skylands
	Four dynamically generated large sky islands and some smaller islands with 
	valuable materials.
	
	@authors Maikel
*/

#include Library_Map


// List for storing the different main islands.
static island_list;

// Called be the engine: draw the complete map here.
protected func InitializeMap(proplist map)
{
	// Set the map size.
	map->Resize(480, 160);
	
	// Calculate the main island positions and return them in terms of rectangles.
	var islands = CalculateIslandPositions(map, 4, 56, 40);
	
	// Store island positions (middle top) in a static variable for the scenario script.
	island_list = [];
	for (var island in islands)
	{
		var x = island.X + island.Wdt / 2;
		var y = island.Y;
		PushBack(island_list, [x, y]);
	}	
	
	// Draw the first main island and then duplicate that onto the others.
	DrawMainIsland(islands[0]);
	for (var i = 1; i < 4; i++)
		DuplicateMainIsland(islands[0], islands[i]);

	// Then draw some smaller islands which will have a certain distance from the main islands.
	DrawSmallIslands(map, 24, islands, 48);
	
	// Fix the liquid borders.
	FixLiquidBorders();

	// Return true to tell the engine a map has been successfully created.
	return true;
}


/*-- Main Sky Islands --*/

public func CalculateIslandPositions(proplist map, int nr_teams, int island_wdt, int island_hgt)
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
	DrawMaterial("Earth-earth_topsoil", island, 4, 12);
	DrawMaterial("Earth-earth_rough", island, 2, 16);
	DrawMaterial("Earth-earth_dry", island, 2, 16);
	DrawMaterial("Earth-earth_midsoil", island, 4, 12);
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
	DrawMaterial("Rock", island_border, 4, 20);
	DrawMaterial("Rock-rock_cracked", island_border, 4, 20);
	
	// Draw the top with earth using another border.
	var island_top = {Algo = MAPALGO_Border, Left = 0, Right = 0, Top = [-1, 1], Bottom = 0, Op = island};
	Draw("Earth", island_top);
	DrawMaterial("Earth-earth_topsoil", island_top, 4, 12);
	DrawMaterial("Earth-earth_rough", island_top, 2, 16);
	DrawMaterial("Earth-earth_dry", island_top, 2, 16);
	DrawMaterial("Earth-earth_midsoil", island_top, 4, 12);
	return;
}

public func DuplicateMainIsland(proplist orig, proplist onto)
{
	// Loop over all pixels in the original and duplicate onto the new island.
	for (var x = 0; x < orig.Wdt; x++)
		for (var y = 0; y < orig.Hgt; y++)
			SetPixel(x + onto.X, y + onto.Y, GetPixel(x + orig.X, y + orig.Y));
	return;
}


/*-- Small Sky Islands --*/

public func DrawSmallIslands(proplist map, int amount, array islands, int dist)
{
	var border_clear = 3;
	// Calculate the centers of the main islands.
	var islands_center = [];
	for (var i = 0; i < GetLength(islands); i++)
		islands_center[i] = [islands[i].X + islands[i].Wdt / 2, islands[i].Y + islands[i].Hgt / 2];
	// Create the amount of islands.	
	for (var i = 0; i < amount; i++)
	{
		// Find a position far away from the main islands.
		var x, y, found = false;
		do
		{
			found = true;
			x = RandomX(border_clear, map.Wdt - border_clear);
			y = RandomX(3 * border_clear, map.Hgt - border_clear);
			for (var j = 0; j < GetLength(islands_center); j++)
			{
				if (Distance(x, y, islands_center[j][0], islands_center[j][1]) < dist)
					found = false;		
			}
		}
		while (!found);
		// Island location found, create a random island.
		var size = RandomX(4, 5);
		var island = {Algo = MAPALGO_Rect, X = x - size - 1, Y = y - size, Wdt = 2 * size + 2, Hgt = 2 * size};
		island = {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [6, 6], Scale = [6, 6], Iterations = 4, Op = island};
		DrawRandomSmallIsland(island);
	}
	return;
}

public func DrawRandomSmallIsland(proplist island)
{
	var rnd = Random(3);
	// Draw a gems core with granite borders.
	if (rnd == 0)
	{
		var border = {Algo = MAPALGO_Border, Wdt = 2, Op = island};
		Draw(["Ruby", "Amethyst"][Random(2)], island);
		Draw("Granite", border);
		return;
	}
	// Mainly firestones with some earth and rock, dangerous!
	if (rnd == 1)
	{
		Draw("Firestone", island);
		DrawMaterial("Rock", island, 2, 40);
		DrawMaterial("Earth", island, 2, 32);
		return;
	}
	// Island with rock, ore and gold.
	if (rnd == 2)
	{
		Draw("Rock", island);
		DrawMaterial("Ore", island, 3, 50);
		DrawMaterial("Gold", island, 3, 30);
		return;
	}
	Draw("Earth", island);
	return;
}


/*-- Helper Functions --*/

// Draws some material inside an island.
public func DrawMaterial(string mat, proplist onto_mask, int speck_size, int ratio)
{
	if (!speck_size)
		speck_size = 4;
	if (!ratio)
		ratio = 15;
	// Use random checker algorithm to draw patches of the material. 
	var rnd_checker = {Algo = MAPALGO_RndChecker, Ratio = ratio, Wdt = speck_size, Hgt = speck_size};
	rnd_checker = {Algo = MAPALGO_Turbulence, Iterations = 4, Op = rnd_checker};
	var algo = {Algo = MAPALGO_And, Op = [onto_mask, rnd_checker]};
	Draw(mat, algo);	
	return;
}
