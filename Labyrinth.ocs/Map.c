/**
	Labyrinth
	Lots of small and large caves situated near a huge amount of gems.
	The caves are connected with tunnels forming a labyrinth.
	
	@authors Maikel
*/

#include Library_Map


// List for storing the different large caves.
static cave_list;

// Called be the engine: draw the complete map here.
protected func InitializeMap(proplist map)
{
	// Set the map size.
	map->Resize(300, 300);
	
	// The map consists of a center, large caves and small caves.
	// Find these in terms of algorithms.
	var center = FindCenter(map, 30);
	var large_caves = FindLargeCaves(map, 4, 30);
	var small_caves = FindSmallCaves(map, large_caves, 100);
	var connections = FindCaveConnections(small_caves, 34);
	
	// Store large cave positions in a static variable for the scenario script.
	cave_list = [];
	for (var cave in large_caves)
		PushBack(cave_list, [cave.X, cave.Y, cave.Wdt, cave.Hgt]);
	
	// Draw the background materials for the whole map.
	DrawBackground();
	// Draw the center with valuable gems.
	DrawCenter(center);
	// Draw the smaller caves.
	DrawSmallCaves(small_caves);
	// Draw the connections
	DrawConnections(connections);
	// Draw the large caves, last because they should be drawn over the 
	// small caves functioning as entrances.
	DrawLargeCaves(large_caves);

	// Return true to tell the engine a map has been successfully created.
	return true;
}


/*-- Cave Creation --*/

// Returns the center of the map, where the gems are located.
public func FindCenter(proplist map, int radius)
{
	// Get the map coordinates.
	var x = map.Wdt / 2;
	var y = map.Hgt / 2;
	var center = {Algo = MAPALGO_Ellipsis, X = x, Y = y, Wdt = radius, Hgt = radius};
	return center;
}

// Returns algorithms for a number of large caves.
public func FindLargeCaves(proplist map, int nr_caves, int cave_size)
{
	// Prepare a mask out of the map in a doughnut like shape to search for main islands.
	var mask = map->CreateLayer();
	mask->Draw("Rock");
	// Contruct the doughnut like shape.
	var x = map.Wdt / 2;
	var y = map.Hgt / 2;
	var rin = 90;
	var rout = 110;
	mask->Draw("Tunnel", {Algo = MAPALGO_Ellipsis, X = x, Y = y, Wdt = rin, Hgt = rin});
	mask->Draw("Tunnel", {Algo = MAPALGO_Not, Op = {Algo = MAPALGO_Ellipsis, X = x, Y = y, Wdt = rout, Hgt = rout}});
	
	var caves = [];
	var border = 3 * cave_size / 2;
	while (nr_caves > 0)
	{
		var cave = {};
		if (!mask->FindPosition(cave, "Rock", [border, border, map.Wdt - border * 2, map.Hgt - border * 2]))
			continue;
		mask->Draw("Tunnel", {Algo = MAPALGO_Ellipsis, X = cave.X, Y = cave.Y, Wdt = 15 * cave_size / 4, Hgt = 15 * cave_size / 4});
		cave = {Algo = MAPALGO_Ellipsis, X = cave.X, Y = cave.Y, Wdt = cave_size, Hgt = 2 * cave_size / 3};
		nr_caves--;
		caves[nr_caves] = cave;		
	}
	return caves;
}

public func FindSmallCaves(proplist map, array large_caves, int nr_caves)
{
	// Prepare a mask out of the map, the center and the large caves in which to search for smaller ones.
	var mask = map->CreateLayer();
	mask->Draw("Rock");
	// Contruct the doughnut like shape.
	var x = map.Wdt / 2;
	var y = map.Hgt / 2;
	var rin = 35;
	var rout = 160;
	mask->Draw("Tunnel", {Algo = MAPALGO_Ellipsis, X = x, Y = y, Wdt = rin, Hgt = rin});
	mask->Draw("Tunnel", {Algo = MAPALGO_Not, Op = {Algo = MAPALGO_Ellipsis, X = x, Y = y, Wdt = rout, Hgt = rout}});
	for (var cave in large_caves)
		mask->Draw("Tunnel", {Algo = MAPALGO_Ellipsis, X = cave.X, Y = cave.Y, Wdt = 3 * cave.Wdt / 2, Hgt = 14 * cave.Hgt / 10});
	// Array for the small caves.	
	var caves = [];	
	// Add caves for the exit points of the large caves.
	for (var cave in large_caves)
	{
		PushBack(caves, {X = cave.X - cave.Wdt - 1, Y = cave.Y, block_dir = COMD_Right});
		PushBack(caves, {X = cave.X + cave.Wdt + 1, Y = cave.Y, block_dir = COMD_Left});
	}	
	// Add caves at random locations around the map.
	var border = 5;
	var cave_dist = 20;
	for (var i = 0; i < nr_caves; i++)
	{
		var cave = {};
		if (!mask->FindPosition(cave, "Rock", [border, border, map.Wdt - border * 2, map.Hgt - border * 2]))
			continue;
		mask->Draw("Tunnel", {Algo = MAPALGO_Ellipsis, X = cave.X, Y = cave.Y, Wdt = cave_dist, Hgt = cave_dist});	
		PushBack(caves, cave);
	}
	return caves;
}

