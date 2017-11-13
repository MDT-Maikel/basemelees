/**
	Guard Tower Cannon Mount
	Helper object on which the guard tower's cannon can be mounted.
	
	@author Maikel
*/

#include Library_CannonFrame


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

/*-- Graphics --*/

private func AttachCannonGraphics(object cannon)
{
	SetGraphics(nil, cannon->GetID(), GFX_Overlay, GFXOV_MODE_Base);
	return;
}

private func UpdateCannonGrapics(object cannon, int aim_angle)
{
	var fsin = Cos(aim_angle, 1000, this.AimingAnglePrecision);
	var fcos = Sin(aim_angle, 1000, this.AimingAnglePrecision);
	var xoff = 0;
	var yoff = -3;
	SetObjDrawTransform(
		+fcos, +fsin, (1000 - fcos) * xoff - fsin * yoff,
		-fsin, +fcos, (1000 - fcos) * yoff + fsin * xoff,
		GFX_Overlay
	);
	return;
}

private func DetachCannonGraphics(object cannon)
{
	SetGraphics(nil, nil, GFX_Overlay, GFXOV_MODE_Base);
	return;
}


/*-- Saving --*/

// Mount is created on tower initialize.
public func SaveScenarioObject() { return false; }


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local Touchable = 1;
local AimingAngleRange = [260, 100];

local ActMap = {
	Attach = {
		Prototype = Action,
		Name = "Attach",
		Procedure = DFA_ATTACH,
		NextAction = "Attach",
		X = 0,
		Y = 0,
		Wdt = 24,
		Hgt = 24,
	}
};