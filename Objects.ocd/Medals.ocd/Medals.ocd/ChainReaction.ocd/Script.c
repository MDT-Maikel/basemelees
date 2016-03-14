/**
	Chain Reaction
	Awarded for setting of a chain reaction of firestone explosions.
	The chain reaction counts when at least 8 firestone explosions
	were triggered by an initial explosion.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 9; }
public func GetMedalReward() { return 20; }


/*-- Medal Scripts --*/

// See Firestone.c script for handling the explosion of firestones.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalChainReaction", nil, 100, 0, nil, Medal_ChainReaction);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalChainReaction", nil);
	return;
}


/*-- Medal Effects --*/

// Medal chain reaction effect keeps track of which players have caused which explosions.
public func FxIntMedalChainReactionStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Create variables which keeps track of created and exploded firestone per player.
	effect.created_firestones = [];
	effect.chained_firestones = [];
	return FX_OK;
}

// Effect callback made by a constructed firestone.
public func FxIntMedalChainReactionOnCreation(object target, proplist effect, int plr, object firestone)
{
	// Don't do anything if the player does not exist.
	if (plr == NO_OWNER)
		return FX_OK;	
	// Get the location to store the firestone.
	var plr_id = GetPlayerID(plr);
	if (effect.created_firestones[plr_id] == nil)
		effect.created_firestones[plr_id] = [];
	// Store the firestone and its creation time in a variable.	
	PushBack(effect.created_firestones[plr_id], { obj = firestone, time = FrameCounter() });
	return FX_OK;
}

// Effect callback made by a destructed firestone.
public func FxIntMedalChainReactionOnDestruction(object target, proplist effect, int plr, object firestone)
{
	// Don't do anything if the player does not exist.
	if (plr == NO_OWNER)
		return FX_OK;	
	// Get the location where the firestones are stored.
	var plr_id = GetPlayerID(plr);
	if (effect.created_firestones[plr_id] == nil)
		effect.created_firestones[plr_id] = [];
	// Find the if the firestone is stored for the player.
	var index = GetExplosiveInArray(effect.created_firestones[plr_id], firestone);
	if (index == -1)
		return FX_OK;
	var delay = FrameCounter() - effect.created_firestones[plr_id][index].time;
	RemoveArrayIndex(effect.created_firestones[plr_id], index);
	// Only process firestones which have their explosion within three seconds of creation.
	if (delay > 36 * 3)
		return FX_OK;
	// Get the location where chained explosions are stored.
	if (effect.chained_firestones[plr_id] == nil)
		effect.chained_firestones[plr_id] = [];
	PushBack(effect.chained_firestones[plr_id], FrameCounter());
	// Then process the chained explosions to see if they really form a chain.
	// A chain is only valid if successive explosions are not more than a second apart.
	var index = 0;
	for (; index < GetLength(effect.chained_firestones[plr_id]) - 1; index++)
	{
		var time1 = effect.chained_firestones[plr_id][index];
		var time2 = effect.chained_firestones[plr_id][index + 1];
		if (time2 - time1 > 36)
			break;
	}
	if (index < GetLength(effect.chained_firestones[plr_id]) - 1)
		effect.chained_firestones[plr_id] = effect.chained_firestones[plr_id][index + 1:GetLength(effect.chained_firestones[plr_id])];
	// Now that only the chained firestones are left check if the chain is longer than 8 firestones and award the medal.
	if (GetLength(effect.chained_firestones[plr_id]) >= 8)
	{
		effect.chained_firestones[plr_id] = [];
		Rule_Medals->AwardMedal(Medal_ChainReaction, plr);
	}
	return FX_OK;
}

// Medal chain reaction effect keeps track of which players have caused which explosions.
public func FxIntMedalChainReactionStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}

// Returns the index of an explosive in the given array.
private func GetExplosiveInArray(array a, object exp)
{
	var index = 0;
	for (; index < GetLength(a); index++)
		if (a[index].obj == exp)
			break;
	if (index == GetLength(a))
		index = -1;
	return index;
}
