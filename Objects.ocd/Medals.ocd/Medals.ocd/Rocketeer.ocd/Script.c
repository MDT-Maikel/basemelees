/**
	Rocketeer
	Awarded for killing an enemy clonk with a rocket explosion.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 10; }
public func GetMedalReward() { return 25; }


/*-- Medal Scripts --*/

// See Medal_Rocketeer.c script for handling the explosion of firestones.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalRocketeer", nil, 100, 0, nil, Medal_Rocketeer);
	return;
}

// Called when the round is finished, i.e. when all players are eliminated
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalRocketeer", nil);
	return;
}

// Called when a crew member dies.
public func OnCrewDeath(object crew, int killed_by)
{
	var killed = crew->GetOwner();
	// Only track rocket kills which are on enemies.
	if (killed_by == NO_OWNER || !Hostile(killed, killed_by))
		return;
	// Transfer the callback to the effect.
	var effect = GetEffect("IntMedalRocketeer", nil);
	if (effect)
		EffectCall(nil, effect, "OnKill", crew, killed_by);
	return;
}


/*-- Medal Effects --*/

// Medal rocketeer effect keeps track of which players made rocket kills.
public func FxIntMedalRocketeerStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}

// Effect callback made by the rocket on its explosion.
public func FxIntMedalRocketeerOnExplosion(object target, proplist effect, object rocket)
{
	// First get the controller of this rocket, which might be awarded a medal.
	var controller = rocket->GetController();
	// If not controlled by a player don't do anything.
	if (controller == NO_OWNER)
		return FX_OK;
	// Otherwise store controller in effect.
	effect.controller = controller;
	// Get a list of clonks the rocket may have hurt, just by distance and store in effect.
	effect.possible_kills = rocket->FindObjects(Find_OCF(OCF_CrewMember), Find_Distance(50));
	// Store the frame of the rocket explosion in the effect.
	effect.explode_frame = FrameCounter();
	return FX_OK;
}

// Effect callback made by when an enemy clonk has been killed.
public func FxIntMedalRocketeerOnKill(object target, proplist effect, object clonk, int killer)
{
	// Check if both this kill and the last rocket explosion were in the same frame.
	if (FrameCounter() != effect.explode_frame)
	{
		// Variables set on rocket explosion can be removed if frame has increased.
		effect.controller = nil;
		effect.possible_kills = nil;
		effect.explode_frame = nil;
		return FX_OK;
	}
	// Check if the killer and the controller of the rocket are the same.
	if (killer == effect.controller)
	{
		// Loop over possible kills and compare with killed clonk.
		for (var crew in effect.possible_kills)
			if (crew == clonk)
				Rule_Medals->AwardMedal(Medal_Rocketeer, killer);
	}
	// Done, keep the explosion variables for a possible multikill with the rocket.
	return FX_OK;
}

// Medal rocketeer effect keeps track of which players made rocket kills.
public func FxIntMedalRocketeerStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
