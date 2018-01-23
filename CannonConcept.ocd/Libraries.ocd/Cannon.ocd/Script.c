/** 
	Cannon Library
	Is used in conjuction with the cannon frame library for the different canon types.
	A cannon should be grabable such that it can be moved to the frame and then be 
	attached. The following callbacks are done in the cannon:
	 * OnCannonMount(object onto_frame)
	 * OnCannonDismount(object from_frame)
	 * QueryRejectCannonMount(object onto_frame)
	 * QueryRejectCannonDismount(object from_frame)

	@author Maikel
*/


// Cannons are destructible.
#include Library_Destructible


// All cannon related local variables are stored in a single proplist.
// This reduces the chances of clashing local variables. See 
// Construction for which variables are being used.
local lib_cannon;


public func Construction()
{
	// Initialize the single proplist for the cannon library.
	if (lib_cannon == nil)
		lib_cannon = {};
	lib_cannon.frame = nil;
	lib_cannon.automated = false;
	lib_cannon.automation_mode = "mode::off";
	return _inherited(...);
}


/*-- Mounting --*/

// Mount this cannon onto the given frame, returns whether successful.
public func MountCannon(object onto_frame)
{
	if (QueryRejectCannonMount(onto_frame) || onto_frame->QueryRejectCannonMount(this))
		return false;
	Enter(onto_frame);
	// Mount callbacks.
	OnCannonMount(onto_frame);
	onto_frame->OnCannonMount(this);
	return true;
}

// Dismount this cannon from the given frame, returns whether successful.
public func DismountCannon(object from_frame)
{
	if (QueryRejectCannonDismount(from_frame) || from_frame->QueryRejectCannonDismount(this))
		return false;
	// Exit cannon and move to bottom of the frame, free if needed.	
	Exit(0, from_frame->GetBottom() - GetBottom(), from_frame->GetR());
	var x = GetX();
	var dx = 0;
	while (dx++ <= 8 && Stuck())
		SetPosition(x + dx, GetY());
	// Dismount callbacks.
	OnCannonDismount(from_frame);
	from_frame->OnCannonDismount(this);
	return false;
}

public func OnCannonMount(object onto_frame)
{
	lib_cannon.frame = onto_frame;
	return;
}

public func OnCannonDismount(object from_frame)
{
	lib_cannon.frame = nil;
	return;
}

public func QueryRejectCannonMount(object onto_frame)
{
	// Cannot mount if already on a frame.
	return !!lib_cannon.frame;
}

public func QueryRejectCannonDismount(object from_frame)
{
	return false;
}

public func GetCannonFrame() { return lib_cannon.frame; }

public func IsMounted() { return !!lib_cannon.frame; }


/*-- Interaction --*/

public func IsInteractable(object clonk)
{
	var frame = FindObject(Find_AtPoint(), Find_NoContainer(), Find_Func("IsCannonFrame"));
	if (!frame)
		return false;
	if (QueryRejectCannonMount(frame) || frame->QueryRejectCannonMount(this))
		return false;
	return true;
}

public func GetInteractionMetaInfo(object clonk)
{
	return { Description = Format("$MsgMountCannon$", this->GetName()) };
}

public func Interact(object clonk)
{
	var frame = FindObject(Find_AtPoint(), Find_Func("IsCannonFrame"));
	if (!frame)
		return false;
	if (QueryRejectCannonMount(frame) || frame->QueryRejectCannonMount(this))
		return false;
	MountCannon(frame);
	return true;
}


/*-- Controls -- */

public func CannonUseStart(object frame, object clonk, int x, int y) { return true; }
public func CannonUseHolding(object frame, object clonk, int x, int y) { return true; }
public func CannonUseStop(object frame, object clonk, int x, int y) { return true; }
public func CannonUseCancel(object frame) { return true; }

// Turns aiming coordinates into the cannon's angle and ranges from 0 to 360 degrees in clockwise direction.
public func CoordinatesToAngle(object frame, int x, int y)
{
	return frame->CoordinatesToAngle(x, y);
}


/*-- Automation --*/

public func HasAutomationModes() { return false; }

public func AddAutomationMechanism(object mechanism)
{
	if (!lib_cannon.automated)
	{
		lib_cannon.automated = true;
		mechanism->RemoveObject();
	}
	return;
}

public func RemoveAutomationMechanism()
{
	if (lib_cannon.automated)
	{
		lib_cannon.automated = false;
		if (lib_cannon.automation_mode != "mode::off")
			SetAutomationMode("mode::off");
		CreateObjectAbove(CannonMechanism, 0, GetBottom(), GetController());
	}
	return;
}

public func IsAutomated()
{
	return lib_cannon.automated;
}

// To be overloaded by the specific cannon must return a list of proplist with the following entries:
// mode (string): string of the form mode::xyz to identify the mode.
// symbol (id): Symbol of the entry.
// graph_name (string): GraphicsName for the entry.
// hover_name (string): name that appears in the menu when hovering.
// hover_desc (string): description that appears in the menu when hovering.
public func GetAutomationModes()
{
	return [];
}

public func SetAutomationMode(string new_mode)
{
	if (lib_cannon.automation_mode == new_mode)
		return;
	this->~OnAutomationModeChange(lib_cannon.automation_mode, new_mode);
	lib_cannon.automation_mode = new_mode;
	return;
}

public func GetAutomationMode() { return lib_cannon.automation_mode; }

public func OnAutomationModeChange(string old_mode, string new_mode)
{
	return;
}

