/**
	Base Melee Scoreboard
	A special scoreboard specifically made for base melees.

	@author Maikel
*/


// Called then the player hits a control with Scoreboard_BaseMelee as spec_id, in this case CON_Scoreboard.
public func ForwardedPlayerControl(int plr, int ctrl, int x, int y, int strength, bool repeat, bool release)
{
	if (release)
		return;
	if (ctrl == CON_Scoreboard)
	{
		if (Scoreboard_BaseMelee->HasScoreboardOpen(plr))
			Scoreboard_BaseMelee->CloseScoreboard(plr);
		else
			Scoreboard_BaseMelee->OpenScoreboard(plr);
	}
	return;
}


/*-- Interface --*/

// Definition call: opens scoreboard for the player.
public func OpenScoreboard(int plr)
{
	var controller = CreateObject(Scoreboard_BaseMelee, nil, nil, plr);
	controller->CreateScoreboardMenu();
	return;
}

// Definition call: closes scoreboard for the player.
public func CloseScoreboard(int plr)
{
	var controller = FindObject(Find_ID(Scoreboard_BaseMelee), Find_Owner(plr));
	if (!controller)
		return;
	controller->CloseScoreboardMenu();
	return;
}

// Definition call: tells whether the player has a scoreboard open.
public func HasScoreboardOpen(int plr)
{
	var controller = FindObject(Find_ID(Scoreboard_BaseMelee), Find_Owner(plr));
	if (!controller)
		return false;
	return controller->~IsScoreboardMenuOpen();
}


/*-- Scoreboard Menu --*/

// Background colors for hovering and bars and description.
static const SRBDMENU_BackgroundColor = 0x77000000;
static const SRBDMENU_HoverColor = 0x99ffffff;
static const SRBDMENU_TeamColor = 0x99888888;

local menu, menu_id;

public func CreateScoreboardMenu()
{
	// If the menu is already open, don't open another instance.
	if (IsScoreboardMenuOpen())
		return;
	// This object functions as menu target and for visibility.
	this.Visibility = VIS_Owner;
	
	// Scoreboard contents.
	var entries = [
		{ icon = Flagpole, team_callback = "GetTeamFlags", plr_callback = "GetPlayerFlags" },
		{ icon = Medal_Template, team_callback = "GetTeamMedals", plr_callback = "GetPlayerMedals", plr_clickcall = "OnMedalClick" },
		{ icon = Scoreboard_Death, team_callback = "GetTeamDeaths", plr_callback = "GetPlayerDeaths"},
		{ icon = Scoreboard_Kill, team_callback = "GetTeamKills", plr_callback = "GetPlayerKills"}
	];
	var name_width = 10;
	var entry_width = 2;
	var margin = 10;
	
	// Scoreboard menu proplist.
	menu =
	{
		Target = this,
		Style = GUI_Multiple,
		Priority = 1,
		Decoration = GUI_MenuDeco,
		Left = Format("100%%%s", ToEmString(-10 * (name_width + entry_width * GetLength(entries)) - margin)),
		Right = Format("100%%%s", ToEmString(-margin)),
		Top = ToEmString(margin),
		Bottom = ToEmString(margin + 30),
	};
	menu.header = GetScoreboardHeader(entries, name_width, entry_width);
	menu = GetScoreboardRows(menu, entries, name_width, entry_width, margin);
		
	// Menu ID.
	menu_id = GuiOpen(menu);
	return;
}

public func CloseScoreboardMenu()
{
	// Close the menu and inform the controller.
	GuiClose(menu_id, nil, this);
	menu_id = nil;
	return;
}

public func Close() { return RemoveObject(); }

public func IsScoreboardMenuOpen() { return menu_id != nil; }


/*-- Scoreboard Contents --*/

private func GetScoreboardHeader(array entries, int name_width, int entry_width)
{
	var header = 
	{
		Target = this,
		ID = 1,
		Top = "0em",
		Bottom = "2em",
		BackgroundColor = {Std = SRBDMENU_BackgroundColor},	
	};
	header.name = 
	{
		Target = this,
		ID = 2,
		Left = "0%",
		Right = Format("%dem", name_width),
		Style = GUI_TextVCenter,
		Text = "$MsgTeamsPlayers$",
	};
	for (var index = 0; index < GetLength(entries); index++)
	{
		var entry = 
		{
			Target = this,
			ID = 3 + index,
			Left = Format("%dem", name_width + index * entry_width),
			Right = Format("%dem", name_width + (index + 1) * entry_width),
			Symbol = entries[index].icon,
		};
		header[Format("entry%d", index)] = entry;	
	}
	return header;
}

