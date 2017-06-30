/**
	Artillery Cannon
	Fires objects great distances with the force of gunpowder.
	
	@author Maikel
*/


#include Library_Cannon
#include Library_ElevatorControl


/*-- Controls --*/

public func CannonUseStart(object frame, object clonk, int x, int y)
{
	// Check for gun powder.
	var keg = CheckForPowderKeg(clonk);
	if (!keg)
	{
		clonk->CancelUse();
		return true;
	}
	var ammo = CheckForAmmunition(clonk);
	if (!ammo)
	{
		clonk->CancelUse();
		return true;
	}
	return true;
}

public func CannonUseHolding(object frame, object clonk, int x, int y)
{
	var angle = CoordinatesToAngle(frame, x, y);
	Trajectory->Create(frame, GetX() + 5, GetY() + 2, Sin(angle, this.ProjectileSpeed, frame.AimingAnglePrecision), -Cos(angle, this.ProjectileSpeed, frame.AimingAnglePrecision));
	return true;
}

public func CannonUseStop(object frame, object clonk, int x, int y)
{
	// Check for gun powder again.
	var keg = CheckForPowderKeg(clonk);
	if (!keg)
		return true;
	
	// Check for ammunition again.
	var projectile = CheckForAmmunition(clonk);
	if (!projectile)
		return true;
	
	// Check if cooling down.
	if (GetEffect("FxCoolDown", this))
		return true;
		
	// Take gun powder.
	keg->DoPowderCount(-1);
	if (keg->GetPowderCount() == 0)
	{
		keg->RemoveObject();
		CreateObjectAbove(Barrel);
	}
	CreateEffect(FxCoolDown, 100, 1);
			
	// Fire cannon.
	FireCannon(projectile, frame, clonk, x, y);
	
	Trajectory->Remove(frame);
	return true;
}

public func CannonUseCancel(object frame)
{
	Trajectory->Remove(frame);
	return true;
}


/*-- Shooting --*/

private func FireCannon(object projectile, object frame, object clonk, int x, int y)
{
	// Launch projectile.
	var angle = CoordinatesToAngle(frame, x, y);
	projectile->SetR(angle / frame.AimingAnglePrecision);
	projectile->SetRDir(RandomX(-4, 4));
	projectile->LaunchProjectile(angle, 17, this.ProjectileSpeed, 0, 0, frame.AimingAnglePrecision);
	projectile->SetController(frame->GetController());
	projectile->~OnCannonShot(this);
	
	// Particles.
	var dist = 25;
	var px = Sin(angle, dist, frame.AimingAnglePrecision);
	var py = -Cos(angle, dist, frame.AimingAnglePrecision) - 4;
	CreateParticle("Flash", px, py, 0, 0, 8, Particles_Flash());
	CreateParticle("Smoke", px, py, PV_Random(px - 20, px + 20), PV_Random(py - 20, py + 20), PV_Random(40, 60), Particles_Smoke(), 20);
	CreateMuzzleFlash(px, py, angle / frame.AimingAnglePrecision, 60);
	
	// Sound.
	Sound("Fire::Blast3");
	return;
}

private func CheckForPowderKeg(object clonk)
{
	// Check for powderkeg, is the only content.
	var keg = Contents(0);
	if (!keg)
	{
		// Look for a keg inside the shooter.
		keg = FindObject(Find_Container(clonk), Find_ID(PowderKeg));
		// If there is a keg, put into cannon.
		if (keg)
		{
			keg->Enter(this);
			Sound("Hits::Materials::Wood::WoodHit?");
		}
		// No keg, stop using cannon.
		else
		{
			PlayerMessage(clonk->GetOwner(), "$MsgNeedsPowderKeg$");
			return nil;
		}
	}
	return keg;
}

private func CheckForAmmunition(object clonk)
{
	var ammo = clonk->GetHandItem(0);
	if (!ammo)
		PlayerMessage(clonk->GetOwner(),"$MsgNeedsAmmo$");	
	return ammo;
}

local FxCoolDown = new Effect
{
	Timer = func(int time)
	{
		if (time >= 72)
			return FX_Execute_Kill;
		return FX_OK;	
	}
};

public func IsArmoryProduct() { return true; }


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local BorderBound = C4D_Border_Sides;
local ContactCalls = true;
local Components = {Metal = 4, Wood = 2};
local ProjectileSpeed = 175;