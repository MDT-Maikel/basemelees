/**
	Cannons
	
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
	clonk->SetPosition(100, 140);
	
	CreateObject(Rule_NoPowerNeed);
	
	var frame = CreateObjectAbove(CannonFrame, 100, 140);
	var artillery_cannon = CreateObjectAbove(ArtilleryCannon, 80, 140);
	artillery_cannon->CreateContents(PowderKeg);
	CreateObjectAbove(Chest, 40, 140)->CreateContents(IronBomb, 10);
	var gatling_gun = CreateObjectAbove(GatlingGun, 120, 140);
	gatling_gun->CreateContents(LeadBullet, 10);
	var liquid_cannon = CreateObjectAbove(LiquidCannon, 60, 140);
	liquid_cannon->MountCannon(frame);
	
	var pump = CreateObjectAbove(Pump, 84, 160, plr);
	var source = CreateObjectAbove(Pipe, 168, 292, plr);
	source->ConnectPipeTo(pump, PIPE_STATE_Source);
	var drain = CreateObjectAbove(Pipe, 240, 100, plr);
	drain->ConnectPipeTo(pump, PIPE_STATE_Drain);
	drain->ConnectPipeTo(liquid_cannon, PIPE_STATE_Drain);
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
