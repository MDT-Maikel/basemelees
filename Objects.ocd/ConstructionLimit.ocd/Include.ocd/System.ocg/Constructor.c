// Necessary modifications to the constructor for implementing a construction limit.

#appendto Library_Constructor

public func GetConstructionPlans(int plr)
{
	var construction_plans = inherited(plr);
	// If the rule exists request for all structures whether the player may still build them.
	var rule = FindObject(Find_ID(Rule_ConstructionLimit));
	if (rule)
	{
		for (var index = GetLength(construction_plans) - 1; index >= 0; index--)
		{
			if (rule->ConstructionLimitReached(construction_plans[index], plr))
				RemoveArrayIndex(construction_plans, index);
		}
	}
	return construction_plans;
}