/**
	Base Respawn 
	Respawn at the nearest base if sufficient clonks and clunkers are available.
	TODO: make more general for all kinds of available crew members.
	
	@author Maikel
*/


// Determines whether the inventory of the crew member is transfered upon respawn.
local inventory_transfer;

// Determines whether a crew member needs to be bought.
local free_crew;

protected func Initialize()
{
	inventory_transfer = false;
	free_crew = false;
	return;
}

public func SetInventoryTransfer(bool transfer)
{
	inventory_transfer = transfer;
	return;
}

public func GetInventoryTransfer()
{
	return inventory_transfer;
}

public func SetFreeCrew(bool free)
{
	free_crew = free;
	return;
}

public func GetFreeCrew()
{
	return free_crew;
}

protected func OnClonkDeath(object clonk)
{
	var plr = clonk->GetOwner();
	var clonk_id = clonk->GetID();
	
	// Only respawn a clonk if it is the last crew member.
	if (GetCrewCount(plr) >= 1)
		return _inherited(clonk, ...); 

	// Find closest base owned by the player (or team) and try to buy a clonk.
	for (var base in clonk->FindObjects(Find_Func("IsBaseBuilding"), Find_Allied(plr), Sort_Distance()))
	{
		if (!base)
			continue;
			
		// If free crew just create a clonk at the base.
		if (free_crew)
		{
			var crew = CreateObjectAbove(clonk_id, base->GetX() - GetX(), base->GetY() + base->GetDefHeight() / 2 - GetX(), plr);
			crew->MakeCrewMember(plr);
			SetCursor(plr, crew);
			// Transfer inventory if turned on.
			if (inventory_transfer)
				crew->GrabContents(clonk);
			break;
		}
		// Try to buy a crewmember at the base.
		var crew = base->~DoBuy(clonk_id, plr, base->GetOwner(), clonk);
		if (crew && GetType(crew) == C4V_C4Object)
		{
			crew->Exit(0, base->GetDefHeight() / 2);
			SetCursor(plr, crew);
			// Transfer inventory if turned on.
			if (inventory_transfer)
				crew->GrabContents(clonk);
			break;
		}
	}	
	return _inherited(clonk, ...);
}


/*-- Rule Menu --*/

protected func Activate(int by_plr)
{
	MessageWindow("$Description$", by_plr);
	return;
}


/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";
local Visibility = VIS_Editor;
local EditorPlacementLimit = 1;
