/**
	Heroic
	Awarded when a crew member survives more than 250 damage points.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 8; }
public func GetMedalReward() { return 40; }


/*-- Medal Scripts --*/

// See ClonkHit.c script for handling energy loss of the crew member.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalHeroic", nil, 100, 0, nil, Medal_Heroic);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalHeroic", nil);
	return;
}


/*-- Medal Effects --*/

// Medal heroic effect keeps track of whether the medal may be awarded.
public func FxIntMedalHeroicStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}

// Effect callback made when a crew member was heroic.
public func FxIntMedalHeroicWasHeroic(object target, proplist effect, int plr)
{
	// Just award the medal.
	Rule_Medals->~AwardMedal(Medal_Heroic, plr);
	return FX_OK;
}

// Medal heroic effect keeps track of whether the medal may be awarded.
public func FxIntMedalHeroicStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
