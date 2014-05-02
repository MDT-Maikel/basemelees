/**
	Kill
	Awarded when you have killed an enemy.

	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 1; }


/*-- Medal Scripts --*/

// Called when a crew member dies.
public func OnCrewDeath(object crew, int killed_by)
{
	// Award a medal if the killer is not the owner of the crew.
	if (killed_by != NO_OWNER)
		if (Hostile(crew->GetOwner(), killed_by))
			Rule_Medals->~AwardMedal(Medal_Kill, killed_by);
	return;
} 