/**
	Slasher
	Awarded for killing an enemy with an in-air sword strike from above.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 13; }
public func GetMedalReward() { return 20; }


/*-- Medal Scripts --*/

// See SwordSlash.c script for the handling of striking enemies with a sword.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalSlasher", nil, 100, 0, nil, Medal_Slasher);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalSlasher", nil);
	return;
}


/*-- Medal Effects --*/

// Medal slasher effect keeps track of whether the medal may be awarded.
public func FxIntMedalSlasherStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}

// Effect callback made when a sword strike occured.
public func FxIntMedalSlasherOnSwordSlash(object target, proplist effect, object striker, object struck)
{
	var by_plr = striker->GetController();
	var to_plr = struck->GetController();
	// Only valid if done to an enemy.
	if (by_plr == NO_OWNER || !Hostile(to_plr, by_plr))
		return FX_OK;
	// Award the medal if the struck crew member died, i.e. is dead.
	if (!struck->GetAlive())
		Rule_Medals->~AwardMedal(Medal_Slasher, by_plr);
	return FX_OK;
}

// Medal slasher effect keeps track of whether the medal may be awarded.
public func FxIntMedalSlasherStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
