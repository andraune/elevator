#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "Elevator.h"


#define FILE_IN			"testdata.in"
#define FILE_NOT_FOUND	1
#define INVALID_FORMAT	2


// function declarations
int readTestInput(Elevator &elev);



// Entry point
int main ()
{
	// Elevator application
	Elevator elevator;


	// Read test input file
	if (0 != readTestInput(elevator))
	{
		printf("Failed to read file '%s'\n", FILE_IN);
		printf("\nPress ANY key to exit ...");
		std::cin.get();
		return 1;
	}

	// Run elevator
	elevator.run();


	// Done!
	//printf("\nPress ANY key to exit ...");
	//std::cin.get();
	return 0;
}


// function definitions

int readTestInput(Elevator &elev)
{
	std::ifstream file(FILE_IN, std::ios::in);

	if (!file.fail())
	{
		int returnCode = 0;
		std::string line;

		// First line: finding elevator's initial floor
		// E:<floor no.>

		if (std::getline(file, line))
		{
			std::stringstream ss(line);
			char e, delim;
			int initFloor;

			if ((ss >> e >> delim >> initFloor) && delim == ':')
			{
				elev.setFloor(initFloor);
			}
			else
			{
				returnCode = INVALID_FORMAT;
			}
		}


		// Lines succeeding the first: Passenger info, one line per floor
		// <floor no>:<passenger1 dest.>,<passenger2 dest.>,...,<passengerN dest>

		while (0 == returnCode && std::getline(file, line))
		{
			std::stringstream lineSS(line);
			char delim;
			int floor;
			std::string passengerList;

			if ((lineSS >> floor >> delim >> passengerList) && delim == ':')
			{	
				std::stringstream listSS(passengerList);
				std::string passengerDest;

				while(std::getline(listSS, passengerDest, ','))
				{
					int destFloor = atoi(passengerDest.c_str());
					
					// add person to queue
					elev.addToQueue(floor, destFloor);
				}
			}
			else
			{
				returnCode = INVALID_FORMAT;
			}
		}

		file.close();
		return returnCode;
	}

	return FILE_NOT_FOUND;
}

