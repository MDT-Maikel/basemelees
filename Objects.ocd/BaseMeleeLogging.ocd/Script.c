/**
	Base Melee Logging
	Rule responsible for the logging in base melees. The following things will be logged:
	* Player joining, same for teams.
	* Player elimination, same for teams
	
	@author Maikel
*/


protected func Initialize()
{
	// Under no circumstance there may by multiple copies of this rule.
	if (ObjectCount(Find_ID(Rule_BaseMeleeLogging)) > 1)
		return RemoveObject();
	return;
}

// Called when a player joins a round, this will be logged.
protected func InitializePlayer(int plr)
{
	var team = GetPlayerTeam(plr);
	// If player has no team, then log this.
	if (!team)
		return LogSpecial(Format("$PlayerJoinedNoTeam$", GetTaggedPlayerName(plr)));
	// If player is the first in the team, log as team leader.
	if (GetPlayerInTeamCount(team) <= 1)
		return LogSpecial(Format("$PlayerJoinedTeamLeader$", GetTaggedPlayerName(plr), GetTaggedTeamName(team)));
	// Otherwise log normal team join.
	LogSpecial(Format("$PlayerJoinedInTeam$", GetTaggedPlayerName(plr), GetTaggedTeamName(team)));
	return;
}

// Called when a player is eliminated, this will be logged.
protected func RemovePlayer(int plr)
{
	// Log that the player has been eliminated.
	LogSpecial(Format("$PlayerEliminated$", GetTaggedPlayerName(plr)));
	// If team has been eliminated, log that as well.	
	var team = GetPlayerTeam(plr);
	if (team && GetPlayerInTeamCount(team) <= 1)
		LogSpecial(Format("$TeamEliminated$", GetTaggedTeamName(team)));
	return;
}

// Callback when a clonk dies, which will be logged.
protected func OnClonkDeath(object clonk, int killed_by)
{
	var plr = clonk->GetOwner();
	var crew_name = "Clonk";
	if (clonk) 
		crew_name = clonk.Prototype->GetName();
	var rand = Random(3) + 1;
	// Log for unknown killer.
	if (!GetPlayerName(killed_by))
		return LogSpecial(Format(Translate(Format("KilledByGaya%d", rand)), GetTaggedPlayerName(plr), crew_name));
	// Log for self kill.	
 	if (plr == killed_by)
		return LogSpecial(Format(Translate(Format("Selfkill%d", rand)), GetTaggedPlayerName(plr), crew_name));
	// Log for team kill.	
 	else if (!Hostile(plr,killed_by))
  		return LogSpecial(Format(Translate(Format("Teamkill%d", rand)), GetTaggedPlayerName(plr), crew_name, GetTaggedPlayerName(killed_by)));
	// Log normal kill of an enemy.
	LogSpecial(Format(Translate(Format("KilledByPlayer%d", rand)), GetTaggedPlayerName(plr), crew_name, GetTaggedPlayerName(killed_by)));
	return;
}

// Callback when a player earns a medal, this will be logged.
protected func OnMedalAwarded(id medal, int to_plr, int reward)
{
	if (!medal->~IsMedal())
		return;
	// The logging format depends on the medal reward.
	var log;
	if (reward)
		log = Format("$AwardedMedalReward$", medal, GetPlayerName(to_plr), medal.Name, reward, Icon_Wealth);
	else
		log = Format("$AwardedMedal$", medal, GetPlayerName(to_plr), medal.Name);
	LogSpecial(log);
	return;
}

// Callback when a flagpole has been destroyed.
protected func OnFlagpoleDestruction(int owner, int destructor)
{
	var team_owner = GetPlayerTeam(owner);
	var team_destructor = GetPlayerTeam(destructor);	
	var self = team_owner == team_destructor;
	var nr_flags_remaining = ObjectCount(Find_ID(Flagpole), Find_Team(team_owner)) - 1;
	var log;
	if (self)
	{
		if (nr_flags_remaining == 0)
			log = Format("$FlagDestructionSelf0$", GetTaggedTeamName(team_owner));
		else if (nr_flags_remaining == 1)
			log = Format("$FlagDestructionSelf1$", GetTaggedTeamName(team_owner));
		else
			log = Format("$FlagDestructionSelfN$", GetTaggedTeamName(team_owner), nr_flags_remaining);
	}
	else
	{
		if (nr_flags_remaining == 0)
			log = Format("$FlagDestructionTeam0$", GetTaggedTeamName(team_destructor), GetTaggedTeamName(team_owner));
		else if (nr_flags_remaining == 1)
			log = Format("$FlagDestructionTeam1$", GetTaggedTeamName(team_destructor), GetTaggedTeamName(team_owner));
		else
			log = Format("$FlagDestructionTeamN$", GetTaggedTeamName(team_destructor), GetTaggedTeamName(team_owner), nr_flags_remaining);
	}
	LogSpecial(log);
	return;
}


/*-- Rule Menu --*/

protected func Activate(int by_plr)
{
	MessageWindow("$Description$", by_plr);
	return;
}


/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";
local Visibility = VIS_Editor;
local EditorPlacementLimit = 1;
