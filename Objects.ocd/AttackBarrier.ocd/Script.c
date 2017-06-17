/**
	Attack Barriers
	Barriers preventing early attacks and allowing for building up a settlement.
	Create blocks with the following definition calls:
	* BlockLine(int x1, int y1, int x2, int y2, int time)
	* BlockRectangle(int x, int y, int wdt, int hgt, int time)
	BlockRectangle must have wdt >= 56 and hgt >= 56.
	@author Maikel
*/


protected func Initialize()
{
	SetSolidMask(0, 0, 40, 8);
	return;
}

public func MakeEdge()
{
	SetGraphics("Edge");
	SetSolidMask(0, 8, 40, 8);
	return;
}

public func MakeEnd()
{
	SetGraphics("End");
	SetSolidMask(0, 16, 40, 8);
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
		// Adapt barriers at the end.
		if (block_nr == 0)
		{
			block->MakeEnd();
			block->SetR(block->GetR() + 180);
		}
		if (block_nr == nr_of_blocks)
		{
			block->MakeEnd();
		}
	}
	return;
}

// Blocks a rectangle for the specified number of frames.
public func BlockRectangle(int x, int y, int wdt, int hgt, int time)
{
	// Check minimum width and height.
	wdt = Max(wdt, 56);
	hgt = Max(hgt, 56);	
	// Create top and bottom barriers.
	for (var lx = x + 24; lx < x + wdt + 16; lx += 40)
	{
		var top = CreateObject(AttackBarrier, Min(lx, x + wdt - 24) , y);
		var bottom = CreateObject(AttackBarrier, Min(lx, x + wdt - 24), y + hgt);
		ScheduleCall(top, "RemoveObject", time, 0);
		ScheduleCall(bottom, "RemoveObject", time, 0);
	}
	// Create left and right barriers.
	for (var ly = y + 24; ly < y + hgt + 16; ly += 40)
	{
		var left = CreateObject(AttackBarrier, x, Min(ly, y + hgt - 24));
		var right = CreateObject(AttackBarrier, x + wdt, Min(ly, y + hgt - 24));
		left->SetR(90);
		right->SetR(90);
		ScheduleCall(left, "RemoveObject", time, 0);
		ScheduleCall(right, "RemoveObject", time, 0);
	}
	// Create rectangle edges.
	var edge;
	edge = CreateObject(AttackBarrier, x, y - 16);
	edge->SetR(270);
	edge->MakeEdge();
	ScheduleCall(edge, "RemoveObject", time, 0);
	edge = CreateObject(AttackBarrier, x + wdt + 16, y);
	edge->MakeEdge();
	ScheduleCall(edge, "RemoveObject", time, 0);
	edge = CreateObject(AttackBarrier, x - 16, y + hgt);
	edge->SetR(180);
	edge->MakeEdge();
	ScheduleCall(edge, "RemoveObject", time, 0);
	edge = CreateObject(AttackBarrier, x + wdt, y + hgt + 16);
	edge->SetR(90);
	edge->MakeEdge();
	ScheduleCall(edge, "RemoveObject", time, 0);
	return;
}


/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";
