#ifndef ENUMS_H
#define ENUMS_H
enum {
    // GS - Game State
    GSExit = 2,
    GSStartNewGame = 1,
    GSLoadGame = 3,

    // MT - Movement Type
    MTTeleport = 0,
    MTStandard = 1,
    MTHandV = 2,

    //SS - Selection State
    SSNone = 0,
    SSSelected = 1,
    SSBlocked = -1
};
#endif // ENUMS_H
