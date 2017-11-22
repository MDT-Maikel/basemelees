/**
	Tesla Cannon
	Shoots a beam of electricity ionizing all gases on its way.
	
	@author Maikel
*/


#include Library_Cannon
#include Library_ElevatorControl
#include Library_PowerConsumer


local tesla_fx;
local tesla_shot_width;

public func Initialize()
{
	// Initialize power system before making any requests.
	_inherited(...);
	// Init show width.
	tesla_shot_width = this.TeslaShotMinWidth;
	return;
}


/*-- Controls --*/

public func CannonUseStart(object frame, object clonk, int x, int y)
{
	// Don't do anything it already shooting.
	if (tesla_fx)
		return true;
	// Create shooting effect.
	var angle = CoordinatesToAngle(frame, x, y);
	PerformTeslaShot(angle);
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
		tesla_fx->DoShot();
	return true;
}

public func CannonUseCancel(object frame)
{
	if (tesla_fx)
		tesla_fx->Remove();
	return true;
}


/*-- Gun Effect --*/

public func PerformTeslaShot(int angle)
{
	tesla_fx = CreateEffect(FxPerformTeslaShot, 100, 1, angle, GetCannonFrame().AimingAnglePrecision);
	// Register power request.	
	RegisterPowerRequest(GetCurrentPowerNeed());
	return;
}

public func GetCurrentPowerNeed()
{
	// The power need depends on the shot width, narrow shots require more power.
	return this.TeslaShotPowerNeedBase + this.TeslaShotPowerNeedAdd * (this.TeslaShotMaxWidth - tesla_shot_width);
}

local FxPerformTeslaShot = new Effect
{
	Construction = func(int angle, int angle_prec)
	{
		this.Interval = 1;
		this.aim_angle = angle;
		this.aim_prec = angle_prec;
		this.has_power = false;
		this.charge = 0;
		// Particles.
		this.part_charge = {
			Prototype = Particles_ElectroSpark1(),
			R = 40,
			G = 40,
			B = 255
		};
		this.part_sphere = {
			Prototype = Particles_Flash(10),
			R = 40,
			G = 40,
			B = 255		
		};
		// Charging sound.
		Target->Sound("Objects::Electrical", nil, 10, nil, +1);
	},
	
	Timer = func(int time)
	{
		// Do charging effects.
		if (this.has_power)
			this.charge++;
		if (this.charge > 0)
			ChargeEffect();
		// Do shot if max charge is reached.
		if (this.charge >= Target.TeslaShotMaxCharge)
		{
			DoShot();
			return FX_Execute_Kill;	
		}
		return FX_OK;
	},
	
	ChargeEffect = func()
	{
		// Color charging according to whether shot is ready.
		this.part_sphere.Alpha = 80 + 175 * this.charge / Target.TeslaShotMaxCharge;
		if (this.charge < Target.TeslaShotMinCharge)
		{
			this.part_sphere.R = 255;
			this.part_sphere.G = 40;
			this.part_sphere.B = 40;
		}
		else
		{
			this.part_sphere.R = 40;
			this.part_sphere.G = 40;
			this.part_sphere.B = 255;
		}
		var barrel_x = Sin(this.aim_angle, 10, this.aim_prec);
		var barrel_y = -Cos(this.aim_angle, 10, this.aim_prec);
		Target->CreateParticle("ElectroSpark", PV_Random(barrel_x - 4, barrel_x + 4), PV_Random(barrel_y - 4, barrel_y + 4), PV_Random(-4, 4), PV_Random(-4, 4), PV_Random(4, 6), this.part_charge, this.charge / 4);
		Target->CreateParticle("SphereSpark", barrel_x, barrel_y, PV_Random(-1, 1), PV_Random(-1, 1), PV_Random(3, 4), this.part_sphere, this.charge / 4);
		// Increase charging sound volume.
		Target->Sound("Objects::Electrical", nil, 10 + 90 * this.charge / Target.TeslaShotMaxCharge, nil, +1);
	},
	
	Destruction = func()
	{
		// Unregister power request.
		Target->UnregisterPowerRequest();
		// End charging sound.
		Target->Sound("Objects::Electrical", nil, nil, nil, -1);
	},

	UpdateAimingAngle = func(int new_angle)
	{
		this.aim_angle = new_angle;	
	},
	
	DoShot = func()
	{
		// Don't shoot if not charged enough.
		if (this.charge < Target.TeslaShotMinCharge)
			return Remove();		
		var strength = 40 + (80 * this.charge - Target.TeslaShotMinCharge) / (Target.TeslaShotMaxCharge - Target.TeslaShotMinCharge);
		var barrel_x = Sin(this.aim_angle, 10, this.aim_prec);
		var barrel_y = -Cos(this.aim_angle, 10, this.aim_prec);
		var xdir = Sin(this.aim_angle, Target->GetTeslaShotSpeed(), this.aim_prec);
		var ydir = -Cos(this.aim_angle, Target->GetTeslaShotSpeed(), this.aim_prec);
		var dev = 2 * Target->GetTeslaShotWidth();
		Target->LaunchLightning(Target->GetX() + barrel_x, Target->GetY() + barrel_y, strength, xdir, ydir, dev, dev, true);
		// Graphical effect for the shot.
		this.part_sphere.Size = PV_Random(10, 18);
		Target->CreateParticle("SphereSpark", barrel_x, barrel_y, PV_Random(-1, 1), PV_Random(-1, 1), PV_Random(4, 5), this.part_sphere, this.charge / 4);
		// Remove this effect, since the shot is done.
		this.charge = 0;
		return Remove();
	},
	
	GetCharge = func()
	{
		return this.charge;
	},
	
	OnEnoughPower = func()
	{
		this.has_power = true;
	},

	OnNotEnoughPower = func()
	{
		this.has_power = false;
	}
};

