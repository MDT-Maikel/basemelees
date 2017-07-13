/**
	Cannon Test
	Test scenario for all the cannon types.
	
	@author Maikel
*/


static script_plr;

public func Initialize()
{
	// Relaunch testers.
	var flag = CreateObjectAbove(Flagpole, 580, 448);
	flag->SetNeutral(true);
	var relaunch_rule = GetRelaunchRule();
	relaunch_rule->SetInventoryTransfer(true);
	relaunch_rule->SetFreeCrew(true);
	relaunch_rule->SetRespawnDelay(1);
	relaunch_rule->SetBaseRespawn(true);
	relaunch_rule->SetDefaultRelaunchCount(nil);
	relaunch_rule->SetAllowPlayerRestart(true);
	relaunch_rule->SetLastClonkRespawn(true);	
	relaunch_rule->SetInitialRelaunch(false);

	// Create different cannons and test grounds.
	InitPower();
	InitLiquidCannon();
	InitGatlingGun();
	InitCrossbow();
	InitArtilleryCannon();
	InitTeslaCannon();
	InitArmory();
	return;
}

public func InitPower()
{
	var steam_engine = CreateObjectAbove(SteamEngine, 254, 520);
	steam_engine->CreateContents(Coal, 100);
	var steam_engine = CreateObjectAbove(SteamEngine, 324, 520);
	steam_engine->CreateContents(Coal, 100);
	
	CreateObjectAbove(Flagpole, 370, 304);
	CreateObjectAbove(Flagpole, 620, 352);
	
	CreateObjectAbove(WindGenerator, 80, 448);
	CreateObjectAbove(WindGenerator, 120, 448);
	
	CreateObjectAbove(Compensator, 232, 560);
	CreateObjectAbove(Compensator, 262, 560);
	CreateObjectAbove(Compensator, 292, 560);
	CreateObjectAbove(Compensator, 322, 560);
	CreateObjectAbove(Compensator, 352, 560);
	CreateObjectAbove(Compensator, 382, 560);	
	
	// No power need for pumping and tesla cannon.
	//CreateObject(Rule_NoPowerNeed);
	return;
}

public func InitLiquidCannon()
{
	// Create liquid cannons and connect to all types of liquid pumps.
	var pump1 = CreateObjectAbove(Pump, 756, 448);
	var pump2 = CreateObjectAbove(Pump, 724, 448);
	var pump3 = CreateObjectAbove(Pump, 692, 448);
	var pump4 = CreateObjectAbove(Pump, 660, 448);
	var pump5 = CreateObjectAbove(Pump, 652, 352);
	pump1->SetMaterialSelection([Water]);
	pump2->SetMaterialSelection([Lava, DuroLava]);
	pump3->SetMaterialSelection([Acid]);
	pump4->SetMaterialSelection([Water]);
	pump5->SetMaterialSelection([Concrete]);
	
	var foundry = CreateObjectAbove(Foundry, 700, 352);
	foundry->CreateContents(Rock, 100);
	foundry->AddToQueue(Concrete, nil, true);
	
	CreateObject(Pipe, 756, 484)->ConnectPipeTo(pump1, PIPE_STATE_Source);
	CreateObject(Pipe, 724, 484)->ConnectPipeTo(pump2, PIPE_STATE_Source);
	CreateObject(Pipe, 692, 484)->ConnectPipeTo(pump3, PIPE_STATE_Source);
	CreateObject(Pipe, 660, 484)->ConnectPipeTo(pump4, PIPE_STATE_Source);
	var source = CreateObjectAbove(Pipe, 660, 484);
	source->ConnectPipeTo(pump5, PIPE_STATE_Source);
	source->ConnectPipeTo(foundry, PIPE_STATE_Source);
	
	var tower1 = CreateObjectAbove(GuardTower, 800, 448);
	var tower2 = CreateObjectAbove(GuardTower, 744, 352);	
	var frame1 = CreateObjectAbove(CannonFrame, 812, 446);
	var frame2 = CreateObjectAbove(CannonFrame, 610, 350);
	var cannon1 = CreateObject(LiquidCannon, 800, 400);
	var cannon2 = CreateObject(LiquidCannon, 800, 400);
	var cannon3 = CreateObject(LiquidCannon, 800, 400);
	var cannon4 = CreateObject(LiquidCannon, 800, 400);
	cannon1->MountCannon(tower1->GetCannonMount());
	cannon2->MountCannon(frame1);
	cannon3->MountCannon(tower2->GetCannonMount());
	cannon4->MountCannon(frame2);
	
	var drain1 = CreateObjectAbove(Pipe, pump4->GetX(), pump4->GetY());
	drain1->ConnectPipeTo(pump4, PIPE_STATE_Drain);
	drain1->ConnectPipeTo(foundry, PIPE_STATE_Drain);
	var drain2 = CreateObjectAbove(Pipe, pump1->GetX(), pump1->GetY());
	drain2->ConnectPipeTo(pump1, PIPE_STATE_Drain);
	drain2->ConnectPipeTo(cannon2, PIPE_STATE_Drain);
	var drain3 = CreateObjectAbove(Pipe, pump2->GetX(), pump2->GetY());
	drain3->ConnectPipeTo(pump2, PIPE_STATE_Drain);
	drain3->ConnectPipeTo(cannon1, PIPE_STATE_Drain);
	var drain4 = CreateObjectAbove(Pipe, pump3->GetX(), pump3->GetY());
	drain4->ConnectPipeTo(pump3, PIPE_STATE_Drain);
	drain4->ConnectPipeTo(cannon3, PIPE_STATE_Drain);
	var drain5 = CreateObjectAbove(Pipe, pump5->GetX(), pump5->GetY());
	drain5->ConnectPipeTo(pump5, PIPE_STATE_Drain);
	drain5->ConnectPipeTo(cannon4, PIPE_STATE_Drain);
		
	// Create a forest which can be burned and extinguished.
	PlaceForest([Tree_Deciduous, Tree_Coniferous2], 840, LandscapeHeight(), 204, true);
	for (var tree in FindObjects(Find_Func("IsTree"), Find_InRect(840, 0, 204, LandscapeWidth())))
	{
		tree.NoBurnDecay = true;
		tree->CreateEffect(FxBlockDamage, 100);
		tree->SetClrModulation(RGB(128, 128, 100));
	}
	
	// Place some bats which can be killed with the acid spray.
	Bat->Place(20, Rectangle(800, 0, 224, 400));
	return;
}

