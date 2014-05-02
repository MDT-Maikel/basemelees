/**
	No Borders
	The left and right border of the map are connected such that a continuous map is created.

	@author Maikel
*/


protected func Initialize()
{
	// Add an effect which checks for objects near the border and transfers them.
	AddEffect("CheckBorders", nil, 100, 1, nil, this->GetID());	
	return;
}

// An effect which checks for all objects which are moving towards the map borders.
protected func FxCheckBordersTimer()
{
	var map_wdt = LandscapeWidth();
	// Find all objects with nonzero velocity in the x direction and near the map border.
	for (var obj in FindObjects(Find_Func("GetXDir")))
	{
		var xdir = obj->GetXDir() / 10;
		var obj_wdt = Abs(obj->GetDefOffset());
		var x = obj->GetX() + xdir;
	
		// Moving towards the left map border?
		if (xdir < 0)
		{
			if (x < obj_wdt)
			{
				// Place object on the opposite site of the map border.
				var new_x = BoundBy(map_wdt + x, 0, map_wdt - obj_wdt);
				var new_y = obj->GetY() + obj->GetYDir() / 10;
				obj->SetPosition(new_x, new_y, true);
				// Notify object via a callback.
				obj->~OnBorderCrossed(DIR_Left);
			}
			continue;
		}
		// Moving towards the right map border?
		if (xdir > 0)
		{
			if (x >= map_wdt - obj_wdt)
			{
				// Place object on the opposite site of the map border.
				var new_x = BoundBy(map_wdt + x, 0, map_wdt - obj_wdt);
				var new_y = obj->GetY() + obj->GetYDir() / 10;
				obj->SetPosition(new_x, new_y, true);
				// Notify object via a callback.
				obj->~OnBorderCrossed(DIR_Right);
			}		
			continue;
		}
	}
	return FX_OK;
}


/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";
