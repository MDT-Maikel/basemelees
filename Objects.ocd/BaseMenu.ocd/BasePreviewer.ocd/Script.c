/**
	BasePreviewer
	Shows the object to be placed in a base and allows locating it.

	@author Maikel
*/


// Local variables for the base object and base rectangle.
local clonk, obj, direction;
local base_rect;
local GFX_StructureOverlay = 1;
local GFX_CombineIconOverlay = 2;

protected func Initialize()
{
	base_rect = {};
	SetProperty("Visibility", VIS_Owner);
}

public func Set(object constructing_clonk, proplist to_construct, proplist rect)
{
	base_rect = rect;
	SetGraphics(nil, to_construct.def, GFX_StructureOverlay, GFXOV_MODE_Base);
	SetGraphics(nil, to_construct.def, 3, GFXOV_MODE_Picture, nil, GFX_BLIT_Wireframe);

	clonk = constructing_clonk;
	obj = to_construct;
	direction = DIR_Left;
	this->AdjustPreview();	
}

// Positions the preview according to the landscape, coloring it green or red.
// Also returning whether an object can be placed.
public func AdjustPreview(bool look_up, bool no_call)
{
	var half_y = obj.def->GetDefHeight() / 2;
	// Place on material
	var search_dir = 1;
	if (look_up) search_dir = -1;
	var x = 0, y = 0, fail = false;
	while(!(!GBackSolid(x, y + half_y) && GBackSolid(x, y + half_y + 1)))
	{
		y += search_dir;
		if (Abs(y) > obj.def->GetDefHeight() / 2)
		{
			fail = true;
			break;
		}
	}
	if (fail && !no_call)
		return this->AdjustPreview(!look_up, true);
	if (fail)
	{
		SetClrModulation(RGBa(255,50,50, 100), GFX_StructureOverlay);
		return false;
	}
	SetPosition(GetX(), GetY() + y);

	if (obj.def->GetCategory() & C4D_Structure && !CheckConstructionSite(obj.def, 0, half_y))
	{
		SetClrModulation(RGBa(255,50,50, 100), GFX_StructureOverlay);
		return false;
	}
	SetClrModulation(RGBa(50,255,50, 100), GFX_StructureOverlay);
	return true;
}

// Positions the preview according to the mouse cursor, calls AdjustPreview afterwards.
// x and y are refined mouse coordinates so always centered at the clonk.
public func Reposition(int x, int y)
{
	x = BoundBy(x, base_rect.x - clonk->GetX(), base_rect.x + base_rect.w - clonk->GetX());
	y = BoundBy(y, base_rect.y - clonk->GetY(), base_rect.y + base_rect.h - clonk->GetY());
		
	x = clonk->GetX() + x;
	y = clonk->GetY() + y;

	SetPosition(x, y);
	AdjustPreview();
}

// Creates a base object at the last known position, the previewer is there.
public func CreateBaseObject()
{
	if (!AdjustPreview())
		return false;

	var created = CreateObjectAbove(obj.def, 0, obj.def->GetDefHeight() / 2, GetOwner());
	if (obj.contents)
		for(c in obj.contents)
			created->CreateContents(c[0], c[1]);
			
	if (obj.basement)
	{
		var basement = CreateObject(Basement, AbsX(created->GetX()), AbsY(created->GetY()) + created->GetBottom() + 4, GetOwner());
		var offset = created->~GetBasementOffset();
		if (offset)
			basement->SetPosition(basement->GetX() + offset[0], basement->GetY() + offset[1]);
		basement->SetParent(created);	
	}

	if (direction)
		created->SetDir(direction);

	return true;
}

// Flips the preview horizontally
public func Flip()
{
	// Flip not allowed?
	if (obj.def->~NoConstructionFlip()) return;

	if (direction == DIR_Left)
	{
		direction = DIR_Right;
		SetObjDrawTransform(-1000, 0, 0, 0, 1000, 0, GFX_StructureOverlay);
	} 
	else
	{
		direction = DIR_Left;
		SetObjDrawTransform(1000, 0, 0, 0, 1000, 0, GFX_StructureOverlay);
	}
}

// UI not saved.
public func SaveScenarioObject() { return false; }


/*-- Proplist --*/

local Plane = 250;
