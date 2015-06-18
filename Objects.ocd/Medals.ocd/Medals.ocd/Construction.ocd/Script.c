/**
	Construction
	Awarded for aiding in the construction of ten structures.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 12; }
public func GetMedalReward() { return 40; }


/*-- Medal Scripts --*/

// See ConstructionAid.c script for handling the construction of structures.
// This medal adds an effect which keeps track of the players' progress.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalConstruction", nil, 100, 0, nil, Medal_Construction);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalConstruction", nil);
	return;
}

// Medal construction effect keeps track of which players have constructed structures.
public func FxIntMedalConstructionStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Create a variable which keeps track of the constructed structures per player.
	effect.constructed_structures = [];
	return FX_OK;
}

// Effect callback made when a player has aided in the construction.
public func FxIntMedalConstructionHasAided(object target, proplist effect, int plr, id structure)
{
	// Increase the constructed structures for the player.
	var plr_id = GetPlayerID(plr);
	if (effect.constructed_structures[plr_id] == nil)
		effect.constructed_structures[plr_id] = 0;
	effect.constructed_structures[plr_id]++;
	// Check whether the constructed structures exceed ten, if so notify rule.
	if (effect.constructed_structures[plr_id] >= 10)
	{
		Rule_Medals->~AwardMedal(Medal_Construction, plr);
		// Reset the constructed structures so that a player can reach the medal again.
		effect.constructed_structures[plr_id] = 0; 
	}	
	return FX_OK;
}

// Medal constructed effect keeps track of which players have constructed structures.
public func FxIntMedalConstructionStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}