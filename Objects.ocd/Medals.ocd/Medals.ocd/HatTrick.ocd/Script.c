/**
	Hat-trick
	Awarded when three enemies are killed in a row.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 1; }
public func GetMedalReward() { return 25; }


/*-- Medal Scripts --*/

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalHatTrick", nil, 100, 0, nil, Medal_HatTrick);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalHatTrick", nil);
	return;
}

// Called when a crew member dies.
public func OnCrewDeath(object crew, int killed_by)
{
	var killed = crew->GetOwner();
	// Only track kills which are on enemies.
	// Self kills and team kills don't spoil hat-tricks.
	if (killed_by == NO_OWNER)
		return;
	if (!Hostile(killed, killed_by))
		return;
	// Transfer the callback to the effect.
	var effect = GetEffect("IntMedalHatTrick", nil);
	if (effect)
		EffectCall(nil, effect, "OnKill", killed, killed_by);
	return;
} 


/*-- Medal Effects --*/

// Medal hat-trick effect keeps track of whether the medal may be awarded.
public func FxIntMedalHatTrickStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Initialize variable to track last kill.
	effect.lastkill = nil;
	return FX_OK;
}

// Effect callback made when a kill has been made.
public func FxIntMedalHatTrickOnKill(object target, proplist effect, int killed, int killed_by)
{
	// Increase kill count if last kill has the same killer.
	if (effect.lastkill && effect.lastkill.killer == killed_by)
		effect.lastkill.count++;
	// Otherwise start a new last kill.
	else
		effect.lastkill = { killer = killed_by, count = 1 };
	
	// Check if kill count exceeds three and award medal.
	if (effect.lastkill && effect.lastkill.count >= 3)
	{
		// Award the medal and reset last kill.
		Rule_Medals->~AwardMedal(Medal_HatTrick, effect.lastkill.killer);
		effect.lastkill = nil;
	}
	return FX_OK;
}

// Medal hat-trick effect keeps track of whether the medal may be awarded.
public func FxIntMedalHatTrickStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}