/**
	Medals Rule
	If this rule is activated players can earn medals by performing certain actions.
	All medals are listed in MedalList.txt and should be added there with a unique ID.
	A medal can be awarded by the definition call to this rule:
	  Rule_Medals->AwardMedal(id medal, int plr);
	And the rule will do an extended gamecall when a medal is awarded:
	  OnMedalAwarded(id medal, int to_plr);
	Players can view their medals and medals of other players in the round by typing
	  /medals 
	in the chat window, which shows a menu with all achieved medals.
	
	@author Maikel
*/

// Variable to store the awarded medals for this round.
local round_medals;
// Variable to store the medal evaluation data summary.
local round_eval_data;
// Variable to store which player types receive medals (default C4PT_User).
local award_plr_type;
// Variable to store whether the medal rule logs, inactive by default.
local logging_active;
// Variable to store whether clunker reward is active, true by default.
local reward_active;
// Variable to store whether a scoreboard column for medals is turned on.
local scoreboard_active;


/*-- Callbacks --*/

protected func Initialize()
{
	// Under no circumstance there may by multiple copies of this rule.
	if (ObjectCount(Find_ID(Rule_Medals)) > 1)
		return RemoveObject();
	// Set player type for awarding medals to normal players.
	award_plr_type = C4PT_User;
	// Set logging to inactive.
	logging_active = false;
	// Set clunker rewarding to active by default.
	reward_active = true;
	// Scoreboard is not active by default.
	scoreboard_active = false;
	// Checks all loaded medals for overlapping medal indices. 
	CheckActiveMedals();
	// Initialize the local round medals to an empty list.
	round_medals = [];
	// Initialize the local round eval data.
	round_eval_data = { most_medals = 0, best_plr = NO_OWNER };
	// Initialize the message board commands.
	InitMedalMessageBoard();		
	// Perform "OnRoundStart" callback for all loaded medals.
	PerformMedalCallbacks("OnRoundStart");
	return;
}

protected func OnGameOver()
{
	// Remove the medal rule after the round has been finished so that
	// no more medals can be awarded. The medal defintions are informed
	// via the "OnRoundFinish" callback in the Destruction call.
	RemoveObject();
	return;
}

protected func Destruction()
{
	// Don't do anything if not the last medal rule.
	if (ObjectCount(Find_ID(Rule_Medals)) > 1)
		return;
	// Perform "OnRoundFinish" callback for all loaded medals.
	PerformMedalCallbacks("OnRoundFinish");
	// When this rule is removed no more medals will be awarded and it
	// is time to add information about the rewarded medals to the round
	// evaluation data.
	AddMedalEvaluationData();
	return;
}

protected func InitializePlayer(int plr)
{
	// Set the round medals to zero for this player.
	var plr_id = GetPlayerID(plr);
	round_medals[plr_id] = "MEDALS__";
	// Init the scoreboard for this player if it is active.
	if (GetScoreboard() && GetPlayerType(plr) == C4PT_User)
	{
		Scoreboard->NewPlayerEntry(plr);
		Scoreboard->SetPlayerData(plr, "medals", 0);
	}
	// Perform "OnInitializePlayer" callback for all loaded medals.
	PerformMedalCallbacks("OnInitializePlayer", plr);
	return;
}