public func FindCaveConnections(array small_caves, int max_length)
{
	var connections = [];
	for (var cave in small_caves)
		cave.conn_count = 0;	
	for (var i = 0; i < GetLength(small_caves) - 1; i++)
	{
		var from_cave = small_caves[i];
		for (var j = i + 1; j < GetLength(small_caves); j++)
		{
			var to_cave = small_caves[j];
			// Check for the maximum connections per cave.
			if (from_cave.conn_count >= 4 || to_cave.conn_count >= 4)
				continue;
			// Cave line parameters.
			var fx = from_cave.X;
			var fy = from_cave.Y;
			var tx = to_cave.X;
			var ty = to_cave.Y;
			// Check for the maximum line distance.
			if (Distance(fx, fy, tx, ty) > max_length)
				continue;
			// Check for block line directions.
			if (from_cave.block_dir != nil && IsBlockedDirection(from_cave.block_dir, fx, fy, tx, ty))
				continue;
			if (to_cave.block_dir != nil && IsBlockedDirection(to_cave.block_dir, tx, ty, fx, fy))
				continue;
			// Check for overlap in existing connections.
			var has_overlap = false;
			for (var line in connections)
				if (IsLineOverlap(fx, fy, tx, ty, line.X[0], line.Y[0], line.X[1], line.Y[1]))	
				{
					has_overlap = true;
					break;
				}
			if (has_overlap)
				continue;
			var tunnel = {Algo = MAPALGO_Polygon, X = [fx, tx], Y = [fy, ty], Wdt = RandomX(2, 3), Open = 1, Empty = 1};
			PushBack(connections, tunnel);
			small_caves[i].conn_count++;
			small_caves[j].conn_count++;
		}
	}
	return connections;
}

public func IsBlockedDirection(int dir, int x1, int y1, int x2, int y2)
{
	// Use COMD dir constants.
	if (dir == COMD_Right && x2 > x1)
		return true;
	if (dir == COMD_Left && x2 < x1)
		return true;
	return false;
}

public func IsLineOverlap(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	// Same starting point is not overlapping.
	if ((x1 == x3 && y1 == y3) || (x1 == x4 && y1 == y4) || (x2 == x3 && y2 == y3) || (x2 == x4 && y2 == y4))
		return false;	
	
	// Check if line from x1,y1 to x2,y2 crosses the line from x3,y3 to x4,y4
	var d1x = x2 - x1, d1y = y2 - y1, d2x = x4 - x3, d2y = y4 - y3, d3x = x3 - x1, d3y = y3 - y1;
	var a = d1y * d3x - d1x * d3y;
	var b = d2y * d3x - d2x * d3y;
	var c = d2y * d1x - d2x * d1y;
	if (!c) 
		return !a && Inside(x3, x1, x2) && Inside(y3, y1, y2); // lines are parallel
	return a * c >= 0 && !(a * a / (c * c + 1)) && b * c >= 0 && !(b * b/(c * c + 1));
}


/*-- Map Drawing --*/

public func DrawBackground()
{
	Draw("Rock");
	DrawVariations("Rock-rock_cracked", 50, 5, 15);
	DrawVariations("Ore", 10, 8, 8);
	DrawVariations("Firestone", 8, 12, 3);
	DrawVariations("Earth", 3, 8, 3);
	DrawVariations("Earth-earth_topsoil", 3, 8, 3);
	DrawVariations("Earth-earth_midsoil", 3, 8, 3);
	DrawVariations("Earth-earth_dry", 3, 8, 3);
	DrawVariations("Earth-earth_rough", 3, 8, 3);
	DrawVariations("Firestone", 6, 12, 3);
	DrawVariations("Coal", 8, 8, 3);
	DrawVariations("Gold", 5, 4, 4);
	DrawVariations("Granite", 14, 15, 5);
	DrawVariations("Granite", 14, 5, 15);
	return true;
}

