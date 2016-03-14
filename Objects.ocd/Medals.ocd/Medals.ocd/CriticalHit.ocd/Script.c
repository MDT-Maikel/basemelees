/**
	Critical Hit
	Awarded for dealing more than 40 damage in a single hit.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 6; }
public func GetMedalReward() { return 20; }


/*-- Medal Scripts --*/

// See ClonkHit.c script for handling energy loss of the crew member.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalCriticalHit", nil, 100, 0, nil, Medal_CriticalHit);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalCriticalHit", nil);
	return;
}


/*-- Medal Effects --*/

// Medal critical hit effect keeps track of whether the medal may be awarded.
public func FxIntMedalCriticalHitStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}

// Effect callback made when a crews energy has been changed.
public func FxIntMedalCriticalHitOnCrewDamage(object target, proplist effect, int damage, int to_plr, int by_plr)
{
	// Only a critical hit if damage >= 40.
	if (damage < 40)
		return FX_OK;
	// Only valid if done to an enemy.
	if (by_plr == NO_OWNER || !Hostile(to_plr, by_plr))
		return FX_OK;
	// Award the medal.
	Rule_Medals->~AwardMedal(Medal_CriticalHit, by_plr);
	return FX_OK;
}

// Medal critical hit effect keeps track of whether the medal may be awarded.
public func FxIntMedalCriticalHitStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
