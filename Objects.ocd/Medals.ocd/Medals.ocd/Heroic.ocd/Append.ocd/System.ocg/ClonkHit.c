// Appendto script for the heroic medal. Tracks energy change in crew memebers.

#appendto Clonk

// Callback from the engine when the energy of a living object changes.
protected func OnEnergyChange(int change, int cause, int by_plr)
{
	// Don't do anything if there is no heroic medal effect or crew member is not alive.
	var effect = GetEffect("IntMedalHeroic", nil);
	if (!effect || !GetAlive())
		return _inherited(change, cause, by_plr, ...);
	
	// Calculate the damage done from the energy change.
	var damage = -change / 1000;
	// Only look at damage, not energy gain.
	if (damage <= 0)
		return _inherited(change, cause, by_plr, ...);
	
	// Add the damage to a local property tracking the damage.
	if (this.medal_heroic_damage == nil)
		this.medal_heroic_damage = damage;
	else
		this.medal_heroic_damage += damage;

	// If the damage dealt is more than the required amount notify the effect to award the medal.
	if (this.medal_heroic_damage >= 250)
	{
		EffectCall(nil, effect, "WasHeroic", GetOwner());
		// Set damage property to zero so that the medal can be awarded again.
		this.medal_heroic_damage = 0;
	}
	return _inherited(change, cause, by_plr, ...);
}