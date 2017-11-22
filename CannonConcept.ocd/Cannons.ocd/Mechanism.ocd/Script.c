/**
	Cannon Mechanism
	Allows the player to automate certain cannon actions.
	
	@author Maikel
*/


public func Hit(x, y)
{
	StonyObjectHit(x, y);
	return true;
}

public func IsArmoryProduct() { return true; }

public func GetSubstituteComponent(id component)
{
	// Can be made from diamond, ruby or amethyst.
	if (component == Diamond)
		return [Ruby, Amethyst];
}


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;
local Components = {Metal = 4, Coal = 2, Diamond = 1};
