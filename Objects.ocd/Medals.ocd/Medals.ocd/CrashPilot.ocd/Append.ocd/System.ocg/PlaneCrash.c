// Appendto script for the crash pilot medal. The medal is awarded if a clonk survives its self-caused plane crash.

#appendto Airplane


// This function is called an object exits the plane.
private func Ejection(object content)
{
	// Check if the ejected content is a crew member and therefore is the pilot.
	if (content->GetOCF() & OCF_CrewMember)
	{
		// Notify the effect that keeps track of the crash pilot medal that a plane has crashed.
		var effect = GetEffect("IntMedalCrashPilot", nil);
		if (effect)
			EffectCall(nil, effect, "OnEjection", this, content, GetController());
	}
	return _inherited(content, ...);
}

// This function is called upon plane crash to eject the clonk and explode the plane.
private func PlaneDeath()
{
	// Check if there is a pilot in this plane and find it.
	var pilot = FindObject(Find_Container(this), Find_OCF(OCF_CrewMember));	
	// Notify the effect that keeps track of the crash pilot medal that a plane has crashed.
	var effect = GetEffect("IntMedalCrashPilot", nil);
	if (effect)
		EffectCall(nil, effect, "OnCrash", this, pilot, GetController());
	// Forward to the normal plane behavior.
	return _inherited(...);
}