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

public func IsOpen()
{
	return (GetContact(-1) & CNAT_Top) && GetComDir() == COMD_Up;
}

public func IsClosed()
{
	return (GetContact(-1) & CNAT_Bottom) && GetComDir() == COMD_Down;
}

public func IsOpening()
{
	return !(GetContact(-1) & CNAT_Top) && GetComDir() == COMD_Up;
}

public func IsClosing()
{
	return !(GetContact(-1) & CNAT_Bottom) && GetComDir() == COMD_Down;
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
