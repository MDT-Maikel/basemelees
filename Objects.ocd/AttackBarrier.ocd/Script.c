/**
	Attack Barriers
	Barriers preventing early attacks and allowing for building up a settlement.
	Create blocks with the following ID calls:
	* BlockLine(int x1, int y1, int x2, int y2, int time)
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

// Blocks a line for the specified number of frames.
public func BlockLine(int x1, int y1, int x2, int y2, int time)
{
	var block_angle = Angle(x1, y1, x2, y2) + 90;
	var nr_of_blocks = Distance(x1, y1, x2, y2) / 40 + 1;
	for (var block_nr = 0; block_nr <= nr_of_blocks; block_nr++)
	{
		var x = x1 + (2 * block_nr + 1) * (x2 - x1) / (2 * nr_of_blocks);
		var y = y1 + (2 * block_nr + 1) * (y2 - y1) / (2 * nr_of_blocks);
		var block = CreateObject(AttackBarrier, x, y);
		block->SetPosition(x, y);
		block->SetR(block_angle);
		ScheduleCall(block, "RemoveObject", time, 0);
	}
	return;
}

// Blocks a rectangle for the specified number of frames.
public func BlockRectangle(int x, int y, int wdt, int hgt, int time)
{
	// Create top and bottom barriers.
	for (var lx = x + 20; lx < x + wdt + 20; lx +=40)
	{
		var top = CreateObject(AttackBarrier, Min(lx, x + wdt - 20) , y);
		var bottom = CreateObject(AttackBarrier, Min(lx, x + wdt - 20), y + hgt);
		ScheduleCall(top, "RemoveObject", time, 0);
		ScheduleCall(bottom, "RemoveObject", time, 0);
	}
	// Create left and right barriers.
	for (var ly = y + 20; ly < y + hgt + 20; ly +=40)
	{
		var left = CreateObject(AttackBarrier, x, Min(ly, y + hgt - 20));
		var right = CreateObject(AttackBarrier, x + wdt, Min(ly, y + hgt - 20));
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
