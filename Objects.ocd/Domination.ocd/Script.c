/**
	Domination
	A team or player is eliminated when all bases are lost.

	@author Maikel	
*/


protected func Initialize()
{
	// Only one rule of this type.
	if (ObjectCount(Find_ID(Rule_Domination)) > 1)
		return RemoveObject();
	// Add an effect which checks whether a team/player lost all its bases.
	AddEffect("IntCheckBase", nil, 100, 5, this);
	return;
}

// Effect which checks every X frames whether players still have a base.
protected func FxIntCheckBaseTimer(object target, proplist effect, int time)
{
	// Check for all players whether they have at least one base.
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var plr = GetPlayerByIndex(i);
		if (!GetPlayerName(plr))
			continue;
		// Check if player has one or more bases.
		if (PlayerHasBase(plr))
			continue;
		// Otherwise check if one of the team members has a base.
		var team = GetPlayerTeam(plr);
		if (TeamHasBase(team))
			continue;
		// Player has yet to choose a team, wait with elimination.
		if (team == -1)
			continue;
		// Eliminate player, since he has no base.	
		EliminatePlayer(plr);
	}
	return FX_OK;
}

// Returns whether the individual player has a base.
private func PlayerHasBase(int plr)
{
	var base = FindObject(Find_Owner(plr), Find_Or(Find_Func("IsBaseBuilding")));
	if (base)
		return true;
	return false;
}

// Returns whether the one of the players in the team has a base.
private func TeamHasBase(int team)
{
	if (team == 0)
		return false;
	for (var i = 0; i < GetPlayerCount(); i++)
	{
		var plr = GetPlayerByIndex(i);
		// Check if member is of same team and has a base.
		if (team == GetPlayerTeam(plr))
			if (PlayerHasBase(plr))
				return true;
	}
	return false;
}


/*-- Rule Menu --*/

protected func Activate(int by_plr)
{
	MessageWindow(Description, by_plr);
	return true;
}


/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";
local Visibility = VIS_Editor;
local EditorPlacementLimit = 1;
