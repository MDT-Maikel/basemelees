/**
	Liquid Cannon
	Shoots a spray of liquid over a short distance.
	
	@author Maikel
*/


#include Library_Cannon
#include Library_ElevatorControl
#include Library_Tank


local liquid_fx;


/*-- Controls --*/

public func CannonUseStart(object frame, object clonk, int x, int y)
{
	var angle = CoordinatesToAngle(frame, x, y) / frame.AimingAnglePrecision;
	liquid_fx = CreateEffect(FxSprayLiquid, 100, nil, angle);
	return true;
}

public func CannonUseHolding(object frame, object clonk, int x, int y)
{
	var angle = CoordinatesToAngle(frame, x, y) / frame.AimingAnglePrecision;
	if (liquid_fx)
		liquid_fx->UpdateAimingAngle(angle);
	return true;
}

public func CannonUseStop(object frame, object clonk, int x, int y)
{
	if (liquid_fx)
		liquid_fx->Remove();
	return true;
}

public func CannonUseCancel(object frame)
{
	if (liquid_fx)
		liquid_fx->Remove();
	return true;
}


/*-- Liquid Spray --*/

// Effect for spraying liquids.
local FxSprayLiquid = new Effect 
{
	Construction = func(int angle)
	{
		// Execute timer every frame.
		this.Interval = 1;
		this.aim_angle = angle;
		Target->Sound("Liquids::Pshshsh", nil, nil, nil, +1);
	},
	UpdateAimingAngle = func(int new_angle)
	{
		// Update angle when holding use.
		this.aim_angle = new_angle;	
	},
	Timer = func(int time)
	{
		// Get the current liquid in the cannon.
		var liquid = Target->RemoveLiquid(nil, Target.LiquidSprayStrength);
		if (liquid[0] == nil)
			return FX_OK;
		
		// Determine liquid type and amount available.
		var mat = liquid[0];
		var amount = liquid[1];
		
		// Cast liquid pixels in the aimed direction.
		var x = Sin(this.aim_angle, 10) + RandomX(-1, 1);
		var y = -Cos(this.aim_angle, 10) + RandomX(-1, 1);
		var spread = Target.LiquidSpraySpread;
		Target->CastPXS(mat, amount, Target.LiquidSpraySpeed, x, y, this.aim_angle, spread);
		
		// Launch a dummy object which performs spray effects at its position.
		var spray = Target->CreateObject(Dummy, x, y);
		spray->SetCategory(C4D_None);
		spray->SetVelocity(RandomX(this.aim_angle - spread / 2, this.aim_angle + spread / 2), Target.LiquidSpraySpeed);
		
		spray->CreateEffect(Target.FxSprayEffects, 100, nil, mat, amount, Target->GetController());
		return FX_OK;
	},
	Destruction = func()
	{
		Target->Sound("Liquids::Pshshsh", nil, nil, nil, -1);
	}
};

// Effect for the effects of the liquid spray, like extinguishing.
local FxSprayEffects = new Effect
{
	Construction = func(string mat, int strength, int causing_plr)
	{
		// Execute timer every frame.
		this.Interval = 1;
		// Get spray properties.
		this.mat = mat;
		this.str = strength;
		this.causing_plr;
		this.x = Target->GetX();
		this.y = Target->GetY();
		this.ext = GetMaterialVal("Extinguisher", "Material", Material(mat));
		this.inc = GetMaterialVal("Incendiary", "Material", Material(mat));
		this.wind_drift = GetMaterialVal("WindDrift", "Material", Material(mat)) - 20;
		// Don't do anything for materials that do not extinguish or incinerate.
		if (!this.ext && !this.inc)
			return FX_Execute_Kill;
	},
	Timer = func(int time)
	{
		// Remove spray effects if it hits solid material or is above landscape bound (PXS
		// are removed if their y-coordinate is above -10).
		if (!PathFree(this.x, this.y, Target->GetX(), Target->GetY()) || Target->GetY() < -10)
		{
			Target->RemoveObject();
			return FX_Execute_Kill;
		}
		
		// Correct dummy movement as PXS are influenced by wind (see C4PXS engine code).
		if (this.wind_drift > 0)
		{
			// Take into account randomness for PXS movement (see C4PXS engine code).
			var to_xdir = 1000 * Target->GetWind() / 15 + 1000 * RandomX(-600, 600) / 256;
			var to_ydir = 1000 * RandomX(-600, 600) / 256;
			var xdir = Target->GetXDir(1000);
			var ydir = Target->GetYDir(1000);
			var add_xdir = (to_xdir - xdir) * this.wind_drift / 800;
			var add_ydir = (to_ydir - ydir) * this.wind_drift / 800;
			Target->SetXDir(xdir + add_xdir, 1000);
			Target->SetYDir(ydir + add_ydir, 1000);
		}
		
		// Extinguish objects at current position of dummy.
		if (this.ext)
			for (var obj in Target->FindObjects(Find_Or(Find_AtPoint(), Find_Distance(6)), Find_OCF(OCF_OnFire)))
				obj->Extinguish(this.str);
		
		// Incinerate objects at current position of dummy.
		if (this.inc)
		{
			for (var obj in Target->FindObjects(Find_Or(Find_AtPoint(), Find_Distance(6))))
			{
				var inf = obj.ContactIncinerate;
				if (!inf)
					continue;
				obj->Incinerate(this.str / Max(1, inf), this.causing_plr);
			}
		}
		return FX_OK;
	}
};


/*-- Liquid Container --*/

// Only accept a single liquid at the same time, but accept any liquid type.
public func IsLiquidContainerForMaterial(string liquid)
{
	for (var liquid_content in GetLiquidContents())
		if (GetLiquidDef(liquid) != GetLiquidDef(liquid_content->GetID()))
			return false;
	return true;
}

public func GetLiquidContainerMaxFillLevel(liquid_name)
{
	return 2400;
}

// The liquid cannon may have one drain pipe.
public func QueryConnectPipe(object pipe)
{
	if (pipe->IsDrainPipe() && GetDrainPipe())
		return true;
	else if (pipe->IsAirPipe() || pipe->IsSourcePipe())
		return true;
	return false;
}

// Set to to drain pipe.
public func OnPipeConnect(object pipe, string specific_pipe_state)
{
	if (PIPE_STATE_Drain == specific_pipe_state)
	{
		SetDrainPipe(pipe);
		pipe->SetDrainPipe();
	}
	else
	{
		if (!GetDrainPipe())
			OnPipeConnect(pipe, PIPE_STATE_Drain);
	}
	pipe->Report("$MsgConnectedPipe$");
}


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local BorderBound = C4D_Border_Sides;
local ContactCalls = true;
local Components = {Metal = 3, Pipe = 1};

// Liquid spray properties: amount of PXS per frame, speed of PXS, spread of the PXS spray.
local LiquidSprayStrength = 10;
local LiquidSpraySpeed = 120;
local LiquidSpraySpread = 8;