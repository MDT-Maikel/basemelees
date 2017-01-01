/** 
	Cannon Library
	Is used in conjuction with the cannon frame library for the different canon types.
	A cannon should be grabable such that it can be moved to the frame and then be 
	attached. The following callbacks are done in the cannon:
	 * OnCannonMount(object onto_frame)
	 * OnCannonDismount(object from_frame)
	 * QueryRejectCannonMount(object onto_frame)
	 * QueryRejectCannonDismount(object from_frame)

	@author Maikel
*/


// All cannon related local variables are stored in a single proplist.
// This reduces the chances of clashing local variables. See 
// Construction for which variables are being used.
local lib_cannon;


public func Construction()
{
	// Initialize the single proplist for the cannon library.
	if (lib_cannon == nil)
		lib_cannon = {};
	lib_cannon.frame = nil;
	return _inherited(...);
}


/*-- Mounting --*/

// Mount this cannon onto the given frame, returns whether successful.
public func MountCannon(object onto_frame)
{
	if (QueryRejectCannonMount(onto_frame) || onto_frame->QueryRejectCannonMount(this))
		return false;
	Enter(onto_frame);
	// Mount callbacks.
	OnCannonMount(onto_frame);
	onto_frame->OnCannonMount(this);
	return true;
}

// Dismount this cannon from the given frame, returns whether successful.
public func DismountCannon(object from_frame)
{
	if (QueryRejectCannonDismount(from_frame) || from_frame->QueryRejectCannonDismount(this))
		return false;
	Exit(0, from_frame->GetBottom() - GetBottom());
	// Dismount callbacks.
	OnCannonDismount(from_frame);
	from_frame->OnCannonDismount(this);
	return false;
}

public func OnCannonMount(object onto_frame)
{
	lib_cannon.frame = onto_frame;
	return;
}

public func OnCannonDismount(object from_frame)
{
	lib_cannon.frame = nil;
	return;
}

public func QueryRejectCannonMount(object onto_frame)
{
	// Cannot mount if already on a frame.
	return !!lib_cannon.frame;
}

public func QueryRejectCannonDismount(object from_frame)
{
	return false;
}

public func GetCannonFrame() { return lib_cannon.frame; }


/*-- Interaction --*/

public func IsInteractable(object clonk)
{
	var frame = FindObject(Find_AtPoint(), Find_NoContainer(), Find_Func("IsCannonFrame"));
	if (!frame)
		return false;
	if (QueryRejectCannonMount(frame) || frame->QueryRejectCannonMount(this))
		return false;
	return true;
}

public func GetInteractionMetaInfo(object clonk)
{
	return { Description = Format("$MsgMountCannon$", this->GetName()) };
}

public func Interact(object clonk)
{
	var frame = FindObject(Find_AtPoint(), Find_Func("IsCannonFrame"));
	if (!frame)
		return false;
	if (QueryRejectCannonMount(frame) || frame->QueryRejectCannonMount(this))
		return false;
	MountCannon(frame);
	return true;
}


/*-- Controls -- */

public func CannonUseStart(object frame, object clonk, int x, int y) { return true; }
public func CannonUseHolding(object frame, object clonk, int x, int y) { return true; }
public func CannonUseStop(object frame, object clonk, int x, int y) { return true; }
public func CannonUseCancel(object frame) { return true; }

// Turns aiming coordinates into the cannon's angle and ranges from 0 to 360 degrees in clockwise direction.
public func CoordinatesToAngle(object frame, int x, int y)
{
	return frame->CoordinatesToAngle(x, y);
}


/*-- Contents --*/

public func IsContainer() { return true; }


/*-- Properties --*/

public func IsCannon() { return true; }

// All cannons must be pushable.
local Touchable = 1;
