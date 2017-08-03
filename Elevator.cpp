#include "Elevator.h"
#include <stdlib.h>

#define FILE_OUT	"testdata.out"
using namespace std;

// Elevator class

Elevator::Elevator()
	: _time(0), _floor(0)
{
	_passengerQueue = std::vector< std::deque<Passenger> > (NUM_FLOORS);
}



void Elevator::run()
{
	// Prepare
	_time = 0;
	_file.open(FILE_OUT, std::ios::out);


	// Run while people are still waiting
	while (true)
	{
		// Anyone going ON of OFF - takes 25 secs
		int passengersOffOn = 0;
		passengersOffOn += passengersOff();
		passengersOffOn += passengersOn();
		
		if (passengersOffOn > 0)
		{
			_time += STOP_INCR;
		}

		// Updating "last stop" for all passengers in front of their floor queue
		updatePassengerWait();


		// Check to see if we are finished
		//if (queue is empty and elevator is empty) break;

		int nextDest = getNextDest();

		if (nextDest >= 0)
		{
			_time += (abs(_floor - nextDest) * FLOOR_INCR);
			_floor = nextDest;
		}
		else
		{
			// Finished!
			if (_file.is_open())
			{
				_file.close();
				printf("File '%s' created.", FILE_OUT);
			}
			
			break;
		}
	}
}


int Elevator::getNextDest() const
{
	if (!_passengersUp.empty())
	{
		// current direction is up
		int passengerNextDest = _passengersUp.top().getDestination();

		if (_passengersUp.size() == CAPACITY)
		{
			return passengerNextDest;
		}
		else
		{
			// check if anyone between "here" and nextPassengerDest can join in
			for (int i = _floor + 1; i < passengerNextDest; i++)
			{
				if (!_passengerQueue[i].empty() && _passengerQueue[i][0].isUpbound())
				{
					return i;
				}
			}

			return passengerNextDest;
		}
	}
	else if (!_passengersDown.empty())
	{
		// current direction is down
		int passengerNextDest = _passengersDown.top().getDestination();

		if (_passengersDown.size() == CAPACITY)
		{
			return passengerNextDest;
		}
		else
		{
			// check if anyone between "here" and nextPassengerDest can join in
			for (int i = _floor - 1; i > passengerNextDest; i--)
			{
				if (!_passengerQueue[i].empty() && _passengerQueue[i][0].isDownbound())
				{
					return i;
				}
			}

			return passengerNextDest;
		}
	}
	else
	{
		// elevator is empty - must calculate where to get next passenger from
		// The first passenger in each floor has its elevatorTime set to 
		// the last time the elevator stopped in that floor
		
		int nextDest = -1;
		int maxWaiting = 0;

		for (unsigned int i = 0; i < _passengerQueue.size(); i++)
		{
			if (!_passengerQueue[i].empty())
			{
				Passenger p = _passengerQueue[i][0];
				int tmpWaiting = _time - p.getElevatorTime();

				if (tmpWaiting > maxWaiting)
				{
					// have waited longer ...
					maxWaiting = tmpWaiting;
					nextDest = i;
				}
				else if (tmpWaiting == maxWaiting)
				{
					// waiting time is equal, must check distance from current floor
					int tmpDiff = abs(nextDest - _floor) - abs((int)i - _floor);

					if (tmpDiff > 0)
					{
						// equal wait, closer to current floor ...
						nextDest = i;
					}
					//else if (tmpDiff == 0) shall use lowest, nextDest < i as i counts up
				}
			}
		}

		if (nextDest >= 0)
		{
			return nextDest;
		}
	}

	return -1;
}


int Elevator::passengersOff()
{
	int retVal = 0;
	
	if (!_passengersUp.empty())
	{
		while (!_passengersUp.empty() && _passengersUp.top().getDestination() == _floor)
		{
			registerOff(_passengersUp.top());
			_passengersUp.pop();
			retVal++;
		}
	}
	else if (!_passengersDown.empty())
	{
		while (!_passengersDown.empty() && _passengersDown.top().getDestination() == _floor)
		{
			registerOff(_passengersDown.top());
			_passengersDown.pop();
			retVal++;
		}
	}

	return retVal;
}


