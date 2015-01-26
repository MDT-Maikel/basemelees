// Appendto script for the marvelous miner. Tracks the mining of gold and gems.

#appendto Nugget
#appendto GoldBar
#appendto Ruby
#appendto Amethyst


// Callback from the seller library, is done just before selling an item.
// This also allows to keep track of who brought in the valuable resource.
public func QueryOnSell(int controller)
{
	// First evaluate _inherited(controller, ...) to find if an actual sale is done.
	var no_sell = _inherited(controller, ...);
	if (no_sell)
		return true;
	// Get value of the resource.
	var value = GetValue();
	// Notify the effect that keeps track of the mining that a valuable has been sold.
	var effect = GetEffect("IntMedalMiner", nil);
	if (effect)
		EffectCall(nil, effect, "HasMined", controller, this->GetID(), value);
	// Allow the selling.
	return false;
}