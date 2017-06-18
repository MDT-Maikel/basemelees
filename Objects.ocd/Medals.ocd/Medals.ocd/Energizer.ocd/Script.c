/**
	Energizer
	Awarded for producing and consuming a certain amount of energy.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 17; }
public func GetMedalReward() { return 20; }


/*-- Medal Settings --*/

// This is the amount of power that needs to be consumed to achieve one medal.
// Equal to x minutes of consuming 100 units of power.
private func GetPowerGoal()
{
	var consume_for_minutes = 10;
	return 36 * 60 * 100 * consume_for_minutes;
}


/*-- Medal Scripts --*/

// See TrackPumping.c script for handling the pumping of liquids.
// This medal adds an effect which keeps track of the players' progress.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	Global->CreateEffect(MedalEnergizerEffect, 100, 0);
	return;
}

// Called when the round is finished, i.e. when all players are eliminated
// or all goals are fulfilled.
public func OnRoundFinish()
{
	var fx = GetEffect(MedalEnergizerEffect->GetName());
	if (fx)
		fx->Remove();
	return;
}

// Effect to keep track of medal awarding.
local MedalEnergizerEffect = new Effect
{
	Construction = func()
	{
		this.consumed_amount = [];
		this.required_amount = Medal_Energizer->GetPowerGoal();	
	},
	OnPowerConsumed = func(int by_plr, int amount_consumed)
	{
		// Increase the amount of power consumed for the player.
		var plr_id = GetPlayerID(by_plr);
		if (this.consumed_amount[plr_id] == nil)
			this.consumed_amount[plr_id] = 0;
		this.consumed_amount[plr_id] += amount_consumed;
		// Check whether the consumed amount exceeds the required amount, if so notify rule.
		if (this.consumed_amount[plr_id] >= this.required_amount)
		{
			Rule_Medals->~AwardMedal(Medal_Energizer, by_plr);
			// Reset the consumed amount so that a player can reach the medal again.
			this.consumed_amount[plr_id] = 0; 
		}
	}
};