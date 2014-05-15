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
	var team_name = GetTaggedTeamName(team);
	if (GetPlayerCountByTeam(team) <= 1)
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
	if (team && GetPlayerCountByTeam(team) <= 1)
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

// Returns the number of players belonging to the given team.
private func GetPlayerCountByTeam(int team)
{
	var count = 0;
	for (var i = 0; i < GetPlayerCount(); i++)
		if (GetPlayerTeam(GetPlayerByIndex(i)) == team)
			count++;
	return count;
}

// Callback when a player earns a medal, this will be logged.
protected func OnMedalAwarded(id medal, int to_plr, int reward)
{
	if (!medal->~IsMedal())
		return;
	// The logging format depends on the medal reward.
	var log;
	if (reward)
		log = Format("$AwardedMedalReward$", medal, GetPlayerName(to_plr), medal.Name, reward, GUI_Wealth);
	else
		log = Format("$AwardedMedal$", medal, GetPlayerName(to_plr), medal.Name);
	LogSpecial(log);
	return;
}

protected func Activate(int byplr)
{
	MessageWindow("$Description$", byplr);
	return;
}


/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";
