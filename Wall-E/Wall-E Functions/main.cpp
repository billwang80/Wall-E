/*
green - cans
red - juice boxes
blue - bottle


*/
bool isGrabbing()
{
    return SensorValue[S2] == (int)colorGreen || SensorValue[S2] == (int)colorBlue || SensorValue[S2] == (int)colorRed;
}

void driveDist(float distance)
{
    nMotorEncoder[motorA] = 0;
    if(distance > 0)
        motor[motorA] = motor[motorD] = 30;

    if(distance < 0)
        motor[motorA] = motor[motorD] = -30;

    while(abs(nMotorEncoder[motorA]*PI*2.75/180) < abs(distance))
    {}
    motor[motorA] = motor[motorD] = 0;
}

bool grabReturn()
{
    nMotorEncoder[motorB] = 0;
    motor[motorB] = 15;
    while(nMotorEncoder[motorB] < 60)
    {}
    motor[motorB] = 0;
    return isGrabbing();
}

void clawPlace()
{
    nMotorEncoder[motorB] = 60;
    motor[motorB] = -15;
    while(nMotorEncoder[motorB] > 0)
    {}
    motor[motorB] = 0;
}

void driveReturn(float xPosition, float yPosition, int colour, int & failCount)
{
    float distance = sqrt(xPosition*xPosition + yPosition*yPosition);
    float angle = acos(xPosition/distance);
    turn(angle);
    driveDist(distance);
    if(grabReturn() == false)
        failCount++;
    driveDist(-distance);
    if(// COLOUR TO SEPARATE INTO PILES)
    {
        turn(90-angle);
        driveDist(20);
    }
    else if(// COLOUR TO SEPARATE INTO PILES)
    {
        turn(-90-angle);
        driveDist(20);
    }
    // organize garbage and bottles and stuff
}

/*
task main()
{

    while(!getButtonPress(Enter))
    {}
    while(getButtonPress(Enter))
    {}
    time1[t1] = 0;

}
*/
