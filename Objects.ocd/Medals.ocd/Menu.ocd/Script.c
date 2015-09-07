/**
	Medal Menu
	Menu which shows the player its medals or the medals of the selected player in this round.
	
	@author Maikel
*/

// Background colors for hovering and bars and description.
static const MEDALMENU_BackgroundColor = 0x77000000;
static const MEDALMENU_HoverColor = 0x99888888;
static const MEDALMENU_BarColor = 0x99888888;

local menu, menu_id, menu_controller;
local view_round_only;
local medals_for_plr;


protected func Initialize()
{
	// By default all medals of a player will be shown.
	view_round_only = false;
	return;
}

// Creates the medal menu for the given player.
public func CreateMedalMenu(int plr)
{
	// Safety: is a definition call.
	if (this != MedalMenu)
		return;	
	var menu_obj = CreateObject(MedalMenu, 0, 0, plr);
	menu_obj->OpenMedalMenu(plr);
	return menu_obj;
}


/*-- Medal Menu --*/

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
	medals_for_plr = plr;
	
	// Medal menu proplist.
	menu =
	{
		Target = this,
		Decoration = GUI_MenuDeco,
		Left = "50% - 12em",
		Right = "50% + 12.1em", // 0.1 needed to fit six entries per row.
		Top = "8%",
		Bottom = "8%+27em",
		BackgroundColor = {Std = MEDALMENU_BackgroundColor},
	};
	// A header showing the contents of this menu and close button.
	menu.header = 
	{
		Target = this,
		ID = 1,
		Right = "100%",
		Bottom = "1.5em",
		header_text = 
		{
			Target = this,
			ID = 2,
			Right = "100%-3em",
			Bottom = "1.5em",
			Text = "$MedalMenuCaption$",
		},
		header_view = 
		{
			Target = this,
			ID = 3,
			Left = "100%-3em",
			Right = "100%-1.5em",
			Bottom = "1.5em",
			Symbol = Medal_Template,
			Tooltip = "$MedalMenuSwitch$",
			BackgroundColor = {Std = 0, Hover = MEDALMENU_HoverColor},
			OnMouseIn = GuiAction_SetTag("Hover"),
			OnMouseOut = GuiAction_SetTag("Std"),
			OnClick = GuiAction_Call(this, "SwitchMedalView"),
			header_view_tag = {
				Target = this,
				ID = 4,
				Left = "0.7em",
				Top = "0.7em",
				Symbol = Icon_Number,
				GraphicsName = "Sum",
			}
		},
		header_close = 
		{
			Target = this,
			ID = 5,
			Left = "100%-1.5em",
			Right = "100%",
			Bottom = "1.5em",
			Symbol = Icon_Cancel,
			Tooltip = "$MedalMenuClose$",
			BackgroundColor = {Std = 0, Hover = MEDALMENU_HoverColor},
			OnMouseIn = GuiAction_SetTag("Hover"),
			OnMouseOut = GuiAction_SetTag("Std"),
			OnClick = GuiAction_Call(this, "CloseMedalMenu")
		},
	};
	if (view_round_only)
		menu.header.headerview.header_view_tag.Symbol = Icon_Cancel;
	// Bar between header and player choice.
	menu.bar1 = 
	{
		Target = this,
		ID = 6,
		Left = "0%",
		Right = "100%",
		Top = "1.5em",
		Bottom = "2em",
		BackgroundColor = {Std = MEDALMENU_BarColor},
	};
	// Player choice takes top ten percent.
	var playerchoice = 
	{
		Target = this,
		ID = 7,
		Left = "0%",
		Right = "100%",
		Top = "2em",
		Bottom = "6em",
		Style = GUI_GridLayout,
		BackgroundColor = {Std = MEDALMENU_BackgroundColor},	
	};
	playerchoice = MenuShowPlayers(playerchoice, plr);
	menu.playerchoice = playerchoice;
	// Bar between player choice and medals.
	menu.bar2 = 
	{
		Target = this,
		ID = 8,
		Left = "0%",
		Right = "100%",
		Top = "6em",
		Bottom = "6.5em",
		BackgroundColor = {Std = MEDALMENU_BarColor},
	};
	// Medals take up middle eighty percent.
	var medals = 
	{
		Target = this,
		ID = 9,
		Left = "0%",
		Right = "100%",
		Top = "6.5em",
		Bottom = "18.5em",
		Style = GUI_GridLayout,
		BackgroundColor = {Std = MEDALMENU_BackgroundColor},
	};
	menu.medals = MenuShowPlayerMedals(medals, plr);
	// Bar between medals and medal info.
	menu.bar3 = 
	{
		Target = this,
		ID = 10,
		Left = "0%",
		Right = "100%",
		Top = "18.5em",
		Bottom = "19em",
		BackgroundColor = {Std = MEDALMENU_BarColor},
	};
	// Medal info bottom ten percent.
	menu.medalinfo = 
	{
		Target = this,
		ID = 11,
		Left = "0%",
		Right = "100%",
		Top = "19em",
		Bottom = "22em",
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
	var all_medals =
	{
		Target = this,
		ID = 100,
		Right = "4em",
		Bottom = "4em",
		Priority = 0,
		Margin = ["0.25em"],
		BackgroundColor = {Std = 0, Hover = MEDALMENU_HoverColor},
		OnMouseIn = GuiAction_SetTag("Hover"),
		OnMouseOut = GuiAction_SetTag("Std"), 		
		OnClick = [GuiAction_SetTag("Hover"), GuiAction_Call(this, "OnPlayerClick", nil)],
		Text = "All Medals",
		Style = GUI_TextHCenter,
		Symbol = Medal_Template,
	};
	parent.players0 = all_medals;
	// For each regular player add an entry which can be selected to show its medals.
	var cnt = 1;
	for (var index = 0; index < GetPlayerCount(C4PT_User); index++)
	{
		var plr = GetPlayerByIndex(index);
		var plr_menu = 
		{
			Target = this,
			ID = cnt + 100,
			Right = "4em",
			Bottom = "4em",
			Priority = cnt,
			Margin = ["0.25em"],
			BackgroundColor = {Std = 0, Hover = MEDALMENU_HoverColor},
			OnMouseIn = GuiAction_SetTag("Hover"),
			OnMouseOut = GuiAction_SetTag("Std"), 		
			OnClick = [GuiAction_SetTag("Hover"), GuiAction_Call(this, "OnPlayerClick", plr)],
			Text = GetTaggedPlayerName(plr),
			Style = GUI_TextHCenter,
			Symbol = GetCursor(plr) ?? Clonk,
		};
		parent[Format("players%d", cnt)] = plr_menu;		
		cnt++;
	}	
	return parent;
}