public func DrawCenter(proplist center)
{
	var x = center.X;
	var y = center.Y;
	var r = center.Wdt;
	var turb_center = {Algo = MAPALGO_Turbulence, Seed= Random(65536), Amplitude = 12, Scale = 8, Op = center};
	var center_inner = {Algo = MAPALGO_Not, Op = {Algo = MAPALGO_Border, Wdt = 2, Op = turb_center}};
	center_inner = {Algo = MAPALGO_And, Op = [turb_center, center_inner]};
	Draw("Granite", turb_center);
	
	// Draw a series of polygons outwards from the center.
	for (var angle = 0; angle < 360; angle += 20)
	{
		var dx = Cos(angle, r - 5);
		var dy = Sin(angle, r - 5);
		var line = {Algo = MAPALGO_Polygon, X = [x, x + dx], Y = [y, y + dy], Wdt = RandomX(1, 2), Open = 1, Empty = 1};
		var turb_line = {Algo = MAPALGO_Turbulence, Amplitude = 12, Scale = 8, Op = line};
		turb_line = {Algo = MAPALGO_And, Op = [turb_line, center_inner]};
		Draw(["Ruby", "Amethyst"][Random(2)], turb_line);
	}
	return;
}

public func DrawLargeCaves(array large_caves)
{
	// Draw the first large cave and then duplicate the others.
	var cave = large_caves[0];
	var cave_border = {Algo = MAPALGO_Border, Left = 5, Right = 5, Bottom = 3, Top = 5, Op = cave};
	var border_holes = {Algo = MAPALGO_Not, Op = {Algo = MAPALGO_Rect, X = cave.X - cave.Wdt, Y = cave.Y - 1, Wdt = 2 * cave.Wdt, Hgt = 4}};
	cave_border = {Algo = MAPALGO_And, Op = [cave_border, border_holes]};
	var lower_half = {Algo = MAPALGO_Rect, X = cave.X - cave.Wdt, Y = cave.Y + 3, Wdt = 2 * cave.Wdt, Hgt = cave.Hgt};
	lower_half = {Algo = MAPALGO_And, Op = [cave, lower_half]};
	
	// Fill all the algorithms.
	Draw("Tunnel", cave);
	DrawMaterial("Tunnel-brickback", cave, 3, 30);
	Draw("Earth", lower_half);
	DrawMaterial("Earth-earth_topsoil", lower_half, 3, 20);
	DrawMaterial("Earth-earth_midsoil", lower_half, 3, 20);
	DrawMaterial("Earth-earth_dry", lower_half, 3, 20);
	DrawMaterial("Ore", lower_half, 3, 20);
	DrawMaterial("Firestone", lower_half, 3, 20);
	DrawMaterial("Coal", lower_half, 3, 20);
	Draw("Granite", cave_border);	

	// Duplicate the other caves.
	for (var i = 1; i < GetLength(large_caves); i++)
		DuplicateLargeCave(large_caves[0], large_caves[i]);

	return;
}

public func DuplicateLargeCave(proplist orig, proplist onto)
{
	// Create a mask to test whether the pixel is inside the cave.
	var mask = CreateLayer();
	mask->Draw("Rock", orig);
	// Loop over all pixels in the original and duplicate onto the new cave.
	for (var x = -orig.Wdt; x < orig.Wdt; x++)
		for (var y = -orig.Hgt; y < orig.Hgt; y++)
			if (mask->GetPixel(x + orig.X, y + orig.Y))
				SetPixel(x + onto.X, y + onto.Y, GetPixel(x + orig.X, y + orig.Y), GetBackPixel(x + orig.X, y + orig.Y));
	return;
}

public func DrawSmallCaves(array small_caves)
{
	for (var cave in small_caves)
	{
		var cave_algo = {Algo = MAPALGO_Ellipsis, X = cave.X, Y = cave.Y, Wdt = RandomX(4, 6), Hgt = RandomX(4, 6)};
		var turb_cave = {Algo = MAPALGO_Turbulence, Amplitude = 12, Scale = 8, Op = cave_algo};
		Draw("Tunnel", turb_cave);
		DrawMaterial("Tunnel-brickback", turb_cave, 2, 15);
	}
	return;
}

public func DrawConnections(connections)
{
	for (var con in connections)
	{
		con = {Algo = MAPALGO_Turbulence, Amplitude = 5, Scale = 3, Op = con};
		Draw("Tunnel", con);
		DrawMaterial("Tunnel-brickback", con, 2, 15);
	}
	return;
}


/*-- Helper Functions --*/

public func DrawVariations(string mat, int ratio, int sx, int sy)
{
	var rand_algo = {Algo=MAPALGO_RndChecker, Ratio = ratio, Wdt = sx, Hgt = sy};
	var turb_algo = {Algo=MAPALGO_Turbulence, Amplitude = 12, Scale = 8, Op = rand_algo};
	return Draw(mat, turb_algo);
}
