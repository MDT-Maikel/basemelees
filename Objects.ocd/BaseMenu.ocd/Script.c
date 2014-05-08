/**
	Base Menu
	Menu which allows the player to place all the startup objects in base melee scenarios.
	
	@author Maikel
*/


// Local variables for the base object list and base rectangle.
local base_objects;
local base_rect;
local show_rect;

// Creates the base menu for the given clonk, objects and rectangle.
global func CreateBaseMenu(object clonk, array objects, proplist rect)
{
	var menu = CreateObject(BaseMenu, 0, 0, clonk->GetOwner());
	menu->SetBaseObjects(objects);
	menu->SetBaseRectangle(rect);
	menu->OpenBaseMenu(clonk);
	return menu;
}

protected func Initialize()
{
	base_objects = [];
	base_rect = Rectangle(0, 0, LandscapeWidth(), LandscapeHeight());
	show_rect = {};	
	return;
}

protected func Destruction()
{
	// Reset player view.
	SetPlrView(GetOwner(), GetCursor(GetOwner()));
	// Delete base rectangle.
	DeleteBaseRect();
	return;
}

public func SetBaseObjects(array objects)
{
	base_objects = objects;
	return;
}

public func RemoveFromBaseObjects(proplist obj)
{
	var index = GetIndexOf(base_objects, obj);
	if (index == -1)
		return;
	
	base_objects[index].amount--;
	if (base_objects[index].amount <= 0)
		RemoveArrayIndex(base_objects, index);
		
	return;
}

public func SetBaseRectangle(proplist rect)
{
	base_rect = rect;
	// Center player view to the rectangle.
	SetPosition(base_rect.x + base_rect.w / 2, base_rect.y + base_rect.h / 2 - 50);
	SetPlrView(GetOwner(), this);
	SetPlayerZoomByViewRange(GetOwner(), 11 * base_rect.w / 10, 11 * base_rect.h / 10, PLRZOOM_LimitMax);
	SetPlayerZoomByViewRange(GetOwner(), 11 * base_rect.w / 10, 11 * base_rect.h / 10, PLRZOOM_Direct);
	SetPlayerViewLock(GetOwner(), true);
	// Show base rectangle.
	ShowBaseRect();
	return;
}


/*-- Base Rectangle --*/

public func ShowBaseRect()
{
	var x1 = base_rect.x, y1 = base_rect.y; 
	var x2 = base_rect.x + base_rect.w, y2 = base_rect.y; 
	var x3 = base_rect.x + base_rect.w, y3 = base_rect.y + base_rect.h; 
	var x4 = base_rect.x, y4 = base_rect.y + base_rect.h; 	
	var clr = 0xffff0000;
	show_rect.top = DebugLine->Create(x1, y1, x2, y2, clr);
	show_rect.right = DebugLine->Create(x2, y2, x3, y3, clr);
	show_rect.bottom = DebugLine->Create(x3, y3, x4, y4, clr);
	show_rect.left = DebugLine->Create(x4, y4, x1, y1, clr);
}

public func DeleteBaseRect()
{
	show_rect.top->RemoveObject();
	show_rect.right->RemoveObject();
	show_rect.bottom->RemoveObject();
	show_rect.left->RemoveObject();
}


/*-- Construction Menu --*/

// Background colors for hovering and bars and description.
static const BASEMENU_BackgroundColor = 0x77000000;
static const BASEMENU_HoverColor = 0x99ffffff;
static const BASEMENU_BarColor = 0x99888888;

local menu, menu_id, menu_controller;

public func OpenBaseMenu(object clonk)
{
	// If the menu is already open, don't open another instance.
	if (clonk->GetMenu() && clonk->GetMenu().ID == menu_id)
		return;
		
	// Only open the menu if there still are base_objects, otherwise remove this object.
	var total_amount = 0;
	for (var obj in base_objects)
		total_amount += obj.amount;
	if (total_amount <= 0)
		return RemoveObject();
		
	// This object functions as menu target and for visibility.
	this.Visibility = VIS_Owner;
	menu_controller = clonk;
	
	var menu_width = 25; 
	var menu_height = 40;
	
	// Construction menu proplist.
	menu =
	{
		Target = this,
		Style = GUI_Multiple,
		Decoration = GUI_MenuDeco,
		Left = Format("%d%", 50 - menu_width),
		Right = Format("%d%", 50 + menu_width),
		Top = Format("%d%", 50 - menu_height),
		Bottom = Format("%d%", 50 + menu_height),
		BackgroundColor = {Std = BASEMENU_BackgroundColor},
	};
	
	// Description top ten percent.
	menu.menu_desc = 
	{
		Target = this,
		ID = 1,
		Left = "0%",
		Right = "100%",
		Top = "0%",
		Bottom = "10%",
		Text = "$MsgPlaceObjects$",
		BackgroundColor = {Std = BASEMENU_BackgroundColor},	
	};
	
	// Bar between description and objects.
	menu.bar1 = 
	{
		Target = this,
		ID = 2,
		Left = "0%",
		Right = "100%",
		Top = "10%",
		Bottom = "12%",
		BackgroundColor = {Std = BASEMENU_BarColor},
	};
	// Ojects middle eighty percent.
	var objects = 
	{
		Target = this,
		ID = 3,
		Left = "0%",
		Right = "100%",
		Top = "12%",
		Bottom = "88%",
		Style = GUI_GridLayout,
		BackgroundColor = {Std = BASEMENU_BackgroundColor},
	};
	objects = MenuAddObjects(objects, clonk);
	menu.objects = objects;
	// Bar between objects and object info.
	menu.bar2 = 
	{
		Target = this,
		ID = 4,
		Left = "0%",
		Right = "100%",
		Top = "86%",
		Bottom = "88%",
		BackgroundColor = {Std = BASEMENU_BarColor},
	};
	// Object info bottom ten percent.
	menu.objectinfo = 
	{
		Target = this,
		ID = 5,
		Left = "0%",
		Right = "100%",
		Top = "88%",
		Bottom = "96%",
		Text = nil,
		BackgroundColor = {Std = BASEMENU_BackgroundColor},
	};
	menu.objectspawn = 
	{
		Target = this,
		ID = 6,
		Left = "0%",
		Right = "100%",
		Top = "96%",
		Bottom = "100%",
		Text = nil,
		BackgroundColor = {Std = BASEMENU_BackgroundColor},
	};
	// Menu ID.
	menu_id = CustomGuiOpen(menu);
	// Notify the clonk and set the menu to be uncloseable.
	clonk->SetMenu(this, true);
	return;
}

