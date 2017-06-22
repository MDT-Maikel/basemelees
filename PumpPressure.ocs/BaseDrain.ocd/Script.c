/**
	Base Drain
	Liquids must be pumped into this object to be transferred to the enemy base.
	
	@author Maikel
*/

#include Library_Tank

public func Initialize()
{
	SetClrModulation(GetPlayerColor(GetOwner()));
	return;
}


/*-- Pipeline --*/

public func GetLiquidContainerMaxFillLevel(liquid_name)
{
	return 10**9;
}

public func IsLiquidContainerForMaterial(string liquid)
{
	// Accepts all liquids
	return true;
}

public func QueryConnectPipe(object pipe)
{
	if (pipe->IsDrainPipe() || pipe->IsNeutralPipe())
	{
		return false;
	}
	else
	{
		pipe->Report("$MsgPipeProhibited$");
		return true;
	}
}

public func OnPipeConnect(object pipe, string specific_pipe_state)
{
	SetNeutralPipe(pipe);
	pipe->Report("$MsgConnectedPipe$");
}


/*-- Properties --*/

local Name = "$Name$";
local Description = "$Description$";