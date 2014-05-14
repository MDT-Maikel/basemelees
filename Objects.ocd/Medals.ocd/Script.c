/**
	Medals Rule
	If this rule is activated players can earn medals by performing certain actions.
	All medals are listed in MedalList.txt and should be added there with a unique ID.
	A medal can be awarded by the definition call to this rule:
	  Rule_Medals->AwardMedal(id medal, int plr);
	And the rule will do an extended gamecall when a medal is awarded:
	  OnMedalAwarded(id medal, int to_plr);
	
	@author Maikel
*/


/*-- Callbacks --*/

protected func Initialize()
{
	// Under no circumstance there may by multiple copies of this rule.
	if (ObjectCount(Find_ID(Rule_Medals)) > 1)
		return RemoveObject();
	// Perform "OnRoundStart" callback for all loaded medals.
	PerformMedalCallbacks("OnRoundStart");
	return;
}

protected func OnGameOver()
{
	// Perform "OnRoundFinish" callback for all loaded medals.
	PerformMedalCallbacks("OnRoundFinish");
	return;
}

protected func InitializePlayer(int plr)
{
	// Perform "OnInitializePlayer" callback for all loaded medals.
	PerformMedalCallbacks("OnInitializePlayer", plr);
	return;
}

protected func RemovePlayer(int plr)
{
	// Perform "OnRemovePlayer" callback for all loaded medals.
	PerformMedalCallbacks("OnRemovePlayer", plr);
	return;
}

protected func OnClonkDeath(object clonk, int killed_by)
{
	// Perform "OnCrewDeath" callback for all loaded medals.
	PerformMedalCallbacks("OnCrewDeath", clonk, killed_by);
	return;
}

protected func Activate(int byplr)
{
	MessageWindow("$Description$", byplr);
	return;
}

// Performs the given callback in all loaded medals.
private func PerformMedalCallbacks(string callback, par1, par2, par3, par4)
{
	// Add "~" to the callback. 
	callback = Format("~%s", callback);
	// Loop over all loaded medals.
	var index = 0, def;
	while (def = GetDefinition(index++))
		if (def->~IsMedal())
			def->Call(callback, par1, par2, par3, par4);
	return;	
}


/*-- Medal Rewarding --*/

// Gives the player the specified medal.
public func AwardMedal(id medal, int plr)
{
	// Safety check: is the passed medal really a medal?
	if (!medal->~IsMedal())
		return;
	
	//Log("Medal %i rewarded for %s", medal, GetPlayerName(plr));
	// Retrieve the medal count from the players medal list using the medal index.
	var medal_index = medal->GetMedalIndex();
	var medal_data = GetMedalData(plr);
	var medal_count = GetMedalCount(medal_data, medal_index);
	//Log("Obtained medal count from medal data (%s)", medal_data);
	//Log("Medal count for medal %i equals %d", medal, medal_count);
	
	// Set the medal count from the players medal list using the medal index.
	medal_data = SetMedalCount(medal_data, medal_index, medal_count + 1);
	//Log("New medal data has been set to (%s)", medal_data);
	SetMedalData(plr, medal_data);
	
	// Perform "OnMedalAwarded" callback for all loaded medals.
	PerformMedalCallbacks("OnMedalAwarded", medal, plr);
	
	// Also perform "OnMedalAwarded" callback in scenario scripts & rules.
	GameCallEx("OnMedalAwarded", medal, plr);
	return;
}

// Clears the complete medal collection of the player.
public func ClearMedals(int plr)
{
	SetMedalData(plr, "MEDALS__");
	return;
}


/*-- Medal Data Handling --*/

// Returns the medal count for a specific medal, taken from medal data.
private func GetMedalCount(string medal_data, int medal_index)
{
	// Medal index ranges from 0 to 254.
	if (!Inside(medal_index, 0, 254))
		return 0; 
	// Each index covers 8 chars and from these we can construct the count.
	var count = 0;
	for (var i = 0; i < 8; i++)
	{
		var char = GetChar(medal_data, (medal_index + 1) * 8 + i);
		// Convert the char to a number.
		var num = 0;
		if (Inside(char, 48, 57))
			num = char - 48;
		// Increase the count.
		count += 10**(7 - i) * num;
	}
	return count;
}

// Sets the medal count for a specific medal, in provided medal data.
private func SetMedalCount(string medal_data, int medal_index, int count)
{
	// Create a list of chars from the medal data.
	var char_list = [];
	for (var i = 0; i < GetLength(medal_data); i++)
		char_list[i] = GetChar(medal_data, i);
	// If the char list is too short to contain the new medal data add zero's in between.
	if (GetLength(char_list) < (medal_index + 1) * 8)
		for (var i = GetLength(char_list); i < (medal_index + 1) * 8; i++)
			char_list[i] = GetChar("0", 0);
	// Change the medal data at the index of the medal.
	var count_str = Format("%8.8d", count);
	for (var i = 0; i < GetLength(count_str); i++)
		char_list[(medal_index + 1) * 8 + i] = GetChar(count_str, i);
	// Conver the char_list to a medal data string again.
	var new_data = "";
	for (var i = 0; i < GetLength(char_list); i++)
		new_data = Format("%s%c", new_data, char_list[i]);
	// Return the new medal data.
	return new_data;
}

// Returns the medal data of the player, this is in a string format.
// Medal data is a string of 2048 characters starting with "MEDALS__".
private func GetMedalData(int plr)
{
	var plr_id = GetPlayerID(plr);
	var data;
	// If there is a league try to retrieve it from the league.

	// Not in a league so get via player extra data.
	data = GetPlrExtraData(plr, "Medals");
	// Check whether it is a string starting with "MEDALS__".
	if (GetType(data) != C4V_String)
		return "MEDALS__";
	if (GetChar(data, 0) != GetChar("M", 0) || GetChar(data, 1) != GetChar("E", 0) || GetChar(data, 2) != GetChar("D", 0) || GetChar(data, 3) != GetChar("A", 0))
		return "MEDALS__";
	// Data is safe and just return it.
	return data;
}

// Sets the medal data of the player, pass in string format.
// Medal data is a string of 2048 characters starting with "MEDALS__".
private func SetMedalData(int plr, string data)
{
	var plr_id = GetPlayerID(plr);
	// If there is a league try to set data in the league.

	// Not in a league so set via player extra data.
	SetPlrExtraData(plr, "Medals", data);
	return;
}


/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";
