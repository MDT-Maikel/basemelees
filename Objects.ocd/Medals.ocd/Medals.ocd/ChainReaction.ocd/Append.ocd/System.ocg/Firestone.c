// Appendto script for the chain reaction. Tracks firestone explosions.

#appendto Firestone


protected func Construction(object constructor)
{
	var owner = GetOwner();
	var controller = GetController();
	// Only notify the chain reaction effect if the controller is a player and the owner is none.
	// This is the case for a firestone created from an explosion.
	if (owner != NO_OWNER || controller == NO_OWNER)
		return _inherited(constructor, ...);
	// Notify the effect that keeps track of the chain reaction that a firestone has been created.
	var effect = GetEffect("IntMedalChainReaction", nil);
	if (effect)
		EffectCall(nil, effect, "OnCreation", controller, this);	
	return _inherited(constructor, ...);
}

protected func Destruction()
{
	var owner = GetOwner();
	var controller = GetController();	
	// Only notify the chain reaction effect if the controller is a player and the owner is none.
	// This is the case for a firestone created from an explosion, which has not been touched in between.
	if (owner != NO_OWNER || controller == NO_OWNER)
		return _inherited(...);
	// Notify the effect that keeps track of the chain reaction that a firestone has been destroyed.
	var effect = GetEffect("IntMedalChainReaction", nil);
	if (effect)
		EffectCall(nil, effect, "OnDestruction", controller, this);
	return _inherited(...);
}