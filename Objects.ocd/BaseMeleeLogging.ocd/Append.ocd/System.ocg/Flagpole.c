// Notify base melee logger about flagpole destruction.

#appendto Flagpole

// Overloads the destroying feature from the structure library.
public func Damage(int change, int cause, int cause_plr)
{
	// Only do stuff if the object has the HitPoints property.
	if (this && this.HitPoints != nil)
		if (GetDamage() >= this.HitPoints)	
			GameCallEx("OnFlagpoleDestruction", GetOwner(), cause_plr);
	return _inherited(change, cause, cause_plr, ...);
}