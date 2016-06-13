// Elevator class
#ifndef Elevator_h_IS_INCLUDED
#define Elevator_h_IS_INCLUDED

#define DOWN		0
#define UP			1
#define NUM_FLOORS	10 // i.e. 0 - 9
#define FLOOR_INCR	5
#define STOP_INCR	25
#define CAPACITY	4

#include <vector>
#include <deque>
#include <queue>
#include <iostream>
#include <fstream>
#include "Passenger.h"


// Defining compare functions to be used on Passenger priority_queue
struct ElevatorUpQueue
{
	bool operator()(Passenger const& pA, Passenger const& pB)
	{
		return pA.getDestination() > pB.getDestination();
	}
};

struct ElevatorDownQueue
{
	bool operator()(Passenger const& pA, Passenger const& pB)
	{
		return pA.getDestination() < pB.getDestination();
	}
};


// Elevator definition

class Elevator
{
private:
	int _time;
	int _floor;
	std::vector< std::deque<Passenger> > _passengerQueue; // waiting for elevator
	std::priority_queue<Passenger, std::vector<Passenger>, ElevatorUpQueue> _passengersUp; // in elevator, upbound
	std::priority_queue<Passenger, std::vector<Passenger>, ElevatorDownQueue> _passengersDown; // in elevator, downbound
	std::ofstream _file;

	int passengersOff();
	int passengersOn();
	void updatePassengerWait();

	int getNextDest() const;
	void registerOff(const Passenger& psg); // writing to file

public:

	Elevator();

	void run();

	void setFloor(int floor);
	void addToQueue(int fromFloor, int toFloor);


	// for debug
	void printQueue() const;
};

#endif
