/**
	Superman
	Awarded when a crew member stays in mid-air for a long time.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 15; }
public func GetMedalReward() { return 25; }


/*-- Medal Settings --*/

// This is the mid-air duration target which a crew member needs to achieve.
private func GetFlightDurationGoal() { return 36 * 16; }


/*-- Medal Scripts --*/

// See ClonkAction.c script for handling the clonk being in air.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalSuperman", nil, 100, 0, nil, Medal_Superman);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalSuperman", nil);
	return;
}


/*-- Medal Effects --*/

// Medal superman effect keeps track of whether the medal may be awarded.
public func FxIntMedalSupermanStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}

// Effect callback made when a crew member was a superman.
public func FxIntMedalSupermanWasSuperman(object target, proplist effect, int plr)
{
	// Just award the medal.
	Rule_Medals->~AwardMedal(Medal_Superman, plr);
	return FX_OK;
}

// Medal superman effect keeps track of whether the medal may be awarded.
public func FxIntMedalSupermanStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
