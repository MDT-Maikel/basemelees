/**
	Medal Test
	Scenario designed for testing the medal scripts and functionality.
	
	@author Maikel
*/


static plr_medalist;
static plr_victim;

public func Initialize()
{
	// Create medal rule.
	CreateObject(Rule_Medals);
	
	// Create script players for these tests.
	CreateScriptPlayer("Medalist", RGB(255, 0, 0), nil, CSPF_NoEliminationCheck);
	CreateScriptPlayer("Victim", RGB(0, 255, 0), nil, CSPF_NoEliminationCheck);
	return;
}

protected func InitializePlayer(int plr, int x, int y, object base, int team)
{
	// Make player hostile to all others for testing.
	Goal_Melee->MakeHostileToAll(plr, team);

	// Initialize script player.
	if (GetPlayerType(plr) == C4PT_Script)
	{
		// Store the player numbers.
		if (GetPlayerName(plr) == "Medalist")
			plr_medalist = plr;
		else if (GetPlayerName(plr) == "Victim")
			plr_victim = plr;
		return;		
	}
	
	// Set zoom to full map size.
	SetPlayerZoomByViewRange(plr, LandscapeWidth(), nil, PLRZOOM_Direct);
	
	// No FoW to see everything happening.
	SetFoW(false, plr);
	
	// Move normal players into a relaunch container.
	var relaunch = CreateObject(RelaunchContainer, LandscapeWidth() / 2, LandscapeHeight() / 2);
	GetCrew(plr)->Enter(relaunch);
	
	// Add test control effect.
	AddEffect("IntTestControl", nil, 100, 2);
	return;	
}

public func GetMedalDataIdentifier() { return "BaseMelees::MedalTest"; }


/*-- Test Effect --*/

global func LaunchTest(id medal)
{
	if (!medal->~IsMedal())
		return;
	
	RemoveEffect("IntTestControl");
	InitTest();
	Call(Format("~Test_%i", medal), GetCrew(plr_medalist), GetCrew(plr_victim));
	return;
}

global func FxIntTestControlStart(object target, effect fx, int temporary)
{
	if (temporary)
		return FX_OK;
	// Set default interval.
	fx.Interval = 2;
	fx.launched = false;
	fx.success = true;
	// Init list of medals to test.
	fx.medals = Rule_Medals->GetActiveMedals();
	for (var index = 0; index < GetLength(fx.medals); index++)
		fx.medals[index] = Format("%i", fx.medals[index]);
	fx.current_medal = nil;
	// Log test start.
	Log("=====================================");
	Log("Running medal tests.");
	Log("=====================================");
	return FX_OK;
}

global func FxIntTestControlTimer(object target, effect fx)
{
	// Launch new test if needed.
	if (!fx.launched)
	{
		InitTest();
		fx.current_medal = PopFront(fx.medals);
		if (!fx.current_medal)
			return FX_Execute_Kill;
		// Start the test if available, otherwise finish test sequence.
		// Call Test_Medal_*(object medalist, object victim).
		if (!Call(Format("~Test_%s", fx.current_medal), GetCrew(plr_medalist), GetCrew(plr_victim)))
		{
			Log("%s: test missing.", fx.current_medal);
			fx.success = false;
			return FX_OK;
		}
		Log("%s: test started.", fx.current_medal);
		fx.launched = true;
	}
	return FX_OK;
}

global func FxIntTestControlStop(object target, effect fx, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	Log("=====================================");
	Log("%d medals have been tested.", GetLength(fx.medals));
	if (fx.success)
		Log("All tests have been completed!");
	else
		Log("Some tests are missing of failed.");	
	Log("=====================================");	
	return FX_OK;
}

global func FxIntTestControlOnMedalAwarded(object target, effect fx, id medal, int plr)
{
	// Log the result if it is the correct medal for the medalist.
	if (Format("%i", medal) == fx.current_medal && plr == plr_medalist && fx.launched)
	{
		Log("%s: test passed.", fx.current_medal);
		fx.launched = false;
	}
	return FX_OK;
}

global func InitTest()
{
	// Remove all objects except the player crew members and relaunch container they are in.
	for (var obj in FindObjects(Find_Not(Find_Or(Find_ID(RelaunchContainer), Find_Category(C4D_Rule)))))
		if (obj && !((obj->GetOCF() & OCF_CrewMember) && GetPlayerType(obj->GetOwner()) == C4PT_User))
			obj->RemoveObject();
			
	// Restore gravity.
	SetGravity(20);

	// Remove all landscape changes.
	DrawMaterialQuad("Brick", 0, 160, LandscapeWidth(), 160, LandscapeWidth(), LandscapeHeight(), 0, LandscapeHeight());
	ClearFreeRect(0, 0, LandscapeWidth(), 160);
	
	// Give script players new crew.
	for (var plr in [plr_medalist, plr_victim])
	{	
		if (!GetCrew(plr))
		{
			var clonk = CreateObjectAbove(Clonk, 100, 150, plr);
			clonk->MakeCrewMember(plr);
			clonk->SetDir(DIR_Right);
			SetCursor(plr, clonk);
			clonk->DoEnergy(clonk.MaxEnergy / 1000);
		}
	}
	GetCrew(plr_medalist)->SetPosition(50, 150);
	GetCrew(plr_victim)->SetPosition(100, 150);
	return;
}

