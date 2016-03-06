/**
	Plumber
	Awarded for pumping a certain amount of liquid.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 12; }
public func GetMedalReward() { return 20; }


/*-- Medal Scripts --*/

// See TrackPumping.c script for handling the pumping of liquids.
// This medal adds an effect which keeps track of the players' progress.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalPlumber", nil, 100, 0, nil, Medal_Plumber);
	return;
}

// Called when the round is finished, i.e. when all players are eliminated
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalPlumber", nil);
	return;
}

// Medal plumber effect keeps track of which players have pumped what amount.
public func FxIntMedalPlumberStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Create a variable which keeps track of the amount pumped per player.
	effect.pumped_amount = [];
	// The required amount is operating a single pump at full capacity for 5 minutes.
	effect.required_amount = Pump->GetPumpSpeed() * 36 * 5 * 60 / 30;
	return FX_OK;
}

// Effect callback made when a player has pumped a certain amount of pixels.
public func FxIntMedalPlumberOnLiquidPumped(object target, proplist effect, int plr, int amount_pumped)
{
	// Increase the amount pumped for the player.
	var plr_id = GetPlayerID(plr);
	if (effect.pumped_amount[plr_id] == nil)
		effect.pumped_amount[plr_id] = 0;
	effect.pumped_amount[plr_id] += amount_pumped;
	// Check whether the pumped amount exceeds the required amount, if so notify rule.
	if (effect.pumped_amount[plr_id] >= effect.required_amount)
	{
		Rule_Medals->~AwardMedal(Medal_Plumber, plr);
		// Reset the pumped amount so that a player can reach the medal again.
		effect.pumped_amount[plr_id] = 0; 
	}
	return FX_OK;
}

// Medal plumber effect keeps track of which players have pumped what amount.
public func FxIntMedalPlumberStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}