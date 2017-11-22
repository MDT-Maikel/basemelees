/**
	Big Stone Door
	A door which can be used in scenarios with lots of bricks.
	But now the big version of it.
	
	@authors Maikel	
*/


#include StoneDoor

private func ForceDigFree()
{
	SetSolidMask();
	DigFreeRect(GetX() - 5, GetY() - 30, 10, 60, true);
	SetSolidMask(0, 0, 10, 60);
}

protected func FxAutoControlTimer(object target, effect, int time)
{
	var d = 0;
	if (IsOpen())
		d = 40;
	var owner = GetOwner();
	var open_door = false;
	
	// Crew nearby: open.
	if (FindObject(Find_OCF(OCF_CrewMember), Find_InRect(-50, d - 50, 100, 120), Find_Allied(owner)))
		open_door = true;
	
	// Airplane flying towards door: open.
	for (var airplane in FindObjects(Find_ID(Airplane), Find_InRect(-180, d - 60, 360, 140), Find_Allied(owner)))
	{
		if (airplane->GetAction() != "Fly")
			continue;		
		var dist_x = airplane->GetX() - GetX();
		if (dist_x < 0 && airplane->GetXDir() > -dist_x / 8)
			open_door = true;
		if (dist_x > 0 && airplane->GetXDir() < -dist_x / 8)
			open_door = true;
	}
	
	if (open_door && IsClosed())
		OpenDoor();
	if (!open_door && IsOpen())
		CloseDoor();
	
	return FX_OK;
}

private func GetStrength() { return 240; }


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
		Wdt = 10,
		Hgt = 60,
		NextAction = "Door",
	},
};