protected func RemovePlayer(int plr)
{
	// When a player is eliminated or removed add its evaluation data. 
	AddPlayerMedalEvaluationData(plr);
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

// Performs the given callback in all loaded medals.
public func PerformMedalCallbacks(string callback, par1, par2, par3, par4)
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


/*-- Medal Awarding --*/

// Gives the player the specified medal.
public func AwardMedal(id medal, int plr)
{
	// Safety check: this is a definition call.
	if (this != Rule_Medals)
	{
		Log("$WarningDefinitionCall$", "AwardMedal", "AwardMedal");
		return;
	}
	// Safety check: is the passed medal really a medal?
	if (!medal->~IsMedal())
	{
		Log("$WarningDefinitionNotMedal$", medal);
		return;
	}
	// Safety check: player may not equal NO_OWNER;
	if (plr == NO_OWNER)	
	{
		Log("$WarningInvalidPlayerNumber$", plr);
		return;
	}

	// Get the active medal rule.
	var active_rule = FindObject(Find_ID(Rule_Medals));
		
	// Make sure the medal rule is still active, otherwise warn scripter.
	if (!active_rule)
	{
		Log("$WarningRuleNotActive$", medal, plr);
		Log("$WarningFixMedalScript$");
		return;
	}
	
	// Don't award medals to wrong player type (only for C4PT_User by default).
	if (!(GetPlayerType(plr) & active_rule->GetAwardingPlayerType()))
		return;
	
	// Retrieve the medal count from the players medal list using the medal index.
	var medal_index = medal->GetMedalIndex();
	var medal_data = GetMedalData(plr);
	var medal_count = GetMedalCount(medal_data, medal_index);
	// Set the medal count from the players medal list using the medal index.
	medal_data = SetMedalCount(medal_data, medal_index, medal_count + 1);
	SetMedalData(plr, medal_data);
	
	// Do the same for the round medals.
	var round_metal_data = GetRoundMedalData(plr);
	var round_metal_count = GetMedalCount(round_metal_data, medal_index);
	round_metal_data = SetMedalCount(round_metal_data, medal_index, round_metal_count + 1);
	SetRoundMedalData(plr, round_metal_data);
	
	// Give the player its reward for obtaining the medal in clunkers.
	if (active_rule->GetRewarding())
	{
		var reward = medal->~GetMedalReward();
		if (reward)
			DoWealth(plr, reward);
	}
	
	// Perform "OnMedalAwarded" callback for all loaded medals.
	PerformMedalCallbacks("OnMedalAwarded", medal, plr);
	
	// Also perform "OnMedalAwarded" callback in scenario scripts & rules.
	GameCallEx("OnMedalAwarded", medal, plr, reward);
	
	// Play a global sound when a medal is rewarded.
	Sound("Rule_Medals::MedalAward", true, 80);
	
	// If logging is active log the rewarding of the medal.
	if (active_rule->GetLogging())
		LogMedalReward(medal, plr);
		
	// Update scoreboard if active.
	if (active_rule->GetScoreboard() && GetPlayerType(plr) == C4PT_User)
		Scoreboard->SetPlayerData(plr, "medals", GetPlayerMedalCount(plr, true));
	return;
}

// Clears the complete medal collection of the player.
public func ClearMedals(int plr)
{
	// Safety check: this is a definition call.
	if (this != Rule_Medals)
	{
		Log("$WarningDefinitionCall$", "ClearMedals", "ClearMedals");
		return;
	}
	// Clear both the stored player data and the round data.
	SetMedalData(plr, "MEDALS__");
	SetRoundMedalData(plr, "MEDALS__");
	return;
}

// Returns a list of the players medals in the format [medal_id, medal_cnt].
// If round_only is true, the medals awarded during this round are returned.
public func GetPlayerMedals(int plr, bool round_only)
{
	// Safety check: this is a definition call.
	if (this != Rule_Medals)
	{
		Log("$WarningDefinitionCall$", "GetPlayerMedals", "GetPlayerMedals");
		return;
	}
	var medal_list = [];
	// Medal data taken from round medals if requested.
	var medal_data = GetMedalData(plr);
	if (round_only)
		medal_data = GetRoundMedalData(plr);
	// Loop over all loaded medals and check the medal count.
	// If non zero add to the players medal list.
	var index = 0, def;
	while (def = GetDefinition(index++))
	{
		if (def->~IsMedal())
		{
			var medal_count = GetMedalCount(medal_data, def->GetMedalIndex());
			if (medal_count > 0)
				PushBack(medal_list, [def, medal_count]);
		}
	}
	return medal_list;
}

// Returns the total amount of medals earned by the player.
// If round_only is true, the medals awarded during this round are returned.
// If for_medal is specified this will only return the count of that medal.
public func GetPlayerMedalCount(int plr, bool round_only, id for_medal)
{
	// Safety check: this is a definition call.
	if (this != Rule_Medals)
	{
		Log("$WarningDefinitionCall$", "GetPlayerMedalCount", "GetPlayerMedalCount");
		return;
	}
	// Medal data taken from round medals if requested.
	var medal_data = GetMedalData(plr);
	if (round_only)
		medal_data = GetRoundMedalData(plr);
	// Count the number of medals by the player.
	var medal_count = 0;
	var index = 0, def;
	while (def = GetDefinition(index++))
		// Add the count if the definition is a medal equal to for_medal if specified.
		if (def->~IsMedal() && (for_medal == nil || def == for_medal))
			medal_count += GetMedalCount(medal_data, def->GetMedalIndex());
	return medal_count;
}

public func SetAwardingPlayerType(int for_type)
{
	award_plr_type = for_type;
	return;
}

public func GetAwardingPlayerType()
{
	return award_plr_type;
}


/*-- Medal Information --*/

// Returns a list of the currently active medals with their ids.
public func GetActiveMedals()
{
	// Safety check: this is a definition call.
	if (this != Rule_Medals)
	{
		Log("$WarningDefinitionCall$", "GetActiveMedals", "GetActiveMedals");
		return;
	}
	var active_medals = [];
	// Loop over all loaded medals and add them to the list.
	// Except for the template medal which is not a real medal.
	var index = 0, def;
	while (def = GetDefinition(index++))
	{
		var medal_index = def->~GetMedalIndex();
		if (def->~IsMedal() && medal_index > 0)
			PushBack(active_medals, {medal = def, index = medal_index});
	}
	// Sort the list according to medal index and then only get the medal id's
	SortArrayByProperty(active_medals, "index");
	for (var index = 0; index < GetLength(active_medals); index++)
		active_medals[index] = active_medals[index].medal;
	return active_medals;
}

// Checks all loaded medals for overlapping and non-valid medal indices. 
private func CheckActiveMedals()
{
	var medals = Rule_Medals->GetActiveMedals();
	// Check for bounds on the medal index.
	for (var medal in medals)
		if (!Inside(medal->GetMedalIndex(), 0, 254))
			Log("$WarningInvalidMedalID$", medal, medal->GetMedalIndex());
	// Check for overlapping indices.
	var medal_ids = [];
	for (var medal in medals)
		PushBack(medal_ids, medal->GetMedalIndex());
	var duplicate_cnt = RemoveDuplicates(medal_ids);
	if (duplicate_cnt > 0)
		Log("$WarningDuplicateMedalID$");
	return;
}


/*-- Medal Data Handling --*/

// Returns the scenario or folder specific medal data identifier.
// This is of the form Medals*, where * is the specific string.
private func GetMedalDataStoreIdentifier()
{
	var add_string = "";
	// First check if the global setting is used.
	var global_id = Global->Call("~GetMedalDataIdentifier");
	if (global_id)
	{
		if (GetType(global_id) == C4V_String)
			add_string = global_id;
		else
			Log("$WarningMedalIdentifierNoString$", global_id);
	}
	// Then check if the scenario local settings is used and overwrite global setting is needed.
	var scenario_id = GameCall("GetMedalDataIdentifier");
	if (scenario_id)
	{
		if (GetType(scenario_id) == C4V_String)
			add_string = scenario_id;
		else
			Log("$WarningMedalIdentifierNoString$", scenario_id);
	}
	return Format("%s%s", "Medals", add_string);
}

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
	// Safety: medal_data must be string.
	if (GetType(medal_data) != C4V_String)
		medal_data = "MEDALS__";
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
	var data;
	// If there is a league try to retrieve it from the league.

	// Not in a league so get via player extra data.
	data = GetPlrExtraData(plr, GetMedalDataStoreIdentifier());
	// Check whether it is a string starting with "MEDALS__".
	var correct_string = "MEDALS__";
	if (GetType(data) != C4V_String)
		return correct_string;
	for (var index = 0; index < GetLength(correct_string); index++)
		if (GetChar(data, index) != GetChar(correct_string, index))
			return correct_string;
	// Data is safe, just return it.
	return data;
}

