// Appendto script for the superman medal. Tracks the action of the clonk to determine whether it stays airborne.

#appendto Clonk

// Callback from the engine when the clonk's action changes.
protected func OnActionChanged(string old_action)
{
	// Don't do anything if there is no superman medal effect.
	var effect = GetEffect("IntMedalSuperman", nil);
	if (!effect)
		return _inherited(old_action, ...);	
	// Get the new action and set the flight duration to zero.
	var new_action = GetAction();
	var flight_duration = 0;
	// If the new action is mid-air reset the flight start counter.
	if (new_action == "Jump" || new_action == "WallJump" || new_action == "Tumble" || new_action == "HangOnto")
	{
		if (this.medal_flight_start == nil)
			this.medal_flight_start = FrameCounter();
		else
			flight_duration = FrameCounter() - this.medal_flight_start;
	}
	// If the action involves contact with the landscape reset the start counter and check 
	else
	{
		if (this.medal_flight_start != nil)
			flight_duration = FrameCounter() - this.medal_flight_start;
		// Reset the flight start time.
		this.medal_flight_start = nil;
	}	
	// Award the medal if the flight duration was more than the goal.
	if (flight_duration > Medal_Superman->GetFlightDurationGoal())
	{
		EffectCall(nil, effect, "WasSuperman", GetOwner());
		// Reset the flight start time.
		this.medal_flight_start = nil;
	}
	return _inherited(old_action, ...);
}