public func InitGatlingGun()
{
	// Create loaded gatling gun.
	var frame = CreateObjectAbove(CannonFrame, 160, 446);
	var gatling_gun = CreateObjectAbove(GatlingGun, 160, 446);
	gatling_gun->CreateContents(LeadBullet, 10);
	gatling_gun->MountCannon(frame);
	
	// Create strawman field to shoot at.
	for (var index = 0; index < 120; index++)
		CreateObjectAbove(StrawMan, RandomX(6, 108), 448);
	return;
}

public func InitCrossbow()
{
	// Create loaded crossbow.
	var frame = CreateObjectAbove(CannonFrame, 240, 446);
	var crossbow = CreateObjectAbove(Crossbow, 240, 446);
	crossbow->CreateContents(Arrow, 8);
	crossbow->CreateContents(FireArrow, 8);
	crossbow->CreateContents(BombArrow, 8);
	crossbow->MountCannon(frame);
	
	// Create flying rockets as targets.
	CreateEffect(FxCreateRockets, 100, 54);
	return;
}

public func InitArtilleryCannon()
{
	var frame = CreateObjectAbove(CannonFrame, 320, 446);
	var artillery_cannon = CreateObjectAbove(ArtilleryCannon, 320, 446);
	artillery_cannon->CreateContents(PowderKeg);
	artillery_cannon->MountCannon(frame);
	var chest = CreateObjectAbove(Chest, 340, 448);
	chest->CreateContents(IronBomb, 20);
	chest->CreateContents(PowderKeg, 10);
	return;
}

public func InitTeslaCannon()
{
	// Create tesla cannon.
	var frame = CreateObjectAbove(CannonFrame, 384, 222);
	var tesla_cannon = CreateObjectAbove(TeslaCannon, 384, 222);
	tesla_cannon->MountCannon(frame);
	
	// Automate tesla cannon.
	var automation = CreateObject(CannonMechanism);
	tesla_cannon->AddAutomationMechanism(automation);
	tesla_cannon->SetAutomationMode("mode::attack_meteor");
	
	// Create meteors as targets.
	CreateEffect(FxCreateMeteors, 100, 54);
	return;
}

public func InitArmory()
{
	var armory = CreateObjectAbove(Armory, 400, 448);
	armory->CreateContents(Wood, 20);
	armory->CreateContents(Metal, 20);
	armory->CreateContents(Coal, 20);
	armory->CreateContents(Firestone, 20);
	armory->CreateContents(Pipe, 5);
	armory->CreateContents(Diamond, 5);
	return;
}

public func InitializePlayer(int plr)
{
	// Set zoom to full map size.
	SetPlayerZoomByViewRange(plr, LandscapeWidth(), nil, PLRZOOM_Direct);
	
	// No FoW to see everything happening.
	SetFoW(false, plr);
	
	// Give the player all construction plans.
	var index = 0, def;
	while (def = GetDefinition(index++))
		SetPlrKnowledge(plr, def);
	
	// Move clonk to middle.
	var clonk = GetCrew(plr);
	clonk->SetPosition(580, 440);
	
		
	// Claim ownership of structures, last player who joins owns all the flags.
	for (var structure in FindObjects(Find_Or(Find_Category(C4D_Structure), Find_Func("IsFlagpole"))))
		structure->SetOwner(plr);
		
	// Some useful tools.
	clonk->CreateContents(WindBag);
	clonk->CreateContents(Hammer);
	clonk->CreateContents(CannonMechanism, 2);
	return;
}

static const FxCreateRockets = new Effect
{
	Timer = func(int time)
	{
		var boomattack = CreateObject(DefenseBoomAttack, 1024, RandomX(10, 30));
		boomattack->SetR(270);
		boomattack.FlySpeed = RandomX(200, 250);
		boomattack->SetWaypoints([
			{X = 724, Y = RandomX(10, 30)},
			{X = RandomX(520, 560), Y = RandomX(100, 140)},
			{X = RandomX(420, 460), Y = RandomX(320, 360)},
			{X = RandomX(80, 120), Y = RandomX(320, 360)},
			{X = 0, Y = RandomX(180, 220)}
		]);	
	}
};

static const FxCreateMeteors = new Effect
{
	Timer = func(int time)
	{
		LaunchMeteor(RandomX(200, 300), -12, RandomX(40, 60), RandomX(-10, 10), RandomX(40, 80));
	}
};
