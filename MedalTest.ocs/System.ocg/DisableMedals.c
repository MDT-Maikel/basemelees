// This disables medals which are shipped with the medal rule so that these id's can be used for new medals.
// Still it is probably better to use unique id's as long as these are available. This also removes medals
// from the medal menu, such that medals irrelevant for this round are not shown to the players.

//#appendto Medal_HatTrick
//#appendto Medal_Miner
//#appendto Medal_Decorated
//#appendto Medal_Kamikaze
//#appendto Medal_Pyromania
//#appendto Medal_CriticalHit
//#appendto Medal_Demolition
//#appendto Medal_Heroic
//#appendto Medal_ChainReaction
//#appendto Medal_Rocketeer
//#appendto Medal_Construction
//#appendto Medal_Plumber
//#appendto Medal_Slasher
//#appendto Medal_Berserker
//#appendto Medal_Superman
//#appendto Medal_CrashPilot
//#appendto Medal_Energizer

// Overloading the IsMedal function to return false suffices.
public func IsMedal() { return false; }
