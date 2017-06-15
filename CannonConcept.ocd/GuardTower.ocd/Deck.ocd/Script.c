/**
	Guard Tower Deck
	Helper object to enter and exit the guard tower's deck.
	
	@author Maikel
*/


local guard_tower;

public func Initialize()
{
	SetEntrance(true);
	return _inherited(...);
}

public func SetTower(object to_tower)
{
	guard_tower = to_tower;
	SetAction("Attach", guard_tower);
	return;
}

public func AttachTargetLost()
{
	RemoveObject();
	return;
}

public func Collection2(object obj)
{
	if (guard_tower && !obj.force_tower_exit)
	{
		obj.force_tower_exit = true;
		obj->Enter(guard_tower);
	}
	else
	{
		obj.force_tower_exit = false;
		obj->SetCommand("Exit");
	}
	return _inherited(obj, ...);
}

/*-- Properties --*/

local Name = GuardTower.Name;
local ActMap = {
	Attach = {
		Prototype = Action,
		Name = "Attach",
		Procedure = DFA_ATTACH,
		NextAction = "Attach"
	}
};