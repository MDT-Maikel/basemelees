// Appendto script for the slasher medal. Tracks sword strikes by crew members.

#appendto Sword

public func ApplyWeaponBash(object to, int strength, int angle, object from)
{
	var real_from = from ?? this;
	var striker = real_from->Contained();
	// Let slasher medal effect know a sword slash has happened.
	if (striker && GetEffect("VisualJumpStrike", striker))
	{
		var effect = GetEffect("IntMedalSlasher", nil);
		if (effect)
			EffectCall(nil, effect, "OnSwordSlash", striker, to);
	}
	// Apply the normal weapon bash.
	return _inherited(to, strength, angle, from, ...);
}