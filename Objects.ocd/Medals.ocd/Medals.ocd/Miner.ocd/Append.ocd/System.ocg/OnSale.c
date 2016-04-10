// Appendto script for the marvelous miner. Tracks the mining of gold and gems.

#appendto Nugget
#appendto GoldBar
#appendto Ruby
#appendto Amethyst


// Callback from the vendor library, is done when an item has been sold.
// This also allows to keep track of who brought in the valuable resource.
public func OnSale(int controller, object at_base)
{
	// Get value of the resource.
	var value = at_base->GetSellValue(this);
	// Notify the effect that keeps track of the mining that a valuable has been sold.
	var effect = GetEffect("IntMedalMiner", nil);
	if (effect)
		EffectCall(nil, effect, "HasMined", controller, this->GetID(), value);
	// Also perform the callback in other scripts.
	return _inherited(controller, at_base, ...);
}