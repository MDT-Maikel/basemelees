// Converts a team number into a base number. This is needed since it is possible that for example the 
// first and the third team are selected and these need to be assigned the first and second base.

// The team to base conversion is stored in a static list.
static team_to_base_list;

// Returns the base number belonging to the given team.
global func TeamToBase(int team)
{
	// Init the team to base list if needed.
	InitTeamToBaseList(team);
	
	// Return the base, team numbers start from one.
	return team_to_base_list[team - 1];
}

// Initializes the list of team to base conversion.
global func InitTeamToBaseList(int team)
{
	// Initialize the list if not done yet.
	if (GetType(team_to_base_list) != C4V_Array)
		team_to_base_list = [];
	// For each team add an entry in the list which gives the base.
	// The teams are given a base in the order this function is called.	
	if (team_to_base_list[team - 1] == nil)
	{
		var base_nr = 0;
		for (var base in team_to_base_list)
			if (base != nil)
				base_nr++;
		team_to_base_list[team - 1] = base_nr;
	}
	return;
}