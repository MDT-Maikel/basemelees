/**
	Marvelous Miner 
	Awarded for mining at least one thousand clunkers during a single round.
	Resources for mining that count are: gold, gold bar, ruby and amethyst.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 2; }
public func GetMedalReward() { return 50; }


/*-- Medal Scripts --*/

// See OnSale.c script for handling the selling of the resources.
// This medal adds an effect which keeps track of the players' progress.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalMiner", nil, 100, 0, nil, Medal_Miner);
	return;
}

// Called when the round is finished, i.e. when all players are elimanted
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalMiner", nil);
	return;
}


/*-- Medal Effects --*/

// Medal miner effect keeps track of which players have gained wealth.
public func FxIntMedalMinerStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Create a variable which keeps track of the mined wealth per player.
	effect.mined_wealth = [];
	return FX_OK;
}

// Effect callback made by a sold resource.
public func FxIntMedalMinerHasMined(object target, proplist effect, int plr, id resource, int value)
{
	// Don't do anything if the player does not exist.
	if (plr == NO_OWNER)
		return FX_OK;	
	// Increase the mined wealth for the player.
	var plr_id = GetPlayerID(plr);
	if (effect.mined_wealth[plr_id] == nil)
		effect.mined_wealth[plr_id] = 0;
	effect.mined_wealth[plr_id] += value;
	// Check whether the wealth exceeds a thousand clunkers, if so notify rule.
	if (effect.mined_wealth[plr_id] >= 1000)
	{
		Rule_Medals->~AwardMedal(Medal_Miner, plr);
		// Reset the mined wealth so that a player can reach the medal again.
		effect.mined_wealth[plr_id] = 0; 
	}	
	return FX_OK;
}

// Medal miner effect keeps track of which players have gained wealth.
public func FxIntMedalMinerStop(object target, proplist effect, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