public func GetTeslaShotControl()
{
	return tesla_fx;
}

public func GetTeslaShotWidth()
{
	return tesla_shot_width;
}

public func GetTeslaShotSpeed()
{
	// Increase the shot speed a bit if the beam become more narrow.
	return this.TeslaShotSpeed + TeslaShotMaxWidth - tesla_shot_width;
}


/*-- Power --*/

public func GetConsumerPriority() { return 75; }

public func OnEnoughPower()
{
	if (tesla_fx)
		tesla_fx->OnEnoughPower();
	return _inherited(...);
}

public func OnNotEnoughPower()
{
	if (tesla_fx)
		tesla_fx->OnNotEnoughPower();
	return _inherited(...);
}


/*-- Interaction --*/

public func HasInteractionMenu() { return true; }

public func GetInteractionMenus(object clonk)
{
	var menus = _inherited(clonk, ...) ?? [];	
	var arrow_menu =
	{
		title = "$TeslaShotWidth$",
		entries_callback = this.GetTeslaShotMenuEntries,
		entries_callback_target = this,
		callback = "OnTeslaSelection",
		callback_hover = "OnTeslaSelectionHover",
		callback_target = this,
		BackgroundColor = RGB(0, 50, 50),
		Priority = 25,
		extra_data = "description"
	};
	PushBack(menus, arrow_menu);
	return menus;
}

public func GetTeslaShotMenuEntries(object clonk)
{
	var menu_entries = [];
	PushBack(menu_entries, {symbol = this, extra_data = "decrease",
		custom = 
		{
			Right = "2em", Bottom = "2em",
			BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
			Symbol = Icon_Number,
			GraphicsName = "Minus"
		}
	});	
	PushBack(menu_entries, {symbol = this, extra_data = "description",
		custom =
		{
			Right = "100%-4em", Bottom = "2em",
			BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
			text = 
			{
				Bottom = "1em",
				Style = GUI_TextVCenter,
				Text = "$TeslaAdjustShotWidth$"
			},
			width = 
			{
				Top = "1em", Right = "50%",
				Style = GUI_TextVCenter,
				Text = Format("$TeslaCurrentShotWidth$", GetTeslaShotWidth())			
			},
			power =
			{
				Top = "1em", Left = "50%",
				Style = GUI_TextVCenter,
				Text = Format("$TeslaCurrentPowerNeed$", GetCurrentPowerNeed() / 10)
			}
		}
	});
	PushBack(menu_entries, {symbol = this, extra_data = "increase",
		custom = 
		{
			Left = "100%-2em", Bottom = "2em",
			BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
			Symbol = Icon_Number,
			GraphicsName = "Plus"
		}
	});
	return menu_entries;
}

