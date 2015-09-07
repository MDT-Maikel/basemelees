/**
	Construction Limit
	Limits how many times a certain structure can be constructed by a player
	or its team.

	@author Maikel	
*/


protected func Initialize()
{
	// Only one rule of this type.
	if (ObjectCount(Find_ID(Rule_ConstructionLimit)) > 1)
		return RemoveObject();
	return;
}

public func AddConstructionLimit(id def, int limit, bool for_teams)
{
	var effect = AddEffect("ConstructionLimit", this, 100, 0, this);
	effect.def = def;
	effect.limit = limit;
	effect.for_teams = for_teams;
	effect.conlist_plr = [];
	effect.conlist_team = [];
	return;
}

public func FxConstructionLimitStart(object target, proplist effect, bool temp)
{
	if (temp)
		return FX_OK;
	return FX_OK;
}

public func GetConstructionLimit(id def, int for_plr)
{
	var for_team = GetPlayerTeam(for_plr);
	var best_limit = 2**10;	
	for (var index = 0; index < GetEffectCount("", this); index++)
	{
		var effect = GetEffect("ConstructionLimit", this, index);
		if (effect.def == def)
		{
			var limit;
			if (effect.for_teams)
				limit = effect.limit - effect.conlist_team[for_team];
			else
				limit = effect.limit - effect.conlist_plr[for_plr];
			
			if (limit < best_limit)
				best_limit = limit;
		}
	}
	return best_limit;
}

public func ConstructionLimitReached(id def, int for_plr)
{
	return  GetConstructionLimit(def, for_plr) <= 0;
}

public func RegisterConstruction(id def, int by_plr)
{
	var by_team = GetPlayerTeam(by_plr);
	for (var index = 0; index < GetEffectCount("", this); index++)
	{
		var effect = GetEffect("ConstructionLimit", this, index);
		if (effect.def == def)
		{
			if (effect.for_teams)
				effect.conlist_team[by_team]++;
			else
				effect.conlist_plr[by_plr]++;
		}
	}
	return;
}

public func UnregisterConstruction(id def, int by_plr)
{
	var by_team = GetPlayerTeam(by_plr);
	for (var index = 0; index < GetEffectCount("", this); index++)
	{
		var effect = GetEffect("ConstructionLimit", this, index);
		if (effect.def == def)
		{
			if (effect.for_teams)
				effect.conlist_team[by_team]--;
			else
				effect.conlist_plr[by_plr]--;
		}
	}
	return;
}


/*-- Rule Menu --*/

protected func Activate(int by_plr)
{
	MessageWindow(Description, by_plr);
	return true;
}


/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";
