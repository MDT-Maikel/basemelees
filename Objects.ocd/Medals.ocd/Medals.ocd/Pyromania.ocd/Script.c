/**
	Pyromania
	Awarded for setting 100 objects on fire in a single round.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 5; }


/*-- Medal Scripts --*/

// See Medal_Pyromania.c script for handling overloading the Incinerate function.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalPyromania", nil, 100, 0, nil, Medal_Pyromania);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalPyromania", nil);
	return;
}


/*-- Medal Effects --*/

// Medal pyromania effect keeps track of which players have set how much on fire.
public func FxIntMedalPyromaniaStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Create a variable which keeps track of the mined wealth per player.
	effect.incinerated_counts = [];
	return FX_OK;
}

// Effect callback made when an object has been incinerated.
public func FxIntMedalPyromaniaOnIncineration(object target, proplist effect, int plr)
{
	// Increase the incinerated count for the player.
	var plr_id = GetPlayerID(plr);
	if (effect.incinerated_counts[plr_id] == nil)
		effect.incinerated_counts[plr_id] = 0;
	effect.incinerated_counts[plr_id]++;
	//Log("%s is playing pyromane and now has incinerated %d objects", GetPlayerName(plr), effect.incinerated_counts[plr_id]);
	// Check whether the incinerated count exceeds 100, if so award medal.
	if (effect.incinerated_counts[plr_id] >= 100)
	{
		Rule_Medals->~AwardMedal(Medal_Pyromania, plr);
		// Reset the incinerated count so that a player can reach the medal again.
		effect.incinerated_counts[plr_id] = 0; 
	}
	return FX_OK;
}

// Medal pyromania effect keeps track of which players have set how much on fire.
public func FxIntMedalMinerStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
