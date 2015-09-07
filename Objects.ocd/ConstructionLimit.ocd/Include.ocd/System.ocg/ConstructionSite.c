// Necessary modifications to the construction site for implementing a construction limit.

#appendto ConstructionSite

public func Set(id def, int dir, object stick)
{
	// Notify the rule that a construction site has been placed.
	var by_plr = GetOwner();
	var rule = FindObject(Find_ID(Rule_ConstructionLimit));
	rule->RegisterConstruction(def, by_plr);
	return _inherited(def, dir, stick, ...);
}

protected func Destruction()
{
	// Notify the rule that a construction site has been removed.
	var by_plr = GetOwner();
	var rule = FindObject(Find_ID(Rule_ConstructionLimit));
	rule->UnregisterConstruction(definition, by_plr);
	return _inherited(...);
}