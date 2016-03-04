/**
	Knowdlege
	Provides global functions to give players knowledge for scenarios,
	related to base melees.
	
	@author Maikel
*/

 
// Gives the player specific knowledge as given in the knowledge array.
global func GivePlayerSpecificKnowledge(int plr, array knowledge)
{
	for (var plan in knowledge)
		SetPlrKnowledge(plr, plan);
	return;
}

// Gives the player specific knowledge as given in the knowledge array.
global func RemovePlayerSpecificKnowledge(int plr, array knowledge)
{
	for (var plan in knowledge)
		SetPlrKnowledge(plr, plan, true);
	return;
}

// Gives the player plans according to basic knowledge.
global func GivePlayerBaseMeleeKnowledge(int plr)
{
	var knowledge = [
		// Structures for a settlement, defense and production of tools and explosives.
		Flagpole, Basement, WindGenerator, SteamEngine, Compensator, Sawmill, Foundry, Elevator, ToolsWorkshop, ChemicalLab, Armory, Chest, Pump, Shipyard, Kitchen, InventorsLab, Windmill, Idol,
		// The resources which can be produced.
		Metal, Loam, GoldBar, Cloth,
		// Tools for mining and tree chopping and loam production, etc.
		Shovel, Hammer, Axe, Pickaxe, Barrel, MetalBarrel, Bucket, Torch, Pipe, WallKit,
		// Weapons and explosives.
		Bow, Arrow, FireArrow, BombArrow, Club, Sword, Javelin, Shield, Musket, LeadShot, Dynamite, DynamiteBox, IronBomb, GrenadeLauncher, PowderKeg,
		// Advanced tools.
		Ropeladder, TeleGlove, WindBag, GrappleBow, Boompack, Balloon,
		// Some basic vehicles which aid in the settlement construction.
		Lorry, Catapult, Cannon, Airship, Airplane
	];
	for (var plan in knowledge)
		SetPlrKnowledge(plr, plan);
	return;
}
