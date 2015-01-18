/**
	Medal Menu
	Menu which shows the player its medals or the medals of the selected player in this round.
	
	@author Maikel
*/


// Creates the medal menu for the given player.
public func CreateMedalMenu(int plr)
{
	// Safety: is a definition call.
	if (this != MedalMenu)
		return;	
	var menu = CreateObject(MedalMenu, 0, 0, plr);
	menu->OpenMedalMenu(plr);
	return menu;
}


/*-- Medal Menu --*/

// Background colors for hovering and bars and description.
static const MEDALMENU_BackgroundColor = 0x77000000;
static const MEDALMENU_HoverColor = 0x99ffffff;
static const MEDALMENU_BarColor = 0x99888888;

local menu, menu_id, menu_controller;

public func OpenMedalMenu(int plr)
{
	// Needs the cursor as command object.
	var clonk = GetCursor(plr);
	
	// If the menu is already open, don't open another instance.
	if (clonk->GetMenu() && clonk->GetMenu().ID == menu_id)
		return;
		
	// This object functions as menu target and for visibility.
	this.Visibility = VIS_Owner;
	menu_controller = clonk;
	
	var menu_width = 25; 
	
	// Medal menu proplist.
	menu =
	{
		Target = this,
		Style = GUI_Multiple,
		Decoration = GUI_MenuDeco,
		Left = Format("%d%", 50 - menu_width),
		Right = Format("%d%", 50 + menu_width),
		Top = "10em",
		Bottom = "50em",
		Style = GUI_FitChildren,
		BackgroundColor = {Std = MEDALMENU_BackgroundColor},
	};
	// A header showing the contents of this menu and close button.
	var header = 
	{
		Target = this,
		ID = 1,
		Right = "100%",
		Bottom = "3em",
		header_text = 
		{
			Target = this,
			ID = 2,
			Right = "100%-3em",
			Bottom = "3em",
			Text = "$MedalMenuCaption$",
		},
		header_close = 
		{
			Target = this,
			ID = 3,
			Left = "100%-3em",
			Right = "100%",
			Bottom = "3em",
			Symbol = Icon_Cancel,
			BackgroundColor = {Std = 0, Hover = MEDALMENU_HoverColor},
			OnMouseIn = GuiAction_SetTag("Hover"),
			OnMouseOut = GuiAction_SetTag("Std"),
			OnClick = GuiAction_Call(this, "CloseMedalMenu")
		},
	};
	menu.header = header;
	// Player choice takes top ten percent.
	var players = 
	{
		Target = this,
		ID = 4,
		Left = "0%",
		Right = "100%",
		Top = "3em",
		Bottom = "11em",
		Style = GUI_GridLayout | GUI_FitChildren,
		BackgroundColor = {Std = MEDALMENU_BackgroundColor},	
	};
	players = MenuShowPlayers(players, plr);
	menu.players = players;
	// Bar between player choice and medals.
	menu.bar1 = 
	{
		Target = this,
		ID = 5,
		Left = "0%",
		Right = "100%",
		Top = "11em",
		Bottom = "12em",
		BackgroundColor = {Std = MEDALMENU_BarColor},
	};
	// Medals take up middle eighty percent.
	var medals = 
	{
		Target = this,
		ID = 6,
		Left = "0%",
		Right = "100%",
		Top = "12em",
		Bottom = "36em",
		Style = GUI_GridLayout | GUI_FitChildren,
		BackgroundColor = {Std = MEDALMENU_BackgroundColor},
	};
	menu.medals = MenuShowMedals(medals, plr);
	// Bar between medals and medal info.
	menu.bar2 = 
	{
		Target = this,
		ID = 7,
		Left = "0%",
		Right = "100%",
		Top = "36em",
		Bottom = "37em",
		BackgroundColor = {Std = MEDALMENU_BarColor},
	};
	// Medal info bottom ten percent.
	menu.medalinfo = 
	{
		Target = this,
		ID = 8,
		Left = "0%",
		Right = "100%",
		Top = "37em",
		Bottom = "40em",
		Text = nil,
		BackgroundColor = {Std = MEDALMENU_BackgroundColor},
	};
	// Menu ID.
	menu_id = GuiOpen(menu);
	// Notify the clonk and set the menu to be uncloseable.
	clonk->SetMenu(this);
	return;
}

