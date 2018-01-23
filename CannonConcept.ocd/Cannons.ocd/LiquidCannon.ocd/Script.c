/**
	Liquid Cannon
	Shoots a spray of liquid over a short distance.
	
	@author Maikel
*/


#include Library_Cannon
#include Library_ElevatorControl
#include Library_Tank


local liquid_fx;
local last_aim_angle;


/*-- Controls --*/

public func CannonUseStart(object frame, object clonk, int x, int y)
{
	last_aim_angle = CoordinatesToAngle(frame, x, y) / frame.AimingAnglePrecision;
	if (!liquid_fx)
		liquid_fx = CreateEffect(FxSprayLiquid, 100, nil, last_aim_angle);
	return true;
}

public func CannonUseHolding(object frame, object clonk, int x, int y)
{
	last_aim_angle = CoordinatesToAngle(frame, x, y) / frame.AimingAnglePrecision;
	if (liquid_fx)
		liquid_fx->UpdateAimingAngle(last_aim_angle);
	return true;
}

public func CannonUseStop(object frame, object clonk, int x, int y)
{
	if (liquid_fx && !liquid_fx.is_automated)
		liquid_fx->Remove();
	return true;
}

public func CannonUseCancel(object frame)
{
	if (liquid_fx && !liquid_fx.is_automated)
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
		// Do not spray liquid if not mounted.
		if (!Target->GetCannonFrame())
			return FX_OK;	
		
		// Get the current liquid in the cannon.
		var liquid = Target->RemoveLiquid(nil, Target.LiquidSprayStrength);
		if (liquid[0] == nil)
			return FX_OK;
		
		// Determine liquid type and amount available.
		var mat = liquid[0];
		var amount = liquid[1];
		// Convert liquid type to material type if needed.
		if (Material(mat) == -1)
			mat = GetDefinition(mat)->~GetLiquidMaterial();
		
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
		this.cor = GetMaterialVal("Corrosive", "Material", Material(mat));
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
		
		// Damages living beings at current position of dummy.
		if (this.cor)
		{
			for (var obj in Target->FindObjects(Find_Or(Find_AtPoint(), Find_Distance(6)), Find_OCF(OCF_Alive)))
			{
				if (obj.CorrosionResist)
					continue;
				obj->DoEnergy(-this.cor * this.str, true, FX_Call_EngCorrosion, this.causing_plr);
				if (!obj)
					continue;
				if (!Random(3))
					obj->~PlaySoundScream();
				else if (!Random(2))
					obj->~PlaySoundHurt();
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
public func QueryConnectPipe(object pipe, bool do_msg)
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


/*-- Interaction --*/

public func OnPipeControl(symbol_or_object, string action, bool alt)
{
	_inherited(symbol_or_object, action, alt, ...);
	// Forward updates to cannon frame if mounted.
	var frame = GetCannonFrame();
	if (frame)
		frame->UpdateInteractionMenus(this.GetPipeControlMenuEntries);
	return;
}


/*-- Name & Description --*/

public func RemoveLiquid(liquid_name, int amount, object destination)
{
	var res = _inherited(liquid_name, amount, destination, ...);
	UpdateName();
	UpdateDescription();	
	return res;
}

public func PutLiquid(liquid_name, int amount, object source)
{
	var res = _inherited(liquid_name, amount, source, ...);
	UpdateName();
	UpdateDescription();	
	return res;
}

private func UpdateName()
{
	var name = "$Name$";
	var liquid = GetLiquidContents()[0];
	if (liquid)
		name = Format("$NameType$", liquid->GetLiquidType());
	this.Name = name;
	return;
}

private func UpdateDescription()
{
	var desc = "$Description$";
	var liquid = GetLiquidContents()[0];
	if (liquid)
		desc = Format("%s $DescriptionType$", desc, liquid->GetLiquidType());
	this.Description = desc;
	return;
}


/*-- Automation --*/

public func HasAutomationModes() { return true; }

public func GetAutomationModes()
{
	return [
		{mode = "mode::continuous", symbol = Icon_Play, hover_name = "$MsgModeContinuous$", hover_desc = "$DescModeContinuous$"},
		{mode = "mode::auto_extinguish", symbol = Icon_Flame, hover_name = "$MsgModeAutoExtinguish$", hover_desc = "$DescModeAutoExtinguish$"}
	];
}

public func OnAutomationModeChange(string old_mode, string new_mode)
{
	if (new_mode == "mode::continuous")
	{
		if (!liquid_fx)
			liquid_fx = CreateEffect(FxSprayLiquid, 100, nil, last_aim_angle);
		liquid_fx.is_automated = true;
	}
	else if (new_mode == "mode::auto_extinguish")
	{
		CreateEffect(FxAutomatedExtinguishControl, 100, 4);
	}
	else if (new_mode == "mode::off")
	{
		var control_fx = GetEffect("FxAutomatedExtinguishControl", this);
		if (control_fx)
			control_fx->Remove();
		if (liquid_fx)
			liquid_fx->Remove();
	}
	return;
}

public func IsArmoryProduct() { return true; }

local FxAutomatedExtinguishControl = new Effect
{
	Construction = func()
	{
		this->Timer(0);
	},
	Timer = func(int time)
	{
		var frame = Target->GetCannonFrame();
		if (!frame)
			return FX_OK;
		var controller = frame->GetController();
		
		// Find anything that is on fire in range.
		var on_fire = Target->FindObject(Find_OCF(OCF_OnFire), Target->Find_Distance(200), Target->Find_PathFree(), Target->Sort_Distance());
		if (!on_fire)
		{
			// Remove liquid control effect.
			if (Target.liquid_fx)
				Target.liquid_fx->Remove();
			return FX_OK;
		}
		
		// Find angle to burning object. TODO: improve.
		var angle = Angle(Target->GetX(), Target->GetY(), on_fire->GetX(), on_fire->GetY());
			
		// Get liquid control effect or turn on.
		if (!Target.liquid_fx)
			Target.liquid_fx = Target->CreateEffect(Target.FxSprayLiquid, 100, nil, angle);
			
		// Update aiming angle.
		Target.liquid_fx->UpdateAimingAngle(angle);
		return FX_OK;
	}
};


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