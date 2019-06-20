/*
green - cans
red - cartons
blue - bottles
black - garbage

start robot from between wheels
*/
#include "PC_FileIO.c"

const int MAX_SIZE = 15;
const int CAN_DIST = -60;
const int BOTTLE_DIST = -20;
const int CARTON_DIST = -100;
const int GARBAGE_DIST = -40;

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

bool isGrabbing()
{
	return SensorValue[S2] == (int)colorGreen || SensorValue[S2] == (int)colorBlue || SensorValue[S2] == (int)colorRed;
}

// our mechanical design has negative values for drive motors
void driveDistSearch(float distance)
{
	nMotorEncoder[motorA] = 0;
	if(distance > 0)
		motor[motorA] = motor[motorD] = -20;

	while(abs(nMotorEncoder[motorA]*PI*2.75/180) < abs(distance))
	{
		while(SensorValue[S3] <= 18)
		{
			motor[motorA] = motor[motorD] = 0;
		}
		motor[motorA] = motor[motorD] = -20;
	}
	motor[motorA] = motor[motorD] = 0;
}

void drive(float distance)
{
	nMotorEncoder[motorA] = 0;
	if(distance > 0)
		motor[motorA] = motor[motorD] = -20;

	if(distance < 0)
		motor[motorA] = motor[motorD] = 20;

	while(abs(nMotorEncoder[motorA]*PI*2.75/180) < abs(distance))
	{}
	motor[motorA] = motor[motorD] = 0;
}

void turn(int angle)
{
	resetGyro(S1);

	if(angle > 0)
	{
		motor[motorA] = 5;
		motor[motorD] = -1 * motor[motorA];

		while(SensorValue(S1) < angle)
		{}

		motor[motorA] = motor[motorD] = 0;
	}
	else
	{
		motor[motorA] = -5;
		motor[motorD] = -1 * motor[motorA];

		while(SensorValue(S1) > angle)
		{}

		motor[motorA] = motor[motorD] = 0;
	}
}

void clawPlace()
{
	nMotorEncoder[motorB] = 0;
	motor[motorB] = -15;
	while(nMotorEncoder[motorB] > -350)
	{}
	motor[motorB] = 0;
}

bool grabReturn()
{
	nMotorEncoder[motorB] = 0;
	motor[motorB] = 15;
	while(nMotorEncoder[motorB] < 350)
	{}
	motor[motorB] = 0;
	if (!isGrabbing())
	{
		wait1Msec(100);
		clawPlace();
	}
	return isGrabbing();
}

void organizeWaste(int & xColour)
{
	int backDistance = 0;
	int sideDistance = xColour;
	int degrees = 0;
	if(SensorValue[S2] == (int)colorGreen)
	{
		backDistance = CAN_DIST;
		degrees = 90;
	}
	if(SensorValue[S2] == (int)colorBlue)
	{
		backDistance = BOTTLE_DIST;
		degrees = 90;
	}
	if(SensorValue[S2] == (int)colorRed)
	{
		backDistance = CARTON_DIST;
		degrees = 90;
	}
	if(SensorValue[S3] == (int)colorBlack)
	{
		backDistance = GARBAGE_DIST;
		degrees = -90;
	}

	drive(backDistance);
	wait1Msec(50);
	turn(degrees);
	wait1Msec(50);
	drive(sideDistance);
	if(SensorValue[S2] != (int)colorBlack)
		xColour -= 7.5;
	clawPlace();
	drive(-sideDistance);
	wait1Msec(50);
	turn(-degrees);
	wait1Msec(50);
	drive(-backDistance);
}

void driveReturn(float xPosition, float yPosition, int & failCount)
{
	float distance = sqrt(pow(xPosition, 2) + pow(yPosition, 2));
	float angle = 0;

	if(xPosition > 0)
	{
		angle = 90-180*asin(yPosition/distance)/PI;
	}
	else
	{
		angle = -90+180*asin(yPosition/distance)/PI;
	}

	turn(angle);
	wait1Msec(50);
	driveDistSearch(distance);
	if(grabReturn() == false)
		failCount++;
	drive(-distance);
	wait1Msec(50);
	turn(-angle);
}

task main()
{
	SensorType[S1] = sensorEV3_Gyro;
	SensorType[S2] = sensorEV3_Color;
	SensorType[S3] = sensorEV3_Ultrasonic;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Gyro_Angle;
	SensorMode[S2] = modeEV3Color_Color;
	wait1Msec(50);
	resetGyro(S1);

	int failCount = 0;
	int xCan = 45;
	int xBottle = 45;
	int xCarton = 45;
	int xGarbage = -30;

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

	while(!getButtonPress(buttonEnter))
	{}
	while(getButtonPress(buttonEnter))
	{}
	time1[T1] = 0;

	int itemCounter = 0;

	while(time1[T1] < 300000 && itemCounter < numEntries)
	{
		driveReturn(xLocations[itemCounter], yLocations[itemCounter], failCount);

		if(SensorValue[S2] == (int)colorGreen)
			organizeWaste(xCan);
		if(SensorValue[S2] == (int)colorBlue)
			organizeWaste(xBottle);
		if(SensorValue[S2] == (int)colorRed)
			organizeWaste(xCarton);
		if(SensorValue[S2] == (int)colorBlack)
			organizeWaste(xGarbage);

		itemCounter++;
	}

	displayString(3, "%d items were missed", failCount);
	wait1Msec(10000);
}
