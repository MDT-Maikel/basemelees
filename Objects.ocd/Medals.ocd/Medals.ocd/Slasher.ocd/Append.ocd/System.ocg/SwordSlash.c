// Appendto script for the slasher medal. Tracks sword strikes by crew members.

#appendto Library_MeleeWeapon

public func ApplyWeaponBash(object to, int strength, int angle, object from)
{
	var real_from = from ?? this;
	var striker = real_from->Contained();
	// If this is a sword let slasher medal effect know a sword slash has happened.
	if (GetID() == Sword && striker && GetEffect("VisualJumpStrike", striker))
	{
		var effect = GetEffect("IntMedalSlasher", nil);
		if (effect)
			EffectCall(nil, effect, "OnSwordSlash", striker, to);
	}
	// Apply the normal weapon bash.
	return _inherited(to, strength, angle, from, ...);
}