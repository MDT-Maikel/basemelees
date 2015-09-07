/**
	LogSpecial Library
	Special logging system for global messages, visible to all players.

	@author Maikel
*/


// Logs the message for given duration, defaults to 2 times message length.
global func LogSpecial(string message, int duration)
{
	return Library_LogSpecial->AddMessage(message, duration);
}

// Definition call to log a message.
public func AddMessage(string message, int duration)
{
	// If duration is not given, set a minimum time to show the message.
	if (duration == nil)
		duration = Max(36, 2 * GetLength(message));
	// Find or create the effect which handles the logging.
	var effect = GetEffect("IntSpecialLog");
	if (!effect)
		effect = AddEffect("IntSpecialLog", nil, 100, 0, nil, Library_LogSpecial);
	// Pass the message to the effect.	
	EffectCall(nil, effect, "AddMessage", message, duration);
	return;
}


/*-- Logging Effect --*/

// Effect for logging messages.
public func FxIntSpecialLogStart(object target, proplist effect, int temporary)
{
	if (temporary)
		return FX_OK;
	// Make sure the effect interval is every two frames.
	effect.Interval = 2;			
	// Init an array to store messages.
	effect.messages = [];
	// Create a menu which displays the messages.
	effect.menu_target = CreateObject(Dummy);
	effect.menu_target.Visibility = VIS_All;
	effect.menu_target.Plane = 1000;
	return FX_OK;
}

// Effect for logging messages.
public func FxIntSpecialLogAddMessage(object target, proplist effect, string message, int duration)
{
	// Message properties are stored in a proplist.
	var prop_message = { text = message, time = duration };
	// Add the message to the array of messages.
	PushBack(effect.messages, prop_message);
	return;
}

// Effect for logging messages.
public func FxIntSpecialLogTimer(object target, proplist effect, int time)
{
	var nr_messages = Min(5, GetLength(effect.messages));
	for (var i = 0; i < nr_messages; i++)
	{
		var prop_message = effect.messages[i];
		if (!prop_message.menu)
		{
			prop_message.menu = DisplayMessage(effect.menu_target, prop_message.text, i);
			prop_message.index = i;	
		}
		else if (prop_message.index != i)
		{
			RemoveMessage(prop_message.menu);
			prop_message.menu = DisplayMessage(effect.menu_target, prop_message.text, i);
			prop_message.index = i;	
		}
		// Decrease message time counter.	
		prop_message.time -= 2;

		// Remove the message if the show time has passed.
		if (prop_message.time <= 0)
		{
			RemoveMessage(prop_message.menu);
			effect.messages[i] = nil;
		}
	}
	RemoveHoles(effect.messages);
	return FX_OK;
}


/*-- Logging Menu --*/

// Displays a message at a different location depending on message_nr.
private func DisplayMessage(object target, string message, int message_nr)
{
	var length = GetLength(message);
	var menu =
	{
		Target = target,
		Style = GUI_Multiple,
		Left = "0.25em",
		Right = Format("%dem", length),
		Top = Format("%dem", 6 + message_nr),
		Bottom = Format("%dem", 6 + (message_nr + 1)),
		BackgroundColor = {Std = 0x00000000},
		Text = message,
	};
	var menu_id = GuiOpen(menu);
	menu.ID = menu_id;
	return menu;
}

// Removes a message created by DisplayMessage.
private func RemoveMessage(proplist menu)
{
	GuiClose(menu.ID, nil, menu.Target);
	return;
}
