/**
	Medal Template
	Template for medals, copy and modify this to create your own medals.

	@author Maikel
*/


/*-- Medal Properties --*/

// A medal should have a name.
local Name = "$Name$";

// A medal should have description.
local Description = "$Description$";

// A medal should have this function and return true.
public func IsMedal() { return true; }

// A medal should have this function and return a unique index.
// Consult the list of existing medals to get a unique index.
// The medal index can be any integer between 0 and 254.
public func GetMedalIndex() { return 0; }


/*-- Medal Scripts --*/

// Medals have various ways to track whether they should be awarded.
// A medal can be awared by making an ID call to the medal rule like:
// Rule_Medals->AwardMedal(medal_id, plr);

// Sometimes it is necessary to append scripts to existing objects.
// These scripts should be a single script per medal called Medal*.c
// and should be placed in the Scripts.ocd/System.ocg folder. See for
// example the Medal_Template.c script.

// There are also callbacks from the medal rule for certain events.
// These are ID calls from the rule to the medal and can be used
// to start effects. Remember that for ID calls you can't use
// local variables. The calls are listed below.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	//Log("OnRoundStart() callback has been done in %i.", Medal_Template);
	return;
}

// Called when the round is finished, i.e. when all players are eliminated
// or all goals are fulfilled.
public func OnRoundFinish()
{
	//Log("OnRoundFinish() callback has been done in %i.", Medal_Template);
	return;
}

// Called when a player is initialized.
public func OnInitializePlayer(int plr)
{
	//Log("OnInitializePlayer(%d) callback has been done in %i.", plr, Medal_Template);
	return;
}

// Called when a player is removed.
public func OnRemovePlayer(int plr)
{
	//Log("OnRemovePlayer(%d) callback has been done in %i.", plr, Medal_Template);
	return;
}

// Called when a crew member dies.
public func OnCrewDeath(object crew, int killed_by)
{
	//Log("OnCrewDeath(%v, %d) callback has been done in %i.", crew, killed_by, Medal_Template);
	return;
}

// Called when a medal is awarded to a player.
public func OnMedalAwarded(id medal, int to_plr)
{
	//Log("OnMedalAwarded(%i, %d) callback has been done in %i.", medal, to_plr, Medal_Template);
	return;
}
