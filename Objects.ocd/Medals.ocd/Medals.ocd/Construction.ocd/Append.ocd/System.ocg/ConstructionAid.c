// Appendto script for the construction medal. Tracks the adding of resources to construction sites.

#appendto ConstructionSite


// Store the materials added by the single players.
local plr_added_mats;

public func Construction()
{
	// Set materials added to zero.
	plr_added_mats = [];
	return _inherited(...);
}

public func Collection2(object obj)
{
	if (!obj)
		return _inherited(obj, ...);
	// Increase materials added.
	var plr_id = GetPlayerID(obj->GetController());
	plr_added_mats[plr_id]++;
	return _inherited(obj, ...);
}

public func Ejection(object obj)
{
	if (!obj)
		return _inherited(obj, ...);
	// Decrease materials added.
	var plr_id = GetPlayerID(obj->GetController());
	plr_added_mats[plr_id]--;
	return _inherited(obj, ...);
}

private func StartConstructing()
{
	if (!definition)
		return _inherited(...);
	if (!full_material)
		return _inherited(...);
	// Notify the effect that keeps track of the construction that a structure has been constructed.
	var effect = GetEffect("IntMedalConstruction", nil);
	if (effect)
	{
		// Do it for all players who have contributed.
		for (var plr_id = 0; plr_id < GetLength(plr_added_mats); plr_id++)
			if (plr_added_mats[plr_id] > 0)
				EffectCall(nil, effect, "HasAided", GetPlayerByID(plr_id), this->GetID());
	}
	return _inherited(...);
}