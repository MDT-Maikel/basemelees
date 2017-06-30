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

/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;
local Components = {Metal = 2, Coal = 1};
