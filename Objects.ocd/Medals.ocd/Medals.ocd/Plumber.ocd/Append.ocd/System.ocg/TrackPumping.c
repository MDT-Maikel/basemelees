// Appendto script for the plumber medal. Tracks the pumping of liquids.

// This function is called by the pump on the source pipe to get the liquid.
global func ExtractLiquidAmount(int x, int y, int amount, bool distant_first)
{
	var result = _inherited(x, y, amount, distant_first, ...);
	if (!this || GetType(this) != C4V_C4Object)
		return result;
	// Only do something if this is a pipe and there was some liquid extracted.
	if (result && result[1] > 0 && this->GetID() == Pipe)
	{
		// Find line and check whether the pipe was connected to a pump.
		var line = FindObject(Find_Func("IsConnectedTo", this));
		if (line)
		{
			var pump = line->GetConnectedObject(this);
			if (pump && pump->GetID() == Pump)
			{
				// Notify the effect that keeps track of the pumping that an amount has been pumped.
				// Take the owner of the pump as the player who pumped the actual liquid.
				var effect = GetEffect("IntMedalPlumber", nil);
				if (effect)
					EffectCall(nil, effect, "OnLiquidPumped", pump->GetOwner(), result[1]);
			}	
		}
	}
	return result;
}