public func OnMedalAwarded(id medal, int plr, int reward)
{
	var fx = GetEffect("IntTestControl");
	if (fx)
		EffectCall(nil, fx, "OnMedalAwarded", medal, plr);
	return;
}


/*-- Medal Tests --*/
// See MedalList.txt for the list of medals, this order is also used for the
// tests in here.

global func Test_Medal_HatTrick(object medalist, object victim)
{
	victim->SetPosition(120, 150);
	victim->DoEnergy(-45);
	
	for (var cnt = 0; cnt < 2; cnt++)
	{
		var clonk = victim->CreateObject(Clonk, 0, 0, plr_victim);
		clonk->MakeCrewMember(plr_victim);
		clonk->DoEnergy(-45);
	}
	
	var firestone = medalist->CreateContents(Firestone);
	medalist->ObjectCommand("Throw", firestone, 20, -10);
	return true;
}

global func Test_Medal_Miner(object medalist, object victim)
{
	var flag = medalist->CreateObjectAbove(Flagpole, 0, medalist->GetBottom(), plr_medalist);
	
	var nr_bars = Medal_Miner->GetMiningWealthGoal() / GoldBar->GetValue();
	for (var cnt = 0; cnt < nr_bars; cnt++)
	{
		var bar = medalist->CreateContents(GoldBar);
		ScheduleCall(bar, "Exit", 10, 0);
		ScheduleCall(flag, "Collect", 10, 0, bar, true);
	}
	return true;
}

global func Test_Medal_Decorated(object medalist, object victim)
{
	Schedule(nil, Format("Rule_Medals->AwardMedal(Medal_HatTrick, %d)", plr_medalist), 2, Medal_Decorated->GetDecorationGoal());
	return true;
}

global func Test_Medal_Kamikaze(object medalist, object victim)
{
	victim->SetPosition(55, 150);
	victim->DoEnergy(-45);
	
	medalist->DoEnergy(-45);
	
	var firestone = medalist->CreateContents(Firestone);
	medalist->ObjectCommand("Throw", firestone, 2, 10);
	return true;
}

global func Test_Medal_Pyromania(object medalist, object victim)
{
	victim->SetPosition(200, 150);
	
	for (var cnt = 0; cnt < Medal_Pyromania->GetIncinerationGoal(); cnt++)
		CreateObjectAbove(Wood, RandomX(100, 120), 160);
	
	var lantern = medalist->CreateContents(Lantern);
	medalist->ObjectCommand("Throw", lantern, 20, -10);
	return true;
}

global func Test_Medal_CriticalHit(object medalist, object victim)
{
	var massive_rock = medalist->CreateContents(Rock);
	massive_rock->SetMass(100000);
	medalist->ObjectCommand("Throw", massive_rock, 20, -10);
	return true;
}

global func Test_Medal_Demolition(object medalist, object victim)
{
	for (var cnt = 0; cnt < Medal_Demolition->GetDemolitionGoal(); cnt++)
		victim->CreateObjectAbove(ToolsWorkshop, 0, victim->GetBottom())->DoDamage(60);
	
	var firestone = medalist->CreateContents(Firestone);
	medalist->ControlThrow(firestone, 20, -20);
	return true;
}

global func Test_Medal_Heroic(object medalist, object victim)
{
	ScheduleCall(medalist, "DoEnergy", 5, 100, -10, false, FX_Call_EngScript, plr_victim);
	ScheduleCall(medalist, "DoEnergy", 5, 100, 10, false, FX_Call_EngScript, plr_medalist);	
	return true;
}

global func Test_Medal_ChainReaction(object medalist, object victim)
{
	DrawMaterialQuad("Firestone", 110, 160, 170, 160, 170, 280, 110, 280);
		
	var firestone = medalist->CreateContents(Firestone);
	medalist->ControlThrow(firestone, 20, -20);
	return true;
}

global func Test_Medal_Rocketeer(object medalist, object victim)
{
	victim->SetPosition(90, 150);
	victim->DoEnergy(-45);

	var boompack = medalist->CreateContents(Boompack);
	ScheduleCall(boompack, "ControlUse", 52, 0, medalist, 20, -2);
	ScheduleCall(boompack, "ControlJump", 56, 0, medalist);
	return true;
}

