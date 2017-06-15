/**
	Cannon Test
	Test scenario for all the cannon types.
	
	@author Maikel
*/


static script_plr;

protected func Initialize()
{

	return;
}

protected func InitializePlayer(int plr)
{
	// Set zoom to full map size.
	SetPlayerZoomByViewRange(plr, LandscapeWidth(), nil, PLRZOOM_Direct);
	
	// No FoW to see everything happening.
	SetFoW(false, plr);
	
	var clonk = GetCrew(plr);
	clonk->SetPosition(90, 150);
	
	CreateObject(Rule_NoPowerNeed);
	
	var frame = CreateObjectAbove(CannonFrame, 90, 140);
	var artillery_cannon = CreateObjectAbove(ArtilleryCannon, 70, 140);
	artillery_cannon->CreateContents(PowderKeg);
	CreateObjectAbove(Chest, 30, 160)->CreateContents(IronBomb, 10);
	var gatling_gun = CreateObjectAbove(GatlingGun, 120, 80);
	gatling_gun->CreateContents(LeadBullet, 10);
	var liquid_cannon = CreateObjectAbove(LiquidCannon, 50, 140);
	//liquid_cannon->MountCannon(frame);
	var crossbow = CreateObjectAbove(Crossbow, 30, 140);
	crossbow->CreateContents(Arrow);
	crossbow->CreateContents(FireArrow);	
	crossbow->CreateContents(BombArrow);
	crossbow->MountCannon(frame);
	
	var tesla_cannon = CreateObjectAbove(TeslaCannon, 110, 150);
		
	var pump = CreateObjectAbove(Pump, 54, 160, plr);
	var source = CreateObjectAbove(Pipe, 168, 292, plr);
	source->ConnectPipeTo(pump, PIPE_STATE_Source);
	var drain = CreateObjectAbove(Pipe, 240, 100, plr);
	drain->ConnectPipeTo(pump, PIPE_STATE_Drain);
	drain->ConnectPipeTo(liquid_cannon, PIPE_STATE_Drain);
	
	var tower = CreateObjectAbove(GuardTower, 120, 160);
	var tower_base = CreateObjectAbove(Basement, 120, 168);
	tower_base->SetParent(tower);
	
	//CreateObjectAbove(Flagpole, 20, 160, plr);
	//CreateObjectAbove(WindGenerator, 20, 160, plr);
	
	tesla_cannon->MountCannon(tower->GetCannonMount());
	return;
}

global func RestoreLiquidLevels(string mat)
{
	if (!mat)
		mat = "Water";

	// Restore water levels.
	DrawMaterialQuad(mat, 144, 168, 208 + 1, 168, 208 + 1, 304, 144, 304, true);
	for (var x = 216; x <= 280; x++)
		for (var y = 24; y <= 120; y++)
			if (GetMaterial(x, y) != Material("BrickSoft"))
				ClearFreeRect(x, y, 1, 1);
	return;
}

global func CreateFire(bool to_be_inc)
{
	for (var index = 0; index < 200; index++)
	{
		if (to_be_inc)
			CreateObjectAbove(Wood, RandomX(220, 400), RandomX(40, 160))->SetCategory(C4D_StaticBack);
		else
		{
			var burn = CreateObjectAbove(Metal, RandomX(220, 400), RandomX(40, 160));
			burn->SetCategory(C4D_StaticBack);
			burn.NoBurnDecay = true;
			burn->Incinerate();
		}
	}
	return;
}

global func CreateTargets()
{
	for (var index = 0; index < 50; index++)
		CreateObjectAbove(StrawMan, RandomX(220, 400), RandomX(40, 160))->SetCategory(C4D_StaticBack);
	return;
}
