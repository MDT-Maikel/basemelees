/* Automatically created objects file */

func InitializeObjects()
{
	var Torch001 = CreateObjectAbove(Torch, 268, 585);
	Torch001->AttachToWall(true);
	var Torch002 = CreateObjectAbove(Torch, 343, 585);
	Torch002->AttachToWall(true);
	var Torch003 = CreateObjectAbove(Torch, 72, 578);
	Torch003->AttachToWall(true);
	var Torch004 = CreateObjectAbove(Torch, 278, 690);
	Torch004->AttachToWall(true);
	var Torch005 = CreateObjectAbove(Torch, 207, 658);
	Torch005->AttachToWall(true);
	var Torch006 = CreateObjectAbove(Torch, 208, 480);
	Torch006->AttachToWall(true);
	var Torch007 = CreateObjectAbove(Torch, 352, 506);
	Torch007->AttachToWall(true);
	var Torch008 = CreateObjectAbove(Torch, 80, 471);
	Torch008->AttachToWall(true);
	var Torch009 = CreateObjectAbove(Torch, 257, 376);
	Torch009->AttachToWall(true);
	var Torch010 = CreateObjectAbove(Torch, 155, 379);
	Torch010->AttachToWall(true);
	var Torch011 = CreateObjectAbove(Torch, 1172, 585);
	Torch011->AttachToWall(true);
	var Torch012 = CreateObjectAbove(Torch, 1097, 585);
	Torch012->AttachToWall(true);
	var Torch013 = CreateObjectAbove(Torch, 1368, 578);
	Torch013->AttachToWall(true);
	var Torch014 = CreateObjectAbove(Torch, 1162, 690);
	Torch014->AttachToWall(true);
	var Torch015 = CreateObjectAbove(Torch, 1233, 658);
	Torch015->AttachToWall(true);
	var Torch016 = CreateObjectAbove(Torch, 1232, 480);
	Torch016->AttachToWall(true);
	var Torch017 = CreateObjectAbove(Torch, 1088, 506);
	Torch017->AttachToWall(true);
	var Torch018 = CreateObjectAbove(Torch, 1360, 471);
	Torch018->AttachToWall(true);
	var Torch019 = CreateObjectAbove(Torch, 1183, 376);
	Torch019->AttachToWall(true);
	var Torch020 = CreateObjectAbove(Torch, 1285, 379);
	Torch020->AttachToWall(true);
	
	var armory;	
	armory = CreateObjectAbove(Armory, 299, 527);
	armory->CreateContents(Metal, 6);
	armory->CreateContents(Wood, 6);
	armory->CreateContents(Firestone, 4);
	armory = CreateObjectAbove(Armory, 1141, 527);
	armory->CreateContents(Metal, 6);
	armory->CreateContents(Wood, 6);
	armory->CreateContents(Firestone, 4);
	armory->SetDir(DIR_Right);

	var shipyard;
	shipyard = CreateObjectAbove(Shipyard, 90, 432);
	shipyard->CreateContents(Metal, 4);
	shipyard->CreateContents(Wood, 4);
	shipyard = CreateObjectAbove(Shipyard, 1350, 432);
	shipyard->CreateContents(Metal, 4);
	shipyard->CreateContents(Wood, 4);

	var workshop;
	workshop = CreateObjectAbove(ToolsWorkshop, 93, 615);
	workshop->CreateContents(Metal, 4);
	workshop->CreateContents(Wood, 4);
	workshop->CreateContents(Hammer, 2);
	workshop->CreateContents(Axe, 2);
	workshop->CreateContents(Shovel, 4);
	workshop->CreateContents(Pickaxe, 4);
	workshop = CreateObjectAbove(ToolsWorkshop, 1347, 615);
	workshop->CreateContents(Metal, 4);
	workshop->CreateContents(Wood, 4);
	workshop->CreateContents(Hammer, 2);
	workshop->CreateContents(Axe, 2);
	workshop->CreateContents(Shovel, 4);
	workshop->CreateContents(Pickaxe, 4);

	var chemlab;
	chemlab = CreateObjectAbove(ChemicalLab, 46, 615);
	chemlab->CreateContents(Firestone, 8);
	chemlab->CreateContents(Coal, 4);
	chemlab->CreateContents(Metal, 4);
	chemlab->CreateContents(Wood, 4);
	chemlab = CreateObjectAbove(ChemicalLab, 1394, 615);
	chemlab->CreateContents(Firestone, 8);
	chemlab->CreateContents(Coal, 4);
	chemlab->CreateContents(Metal, 4);
	chemlab->CreateContents(Wood, 4);
	
	var foundry;
	foundry = CreateObjectAbove(Foundry, 300, 720);
	foundry->CreateContents(Ore, 4);
	foundry->CreateContents(Coal, 4);
	foundry->CreateContents(Bucket, 2);
	foundry->CreateContents(Barrel, 1);
	foundry = CreateObjectAbove(Foundry, 1140, 720);
	foundry->CreateContents(Ore, 4);
	foundry->CreateContents(Coal, 4);
	foundry->CreateContents(Bucket, 2);
	foundry->CreateContents(Barrel, 1);

	CreateObjectAbove(Sawmill, 245, 718);
	CreateObjectAbove(Sawmill, 1195, 718);
	
	var inventorslab;
	inventorslab = CreateObjectAbove(InventorsLab, 116, 528);
	inventorslab->CreateContents(Metal, 4);
	inventorslab->CreateContents(Wood, 4);
	inventorslab->CreateContents(Cloth, 4);
	inventorslab = CreateObjectAbove(InventorsLab, 1324, 528);
	inventorslab->CreateContents(Metal, 4);
	inventorslab->CreateContents(Wood, 4);
	inventorslab->CreateContents(Cloth, 4);

	CreateObject(Compensator, 52, 462);
	CreateObject(Compensator, 27, 462);
	CreateObject(Compensator, 1388, 462);
	CreateObject(Compensator, 1413, 462);

	CreateObjectAbove(Flagpole, 120, 616);
	CreateObjectAbove(Flagpole, 243, 527);
	CreateObjectAbove(Flagpole, 1320, 616);
	CreateObjectAbove(Flagpole, 1197, 527);

	CreateObject(WindGenerator, 345, 398);
	CreateObject(WindGenerator, 1095, 398);

	var steamengine;
	steamengine = CreateObject(SteamEngine, 50, 504);
	steamengine->CreateContents(Coal, 10);
	steamengine = CreateObject(SteamEngine, 1390, 504);
	steamengine->CreateContents(Coal, 10);

	var chest;
	chest = CreateObjectAbove(Chest, 148, 615);
	chest->CreateContents(Bow, 2);
	chest->CreateContents(Arrow, 2);
	chest->CreateContents(Javelin, 2);
	chest = CreateObjectAbove(Chest, 132, 615);
	chest->CreateContents(Sword, 2);
	chest->CreateContents(Shield, 2);
	chest->CreateContents(Club, 2);
	chest = CreateObjectAbove(Chest, 1292, 615);
	chest->CreateContents(Bow, 2);
	chest->CreateContents(Arrow, 2);
	chest->CreateContents(Javelin, 2);
	chest = CreateObjectAbove(Chest, 1308, 615);
	chest->CreateContents(Sword, 2);
	chest->CreateContents(Shield, 2);
	chest->CreateContents(Club, 2);

	var Elevator001 = CreateObjectAbove(Elevator, 239, 431);
	Elevator001->SetDir(DIR_Right);
	Elevator001->SetColor(0xfff40000);
	Elevator001->CreateShaft(289);
	Elevator001->SetCasePosition(708);
	var Elevator002 = CreateObjectAbove(Elevator, 177, 431);
	Elevator002->SetColor(0xfff40000);
	Elevator002->CreateShaft(289);
	Elevator002->SetCasePosition(708);
	Elevator002->LetsBecomeFriends(Elevator001);
	var Elevator003 = CreateObjectAbove(Elevator, 1201, 431);
	Elevator003->SetColor(0xfff40000);
	Elevator003->CreateShaft(289);
	Elevator003->SetCasePosition(708);
	var Elevator004 = CreateObjectAbove(Elevator, 1263, 431);
	Elevator004->SetDir(DIR_Right);
	Elevator004->SetColor(0xfff40000);
	Elevator004->CreateShaft(289);
	Elevator004->SetCasePosition(708);
	Elevator004->LetsBecomeFriends(Elevator003);
	
	var StoneDoor001 = CreateObject(StoneDoor, 412, 595);
	var SpinWheel001 = CreateObjectAbove(SpinWheel, 346, 616);
	SpinWheel001->SetStoneDoor(StoneDoor001);
	var StoneDoor002 = CreateObject(StoneDoor, 428, 596);
	var SpinWheel002 = CreateObjectAbove(SpinWheel, 370, 616);
	SpinWheel002->SetStoneDoor(StoneDoor002);
	var StoneDoor003 = CreateObject(StoneDoor, 1028, 595);
	var SpinWheel003 = CreateObjectAbove(SpinWheel, 1094, 616);
	SpinWheel003->SetStoneDoor(StoneDoor003);
	var StoneDoor004 = CreateObject(StoneDoor, 1012, 596);
	var SpinWheel004 = CreateObjectAbove(SpinWheel, 1070, 616);
	SpinWheel004->SetStoneDoor(StoneDoor004);
		
	CreateObject(StoneDoor, 164, 596)->SetAutoControl();
	CreateObject(StoneDoor, 388, 516)->SetAutoControl();
	CreateObject(StoneDoor, 292, 412)->SetAutoControl();
	CreateObject(StoneDoor, 164, 700)->SetAutoControl();
	CreateObject(StoneDoor, 1276, 596)->SetAutoControl();
	CreateObject(StoneDoor, 1052, 516)->SetAutoControl();
	CreateObject(StoneDoor, 1148, 412)->SetAutoControl();
	CreateObject(StoneDoor, 1276, 700)->SetAutoControl();

	var lorry;
	lorry = CreateObjectAbove(Lorry, 276, 719);
	lorry->CreateContents(Wood, 8);
	lorry->CreateContents(Metal, 8);
	lorry->CreateContents(Rock, 8);
	lorry = CreateObjectAbove(Lorry, 1164, 719);
	lorry->CreateContents(Wood, 8);
	lorry->CreateContents(Metal, 8);
	lorry->CreateContents(Rock, 8);

	return true;
}
