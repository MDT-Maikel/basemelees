/**
	Berserker
	Awarded when a player deals a lot of damage points in a short time window.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 14; }
public func GetMedalReward() { return 40; }


/*-- Medal Settings --*/

private func GetDamageGoal() { return 200; }
private func GetDamageDuration() { return 36 * 60; }


/*-- Medal Scripts --*/

// See ClonkDamage.c script for handling energy loss of the crew member.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalBerserker", nil, 100, 0, nil, Medal_Berserker);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalBerserker", nil);
	return;
}


/*-- Medal Effects --*/

// Medal berserker effect keeps track of whether the medal may be awarded.
public func FxIntMedalBerserkerStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Initialize an array to keep track of the damage dealt by the players.
	effect.damage_dealt = [];
	// Initialize damage goal and duration for achieving it.
	effect.damage_goal = Medal_Berserker->GetDamageGoal();
	effect.damage_duration = Medal_Berserker->GetDamageDuration();
	return FX_OK;
}

// Effect callback made when a crew member was being dealt damage to.
public func FxIntMedalBerserkerOnDamageDealt(object target, proplist effect, int damage, int by_plr, int to_plr)
{
	// Only track damage done to an enemy.
	if (by_plr == NO_OWNER || !Hostile(to_plr, by_plr))
		return FX_OK;
	// Store the damage dealt by the player and the frame of the damage.
	var plr_id = GetPlayerID(by_plr);
	if (effect.damage_dealt[plr_id] == nil)
		effect.damage_dealt[plr_id] = [];
	PushBack(effect.damage_dealt[plr_id], { damage = damage, time = FrameCounter() });
	// Remove damage entries which happened outside the alloted duration.
	RemoveOldDamageEvents(effect.damage_dealt[plr_id], FrameCounter() - effect.damage_duration);
	// Sum damage dealt in the alloted damage period.
	var damage_dealt = CountDamageInEvents(effect.damage_dealt[plr_id]);
	// Award the medal if the damage dealt is more than the damage goal.
	if (damage_dealt >= effect.damage_goal)
	{
		effect.damage_dealt[plr_id] = [];		
		Rule_Medals->~AwardMedal(Medal_Berserker, by_plr);
	}
	return FX_OK;
}

// Medal berserker effect keeps track of whether the medal may be awarded.
public func FxIntMedalBerserkerStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}

// Remove the damage entries which are in past by more than the alloted duration.
private func RemoveOldDamageEvents(array damage_list, int time)
{
	for (var index = GetLength(damage_list) - 1; index >= 0; index--)
	{ 
		var event = damage_list[index];
		if (event.time < time)
			RemoveArrayIndex(damage_list, index);
	}
	return;
}

// Returns the total damage in all active damage events for the player.
private func CountDamageInEvents(array damage_list)
{
	var total_damage = 0;
	for (var event in damage_list)
		total_damage += event.damage;
	return total_damage;
}