// Passenger class

#ifndef Passenger_h_IS_INCLUDED
#define Passenger_h_IS_INCLUDED


class Passenger
{
private:
	int _orig;
	int _dest;
	int _lastStop;

public:
	Passenger(): _orig(0), _dest(0), _lastStop(0) {}

	Passenger(int origin, int destination)
		: _orig(origin), _dest(destination), _lastStop(0)
	{}

	int getOrigin() const { return _orig; }
	int getDestination() const { return _dest; }
	int getElevatorTime() const { return _lastStop; }
	
	bool isUpbound() const { return _orig < _dest; }
	bool isDownbound() const { return _orig> _dest; }

	void setElevatorTime(int time) { _lastStop = time; }
};


#endif
