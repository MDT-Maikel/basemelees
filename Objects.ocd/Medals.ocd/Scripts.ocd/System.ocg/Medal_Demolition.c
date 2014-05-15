// Appendto script for the demolition medal. Tracks the destruction of structures.

#appendto Library_Structure


// Callback when damage has been done to this object.
public func Damage(int change, int cause, int cause_plr)
{
	// Only do stuff if the object has the HitPoints property.
	if (this && this.HitPoints != nil)
	{
		// Structure will be destroyed if damage > hit points.
		if (GetDamage() >= this.HitPoints)
		{		
			// Notify the effect that keeps track of the demolition that a structure has been destroyed.
			var effect = GetEffect("IntMedalDemolition", nil);
			if (effect)
				EffectCall(nil, effect, "HasDestroyed", cause_plr, this->GetID());
		}
	}
	return _inherited(change, cause, cause_plr, ...);
}
