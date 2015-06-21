/**
	Big Stone Door
	A door which can be used in scenarios with lots of bricks.
	But now the big version of it.
	
	@authors Maikel	
*/


#include StoneDoor

protected func FxAutoControlTimer(object target, effect, int time)
{
	var d = 0;
	if (IsOpen())
		d = 30;
	var owner = GetOwner();
	var team = effect.Team;
	var open_door = false;
	// Team control
	if (team != nil)
		for (var clonk in FindObjects(Find_OCF(OCF_CrewMember), Find_InRect(-50, d - 50, 100, 120)))
		{
			var plr = clonk->GetOwner();
			var plr_team = GetPlayerTeam(plr);
			if (team == 0 || plr_team == team)
				open_door = true;			
		}
	// Player control
	else
		if (FindObject(Find_OCF(OCF_CrewMember), Find_InRect(-50, d - 50, 100, 120), Find_Allied(owner)))
			open_door = true;
	
	// Keep door closed if hostile?
	// TODO?
	
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