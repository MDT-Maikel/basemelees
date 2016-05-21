// Appendto script for the energizer medal. Tracks the consumption of power.

#appendto Library_PowerConsumer


// Callback from the power library saying there is enough power.
public func OnEnoughPower(int amount)
{
	// Add tracking effect.
	if (!this->~IsPowerStorage())
		if (!GetEffect(PowerConsumptionTrackingEffect->GetName(), this))
			CreateEffect(PowerConsumptionTrackingEffect, 100, 80, amount);
	return _inherited(amount, ...);
}

// Callback from the power library saying there is not enough power.
public func OnNotEnoughPower(int amount)
{
	// Remove tracking effect.
	if (!this->~IsPowerStorage())
	{
		var fx = GetEffect(PowerConsumptionTrackingEffect->GetName(), this);
		if (fx)
			fx->Remove();
	}
	return _inherited(amount, ...);
}

local PowerConsumptionTrackingEffect = new Effect
{
	Construction = func(int amount)
	{
		this.consumption_amount = amount;	
	},	
	Timer = func(int time)
	{
		var fx = GetEffect("MedalEnergizerEffect");
		if (fx)
			fx->OnPowerConsumed(Target->GetOwner(), this.Interval * this.consumption_amount);
		return FX_OK;
	},
	Destruction = func()
	{
		var fx = GetEffect("MedalEnergizerEffect");
		if (fx)
			fx->OnPowerConsumed(Target->GetOwner(), (this.Time % this.Interval) * this.consumption_amount);
		return FX_OK;
	}
};