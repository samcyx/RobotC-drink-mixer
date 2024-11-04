task liquidDispense()
{
	taskLiquid = false;

	const int LIQVOL = 250; //volume in mL
	const float LIQCONV = //mL into encoder counts

	nMotorEncoder(motorA) = 0;

	motor[motorA] = 100;

	while (abs(nMotorEncoder(motorA)) < LIQVOL * LIQCONV)
	{}

	motor[motorA] = 0;

	taskLiquid = true;
}

#include "PC_FileIO.c"

TFileHandle reciept;
bool fileOkay = openWritePC(reciept, "reciept.txt");

void recieptPrint (short cupCount, int time)
{
	string cup = "cup ";

	writeTextPC(reciept, cup);

	writeLongPC(reciept, cupCount);

	writeLongPC{reciept, time);

	writeEndlPC(reciept);
}
