/**
	Crash Pilot
	Awarded for surviving a self-caused plane crash.
	
	@author Maikel
*/


/*-- Medal Properties --*/

local Name = "$Name$";
local Description = "$Description$";

public func IsMedal() { return true; }
public func GetMedalIndex() { return 16; }
public func GetMedalReward() { return 25; }


/*-- Medal Scripts --*/

// See Medal_PlaneCrash.c script for handling the airplane.

// Called when the round starts, technically when the rule is created.
public func OnRoundStart()
{
	AddEffect("IntMedalCrashPilot", nil, 100, 0, nil, Medal_CrashPilot);
	return;
}

// Called when the round is finished, i.e. when all players are eliminated
// or all goals are fulfilled.
public func OnRoundFinish()
{
	RemoveEffect("IntMedalCrashPilot", nil);
	return;
}


/*-- Medal Effects --*/

// Medal crash pilot effect keeps track of which players crashed their planes.
public func FxIntMedalCrashPilotStart(object target, proplist fx, int temporary)
{
	if (temporary)
		return FX_OK;
	// Create ejections list.
	fx.ejections = [];
	return FX_OK;
}

// Effect callback made by the plane on pilot ejection.
public func FxIntMedalCrashPilotOnEjection(object target, proplist fx, object plane, object pilot, int controller)
{
	// Store the airplane, pilot and controller in a proplist.
	var ejection = {plane = plane, pilot = pilot, controller = controller, frame = FrameCounter()};
	// Check if the existing ejections already contain this airplane then reset.
	var existing = false;
	for (var index = 0; index < GetLength(fx.ejections); index++)
	{
		if (fx.ejections[index].plane == plane)
		{
			fx.ejections[index] = ejection;
			existing = true;
			break;
		}
	}
	// Otherwise add ejection to the list.
	if (!existing)
		PushBack(fx.ejections, ejection);
	return FX_OK;
}

// Effect callback made by the plane on its crash.
public func FxIntMedalCrashPilotOnCrash(object target, proplist fx, object plane, object pilot, int controller)
{
	// Scan through the list of ejections to see if this plane had one.
	for (var index = 0; index < GetLength(fx.ejections); index++)
	{
		var ejection = fx.ejections[index];
		if (ejection.plane == plane)
		{
			// Check if there is no pilot, the controller is the same and if the ejection was less than five seconds ago.
			if (!pilot && ejection.controller == controller && FrameCounter() - ejection.frame < 5 * 36)
			{
				// Then we can award the medal for a successful ejection before a crash for the controller.
				Rule_Medals->AwardMedal(Medal_CrashPilot, controller);
			}
			// Always remove this entry from the list since the plane died.
			RemoveArrayIndex(fx.ejections, index);
			break;		
		}
	}
	// If the plane is not found in the list of ejections, don't do anything.
	return FX_OK;
}

// Medal crash pilot effect keeps track of which players crashed their planes.
public func FxIntMedalCrashPilotStop(object target, proplist fx, int reason, bool temporary)
{
	if (temporary)
		return FX_OK;
	return FX_OK;
}