global func Test_Medal_Construction(object medalist, object victim)
{
	var nr_structures = Medal_Construction->GetConstructionGoal();
	
	var wood_pieces = [];
	for (var cnt = 0; cnt < 2 * nr_structures; cnt++)
		PushBack(wood_pieces, medalist->CreateContents(Wood));
	
	for (var cnt = 0; cnt < nr_structures; cnt++)
	{
		var site = medalist->CreateObjectAbove(ConstructionSite, 0, medalist->GetBottom());
		site->Set(Chest);
		ScheduleCall(PopBack(wood_pieces), "Enter", 10, 0, site);
		ScheduleCall(PopBack(wood_pieces), "Enter", 10, 0, site);
	}
	return true;
}

global func Test_Medal_Plumber(object medalist, object victim)
{
	DrawMaterialQuad("Water", 300, 160, 600, 160, 600, 280, 300, 280);
	DrawMaterialQuad("Brick", 0, 160, 8, 160, 8, 152, 0, 152);
	
	CreateObjectAbove(SteamEngine, 100, 160, plr_medalist)->CreateContents(Coal, 10);
	CreateObjectAbove(SteamEngine, 200, 160, plr_medalist)->CreateContents(Coal, 10);
	
	var pump, source;
	
	pump = CreateObjectAbove(Pump, 80, 160, plr_medalist);
	pump.Plane = pump.Plane + 1;
	source = CreateObjectAbove(Pipe, 350, 280, plr_medalist);
	source->ConnectPipeTo(pump, PIPE_STATE_Source);
	
	pump = CreateObjectAbove(Pump, 120, 160, plr_medalist);
	pump.Plane = pump.Plane + 1;
	source = CreateObjectAbove(Pipe, 350, 280, plr_medalist);
	source->ConnectPipeTo(pump, PIPE_STATE_Source);
	
	pump = CreateObjectAbove(Pump, 180, 160, plr_medalist);
	pump.Plane = pump.Plane + 1;
	source = CreateObjectAbove(Pipe, 350, 280, plr_medalist);
	source->ConnectPipeTo(pump, PIPE_STATE_Source);
	
	pump = CreateObjectAbove(Pump, 220, 160, plr_medalist);
	pump.Plane = pump.Plane + 1;
	source = CreateObjectAbove(Pipe, 350, 280, plr_medalist);
	source->ConnectPipeTo(pump, PIPE_STATE_Source);
	return true;
}

global func Test_Medal_Slasher(object medalist, object victim)
{
	victim->SetPosition(60, 150);
	victim->DoEnergy(-45);
	
	var sword = medalist->CreateContents(Sword);
	ScheduleCall(medalist, "ControlJump", 4, 0);
	ScheduleCall(sword, "ControlUse", 30, 0, medalist, 10, 30);
	return true;
}

global func Test_Medal_Berserker(object medalist, object victim)
{
	victim->SetPosition(60, 150);
	victim.MaxEnergy = 2000 * Medal_Berserker->GetDamageGoal();
	victim->DoEnergy(2 * Medal_Berserker->GetDamageGoal());
	
	var sword = medalist->CreateContents(Sword);
	ScheduleCall(sword, "ControlUse", 40, 40, medalist);
	return true;
}

global func Test_Medal_Superman(object medalist, object victim)
{
	ScheduleCall(medalist, "ControlJump", 8, 0);
	ScheduleCall(nil, "SetGravity", 26, 0, 0);
	ScheduleCall(nil, "SetGravity", Medal_Superman->GetFlightDurationGoal(), 0, GetGravity());
	return true;
}

global func Test_Medal_CrashPilot(object medalist, object victim)
{
	DrawMaterialQuad("Brick", 600, 0, LandscapeWidth(), 0, LandscapeWidth(), LandscapeHeight(), 600, LandscapeHeight());
	
	var plane = CreateObjectAbove(Airplane, 100, 60);
	plane->FaceRight();
	plane->PlaneMount(medalist);
	medalist->Enter(plane);
	medalist->SetAction("Walk");
	plane->StartInstantFlight(85, 15);
	plane->DoDamage(plane.HitPoints - 5);
	
	ScheduleCall(medalist, "Exit", 90, 0);
	return true;
}

global func Test_Medal_Energizer(object medalist, object victim)
{
	CreateObjectAbove(SteamEngine, 100, 160, plr_medalist)->CreateContents(Coal, 10);
	CreateObjectAbove(SteamEngine, 200, 160, plr_medalist)->CreateContents(Coal, 10);
	CreateObjectAbove(SteamEngine, 300, 160, plr_medalist)->CreateContents(Coal, 10);
	CreateObjectAbove(SteamEngine, 400, 160, plr_medalist)->CreateContents(Coal, 10);
	CreateObjectAbove(SteamEngine, 500, 160, plr_medalist)->CreateContents(Coal, 10);	
			
	for (var index = 0; index < 10; index++)
	{	
		var workshop = CreateObjectAbove(ToolsWorkshop, 60 * (index + 1), 160, plr_medalist);
		workshop->CreateContents(Wood, 20);
		workshop->CreateContents(Metal, 20);
		workshop->AddToQueue(Shovel, 20);
		workshop.Plane = workshop.Plane = 1;
	}
	return true;
}