public func GetAutomationModeDescription(string for_mode)
{
	if (for_mode == "mode::off")
		return Format("<c cc0000>%s:</c> %s", "$MsgModeOff$", "$DescModeOff$");
	for (var auto_mode in this->GetAutomationModes())
	{
		if (auto_mode.mode == for_mode)
			return Format("<c cc0000>%s:</c> %s", auto_mode.hover_name, auto_mode.hover_desc);
	}
	return "AUTOMATION DESCRIPTION MISSING";
}


/*-- Automation Menu --*/

public func HasInteractionMenu() { return true; }

public func GetInteractionMenus(object clonk)
{
	var menus = _inherited(clonk, ...) ?? [];
	if (this->HasAutomationModes())
	{
		var automation_menu =
		{
			title = "$AutomationControl$",
			entries_callback = this.GetAutomationMenuEntries,
			entries_callback_target = this,
			callback = "OnAutomationControl",
			callback_hover = "OnAutomationControlHover",
			callback_target = this,
			BackgroundColor = RGB(0, 50, 50),
			Priority = 40
		};
		PushBack(menus, automation_menu);
	}
	return menus;
}

public func GetAutomationMenuEntries(object clonk)
{
	var menu_entries = [];
	// Add info message about automation control.
	var text_automated = "$MsgNotAutomated$";
	var symbol_automated = Icon_Cancel;
	if (lib_cannon.automated)
	{
		text_automated = "$MsgAutomated$";
		symbol_automated = nil;
	}
	PushBack(menu_entries, {symbol = this, extra_data = "description",
		custom =
		{
			Right = "100%", Bottom = "1.2em",
			BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
			Priority = -1,
			BackgroundColor = RGB(100, 25, 25),
			text = {Left = "2em", Text = text_automated},
			image = 
			{
				Right = "2em", Symbol = CannonMechanism,
				on = {Top = "50%", Symbol = symbol_automated}	
			}
		}
	});

	// Add or remove a mechanism.
	var mechanism = FindObject(Find_Container(clonk), Find_ID(CannonMechanism));
	if (lib_cannon.automated)
	{
		PushBack(menu_entries, GetAutomationControlMenuEntry(Icon_Cancel, "$MsgRemoveAutomationMechanism$", 1, "remove_mechanism"));
	}
	else
	{
		if (mechanism)
			PushBack(menu_entries, GetAutomationControlMenuEntry(mechanism, "$MsgAddAutomationMechanism$", 1, "add_mechanism"));
	}
	
	// Add info message about automation modes.
	if (lib_cannon.automated)
	{
		PushBack(menu_entries, {symbol = this, extra_data = "desc_automation_modes",
			custom =
			{
				Right = "100%", Bottom = "1.2em",
				BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
				Priority = 2,
				BackgroundColor = RGB(100, 25, 25),
				text = {Left = "2em", Text = "$MsgSelectMode$"},
				image = {Right = "2em", Symbol = CannonMechanism}
			}
		});
	}
			
	// Add available automation modes.
	if (lib_cannon.automated)
	{
		var modes = GetAutomationModes();
		PushFront(modes, 
			{
				mode = "mode::off",
				symbol = Icon_Cancel,
				graph_name = nil
			}
		);
		for (var index = 0; index < GetLength(modes); index++)
		{
			var mode = modes[index];
			var status = Icon_Cancel;
			if (mode.mode == lib_cannon.automation_mode)
				status = Icon_Ok;
			PushBack(menu_entries, {symbol = mode.symbol, extra_data = mode.mode, 
				custom =
				{
					Right = "2em", Bottom = "2em",
					BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
					Priority = index + 3,
					status = {Right = "1em", Top = "1em", Symbol = status},
					image = {Symbol = mode.symbol}
				}
			});
		}
	}
	return menu_entries;
}

public func GetAutomationControlMenuEntry(symbol, string text, int priority, extra_data)
{
	return {symbol = symbol, extra_data = extra_data, 
		custom =
		{
			Right = "100%", Bottom = "2em",
			BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
			image = {Left = "2em", Text = text},
			text = {Right = "2em", Symbol = symbol},
			Priority = priority
		}};
}

public func OnAutomationControlHover(id symbol, string action, desc_menu_target, menu_id)
{
	var text = "";
	if (action == "add_mechanism")
		text = "$DescAddAutomation$";
	else if (action == "remove_mechanism")
		text = "$DescRemoveAutomation$";
	else if (action == "description")
	{
		if (lib_cannon.automated)
			text = "$DescAutomated$";
		else
			text = "$DescNotAutomated$";
	}
	else if (action == "desc_automation_modes")
		text = "$DescSelectMode$";
	else if (WildcardMatch(action, "mode::*"))
		text = GetAutomationModeDescription(action);
	GuiUpdateText(text, menu_id, 1, desc_menu_target);
	return;
}

public func OnAutomationControl(symbol_or_object, string action, bool alt)
{
	if (action == "add_mechanism")
		AddAutomationMechanism(symbol_or_object);
	else if (action == "remove_mechanism")
		RemoveAutomationMechanism();
	else if (WildcardMatch(action, "mode::*"))
		SetAutomationMode(action);
	
	UpdateInteractionMenus(this.GetAutomationMenuEntries);
	// Forward updates to cannon frame if mounted.
	if (lib_cannon.frame)
		lib_cannon.frame->UpdateInteractionMenus(this.GetAutomationMenuEntries);
	return;
}


/*-- Contents --*/

public func IsContainer() { return true; }

public func QueryRejectDeparture(object container)
{
	// Don't allow removal via contents menu if mounted.
	if (container == lib_cannon.frame)
		return true;
	return false;
}


/*-- Properties --*/

public func IsCannon() { return true; }

// All cannons must be pushable.
local Touchable = 1;
// As default cannons have 100 hit points.
local HitPoints = 100;
