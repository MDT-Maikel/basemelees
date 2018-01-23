/**
	Cannon Frame
	Portable cannon frame which can hold all types of cannons.
	
	@author Maikel
*/


#include Library_CannonFrame
#include Library_ElevatorControl
#include Library_Destructible


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


/*-- Destruction --*/

public func OnDestruction(int change, int cause, int by_player)
{

	return false;
}

public func OnHalfDamage(int change, int cause, int by_player)
{
	return;
}


/*-- Production --*/

public func IsArmoryProduct() { return true; }


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local Touchable = 1;
local BorderBound = C4D_Border_Sides;
local ContactCalls = true;
local Components = {Metal = 1, Wood = 2};
local HitPoints = 150;
local AimingAngleRange = [260, 100];