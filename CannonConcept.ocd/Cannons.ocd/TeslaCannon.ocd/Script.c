/**
	Tesla Cannon
	Shoots a beam of electricity ionizing all gases on its way.
	
	@author Maikel
*/


#include Library_Cannon
#include Library_ElevatorControl
#include Library_PowerConsumer


local tesla_fx;
local charge_fx;

public func Initialize()
{
	// Initialize power system before making a request.
	_inherited(...);
	// Add an effect to increase charge if power is available.
	charge_fx = CreateEffect(FxCharging, 100, 3);
	return;
}


/*-- Controls --*/

public func CannonUseStart(object frame, object clonk, int x, int y)
{
	// Check for charge.
	var is_charged = CheckCharge(clonk);
	if (!is_charged)
	{
		clonk->CancelUse();
		return true;
	}
	// Create shooting effect.
	var angle = CoordinatesToAngle(frame, x, y);
	tesla_fx = CreateEffect(FxTeslaCannon, 100, this.FireRate, clonk, angle, frame.AimingAnglePrecision, charge_fx);
	return true;
}

public func CannonUseHolding(object frame, object clonk, int x, int y)
{
	var angle = CoordinatesToAngle(frame, x, y);
	if (tesla_fx)
		tesla_fx->UpdateAimingAngle(angle);
	return true;
}

public func CannonUseStop(object frame, object clonk, int x, int y)
{
	if (tesla_fx)
		tesla_fx->Remove();
	return true;
}

public func CannonUseCancel(object frame)
{
	if (tesla_fx)
		tesla_fx->Remove();
	return true;
}

private func CheckCharge(object clonk)
{
	if (charge_fx->GetCharge() > 0)
		return true;
	PlayerMessage(clonk->GetOwner(), "$MsgNeedsCharge$");
	return false;
}


/*-- Gun Effect --*/

local FxTeslaCannon = new Effect 
{
	Construction = func(object clonk, int angle, int angle_prec, effect fx_charge)
	{
		this.Interval = Target.FireRate;
		this.shooter = clonk;
		this.aim_angle = angle;
		this.aim_prec = angle_prec;
		this.fx_charge = fx_charge;
		// Perform the first beam.
		this->Timer(0);
	},
	UpdateAimingAngle = func(int new_angle)
	{
		this.aim_angle = new_angle;	
	},
	Timer = func(int time)
	{
		Target->CheckCharge(this.shooter);
		this.fx_charge->DoCharge(-1);
		var barrel_x = Sin(this.aim_angle, 10, this.aim_prec);
		var barrel_y = -Cos(this.aim_angle, 10, this.aim_prec);
		var xdir = Sin(this.aim_angle, 20, this.aim_prec);
		var ydir = -Cos(this.aim_angle, 20, this.aim_prec);
		LaunchLightning(Target->GetX() + barrel_x, Target->GetY() + barrel_y, 40, xdir, ydir, 2, 2, true);
		return FX_OK;
	}
};


/*-- Power --*/

public func GetConsumerPriority() { return 25; }

public func OnEnoughPower()
{
	if (!charge_fx)
		return;
	// Make charging effect active and start sound.
	charge_fx.active = true;
	return _inherited(...);
}

public func OnNotEnoughPower()
{
	if (!charge_fx)
		return;
	// Make charging inactive and stop sound.
	charge_fx.active = false;
	return _inherited(...);
}

local FxCharging = new Effect
{
	Construction = func()
	{
		this.Interval = 3;
		// Also ensure initial charge and power need are known.
		this.active = false;
		this.charge = 0;
		this.power_need = 40;
		this.max_charge = 50;
		this.sound = false;
		this.request = false;
	},
	Timer = func()
	{
		// Increase charge if there is power and control sound.
		if (this.active && Target->IsMounted())
		{
			this.charge++;
			if (!this.sound)
			{
				this.sound = true;
				Target->Sound("Objects::Electrical", nil, 50, nil, 1);
			}
			if (this.charge >= this.max_charge)
			{
				this.request = false;
				Target->UnregisterPowerRequest();
				this.active = false;
			}
		}
		else
		{
			if (this.sound)
			{
				this.sound = false;
				Target->Sound("Objects::Electrical", nil, 50, nil, -1);
			}
			if (!this.request && this.charge < this.max_charge)
			{
				this.request = true;
				Target->RegisterPowerRequest(this.power_need);
			}
		}
	},
	Destruction = func()
	{
		// Unmake this object a power consumer.
		Target->UnregisterPowerRequest();
		// Turn off sound if it was still on.
		if (this.sound)
			Target->Sound("Objects::Electrical", nil, 50, nil, -1);
	},
	
	SetCharge = func(int to)
	{
		this.charge = to;
		return;
	},
	GetCharge = func()
	{
		return this.charge;
	},
	DoCharge = func(int add)
	{
		this.charge += add;
		return;
	},
	GetChargePercentage = func()
	{
		return 100 * this.charge / this.max_charge;
	}
};


/*-- Interaction Menu --*/

public func HasInteractionMenu() { return true; }

public func GetInteractionMenus(object clonk)
{
	var menus = _inherited(clonk, ...) ?? [];
	
	var charge_menu =
	{
		title = "$MenuCannonCharge$",
		entries_callback = this.GetChargeMenuEntries,
		entries_callback_target = this,
		//callback = "OnDoorControl",
		//callback_hover = "OnDoorControlHover",
		callback_target = this,
		BackgroundColor = RGB(0, 50, 50),
		Priority = 30
	};
	PushBack(menus, charge_menu);
	return menus;
}

public func GetChargeMenuEntries(object clonk)
{
	var menu_entries = [];
	PushBack(menu_entries, GetChargeMenuEntry(Library_PowerConsumer, Format("$MsgCannonCharge$", charge_fx->GetChargePercentage()), 1, "closeright"));		
	return menu_entries;
}

public func GetChargeMenuEntry(symbol, string text, int priority, extra_data)
{
	var custom_entry = 
	{
		Right = "100%", Bottom = "2em",
		BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
		image = {Right = "2em"},
		text = {Left = "2em"}
	};
	return {symbol = symbol, extra_data = extra_data, 
		custom =
		{
			Prototype = custom_entry,
			Priority = priority,
			text = {Prototype = custom_entry.text, Text = text},
			image = {Prototype = custom_entry.image, Symbol = symbol}
		}
	};
}


/*-- Contents --*/

public func IsContainer() { return false; }

public func RejectCollect(id def, object obj)
{
	// Don't accept any contents.
	return true;
}

public func IsArmoryProduct() { return true; }


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local BorderBound = C4D_Border_Sides;
local ContactCalls = true;
local Components = {Metal = 3, Coal = 2, Wood = 2, Diamond = 1};
// Number of frames for one beam.
local FireRate = 2;

