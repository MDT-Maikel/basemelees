// Appendto script for the berserker medal. Tracks energy change in crew memebers.

#appendto Clonk

// Callback from the engine when the energy of a living object changes.
protected func OnEnergyChange(int change, int cause, int by_plr)
{
	// Don't do anything if there is no berserker medal effect or crew member is not alive.
	var effect = GetEffect("IntMedalBerserker", nil);
	if (!effect || !GetAlive())
		return _inherited(change, cause, by_plr, ...);
	// Calculate the damage done from the energy change.
	var damage = -change / 1000;
	// Only look at damage, not energy gain.
	if (damage <= 0)
		return _inherited(change, cause, by_plr, ...);
	// Pass the damage dealt to the berserker medal effect.
	EffectCall(nil, effect, "OnDamageDealt", damage, by_plr, GetOwner());
	return _inherited(change, cause, by_plr, ...);
}