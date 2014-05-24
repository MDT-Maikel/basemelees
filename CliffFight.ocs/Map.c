/**
	Cliff Fight
	Two opposing cliffs with a sky island and lake in the middle.

	@author Maikel
*/

#include Library_Map


// Called be the engine: draw the complete map here.
protected func InitializeMap(proplist map)
{
	// Set the map size.
	map->Resize(320, 160);
	
	// Draw the left cliff.
	var cliff = DrawCliff(map);
	
	// Draw a lake next to the cliff.
	DrawLake(map, cliff);
		
	// Draw a sky island in the center of map.	
	DrawSkyIsland(map);

	// Symmetrize map.
	SymmetrizeMap(map);

	// Return true to tell the engine a map has been successfully created.
	return true;
}


/*-- Map Drawing --*/

public func DrawCliff(proplist map)
{
	var w = map.Wdt;
	var h = map.Hgt;
	// Draw the contours of a cliff.
	var cliff = {Algo = MAPALGO_Polygon, X = [0, 2*w/7, w/5, w/5, w/4, w/4, w/5, 0], Y = [5*h/9, h/2, 2*h/3, 5*h/7, 5*h/7, 5*h/6, h, h]};
	cliff = {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [15, 0], Scale = [6, 0], Iterations = 1, Op = cliff};
	cliff = {Algo = MAPALGO_Or, Op = [cliff, {Algo = MAPALGO_Rect, X = 0, Y = 5*h/9, Wdt = 8, Hgt = 4*h/9}]};

	// Draw the inner materials of the cliff.
	Draw("Earth", cliff);
	DrawMaterial("Earth-earth_topsoil", cliff, 4, 12);
	DrawMaterial("Earth-earth_rough", cliff, 2, 16);
	DrawMaterial("Earth-earth_dry", cliff, 2, 16);
	DrawMaterial("Earth-earth_midsoil", cliff, 4, 12);
	DrawMaterial("Coal", cliff, 3, 8);
	DrawMaterial("Firestone", cliff, 4, 6);
	DrawMaterial("Ore", cliff, 3, 8);
	DrawMaterial("Gold", cliff, 4, 4);
	DrawMaterial("Tunnel", cliff, 5, 20);
	DrawMaterial("Coal", cliff, 3, 8);
	DrawMaterial("Firestone", cliff, 4, 6);
	DrawMaterial("Ore", cliff, 3, 8);
	
	// The top border should be earth only.
	var top = {Algo = MAPALGO_Border, Top = 6, Op = cliff};
	Draw("Earth", top);
	DrawMaterial("Earth-earth_topsoil", top, 4, 12);
	DrawMaterial("Earth-earth_rough", top, 2, 16);
	DrawMaterial("Earth-earth_dry", top, 2, 16);
	DrawMaterial("Earth-earth_midsoil", top, 4, 12);
	
	// The right border of the cliff should be rock and granite.
	var right = {Algo = MAPALGO_Border, Right = 6, Op = cliff};
	Draw("Granite", right);
	DrawMaterial("Rock", right, 4, 20);
	DrawMaterial("Rock-rock_cracked", right, 4, 20);
	
	return cliff;
}

public func DrawSkyIsland(proplist map)
{
	var x = map.Wdt / 2;
	var y = map.Hgt / 2;
	var island = {Algo = MAPALGO_Ellipsis, X = x, Y = y, Wdt = 14, Hgt = 8};
	island = {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [8, 8], Scale = [4, 4], Iterations = 1, Op = island};	
	Draw("Earth", island);
	DrawMaterial("Gold", island, 6, 80);

	var island_top = {Algo = MAPALGO_Border, Top = 2, Op = island};
	Draw("Earth", island_top);
	DrawMaterial("Earth-earth_topsoil", island_top, 4, 12);
	DrawMaterial("Earth-earth_rough", island_top, 2, 16);
	DrawMaterial("Earth-earth_dry", island_top, 2, 16);
	DrawMaterial("Earth-earth_midsoil", island_top, 4, 12);
	return;
}

public func DrawLake(proplist map, proplist cliff)
{
	var w = map.Wdt;
	var h = map.Hgt;
	// Draw the water body.
	var lake = {Algo = MAPALGO_Rect, X = 0, Y = 5*h/7 + 1, Wdt = w, Hgt = h/3};
	lake = {Algo = MAPALGO_And, Op = [lake, {Algo = MAPALGO_Not, Op = cliff}]};
	Draw("Water", lake);
	// Draw the lake bottom.
	var lake_bottom = {Algo = MAPALGO_Rect, X = 0, Y = 6*h/7, Wdt = w, Hgt = h/3};
	lake_bottom = {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [16, 16], Scale = [8, 8], Iterations = 1, Op = lake_bottom};
	lake_bottom = {Algo = MAPALGO_And, Op = [lake_bottom, {Algo = MAPALGO_Not, Op = cliff}]};
	Draw("Earth", lake_bottom);
	DrawMaterial("Earth-earth_topsoil", lake_bottom, 4, 12);
	DrawMaterial("Earth-earth_rough", lake_bottom, 2, 16);
	DrawMaterial("Earth-earth_dry", lake_bottom, 2, 16);
	DrawMaterial("Earth-earth_midsoil", lake_bottom, 4, 12);
	DrawMaterial("Coal", lake_bottom, 5, 10);
	DrawMaterial("Firestone", lake_bottom, 6, 8);
	DrawMaterial("Ore", lake_bottom, 5, 10);
	// The top is partially out of sand.
	var border = {Algo = MAPALGO_Border, Top = 2, Op = lake_bottom};
	Draw("Earth", border);
	DrawMaterial("Sand", border, 4, 80);
	// Draw a small island in the lake.
	var island = {Algo = MAPALGO_Ellipsis, X = w/3 + 4, Y = 5*h/7 + 1, Wdt = 12, Hgt = 5};
	island = {Algo = MAPALGO_Turbulence, Seed = Random(65536), Amplitude = [6, 6], Scale = [4, 4], Iterations = 1, Op = island};
	var island_top = {Algo = MAPALGO_Border, Top = 2, Op = island};
	Draw("Earth", island);
	DrawMaterial("Sand", island_top, 4, 80);
	return;
}


/*-- Helper Functions --*/

// Symmetrizes the map by taking the left part and copying it to the right part.
public func SymmetrizeMap(proplist map)
{
	// Loop over all pixels of the left side and copy onto the right side.
	for (var x = 0; x < map.Wdt / 2; x++)
		for (var y = 0; y < map.Hgt; y++)
			map->SetPixel(map.Wdt - x - 1, y, map->GetPixel(x, y));
	return;
}

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
