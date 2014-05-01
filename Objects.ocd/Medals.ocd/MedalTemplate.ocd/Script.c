/**
	Medal Template
	Template for medals.

	@author Maikel
*/

// A medal should have this function and return true.
public func IsMedal() { return true; }

// A medal should have this function and return a unique index.
// Consult the list of existing medals to get a unique index.
public func GetMedalIndex() { return 0; }

// A medal should have a name.
local Name = "$Name$";

// A medal should have description.
local Description = "$Description$";


// Medals have various ways to track whether they should be awarded.
// A medal can be awared by making an ID call to the medal rule like:
// Rule_Medals->AwardMedal(medal_id, plr);

// Sometimes it is necessary to append scripts to existing objects.
// These scripts should be a single script per medal called Medal*.c
// and should be placed in the Scripts.ocd/System.ocg folder. See for
// example the MedalTamplate.c script.

// There are also callbacks from the medal rule for certain events.
// These are ID calls from the rule to the medal and can be used
// to start effects. Remember that for ID calls you can't use
// local variables. The calls are listed below.

// Called when the rule is initialized.
public func OnInitialize()
{
	Log("OnInitialize() callback has been done in %i.", Medal_Template);
	return;
}

// Called when a player is initialized.
public func OnInitializePlayer(int plr)
{
	Log("OnInitializePlayer(%d) callback has been done in %i.", plr, Medal_Template);
	return;
}

// Called when a player is removed.
public func OnRemovePlayer(int plr)
{
	Log("OnRemovePlayer(%d) callback has been done in %i.", plr, Medal_Template);
	return;
}