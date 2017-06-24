/**
	Crossbow
	Shoots arrows at a fast rate.
	
	@author Maikel
*/


#include Library_Cannon
#include Library_ElevatorControl


local crossbow_fx;
local arrow_priority;

public func Initialize()
{
	// Fill arrow priority list with loaded arrows.
	arrow_priority = [];
	var index = 0, def;
	while (def = GetDefinition(index++))
		if (def->~IsArrow())
			PushBack(arrow_priority, def);		
	return _inherited(...);
}


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
		var arrow = FindObject(Find_Container(Target), Find_Func("IsArrow"), Sort_Func("SortArrowPriority"));
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


/*-- Interaction --*/

public func HasInteractionMenu() { return true; }

public func GetInteractionMenus(object clonk)
{
	var menus = _inherited(clonk, ...) ?? [];		
	var arrow_menu =
	{
		title = "$ArrowPriority$",
		entries_callback = this.GetArrowSelectionMenuEntries,
		callback = "OnArrowSelection",
		callback_hover = "OnArrowSelectionHover",
		callback_target = this,
		BackgroundColor = RGB(0, 50, 50),
		Priority = 25,
		extra_data = "description"
	};
	PushBack(menus, arrow_menu);
	return menus;
}

public func GetArrowSelectionMenuEntries(object clonk)
{
	var menu_entries = [];
	// Add arrow types to the selection.
	var index = 0;
	for (var arrow in arrow_priority)
	{
		index++;
		var act = "move_up";
		PushBack(menu_entries, 
			{
				symbol = arrow, extra_data = act, 
				custom =
				{
					Right = "2em", Bottom = "2em",
					BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
					Priority = index,
					status = {Right = "1em", Top = "1em", Symbol = Icon_Number, GraphicsName = Format("%d", index)},
					image = {Symbol = arrow}
			}}
		);
	}
	return menu_entries;
}

public func OnArrowSelectionHover(id symbol, string action, desc_menu_target, menu_id)
{
	var text = "";
	if (action == "move_up") text = Format("$ArrowMoveUp$", symbol->GetName());
	else if (action == "description") text = "$ArrowPriorityDescription$";
	GuiUpdateText(text, menu_id, 1, desc_menu_target);
}

public func OnArrowSelection(symbol_or_object, string action, bool alt)
{
	if (action == "move_up")
		MoveArrowUpPriority(symbol_or_object);
	UpdateInteractionMenus(this.GetArrowSelectionMenuEntries);	
}

public func MoveArrowUpPriority(id arrow)
{
	var arrow_index = GetIndexOf(arrow_priority, arrow);
	if (arrow_index <= 0)
		return;
	var swap = arrow_priority[arrow_index];
	arrow_priority[arrow_index] = arrow_priority[arrow_index - 1];
	arrow_priority[arrow_index - 1] = swap;
	return;
}


// Todo: make this non-global.
global func SortArrowPriority()
{
	return GetIndexOf(Contained().arrow_priority, this->GetID());
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
