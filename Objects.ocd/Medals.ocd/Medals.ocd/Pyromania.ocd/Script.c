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
public func GetMedalReward() { return 40; }


/*-- Medal Settings --*/

// This is the number of objects that need to be set on fire to be awarded the medal.
private func GetIncinerationGoal() { return 100; }


/*-- Medal Scripts --*/

// See Incineration.c script for handling overloading the Incinerate function.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalPyromania", nil, 100, 0, nil, Medal_Pyromania);
	return;
}

// Called when the round is finished, i.e. when all players are eliminated
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
	effect.incinerated_objects = [];
	effect.incineration_goal = Medal_Pyromania->GetIncinerationGoal();
	return FX_OK;
}

// Effect callback made when an object has been incinerated.
public func FxIntMedalPyromaniaOnIncineration(object target, proplist effect, object incinerated, int by_plr)
{
	// Increase the incinerated count for the player.
	var plr_id = GetPlayerID(by_plr);
	if (effect.incinerated_counts[plr_id] == nil)
		effect.incinerated_counts[plr_id] = 0;
	if (effect.incinerated_objects[plr_id] == nil)
		effect.incinerated_objects[plr_id] = [];
	RemoveHoles(effect.incinerated_objects[plr_id]);
	// Check if player did not already incinerate this object.
	if (!IsValueInArray(effect.incinerated_objects[plr_id], incinerated))
	{
		effect.incinerated_counts[plr_id]++;
		PushBack(effect.incinerated_objects[plr_id], incinerated);
	}
	// Check whether the incinerated count exceeds 100, if so award medal.
	if (effect.incinerated_counts[plr_id] >= effect.incineration_goal)
	{
		Rule_Medals->~AwardMedal(Medal_Pyromania, by_plr);
		// Reset the incinerated count so that a player can reach the medal again.
		effect.incinerated_counts[plr_id] = 0;
		effect.incinerated_objects[plr_id] = [];
	}
	return FX_OK;
}

// Medal pyromania effect keeps track of which players have set how much on fire.
public func FxIntMedalPyromaniaStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
