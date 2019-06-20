#include "PC_FileIO.c"

const int MAX_SIZE = 15;

int fileToArrays(TFileHandle & in, float * locationsX, float * locationsY)
{
	int numberOfEntries = 0;

	readIntPC(in, numberOfEntries);

	float xValue = 0, yValue = 0;
	int index = 0;

	while(readFloatPC(in, xValue))
	{
		readFloatPC(in, yValue);

		locationsX[index] = xValue;
		locationsY[index] = yValue;

		index++;
	}

	return numberOfEntries;
}

task main()
{
	TFileHandle fin;
	bool fileOkay = openReadPC(fin, "locations.txt");

	float xLocations[MAX_SIZE], yLocations[MAX_SIZE];

	for (int count = 0; count < MAX_SIZE; count++)
	{
		xLocations[count] = 0;
		yLocations[count] = 0;
	}

	if (fileOkay)
		int numEntries = fileToArrays(fin, xLocations, yLocations);


	displayString(0, "%2f", numEntries);

	for (int index = 1; index <= numEntries; index++)
		displayString(index, "%2f %2f", xLocations[index-1], yLocations[index-1]);

	wait1Msec(100000);

}