public func MenuShowPlayers(proplist parent, int for_plr)
{
	var cnt = 0;
	for (var index = 0; index < GetPlayerCount(C4PT_User); index++)
	{
		var plr = GetPlayerByIndex(index);
		var plr_menu = 
		{
			Target = this,
			ID = cnt + 100,
			Right = "8em",
			Bottom = "8em",
			// Margin = ["0.5em", "0.5em", "0.5em", "0.5em"],
			BackgroundColor = {Std = 0, Hover = MEDALMENU_HoverColor},
			OnMouseIn = GuiAction_SetTag("Hover"),
			OnMouseOut = GuiAction_SetTag("Std"), 		
			OnClick = [GuiAction_SetTag("Hover"), GuiAction_Call(this, "OnPlayerClick", plr)],
			Text = GetTaggedPlayerName(plr),
			Style = GUI_TextHCenter,
			Symbol = GetCursor(plr) ?? Clonk,
		};
		parent[Format("players%d", cnt + 4)] = plr_menu;		
		cnt++;
	}	
	return parent;
}

public func MenuShowMedals(proplist parent, int plr)
{
	// Show all the medals this player has.
	var medals = Rule_Medals->GetMedals(plr);
	var cnt = 0;
	for (var medal in medals)
	{
		var medal_id = medal[0];
		var medal_count = medal[1];
		if (medal_count <= 0)
			continue;
		var medal =
		{
			Target = this,
			ID = cnt + 1000,
			Priority = 1000 - cnt,
			Right = "8em",
			Bottom = "8em",
			// Margin = ["0.5em", "0.5em", "0.5em", "0.5em"],
			BackgroundColor = {Std = 0, Hover = MEDALMENU_HoverColor},
			OnMouseIn = [GuiAction_SetTag("Hover"), GuiAction_Call(this, "OnMedalHover", medal_id)],
			OnMouseOut = GuiAction_SetTag("Std"), 
			Symbol = medal_id,
			Text = Format("%dx", medal_count),
		};
		parent[Format("medal%d", cnt + 4)] = medal;		
		cnt++;
	}
	return parent;
}

public func OnPlayerClick(int plr)
{
	// Clear all possible previous medal entries.
	var index = 0;
	while (menu.medals[Format("medal%d", index + 4)])
	{
		var medal = menu.medals[Format("medal%d", index + 4)];
		GuiClose(menu_id, medal.ID, medal.Target);
		menu.medals[Format("medal%d", index + 4)] = nil;
		index++;
	}
	// Then reinit the medals for the new player and update the menu.
	menu.medals = MenuShowMedals(menu.medals, plr);
	GuiUpdate(menu.medals, menu_id, menu.medals.ID, this);
	return;
}

public func OnMedalHover(id medal_id)
{
	// Update the description of the object.
	menu.medalinfo.Text = Format("<c %x>%s (%d</c> {{GUI_Wealth}}<c %x>):</c> %s", 0xffff0000, medal_id.Name, medal_id->GetMedalReward(), 0xffff0000, medal_id.Description);
	GuiUpdate(menu.medalinfo, menu_id, menu.medalinfo.ID, this);
	return;
}

public func CloseMedalMenu()
{
	// Close the menu and inform the controller.
	GuiClose(menu_id, nil, this);
	menu_id = nil;
	if (menu_controller)
		menu_controller->MenuClosed();
	menu_controller = nil;
	return;
}

public func Close() { return RemoveObject(); }


// UI not saved.
public func SaveScenarioObject() { return false; }


/*-- Proplist --*/

local Name = "$Name$";