public func OnTeslaSelectionHover(id symbol, string action, desc_menu_target, menu_id)
{
	var text = "";
	if (action == "decrease") text = "$DescDecreaseWidth$";
	else if (action == "increase") text = "$DescIncreaseWidth$";
	else if (action == "description") text = "$DescTeslaShotWidth$";
	GuiUpdateText(text, menu_id, 1, desc_menu_target);
}

public func OnTeslaSelection(symbol_or_object, string action, bool alt)
{
	if (action == "increase")
		tesla_shot_width = Min(tesla_shot_width + 1, this.TeslaShotMaxWidth);
	else if (action == "decrease")
		tesla_shot_width = Max(tesla_shot_width - 1, this.TeslaShotMinWidth);
	UpdateInteractionMenus(this.GetTeslaShotMenuEntries);
	var frame = GetCannonFrame();
	if (frame)
		frame->UpdateInteractionMenus(this.GetTeslaShotMenuEntries);
	return;		
}


/*-- Lightning Attraction --*/

/*public func IsLightningStrikable(object lightning) { return true; }

public func GetLightningAttractionOffset() { return [0, 0]; }

public func OnLightningStrike(object lightning, int damage)
{
	Log("damage = %d", damage);
	return;
}

public func OnCannonMount(object onto_frame)
{
	onto_frame.IsLightningAttractor = this.IsLightningAttractor;
	onto_frame.IsLightningStrikable = this.IsLightningStrikable;
	onto_frame.OnLightningStrike = this.OnLightningStrike;
	return _inherited(onto_frame);;
}

public func OnCannonDismount(object from_frame)
{
	from_frame.IsLightningAttractor =  from_frame->GetID().IsLightningAttractor;
	from_frame.IsLightningStrikable = from_frame->GetID().IsLightningStrikable;
	from_frame.OnLightningStrike = from_frame->GetID().OnLightningStrike;
	return _inherited(from_frame);;
}*/


/*-- Contents --*/

public func IsContainer() { return false; }

public func RejectCollect(id def, object obj)
{
	// Don't accept any contents.
	return true;
}


/*-- Automation --*/

public func HasAutomationModes() { return true; }

public func GetAutomationModes()
{
	return [
		{mode = "mode::attack_enemy", symbol = Icon_Skull, hover_name = "$MsgModeAttackEnemy$", hover_desc = "$DescModeAttackEnemy$"},
		{mode = "mode::attack_meteor", symbol = Meteor, hover_name = "$MsgModeAttackMeteor$", hover_desc = "$DescModeAttackMeteor$"}
	];
}

