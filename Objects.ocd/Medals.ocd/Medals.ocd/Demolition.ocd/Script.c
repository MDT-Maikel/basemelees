/**
	Demolition
	Awarded for destroying ten structures.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 7; }
public func GetMedalReward() { return 50; }


/*-- Medal Settings --*/

// This is the number of structures that need to be destroyed to achieve the medal.
private func GetDemolitionGoal() { return 10; }


/*-- Medal Scripts --*/

// See StructureDamage.c script for handling the destroying of structures.
// This medal adds an effect which keeps track of the players' progress.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalDemolition", nil, 100, 0, nil, Medal_Demolition);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalDemolition", nil);
	return;
}

// Medal demolition effect keeps track of which players have destroyed structures.
public func FxIntMedalDemolitionStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Create a variable which keeps track of the destroyed structures per player.
	effect.destroyed_structures = [];
	effect.demolition_goal = Medal_Demolition->GetDemolitionGoal();
	return FX_OK;
}

// Effect callback made by a destroyed structure.
public func FxIntMedalDemolitionHasDestroyed(object target, proplist effect, int plr, id structure)
{
	// Increase the destroyed structures for the player.
	var plr_id = GetPlayerID(plr);
	if (effect.destroyed_structures[plr_id] == nil)
		effect.destroyed_structures[plr_id] = 0;
	effect.destroyed_structures[plr_id]++;
	// Check whether the destroyed structures exceed ten, if so notify rule.
	if (effect.destroyed_structures[plr_id] >= effect.demolition_goal)
	{
		Rule_Medals->~AwardMedal(Medal_Demolition, plr);
		// Reset the destroyed structures so that a player can reach the medal again.
		effect.destroyed_structures[plr_id] = 0; 
	}	
	return FX_OK;
}

// Medal demolition effect keeps track of which players have destroyed structures.
public func FxIntMedalDemolitionStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}