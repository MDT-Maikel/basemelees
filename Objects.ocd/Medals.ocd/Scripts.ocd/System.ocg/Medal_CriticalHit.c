// Appendto script for the critical hit medal. Tracks energy change in crew memebers.

#appendto Clonk

// Callback from the engine when the energy of a living object changes.
protected func OnEnergyChange(int change, int cause, int by_plr)
{
	//Log("%s energy: change %d, cause %d, by player %d", GetName(), change, cause, by_plr);
	// Forward energy change to the critical hit effect.
	var damage = -change / 1000;
	var effect = GetEffect("IntMedalCriticalHit", nil);
	if (effect)
		EffectCall(nil, effect, "OnCrewDamage", damage, GetOwner(), by_plr);
	return _inherited(change, cause, by_plr, ...);
}