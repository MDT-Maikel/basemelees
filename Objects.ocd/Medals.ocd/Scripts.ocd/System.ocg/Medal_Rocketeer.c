// Appendto script for the rocketeer medal. The medal is awarded if a clonk dies cause of the boompack explosion.

#appendto Boompack


// The explode call happens in this function.
public func DoFireworks()
{
	// Notify the effect that keeps track of the rocketeer that the boompack will explode now.
	var effect = GetEffect("IntMedalRocketeer", nil);
	if (effect)
		EffectCall(nil, effect, "OnExplosion", this);
	// Make the boompack explode normally.
	return _inherited(...);
}