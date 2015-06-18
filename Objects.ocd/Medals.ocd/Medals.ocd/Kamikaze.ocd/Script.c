/**
	Kamikaze
	Awarded when you kill yourself and an enemy clonk simultaneously.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 4; }
public func GetMedalReward() { return 25; }


/*-- Medal Scripts --*/

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalKamikaze", nil, 100, 0, nil, Medal_Kamikaze);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalKamikaze", nil);
	return;
}

// Called when a crew member dies.
public func OnCrewDeath(object crew, int killed_by)
{
	// Only trace kills which have been done on and by a player.
	if (killed_by == NO_OWNER)
		return;
	if (crew->GetOwner() == NO_OWNER)
		return;
	// Notify the effect that keeps track of the kamikaze medal.
	var effect = GetEffect("IntMedalKamikaze", nil);
	if (effect)
		EffectCall(nil, effect, "OnKill", killed_by, crew->GetOwner());
	return;
}


/*-- Medal Effects --*/

// Medal kamikaze effect keeps track of which players have made kills at which frames.
public func FxIntMedalKamikazeStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Create a variable which keeps track of the awarded medals per player.
	effect.kills = [];
	return FX_OK;
}

// Effect callback made when a kill has been done.
public func FxIntMedalKamikazeOnKill(object target, proplist effect, int killer, int killed)
{
	var frame = FrameCounter();
	// Remove all kills which have been done in previous frames.
	RemoveOldKills(effect.kills, frame);	
	// Check if in previous kills there has been a kill compatible with this one.
	var kamikaze = false;
	for (var i = 0; i < GetLength(effect.kills); i++)
	{
		var kill = effect.kills[i];
		if (killer == killed)
		{
			// Made selfkill so look for an enemy kill by this killer.
			if (kill.killer == killer && kill.killed != kill.killer)
			{
				kamikaze = true;
				effect.kills[i] = nil;
			}
		}
		else
		{
			// Made an enemy kill so look for a selfkill by this killer.
			if (kill.killer == killer && kill.killed == kill.killer)
			{
				kamikaze = true;
				effect.kills[i] = nil;
			}		
		}
	}
	RemoveHoles(effect.kills);
	// Award medal if kamikaze has been detected.
	if (kamikaze)
	{
		Rule_Medals->~AwardMedal(Medal_Kamikaze, killer);
	}
	// Otherwise add this kill to the kill list.
	{
		var kill = { frame = frame, killer = killer, killed = killed };
		PushBack(effect.kills, kill);
	}
	return FX_OK;
}

// Removes the old kills from the array kills.
public func RemoveOldKills(array kills, int current_frame, int delay)
{
	if (delay == nil)
		delay = 0;
	for (var i = 0; i < GetLength(kills); i++)
		if (kills[i].frame + delay < current_frame)
			kills[i] = nil;
	RemoveHoles(kills);
	return;
}

// Medal kamikaze effect keeps track of which players have made kills at which frames.
public func FxIntMedalKamikazeStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
