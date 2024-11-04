task stir()//Devin
{
	taskStir = false;
	nMotorEncoder[motorA] = 0;
	motor[motorA] = -50;
	while(abs(nMotorEncoder[motorA])<(360*18))
	{}
	motor[motorA] = 0;
	wait1Msec(100);
	motor[motorA] = 50;
	nMotorEncoder[motorA] = 0;
	while(abs(nMotorEncoder[motorA])<(360*18))
	{}
	motor[motorA] = 0;
	taskStir = true;
}
