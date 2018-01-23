/**
	Guard Tower
	Protects the borders of settlements, has two doors and an upper deck which has a cannon mount.

	@author Maikel
*/

#include Library_Structure
#include Library_Ownable
#include Library_LampPost

// Store the guard tower's doors.
local door_left, door_right;
// Store the guard tower's deck.
local deck;
// Store the guard tower's cannon mount.
local cannon_mount;


public func Construction(object creator)
{
	return _inherited(creator, ...);
}

public func Initialize()
{
	// Open entrance to deck.
	SetEntrance(true);
	deck = CreateObject(GuardTowerDeck, 0, -20, GetOwner());
	deck->SetTower(this);
	// Create doors.
	door_left = CreateObjectAbove(GuardTowerDoor, -12, GetBottom() - 24, GetOwner());
	door_right = CreateObjectAbove(GuardTowerDoor, 12, GetBottom() - 24, GetOwner());
	OpenDoorLeft();
	OpenDoorRight();
	// Create cannon mount on tower deck.
	cannon_mount = CreateObject(GuardTowerCannonMount, 0, -20, GetOwner());
	cannon_mount->SetTower(this);
	return _inherited(...);
}

public func LampPosition(id def) { return [GetCalcDir() * 28, 4]; }

public func IsHammerBuildable() { return true; }

public func Destruction()
{
	if (door_left)
		door_left->RemoveObject();
	if (door_right)
		door_right->RemoveObject();
	return _inherited(...);
}


/*-- Interaction Menu --*/

public func HasInteractionMenu() { return true; }

public func GetInteractionMenus(object clonk)
{
	var menus = _inherited(clonk, ...) ?? [];
	
	if (door_left || door_right)
	{
		var door_menu =
		{
			title = "$MenuDoorControl$",
			entries_callback = this.GetDoorControlMenuEntries,
			callback = "OnDoorControl",
			callback_hover = "OnDoorControlHover",
			callback_target = this,
			BackgroundColor = RGB(0, 50, 50),
			Priority = 30
		};
		PushBack(menus, door_menu);
	}
	return menus;
}

public func GetDoorControlMenuEntries(object clonk)
{
	var menu_entries = [];
	if (door_left && !door_left->IsOpen())
		PushBack(menu_entries, GetDoorMenuEntry(door_left, "$MsgOpenDoorLeft$", 1, "openleft"));
	if (door_right && !door_right->IsOpen())
		PushBack(menu_entries, GetDoorMenuEntry(door_right, "$MsgOpenDoorRight$", 1, "openright"));

	if (door_left && !door_left->IsClosed())
		PushBack(menu_entries, GetDoorMenuEntry(door_left, "$MsgCloseDoorLeft$", 1, "closeleft"));
	if (door_right && !door_right->IsClosed())
		PushBack(menu_entries, GetDoorMenuEntry(door_right, "$MsgCloseDoorRight$", 1, "closeright"));		
	return menu_entries;
}

public func GetDoorMenuEntry(symbol, string text, int priority, extra_data)
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
		}};
}

public func OnDoorControlHover(id symbol, string action, desc_menu_target, menu_id)
{
	var text = "";
	if (action == "openleft")
		text = "$DescOpenDoorLeft$";
	else if (action == "openright")
		text = "$DescOpenDoorRight$";
	else if (action == "closeleft")
		text = "$DescCloseDoorLeft$";
	else if (action == "closeright")
		text = "$DescCloseDoorRight$";
	GuiUpdateText(text, menu_id, 1, desc_menu_target);
	return;
}

public func OnDoorControl(symbol_or_object, string action, bool alt)
{
	if (action == "openleft")
		this->OpenDoorLeft();
	else if (action == "openright")
		this->OpenDoorRight();
	else if (action == "closeleft")
		this->CloseDoorLeft();
	else if (action == "closeright")
		this->CloseDoorRight();		
	UpdateInteractionMenus(this.GetDoorControlMenuEntries);	
	return;
}


/*-- Door Control --*/

public func GetLeftDoor() { return door_left; }
public func GetRightDoor() { return door_right; }

public func OpenDoorLeft()
{
	if (door_left)
		door_left->OpenDoor();
	return;
}

public func OpenDoorRight()
{
	if (door_right)
		door_right->OpenDoor();
	return;
}

public func CloseDoorLeft()
{
	if (door_left)
		door_left->CloseDoor();
	return;
}

public func CloseDoorRight()
{
	if (door_right)
		door_right->CloseDoor();
	return;
}


/*-- Entrance Control --*/

public func Collection2(object obj)
{
	if (deck && !obj.force_tower_exit)
	{
		obj.force_tower_exit = true;
		obj->Enter(deck);
	}
	else
	{
		obj.force_tower_exit = false;
		obj->SetCommand("Exit");
	}
	return _inherited(obj, ...);
}

// Allows vehicle entrance while not being a container.
public func AllowsVehicleEntrance() { return true; }


/*-- Cannon Control --*/

public func GetCannonMount() { return cannon_mount; }
 

/*-- Properties --*/

public func Definition(proplist def)
{
	return _inherited(def, ...);
}

local Name = "$Name$";
local Description ="$Description$";
local ContainBlast = true;
local HitPoints = 240;
local Components = {Wood = 1, Metal = 1, Rock = 6};
