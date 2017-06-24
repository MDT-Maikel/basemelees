/** 
	Cannon Frame Library
	Cannons are mounted on the frame, must be included by the cannon frame, cannon tower, airship, etc.
	All of the cannons can rotate and the frame should indicate the range of rotation.
	The following callbacks are done in the cannon frame for mounting a cannon:
	 * OnCannonMount(object cannon)
	 * OnCannonDismount(object cannon)
	 * QueryRejectCannonMount(object cannon)
	 * QueryRejectCannonDismount(object cannon)
	The graphical representation of the cannon can be adjusted in the functions:
	 * AttachCannonGraphics(object cannon)
	 * UpdateCannonGrapics(object cannon, int aim_angle)
	 * DetachCannonGraphics(object cannon)

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
	lib_cannon.cannon = nil;
	return _inherited(...);
}


/*-- Mounting --*/

public func OnCannonMount(object cannon)
{
	lib_cannon.cannon = cannon;
	AttachCannonGraphics(cannon);
	return;
}

public func OnCannonDismount(object cannon)
{
	lib_cannon.cannon = nil;
	DetachCannonGraphics(cannon);
	return;
}

public func QueryRejectCannonMount(object cannon)
{
	// Cannot accept a cannon if already one is mounted.
	return !!lib_cannon.cannon;
}

public func QueryRejectCannonDismount(object cannon)
{
	return false;
}

public func GetCannon() { return lib_cannon.cannon; }


/*-- Controls --*/

public func HoldingEnabled() { return true; }

public func ControlUseStart(object clonk, int x, int y)
{
	// Don't do anything if no cannon is mounted.
	if (!lib_cannon.cannon)
	{
		PlayerMessage(clonk->GetOwner(),"$MsgNoCannonMounted$");
		clonk->~PlaySoundDecline();
		clonk->CancelUse();
		return true;
	}
	var angle = CoordinatesToAngle(x, y);
	UpdateCannonGrapics(lib_cannon.cannon, angle);
	return lib_cannon.cannon->CannonUseStart(this, clonk, x, y);
}

public func ControlUseHolding(object clonk, int x, int y)
{
	var angle = CoordinatesToAngle(x, y);
	if (lib_cannon.cannon)
	{
		UpdateCannonGrapics(lib_cannon.cannon, angle);
		return lib_cannon.cannon->CannonUseHolding(this, clonk, x, y);
	}
	return true;
}

public func ControlUseStop(object clonk, int x, int y)
{
	var angle = CoordinatesToAngle(x, y);
	if (lib_cannon.cannon)
	{
		UpdateCannonGrapics(lib_cannon.cannon, angle);
		return lib_cannon.cannon->CannonUseStop(this, clonk, x, y);
	}
	return true;
}

public func ControlUseCancel()
{
	if (lib_cannon.cannon)
		return lib_cannon.cannon->CannonUseCancel(this);
	return true;
}

// Turns aiming coordinates into the cannon's angle and ranges from 0 to 360 degrees in clockwise direction.
public func CoordinatesToAngle(int x, int y)
{
	var angle = Angle(0, 0, x, y, this.AimingAnglePrecision);
	if (this.AimingAngleRange != nil)
	{
		var min = this.AimingAngleRange[0] * this.AimingAnglePrecision;
		var max = this.AimingAngleRange[1] * this.AimingAnglePrecision;
		if (min > max)
			min -= 360 * this.AimingAnglePrecision;
		var med = (min + max) / 2;
		var del = (max - min) / 2;
		angle = Normalize(angle, med - 180 * this.AimingAnglePrecision, this.AimingAnglePrecision) - med;
		angle = BoundBy(angle, -del, +del);
	}
	return angle + med;
}


/*-- Graphics --*/

private func AttachCannonGraphics(object cannon)
{
	return;
}

private func UpdateCannonGrapics(object cannon, int aim_angle)
{
	return;
}

private func DetachCannonGraphics(object cannon)
{
	return;
}


/*-- Contents --*/

public func IsContainer() { return true; }

public func Destruction()
{
	// Remove cannon from frame before destruction happens.
	if (lib_cannon.cannon)
		lib_cannon.cannon->DismountCannon(this);
	return;
}


/*-- Interaction Menu --*/

public func HasInteractionMenu() { return true; }

public func GetInteractionMenus(object clonk)
{
	var menus = _inherited(clonk, ...) ?? [];		
	var mount_menu =
	{
		title = "$MsgCannonMounting$",
		entries_callback = this.GetCannonMountingMenuEntries,
		callback = "OnCannonMountingClick",
		callback_hover = "OnCannonMountingHover",
		callback_target = this,
		BackgroundColor = RGB(0, 50, 50),
		Priority = 20
	};
	PushBack(menus, mount_menu);
	return menus;
}

public func GetCannonMountingMenuEntries(object clonk)
{
	var menu_entries = [];
	// Add entry to dismount cannon.
	if (lib_cannon.cannon)
	{
		PushBack(menu_entries, 
			{
				symbol = lib_cannon.cannon, extra_data = "dismount", 
				custom =
				{
					Bottom = "2em",
					BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
					Priority = 1,
					image = {Right = "2em", Symbol = lib_cannon.cannon},
					text = {Left = "2em", Style = GUI_TextVCenter, Text = Format("$MsgDismount$", lib_cannon.cannon->GetName())}
				}
			}
		);
	}
	// Add entry to mount cannons: take those which are close to the frame.
	var sh = GetShape();
	for (var cannon in FindObjects(Find_AtRect(sh[0], sh[1], sh[2], sh[3]), Find_NoContainer(), Find_Func("IsCannon")))
	{
		if (!QueryRejectCannonMount(cannon) && !cannon->QueryRejectCannonMount(this))
		{
			PushBack(menu_entries, 
				{
					symbol = cannon, extra_data = "mount", 
					custom =
					{
						Bottom = "2em",
						BackgroundColor = {Std = 0, OnHover = 0x50ff0000},
						Priority = 2,
						image = {Right = "2em", Symbol = cannon},
						text = {Left = "2em", Style = GUI_TextVCenter, Text = Format("$MsgMount$", cannon->GetName())}
					}
				}
			);
		}
	}
	return menu_entries;
}

public func OnCannonMountingHover(id cannon, string action, desc_menu_target, menu_id)
{
	var text = "";
	if (action == "dismount")
		text = Format("$MsgDismount$", cannon->GetName());
	else if (action == "mount")
		text = Format("$MsgMount$", cannon->GetName());
	return GuiUpdateText(text, menu_id, 1, desc_menu_target);
}

public func OnCannonMountingClick(object cannon, string action, bool alt)
{
	if (action == "dismount" && cannon == lib_cannon.cannon)
	{
		lib_cannon.cannon->DismountCannon(this);
	}
	else if (action == "mount")
	{
		cannon->MountCannon(this);
	}
	return UpdateInteractionMenus(this.GetCannonMountingMenuEntries);
}


/*-- Properties --*/

public func IsCannonFrame() { return true; }

// Determines the precision of the cannon angle calculations.
local AimingAnglePrecision = 1000;
// Determines the range of the aiming angle in range from 0 to 360 as [min, max].
local AimingAngleRange = nil;