public func OnAutomationModeChange(string old_mode, string new_mode)
{

	if (new_mode == "mode::attack_enemy" || new_mode == "mode::attack_meteor")
	{
		CreateEffect(FxAutomatedControl, 100, 1, new_mode);
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
	Construction = func(string new_mode)
	{
		this.attack_mode = new_mode;
		this.Interval = 1;
		this.target = nil;
		this.tesla_shot = nil;
	},
	Timer = func(int time)
	{
		var frame = Target->GetCannonFrame();
		if (!frame)
			return FX_OK;
		var controller = frame->GetController();
			
		// Determine shot parameters
		var shot_speed = 10 * Target->GetTeslaShotSpeed();
		var max_distance = 300 + shot_speed;
			
		// Find a target.
		if (!this.target)
		{
			if (this.attack_mode == "mode::attack_meteor")
				this.target = Target->FindObject(Find_ID(Meteor), Target->Find_Distance(max_distance), Target->Find_PathFree(), Target->Sort_Distance());
			else if (this.attack_mode == "mode::attack_enemy")	
				this.target = Target->FindObject(Find_Hostile(controller), Find_Func("IsProjectileTarget", nil, Target), Target->Find_Distance(max_distance), Target->Find_PathFree(), Target->Sort_Distance());
		}
		if (!this.target)
		{
			// Remove current shot if is running.
			if (this.tesla_shot)
				this.tesla_shot->Remove();
			return FX_OK;
		}
		
		// Determine where to shoot.
		var x = Target->GetX();
		var y = Target->GetY();
		var tx = this.target->GetX();
		var ty = this.target->GetY();
		var distance = Distance(x, y, tx, ty);
		var dt = distance * 10 / shot_speed;
		tx += AI_HelperFunctions->GetTargetXDir(this.target, dt);
		ty += AI_HelperFunctions->GetTargetYDir(this.target, dt);
		
		// Check if path of shot is free and is not outside of bounds.
		if (!PathFree(x, y, tx, ty) || !Inside(tx, 0, LandscapeWidth()) || !Inside(ty, 0, LandscapeHeight()))
		{
			// Try to find target in the next iteration.
			this.target = nil;
			return FX_OK;
		}
		
		// Check for allies on shot path.
		if (!FindObject(Find_ID(Rule_NoFriendlyFire)))
		{
			var ally = Target->FindObject(Find_OnLine(0, 0, tx - x, ty - y), Find_Allied(controller), Find_Func("IsProjectileTarget", nil, Target), Find_Exclude(frame));	
			if (ally)
			{
				// Remove shot and restart if ally is not path any more.
				this.target = nil;
				if (this.tesla_shot)
					this.tesla_shot->Remove();
				return FX_OK;
			}
		}
		
		// Determine angle to target.
		var angle_prec = Target->GetCannonFrame().AimingAnglePrecision;
		var angle = Angle(x, y, tx, ty, angle_prec);		

		// Init the shot if not already existing.
		this.tesla_shot = this.tesla_shot ?? Target->GetTeslaShotControl();
		this.tesla_shot = this.tesla_shot ?? Target->PerformTeslaShot(angle);	
		if (!this.tesla_shot)
			return FX_OK;
		
		// Update angle to target.
		this.tesla_shot->UpdateAimingAngle(angle);
		
		// Do shot if charged and target in reach.
		if (this.tesla_shot->GetCharge() >= Target.TeslaShotMinCharge + Max(0, distance - 160) * (Target.TeslaShotMaxCharge - Target.TeslaShotMinCharge) / (max_distance - 160))
		{
			if (this.target)
				this.tesla_shot->DoShot();
			else
				this.tesla_shot->Remove();
			return FX_OK;
		}
		return FX_OK;
	},
	Destruction = func()
	{
		if (this.tesla_shot)
			this.tesla_shot->Remove();
	}
};

public func IsArmoryProduct() { return true; }

public func GetSubstituteComponent(id component)
{
	// Can be made from diamond, ruby or amethyst.
	if (component == Diamond)
		return [Ruby, Amethyst];
}


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local BorderBound = C4D_Border_Sides;
local ContactCalls = true;
local Components = {Metal = 3, Coal = 2, Wood = 2, Diamond = 1};
// Attracts lightning.
local IsLightningAttractor = true;
// Propagation speed for the tesla beam.
local TeslaShotSpeed = 20;
// Amount of power needed while shooting and minimum and maximum shot charging times.
local TeslaShotPowerNeedBase = 250;
local TeslaShotPowerNeedAdd = 10;
local TeslaShotMinCharge = 18;
local TeslaShotMaxCharge = 54;
// Minimum and maximum width of the lightning strike.
local TeslaShotMinWidth = 1;
local TeslaShotMaxWidth = 8;

