/**
	Decorated
	Awarded when you get awarded with more than ten medals in a single round.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 3; }


/*-- Medal Scripts --*/

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalDecorated", nil, 100, 0, nil, Medal_Decorated);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalDecorated", nil);
	return;
}

// Called when a medal is awarded to a player.
public func OnMedalAwarded(id medal, int to_plr)
{
	// Notify the effect that keeps track of the medal awarding that a medal has been awarded.
	var effect = GetEffect("IntMedalDecorated", nil);
	if (effect)
		EffectCall(nil, effect, "AwardedMedal", to_plr, medal);
	return;
}


/*-- Medal Effects --*/

// Medal decorated effect keeps track of which players have gained how many medals.
public func FxIntMedalDecoratedStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Create a variable which keeps track of the awarded medals per player.
	effect.awarded_medals = [];
	return FX_OK;
}

// Effect callback made when a medal is awarded.
public func FxIntMedalDecoratedAwardedMedal(object target, proplist effect, int plr, id medal)
{
	// Only if it is a real medal and not this medal.
	if (!medal->~IsMedal() || medal == Medal_Decorated)
		return FX_OK;
	// Increase the awarded medals for the player.
	var plr_id = GetPlayerID(plr);
	if (effect.awarded_medals[plr_id] == nil)
		effect.awarded_medals[plr_id] = 0;
	effect.awarded_medals[plr_id]++;
	//Log("%s has been awarded %i and now has %d medals in this round", GetPlayerName(plr), medal, effect.awarded_medals[plr_id]);
	// Check whether the number of awarded medals exceeds ten, if so notify rule.
	if (effect.awarded_medals[plr_id] >= 10)
	{
		// Reset the awarded medals so that a player can reach the medal again.
		effect.awarded_medals[plr_id] = 0; 
		// And award the medal.
		Rule_Medals->~AwardMedal(Medal_Decorated, plr);
	}	
	return FX_OK;
}

// Medal decorated effect keeps track of which players have gained how many medals.
public func FxIntMedalDecoratedStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