public func MenuAddObjects(proplist struct, object clonk)
{
	// Show all the objects which the team leader is allowed to place.
	var cnt = 0;
	for (var obj in base_objects)
	{
		if (obj.amount <= 0)
			continue;
							
		var str =
		{
			Target = this,
			ID = cnt + 1000,
			Right = "12em",
			Bottom = "12em",
			BackgroundColor = {Std = 0, Hover = BASEMENU_HoverColor},
			OnMouseIn = [GuiAction_SetTag("Hover"), GuiAction_Call(this, "OnObjectHover", obj)],
			OnMouseOut = GuiAction_SetTag("Std"), 
			OnClick = GuiAction_Call(this, "OnObjectSelection", {base_obj = obj, constructor = clonk}),
			picture = 
			{
				Target = this,
				ID = cnt + 2000,
				Left = "8%",
				Right = "92%",
				Top = "8%",
				Bottom = "92%",
				Symbol = obj.def,
			},
			amount = 
			{
				Target = this,
				ID = cnt + 3000,
				Left = "0%",
				Right = "100%",
				Top = "0%",
				Bottom = "20%",
				Text = Format("%dx", obj.amount),
			},
		};
		struct[Format("struct%d", cnt + 4)] = str;		
		cnt++;
	}
	return struct;
}

public func OnObjectSelection(proplist obj)
{
	// Show the previewer to place the object.
	ShowObjectPreview(obj.constructor, obj.base_obj);
	CloseConstructionMenu();
	return;
}

public func OnObjectHover(proplist obj)
{
	// Update the description of the object.
	menu.objectinfo.Text = obj.def.Description;
	CustomGuiUpdate(menu.objectinfo, menu_id, menu.objectinfo.ID, this);
	// Update the description of the object.
	var content_str = "";
	if (obj.contents)
	{
		for (c in obj.contents)
			content_str = Format("%s %dx{{%i}}", content_str, c[1], c[0]);	
		content_str	= Format("$MsgSpawnContents$ %s", 0xffff0000, content_str);
	}
	menu.objectspawn.Text = content_str;
	CustomGuiUpdate(menu.objectspawn, menu_id, menu.objectspawn.ID, this);
	return;
}

public func CloseConstructionMenu()
{
	// Close the menu and inform the controller.
	CustomGuiClose(menu_id, nil, this);
	menu_id = nil;
	if (menu_controller)
		menu_controller->MenuClosed();
	menu_controller = nil;
	return;
}

public func Close() { return RemoveObject(); }


/*-- Base Object Previewer --*/

public func ShowObjectPreview(object clonk, proplist obj)
{
	AddEffect("ControlBasePreview", clonk, 1, 0, this, nil, obj, clonk);
	SetPlayerControlEnabled(clonk->GetOwner(), CON_Aim, true);
	return true;
}

public func FxControlBasePreviewStart(object clonk, effect, int temp, proplist obj, object clonk)
{
	if (temp) 
		return;

	effect.obj = obj;
	effect.flipable = !effect.obj.def->~NoConstructionFlip();
	if (!effect.preview) 
		effect.preview = CreateObject(BasePreviewer, AbsX(clonk->GetX()), AbsY(clonk->GetY()), clonk->GetOwner());
	effect.preview->Set(clonk, effect.obj, base_rect);
}

// Called by Control2Effect
public func FxControlBasePreviewControl(object clonk, effect, int ctrl, int x, int y, int strength, bool repeat, bool release)
{
	if (ctrl != CON_Aim)
	{
		// CON_Use is accept
		if (ctrl == CON_Use)
			if (effect.preview->CreateBaseObject())
			{
				// substract from original list.
				RemoveFromBaseObjects(effect.obj);
			}
		// movement is not allowed
		else if (IsMovementControl(ctrl))
			return true;
		// Flipping
		// this is actually realized twice. Once as an Extra-Interaction in the clonk, and here. We don't want the Clonk to get any non-movement controls though, so we handle it here too.
		// (yes, this means that actionbar-hotkeys wont work for it. However clicking the button will.)
		else if (IsInteractionControl(ctrl))
		{
			if (release)
				effect.preview->Flip();
			return true;
		}

		// everything else declines
		RemoveEffect("ControlBasePreview", clonk, effect);
		return true;
	}
	// Reposition the previewer on mouse movement.	
	effect.preview->Reposition(x, y);
	return true;
}

public func FxControlBasePreviewStop(object clonk, effect, int reason, bool temp)
{
	if (temp)
		return;
	// Remove the previewer.
	effect.preview->RemoveObject();
	SetPlayerControlEnabled(clonk->GetOwner(), CON_Aim, false);
	// Always try to open the base menu.
	OpenBaseMenu(clonk);
	return;
}

// UI not saved.
public func SaveScenarioObject() { return false; }


/*-- Proplist --*/

local Name = "$Name$";
