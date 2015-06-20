/**
	Scoreboard Data
	Environment object that keeps track of certain scoreboard data values.

	@author Maikel
*/


// Scoreboard data variables.
local score_death_list;
local score_kill_list;


/*-- Data Counting --*/

protected func Initialize()
{
	// Only one scoreboard data handling object.
	if (ObjectCount(Find_ID(Scoreboard_BaseMelee_Data)) > 1) 
		return RemoveObject();
	// Init data variables.
	score_death_list = [];
	score_kill_list = [];
	// Initialize for already existing players.
	for (var plr in GetPlayers())
		InitializePlayer(plr);
	return;
}

protected func InitializePlayer(int plr)
{
	// Init data variables for the player.
	var plr_id = GetPlayerID(plr);
	score_death_list[plr_id] = 0;
	score_kill_list[plr_id] = 0;
	return;
}

protected func OnClonkDeath(object clonk, int killer)
{
	var plr = clonk->GetOwner();
	// Award a kill if player exists and is hostile.
	if (killer != plr && killer != NO_OWNER && Hostile(plr, killer))
	{
		var killer_id = GetPlayerID(killer);
		score_kill_list[killer_id]++;
	}
	// Always award a death for a clonk.
	var plr_id = GetPlayerID(plr);
	score_death_list[plr_id]++;
	return;
}


/*-- Data Access --*/

public func GetDeathsForPlayer(int plr)
{
	var plr_id = GetPlayerID(plr);
	return score_death_list[plr_id];
}

public func GetKillsForPlayer(int plr)
{
	var plr_id = GetPlayerID(plr);
	return score_kill_list[plr_id];
}


/*-- Interface --*/

// Creates the data tracking and storage object.
public func Init()
{
	if (this != Scoreboard_BaseMelee_Data)
		return;
	if (FindObject(Find_ID(Scoreboard_BaseMelee_Data)))
		return;
	CreateObject(Scoreboard_BaseMelee_Data);
	return;
}

// Returns the number of deaths for the given player.
public func GetDeaths(int plr)
{
	if (this != Scoreboard_BaseMelee_Data)
		return;
	this->Init();
	var data = FindObject(Find_ID(Scoreboard_BaseMelee_Data));
	return data->GetDeathsForPlayer(plr);
}

// Returns the number of kills for the given player.
public func GetKills(int plr)
{
	if (this != Scoreboard_BaseMelee_Data)
		return;
	this->Init();
	var data = FindObject(Find_ID(Scoreboard_BaseMelee_Data));
	return data->GetKillsForPlayer(plr);
}