private func GetScoreboardRows(proplist scoreboard, array entries, int name_width, int entry_width, int margin)
{
	var total_length = 2;
	for (var team_index = 0; team_index < GetTeamCount(); team_index++)
	{
		var team = GetTeamByIndex(team_index);
		var nr_plr = GetPlayerInTeamCount(team);
		var team_plrs = GetPlayers(nil, team);
		var team_menu = 
		{
			Target = this,
			ID = 1000 * (team_index + 1),
			Left = "0%",
			Right = "100%",
			Top = Format("%dem", total_length),
			Bottom = Format("%dem", total_length + 2 + 2 * nr_plr),
		};
		team_menu.teamrow = MakeTeamRow(team, team_index, entries, name_width, entry_width);
		for (var plr_index = 0; plr_index < GetLength(team_plrs); plr_index++)
		{
			var plr = team_plrs[plr_index];
			var plr_menu = MakePlayerRow(plr, team_index, plr_index, entries, name_width, entry_width);
			team_menu[Format("plrrow%d", plr_menu.ID)] = plr_menu;
		}
		scoreboard[Format("team%d", team_menu.ID)] = team_menu;
		total_length += 2 + 2 * nr_plr;
	}
	scoreboard.Bottom = ToEmString(margin + 10 * total_length);
	return scoreboard;
}

private func MakeTeamRow(int team, int team_index, array entries, int name_width, int entry_width)
{
	var row = {
		Target = this,
		ID = 1000 * (team_index + 1) + 100,
		Left = "0%",
		Right = "100%",
		Top = "0em",
		Bottom = "2em",
		BackgroundColor = {Std = SRBDMENU_TeamColor},
	};
	row.name = 
	{
		Target = this,
		ID = 1000 * (team_index + 1) + 100 + 1,
		Left = "0em",
		Right = Format("%dem", name_width),
		Style = GUI_TextVCenter,
		Text = GetTaggedTeamName(team),
	};	
	for (var index = 0; index < GetLength(entries); index++)
	{
		var entry = 
		{
			Target = this,
			ID = 1000 * (team_index + 1) + 100 + 2 + index,
			Left = Format("%dem", name_width + index * entry_width),
			Right = Format("%dem", name_width + (index + 1) * entry_width),
			Style = GUI_TextVCenter | GUI_TextHCenter,
			Text = Format("%d", Call(entries[index].team_callback, team)),
		};
		row[Format("entry%d", index)] = entry;	
	}
	return row;
}

private func MakePlayerRow(int plr, int team_index, int player_index, array entries, int name_width, int entry_width)
{
	var row = {
		Target = this,
		ID = 1000 * (team_index + 1) + 100 * (player_index + 2),
		Left = "0%",
		Right = "100%",
		Top = Format("%dem", 2 * (player_index + 1)),
		Bottom = Format("%dem", 2 * (player_index + 2)),
		BackgroundColor = {Std = SRBDMENU_BackgroundColor},
	};
	row.name = 
	{
		Target = this,
		ID = 1000 * (team_index + 1) + 100 * (player_index + 2) + 1,
		Left = "0%",
		Right = Format("%dem", name_width),
		Style = GUI_TextVCenter,
		Text = Format("    %s", GetTaggedPlayerName(plr)),
	};
	for (var index = 0; index < GetLength(entries); index++)
	{
		var entry = 
		{
			Target = this,
			ID = 1000 * (team_index + 1) + 100 + 2 + index,
			Left = Format("%dem", name_width + index * entry_width),
			Right = Format("%dem", name_width + (index + 1) * entry_width),
			Style = GUI_TextVCenter | GUI_TextHCenter,
			Text = Format("%d", Call(entries[index].plr_callback, plr)),
		};
		if (entries[index].plr_clickcall)
			entry.OnClick = GuiAction_Call(this, entries[index].plr_clickcall, plr);
		row[Format("entry%d", index)] = entry;	
	}
	return row;
}


/*-- Scoreboard Data: Flags --*/

private func GetPlayerFlags(int plr)
{
	return ObjectCount(Find_ID(Flagpole), Find_Owner(plr));
}

private func GetTeamFlags(int team)
{
	var flag_cnt = 0;
	for (var plr in GetPlayers(nil, team))
		flag_cnt += GetPlayerFlags(plr);
	return flag_cnt;
}


/*-- Scoreboard Data: Medals --*/

private func GetPlayerMedals(int plr)
{
	var medals = Rule_Medals->GetPlayerMedals(plr, true);
	var medal_cnt = 0;
	for (var medal in medals)
		medal_cnt += medal[1];	
	return medal_cnt;
}

private func GetTeamMedals(int team)
{
	var medal_cnt = 0;
	for (var plr in GetPlayers(nil, team))
		medal_cnt += GetPlayerMedals(plr);
	return medal_cnt;
}

public func OnMedalClick(int plr)
{
	// Open medal menu for the specific player.
	MedalMenu->CreateMedalMenu(plr);
	return;
}


/*-- Scoreboard Data: Deaths --*/

private func GetPlayerDeaths(int plr)
{
	return Scoreboard_BaseMelee_Data->GetDeaths(plr);
}

private func GetTeamDeaths(int team)
{
	var death_cnt = 0;
	for (var plr in GetPlayers(nil, team))
		death_cnt += GetPlayerDeaths(plr);
	return death_cnt;
}


/*-- Scoreboard Data: Kills --*/

private func GetPlayerKills(int plr)
{
	return Scoreboard_BaseMelee_Data->GetKills(plr);
}

private func GetTeamKills(int team)
{
	var kill_cnt = 0;
	for (var plr in GetPlayers(nil, team))
		kill_cnt += GetPlayerKills(plr);
	return kill_cnt;
}


/*-- Scenario Saving --*/

// Don't save the scoreboard.
public func SaveScenarioObject() { return false; }


/*-- Properties --*/

local Plane = 1000;