int Elevator::passengersOn()
{
	int retVal = 0;

	if (!_passengersUp.empty())
	{
		// current direction is UP
		while (_passengersUp.size() < CAPACITY)
		{
			if (!_passengerQueue[_floor].empty() && 
				_passengerQueue[_floor][0].isUpbound())
			{
				_passengerQueue[_floor][0].setElevatorTime(_time);
				_passengersUp.push(_passengerQueue[_floor][0]);
				_passengerQueue[_floor].pop_front();
				retVal++;
			}
			else
			{
				break;
			}
		}
	}
	else if (!_passengersDown.empty())
	{
		// current direction is DOWN
		while (_passengersDown.size() < CAPACITY)
		{
			if (!_passengerQueue[_floor].empty() && _passengerQueue[_floor][0].isDownbound())
			{
				_passengerQueue[_floor][0].setElevatorTime(_time);
				_passengersDown.push(_passengerQueue[_floor][0]);
				_passengerQueue[_floor].pop_front();
				retVal++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		// elevator is empty - first one gets served
		if (!_passengerQueue[_floor].empty())
		{
			Passenger p = _passengerQueue[_floor][0];

			if (p.isUpbound())
			{
				do
				{
					p.setElevatorTime(_time);
					_passengersUp.push(p);
					_passengerQueue[_floor].pop_front();
					retVal++;
					
					if (_passengerQueue[_floor].empty())
					{
						break;
					}

					p = _passengerQueue[_floor][0];

				} while (_passengersUp.size() < CAPACITY && p.isUpbound());
			}
			else
			{
				do
				{
					p.setElevatorTime(_time);
					_passengersDown.push(p);
					_passengerQueue[_floor].pop_front();
					retVal++;

					if (_passengerQueue[_floor].empty())
					{
						break;
					}

					p = _passengerQueue[_floor][0];
					
				} while (_passengersDown.size() < CAPACITY && p.isDownbound());
			}
				
		}
	}

	return retVal;
}


void Elevator::updatePassengerWait()
{
	if (!_passengerQueue[_floor].empty())
	{
		_passengerQueue[_floor][0].setElevatorTime(_time);
	}
}


void Elevator::registerOff(const Passenger& psg)
{
	if (_file.is_open())
	{
		_file << psg.getOrigin() << "->" << psg.getDestination();
		_file << " left=" << psg.getElevatorTime() << "s arrived=" << _time << endl;
	}
	else
	{
		printf("%i->%i left=%is arrived=%is\n", 
			psg.getOrigin(),
			psg.getDestination(),
			psg.getElevatorTime(),
			_time);
	}
}


void Elevator::addToQueue(int fromFloor, int toFloor)
{
	// checking valid values
	if (fromFloor < 0 || toFloor < 0)
	{
		return;
	}

	if (fromFloor >= NUM_FLOORS || toFloor >= NUM_FLOORS)
	{
		return;
	}


	// add to queue	
	if (fromFloor != toFloor)
	{	
		_passengerQueue[fromFloor].push_back(Passenger(fromFloor, toFloor));
	}
}


void Elevator::setFloor(int floor)
{
	if (floor < 0)
	{
		_floor = 0;
	}
	else if (floor >= NUM_FLOORS)
	{
		_floor = NUM_FLOORS - 1;
	}
	else
	{
		_floor = floor;
	}
}


void Elevator::printQueue() const
{
	printf("Passengers in Queue: \n");

	for (unsigned int i = 0; i < _passengerQueue.size(); i++)
	{
		printf("Floor %i: ", i);

		for (unsigned int j = 0; j < _passengerQueue[i].size(); j++)
		{
			printf("%i, ", _passengerQueue[i][j].getDestination());
		}

		printf("\n");
	}
}
