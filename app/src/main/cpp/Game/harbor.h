#ifndef PHEROES_GAME_HARBOR_H_
#define PHEROES_GAME_HARBOR_H_

// an interface to an object that can build ships (e.g. castle or harbor visitable)
class iShip;
class IHarbor {
public:
	enum State {
		HS_NA = 0,
		HS_BUILD,
		HS_OCCUPY,
		HS_OCCUPIED
	};

public:
	virtual void ShipBought(uint8 dock) =0; // informs the structure that ship was bought in a dock
	virtual uint8 ShipsAvailable(uint8 dock) =0; // returns the number of ships available in the dock
	virtual iShip* GetShip( uint8 dockType ) =0; // returns the ship in the selected dock or NULL if none or unsupported
	virtual iPoint GetShipPos( uint8 dockType ) =0; // returns the ship position for the selected dock
	virtual bool IsShipBuildSupported() =0; // returns if the structure can build ships
	virtual uint8 NumOfDocks() =0; // returns number of docks available
	virtual iPlayer* GetOwner() =0; // get owner of the structure
	virtual bool IsBoardable() = 0; // returns if the harbor has the ability to board hero directly from it

};

#endif //PHEROES_GAME_HARBOR_H_
