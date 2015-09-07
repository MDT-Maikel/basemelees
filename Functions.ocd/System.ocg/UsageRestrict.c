// Restricts the usage of the included items to be only within the range of the base.
// This restriction is valid for all base melees for a consistent setting for the player.


// Limit the use of the wall kit, since it is too powerful. 
#appendto WallKit


// Overload ControlUse and just return true if use is restricted.
public func ControlUse(object clonk, int x, int y)
{
	if (!ControlUseCheckRestriction(clonk, x, y))
	{
		clonk->Message("$MsgUsageRestricted$", this.Name);
		return true;
	}
	return _inherited(clonk, x, y, ...);
}

// Overload ControlUseStart and just return true if use is restricted.
public func ControlUseStart(object clonk, int x, int y)
{
	if (!ControlUseCheckRestriction(clonk, x, y))
	{
		clonk->Message("$MsgUsageRestricted$", this.Name);
		return true;
	}
	return _inherited(clonk, x, y, ...);
}

// Overload ControlUseHolding and cancel the use when restricted.
// This removes a use that has been started within the base area.
public func ControlUseHolding(object clonk, int x, int y)
{
	if (!ControlUseCheckRestriction(clonk, x, y))
	{
		clonk->Message("$MsgUsageRestricted$", this.Name);
		clonk->CancelUse();
		return true;	
	}
	return _inherited(clonk, x, y, ...);
}

// Overload ControlUseStop and cancel the use when restricted.
// This removes a use that has been started within the base area.
public func ControlUseStop(object clonk, int x, int y)
{
	if (!ControlUseCheckRestriction(clonk, x, y))
	{
		clonk->Message("$MsgUsageRestricted$", this.Name);
		clonk->CancelUse();
		return true;	
	}
 	return _inherited(clonk, x, y, ...);
}

// Overload ControlUseCancel, however, just let it do its normal
// behaviour, since use started inside the base area must be
// cancelled outside the base area.
public func ControlUseCancel(object clonk, int x, int y)
{
	return _inherited(clonk, x, y, ...);
}

// Returns whether the object may be used.
public func ControlUseCheckRestriction(object clonk, int x, int y)
{
	var for_plr = clonk->GetController();
	var flag = clonk->GetFlagpoleForPosition();
	if (!flag)
		return false;
	var flag_owner = flag->GetOwner();
	return for_plr == flag_owner || GetPlayerTeam(for_plr) == GetPlayerTeam(flag_owner);
}
