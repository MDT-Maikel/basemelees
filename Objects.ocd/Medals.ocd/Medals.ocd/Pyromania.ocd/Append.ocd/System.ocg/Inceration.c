// Appendto script for the pyromania. Tracks the incineration of objects.

// Overload the global Incinerate function and forward incineration to the medal effect.
global func Incinerate(strength, int caused_by)
{
	// If there is no object, or the object is already on fire don't do anything.
	if (!this || GetEffect("Fire", this))
		return _inherited(strength, caused_by, ...);
	
	// Notify the effect that keeps track of the pyromania that an object has been incinerated.
	var effect = GetEffect("IntMedalPyromania", nil);
	if (effect && caused_by != NO_OWNER)
		EffectCall(nil, effect, "OnIncineration", caused_by);	
		
	// Return back to the overloaded function.
	return _inherited(strength, caused_by, ...);
}