/**
	Guard Tower Door
	Door for the guard tower.
	
	@author Maikel
*/


protected func Initialize()
{
	SetAction("Door");
	SetComDir(COMD_Stop);
	return;
}

private func IsOpen()
{
	if (GetContact(-1) & CNAT_Top)
	 	return true;
	return false;
}

private func IsClosed()
{
	if (GetContact(-1) & CNAT_Bottom)
	 	return true;
	return false;
}

public func OpenDoor()
{
	ForceDigFree();
	SetComDir(COMD_Up);
	Sound("Structures::StoneGate::GateMove");
	return;
}

public func CloseDoor()
{
	ForceDigFree();
	SetComDir(COMD_Down);
	Sound("Structures::StoneGate::GateMove");
	return;
}

public func Hit()
{
	Sound("Structures::StoneGate::GateHit");
	return;
}

// Digs away earth behind the door. Needs to temporarily disable the solid mask, though.
private func ForceDigFree()
{
	SetSolidMask();
	DigFreeRect(GetX() - 2, GetY() - 12, 4, 24, true);
	SetSolidMask(0, 0, 4, 24);
	return;
}


/*-- Saving --*/

// Door is created on tower initialize.
public func SaveScenarioObject() { return false; }


/*-- Properties --*/

local ActMap = {
	Door = {
		Prototype = Action,
		Name = "Door",
		Procedure = DFA_FLOAT,
		Speed = 150,
		Accel = 12,
		Decel = 12,
		Length = 1,
		Delay = 1,
		X = 0,
		Y = 0,
		Wdt = 4,
		Hgt = 24,
		NextAction = "Door",
	},
};

local Plane = 400;