// Sets the medal data of the player, pass in string format.
// Medal data is a string of 2048 characters starting with "MEDALS__".
private func SetMedalData(int plr, string data)
{
	// If there is a league try to set data in the league.

	// Not in a league so set via player extra data.
	SetPlrExtraData(plr, GetMedalDataStoreIdentifier(), data);
	return;
}

// Returns the medal data of the player for this round only.
private func GetRoundMedalData(int plr)
{
	var rule = FindObject(Find_ID(Rule_Medals));
	if (!rule)
		return;
	return rule->GetRoundPlayerMedalData(plr);
}

// Sets the medal data of the player for this round only.
private func SetRoundMedalData(int plr, string data)
{
	var rule = FindObject(Find_ID(Rule_Medals));
	if (!rule)
		return;
	rule->SetRoundPlayerMedalData(plr, data);
	return;	
}

// Get the player's medal round data from the medal rule object.
public func GetRoundPlayerMedalData(int plr)
{
	var plr_id = GetPlayerID(plr);
	return round_medals[plr_id];
}

// Set the player's medal round data from the medal rule object.
public func SetRoundPlayerMedalData(int plr, string data)
{
	var plr_id = GetPlayerID(plr);
	round_medals[plr_id] = data;
	return;
}


/*-- Medal Logging --*/