public func MenuShowAllMedals(proplist parent)
{
	// Show all the active medals in this round.
	var medals = Rule_Medals->GetActiveMedals();
	var cnt = 0;
	for (var medal_id in medals)
	{
		var medal =
		{
			Target = this,
			ID = cnt + 1000,
			Priority = medal_id->GetMedalIndex(),
			Right = "3em",
			Bottom = "3em",
			Margin = ["0.15em"],
			BackgroundColor = {Std = 0, Hover = MEDALMENU_HoverColor},
			OnMouseIn = [GuiAction_SetTag("Hover"), GuiAction_Call(this, "OnMedalHoverIn", medal_id)],
			OnMouseOut = [GuiAction_SetTag("Std"), GuiAction_Call(this, "OnMedalHoverOut", medal_id)], 
			Symbol = medal_id,
			Text = Format("%d{{Icon_Wealth}}", medal_id->GetMedalReward()),
		};
		parent[Format("medal%d", cnt)] = medal;		
		cnt++;
	}
	return parent;
}

public func MenuShowPlayerMedals(proplist parent, int plr)
{
	// Show all the medals this player has.
	var medals = Rule_Medals->GetMedals(plr, view_round_only);
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
			Priority = medal_id->GetMedalIndex(),
			Right = "3em",
			Bottom = "3em",
			Margin = ["0.15em"],
			BackgroundColor = {Std = 0, Hover = MEDALMENU_HoverColor},
			OnMouseIn = [GuiAction_SetTag("Hover"), GuiAction_Call(this, "OnMedalHoverIn", medal_id)],
			OnMouseOut = [GuiAction_SetTag("Std"), GuiAction_Call(this, "OnMedalHoverOut", medal_id)], 
			Symbol = medal_id,
			Text = Format("%dx", medal_count),
		};
		parent[Format("medal%d", cnt)] = medal;		
		cnt++;
	}
	return parent;
}

public func SwitchMedalView()
{
	// Switch the header view tag.
	view_round_only = !view_round_only;
	if (view_round_only)
	{
		menu.header.header_view.header_view_tag.Symbol = Icon_World;
		menu.header.header_view.header_view_tag.GraphicsName = nil;
	}
	else
	{
		menu.header.header_view.header_view_tag.Symbol = Icon_Number;
		menu.header.header_view.header_view_tag.GraphicsName = "Sum";
	}
	GuiClose(menu_id, menu.header.header_view.header_view_tag.ID, menu.header.header_view.header_view_tag.Target);
	GuiUpdate(menu.header.header_view, menu_id, menu.header.header_view.ID, this);
	// Reinitialize the medals for the current player.
	OnPlayerClick(medals_for_plr);
	return;
}

public func OnPlayerClick(int plr)
{
	medals_for_plr = plr;
	// Clear all possible previous medal entries.
	var index = 0;
	while (menu.medals[Format("medal%d", index)])
	{
		var medal = menu.medals[Format("medal%d", index)];
		GuiClose(menu_id, medal.ID, medal.Target);
		menu.medals[Format("medal%d", index)] = nil;
		index++;
	}
	// Then re-init the medals for the new player and update the menu.
	if (plr != nil)
		menu.medals = MenuShowPlayerMedals(menu.medals, plr);
	else
		menu.medals = MenuShowAllMedals(menu.medals);
	GuiUpdate(menu.medals, menu_id, menu.medals.ID, this);
	return;
}

public func OnMedalHoverIn(id medal_id)
{
	// Update the description of the medal.
	menu.medalinfo.Text = Format("<c %x>%s (%d</c>{{Icon_Wealth}}<c %x>):</c> %s", 0xffff0000, medal_id.Name, medal_id->GetMedalReward(), 0xffff0000, medal_id.Description);
	GuiUpdate(menu.medalinfo, menu_id, menu.medalinfo.ID, this);
	return;
}

public func OnMedalHoverOut(id medal_id)
{
	// Remove the description of the medal.
	menu.medalinfo.Text = nil;
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
