/**
	Gatling Gun
	Shoots bullets at a fast rate.
	
	@author Maikel
*/


#include Library_Cannon
#include Library_ElevatorControl


local gatling_fx;


/*-- Controls --*/

public func CannonUseStart(object frame, object clonk, int x, int y)
{
	// Check for ammunition.
	var has_bullets = CheckForBullets(clonk);
	if (!has_bullets)
	{
		clonk->CancelUse();
		return true;
	}
	// Create shooting effect.
	var angle = CoordinatesToAngle(frame, x, y);
	gatling_fx = CreateEffect(FxGatlingGun, 100, this.FireRate, clonk, angle, frame.AimingAnglePrecision);
	return true;
}

public func CannonUseHolding(object frame, object clonk, int x, int y)
{
	var angle = CoordinatesToAngle(frame, x, y);
	if (gatling_fx)
		gatling_fx->UpdateAimingAngle(angle);
	return true;
}

public func CannonUseStop(object frame, object clonk, int x, int y)
{
	if (gatling_fx)
		gatling_fx->Remove();
	return true;
}

public func CannonUseCancel(object frame)
{
	if (gatling_fx)
		gatling_fx->Remove();
	return true;
}

// Takes bullets from either the clonk or the gun itself.
private func CheckForBullets(object clonk)
{
	if (FindObject(Find_Container(this), Find_Func("IsBullet")))
		return true;
	var clonk_ammo = FindObject(Find_Container(clonk), Find_Func("IsBullet"));
	if (clonk_ammo)
	{
		clonk_ammo->Enter(this);
		return true;
	}
	PlayerMessage(clonk->GetOwner(), "$MsgNeedsAmmo$");
	return false;
}


/*-- Gun Effect --*/

local FxGatlingGun = new Effect 
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
		Target->CheckForBullets(this.shooter);
		var ammo = FindObject(Find_Container(Target), Find_Func("IsBullet"));
		if (!ammo)
			return FX_Execute_Kill;
		Target->FireBullet(ammo, this.aim_angle, this.aim_prec);
		return FX_OK;
	}
};

public func FireBullet(object ammo, int angle, int ang_prec)
{
	// Take one bullet and shoot it.
	var shot = ammo->TakeObject();
	shot->Launch(this, angle + RandomX(-2 * ang_prec, 2 * ang_prec), 10, 200, 0, 0, ang_prec);
	Sound("Objects::Weapons::Blunderbuss::GunShoot?");

	// Muzzle flash and gun smoke.
	var barrel_x = Sin(angle, 10, ang_prec);
	var barrel_y = -Cos(angle, 10, ang_prec);
	CreateParticle("Smoke", barrel_x, barrel_y, PV_Random(barrel_x / 2 - 5, barrel_x / 2 + 5), PV_Random(barrel_y / 2 - 5, barrel_y / 2 + 5), PV_Random(40, 60), Particles_Smoke(), 8);
	CreateMuzzleFlash(barrel_x, barrel_y, angle / ang_prec, 20);
	CreateParticle("Flash", barrel_x, barrel_y, 0, 0, 8, Particles_Flash());
	return;
}


/*-- Contents -- */

public func RejectCollect(id def, object obj)
{
	// Only accept lead bullets and at most 10 packs.
	if (def != LeadBullet || ContentsCount() >= 10)
		return true;
	return false;
}


/*-- Automation --*/

public func HasAutomationModes() { return true; }

public func GetAutomationModes()
{
	return [
		{mode = "mode::attack_enemy", symbol = Icon_Skull, hover_name = "$MsgModeAttackEnemy$", hover_desc = "$DescModeAttackEnemy$"}
	];
}

public func OnAutomationModeChange(string old_mode, string new_mode)
{
	if (new_mode == "mode::attack_enemy")
	{
		CreateEffect(FxAutomatedControl, 100, this.FireRate);
	}
	else if (new_mode == "mode::off")
	{
		var control_fx = GetEffect("FxAutomatedControl", this);
		if (control_fx)
			control_fx->Remove();
	}
	return;
}

local FxAutomatedControl = new Effect
{
	Construction = func()
	{
		this.Interval = Target.FireRate;
		// Perform the first shot.
		this->Timer(0);
	},
	Timer = func(int time)
	{
		var frame = Target->GetCannonFrame();
		if (!frame)
			return FX_OK;
		var controller = frame->GetController();
		// Find anything hostile that can be hit by a projectile in cannon's range.
		var enemy = Target->FindObject(Find_Hostile(controller), Find_Func("IsProjectileTarget", nil, Target), Target->Find_Distance(400), Target->Find_PathFree(), Target->Sort_Distance());
		if (!enemy)
			return FX_OK;
		var ammo = FindObject(Find_Container(Target), Find_Func("IsBullet"));
		if (!ammo)
			return FX_OK;
		
		// Determine where to shoot.
		var x = Target->GetX();
		var y = Target->GetY();
		var tx = enemy->GetX();
		var ty = enemy->GetY();
		var distance = Distance(x, y, tx, ty);
		var dt = distance * 10 / 200;
		tx += AI_HelperFunctions->GetTargetXDir(enemy, dt);
		ty += AI_HelperFunctions->GetTargetYDir(enemy, dt);
		
		// Check if path of shot is free and is not outside of bounds.
		if (!PathFree(x, y, tx, ty) || !Inside(tx, 0, LandscapeWidth()) || !Inside(ty, 0, LandscapeHeight()))
			return FX_OK;
			
		// Check for allies on shot path.
		if (!FindObject(Find_ID(Rule_NoFriendlyFire)))
		{
			var ally = Target->FindObject(Find_OnLine(0, 0, tx - x, ty - y), Find_Allied(controller), Find_Func("IsProjectileTarget", nil, Target), Find_Exclude(frame));	
			if (ally)
				return FX_OK;
		}
			
		// Determine angle to target.
		var angle_prec = frame.AimingAnglePrecision;
		var angle = Angle(x, y, tx, ty, angle_prec) + RandomX(-angle_prec, angle_prec);
		
		// Fire a bullet.
		Target->FireBullet(ammo, angle, angle_prec);
		return FX_OK;
	}
};

public func IsArmoryProduct() { return true; }


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local BorderBound = C4D_Border_Sides;
local ContactCalls = true;
local Components = {Metal = 3, Coal = 1};

// Number of frames for one shot.
local FireRate = 4;
