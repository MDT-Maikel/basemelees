/**
	Crossbow
	Shoots arrows at a fast rate.
	
	@author Maikel
*/


#include Library_Cannon
#include Library_ElevatorControl


local crossbow_fx;


/*-- Controls --*/

public func CannonUseStart(object frame, object clonk, int x, int y)
{
	// Check for ammunition.
	var has_bullets = CheckForArrows(clonk);
	if (!has_bullets)
	{
		clonk->CancelUse();
		return true;
	}
	// Create shooting effect.
	var angle = CoordinatesToAngle(frame, x, y);
	crossbow_fx = CreateEffect(FxCrossbow, 100, this.FireRate, clonk, angle, frame.AimingAnglePrecision);
	return true;
}

public func CannonUseHolding(object frame, object clonk, int x, int y)
{
	var angle = CoordinatesToAngle(frame, x, y);
	if (crossbow_fx)
		crossbow_fx->UpdateAimingAngle(angle);
	return true;
}

public func CannonUseStop(object frame, object clonk, int x, int y)
{
	if (crossbow_fx)
		crossbow_fx->Remove();
	return true;
}

public func CannonUseCancel(object frame)
{
	if (crossbow_fx)
		crossbow_fx->Remove();
	return true;
}

// Takes bullets from either the clonk or the gun itself.
private func CheckForArrows(object clonk)
{
	if (FindObject(Find_Container(this), Find_Func("IsArrow")))
		return true;
	var clonk_ammo = FindObject(Find_Container(clonk), Find_Func("IsArrow"));
	if (clonk_ammo)
	{
		clonk_ammo->Enter(this);
		return true;
	}
	PlayerMessage(clonk->GetOwner(), "$MsgNeedsArrows$");
	return false;
}


/*-- Gun Effect --*/

local FxCrossbow = new Effect 
{
	Construction = func(object clonk, int angle, int angle_prec)
	{
		this.Interval = Target.FireRate;
		this.shooter = clonk;
		this.aim_angle = angle;
		this.aim_prec = angle_prec;
		// Perform the first shot.
		this->Timer(0);
	},
	UpdateAimingAngle = func(int new_angle)
	{
		this.aim_angle = new_angle;	
	},
	Timer = func(int time)
	{
		Target->CheckForArrows(this.shooter);
		var arrow = FindObject(Find_Container(Target), Find_Func("IsArrow"));
		if (!arrow)
			return FX_Execute_Kill;
		Target->FireArrow(arrow, this.aim_angle, this.aim_prec);
		return FX_OK;
	}
};

public func FireArrow(object arrow, int angle, int ang_prec)
{
	// Take one bullet and shoot it.
	var shot = arrow->TakeObject();
	shot->Launch(angle / ang_prec, this.FireSpeed, this);
	Sound("Objects::Weapons::Bow::Shoot?");
	return;
}


/*-- Contents -- */

public func RejectCollect(id def, object obj)
{
	// Only accept arrows and at most 10 packs.
	if (!obj->~IsArrow() || ContentsCount() >= 10)
		return true;
	return false;
}


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local BorderBound = C4D_Border_Sides;
local ContactCalls = true;
local Components = {Wood = 2, Metal = 2};

// Number of frames for one shot and speed of the shot arrow.
local FireRate = 12;
local FireSpeed = 180;
