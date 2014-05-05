/**
	Attack Barriers
	Barriers preventing early attacks and allowing for building up a settlement.
	Create blocks with the following ID calls:
	* BlockRectangle(int x, int y, int wdt, int hgt, int time)

	@author Maikel
*/



protected func Initialize()
{
	SetGraphics(Format("%d", 1 + Random(4)));
	SetSolidMask(0, 0, 40, 8);
	return;
}


/*-- Interface --*/

// Blocks a rectangle for the specified number of frames.
public func BlockRectangle(int x, int y, int wdt, int hgt, int time)
{
	// Create top and bottom barriers.
	for (var lx = x + 20; lx < x + wdt + 20; lx +=40)
	{
		var top = CreateObject(AttackBarrier, Min(lx, x + wdt - 20) , y + 4);
		var bottom = CreateObject(AttackBarrier, Min(lx, x + wdt - 20), y + hgt + 4);
		ScheduleCall(top, "RemoveObject", time, 0);
		ScheduleCall(bottom, "RemoveObject", time, 0);
	}
	// Create left and right barriers.
	for (var ly = y + 20; ly < y + hgt + 20; ly +=40)
	{
		var left = CreateObject(AttackBarrier, x, Min(ly, y + hgt - 20) + 4);
		var right = CreateObject(AttackBarrier, x + wdt, Min(ly, y + hgt - 20) + 4);
		left->SetR(90);
		right->SetR(90);
		ScheduleCall(left, "RemoveObject", time, 0);
		ScheduleCall(right, "RemoveObject", time, 0);
	}
	return;
}


/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";