public func SetLogging(bool active)
{
	logging_active = active;
	return;
}

public func GetLogging()
{
	return logging_active;
}

// Logs the rewarding of a medal to a player.
private func LogMedalReward(id medal, int to_plr)
{
	// Basic rewarding message.
	var medal_msg = Format("$LogAwardedMedal$", medal, GetTaggedPlayerName(to_plr), medal.Name);
	// Get all the relevant medal data.
	var nr_medals = Rule_Medals->GetPlayerMedalCount(to_plr);
	var nr_medals_round = Rule_Medals->GetPlayerMedalCount(to_plr, true);
	var nr_active_medals = GetLength(Rule_Medals->GetActiveMedals());
	var nr_unique_medals = GetLength(Rule_Medals->GetPlayerMedals(to_plr));
	var nr_medals_thistype = Rule_Medals->GetPlayerMedalCount(to_plr, false, medal);
	// Add extra message if first medal ever or first medal of this round.
	if (nr_medals == 1)
		medal_msg = Format("%s $LogAwardedMedalFirst$", medal_msg, GetTaggedPlayerName(to_plr));
	else if (nr_medals_round == 1)
		medal_msg = Format("%s $LogAwardedMedalFirstRound$", medal_msg, GetTaggedPlayerName(to_plr));
	// Add extra message if the player got awarded all medals, however, at least one.
	else if (nr_unique_medals == nr_active_medals && nr_unique_medals >= 1)
		medal_msg = Format("%s $LogAwardedMedalAllMedals$", medal_msg, GetTaggedPlayerName(to_plr));
	// Add extra message if it is the first medal of this type.
	else if (nr_medals_thistype == 1)
		medal_msg = Format("%s $LogAwardedMedalFirstType$", medal_msg, GetTaggedPlayerName(to_plr));
	// Log the full message.
	return Log(medal_msg);
}


/*-- Medal Rewards --*/

public func SetRewarding(bool active)
{
	reward_active = active;
	return;
}

public func GetRewarding()
{
	return reward_active;
}


/*-- Scoreboard --*/

public func SetScoreboard(bool active)
{
	scoreboard_active = active;
	if (scoreboard_active)
	{
		Scoreboard->Init([
			{key = "medals", title = Medal_Template, sorted = true, desc = true, default = "0", priority = 0}
		]);
	}
	return;
}

public func GetScoreboard()
{
	return scoreboard_active;
}


/*-- Message Board --*/

// Adds message board commands for the medal rule.
private func InitMedalMessageBoard()
{
	// Add a message board command "/medals".
	AddMsgBoardCmd("medals", "Rule_Medals->~ShowPlayerMedals(%player%)");
	return;
}

// Callback from message board command "/medals".
public func ShowPlayerMedals(int plr)
{
	if (plr == NO_OWNER)
		return;
	// Show the medal menu for this player.
	MedalMenu->CreateMedalMenu(plr);
	return;
}

/*-- Evaluation Data --*/

private func AddMedalEvaluationData()
{
	// Get rid of settlement score to not cluther the displaying of the medals.
	HideSettlementScoreInEvaluation(true);
	// Loop over all active players and add their evaluation data.
	for (var plr in GetPlayers(C4PT_User))
		AddPlayerMedalEvaluationData(plr);
	// Add evaluation data only if there is a best player.
	if (round_eval_data.best_plr != NO_OWNER)
	{
		var eval_msg = Format("$EvalDataMostMedals$", round_eval_data.player_name, round_eval_data.most_medals, round_eval_data.total_medals);
		AddEvaluationData(eval_msg, 0);
	}
	return;
}

// Adds the evaluation data for this player.
private func AddPlayerMedalEvaluationData(int plr)
{
	// Gather info relevant for evaluation data.
	var medals_awarded = Rule_Medals->GetPlayerMedalCount(plr, true);
	// Set the eval data of this player.
	var plr_id = GetPlayerID(plr);
	var eval_msg = "$EvalDataNoMedals$";
	if (medals_awarded > 0)
		eval_msg = Format("$EvalDataGotMedals$", medals_awarded);
	AddEvaluationData(eval_msg, plr_id);
	// Update best player if he has the most medals.
	if (medals_awarded > round_eval_data.most_medals)
	{
		round_eval_data.most_medals = medals_awarded;
		round_eval_data.best_plr = plr;
		round_eval_data.total_medals = Rule_Medals->GetPlayerMedalCount(plr);
		round_eval_data.player_name = GetTaggedPlayerName(plr);
	}
	return;
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
