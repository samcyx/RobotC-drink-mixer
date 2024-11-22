#include "PC_FileIO.c"


TFileHandle reciept;
bool fileOkay = openWritePC(reciept, "reciept.txt");

typedef struct {
	byte curSpot;
	int time;
	bool highPowder;
} Cup;


bool taskLiquid = true, taskPowder= true, taskStir = true;
bool finishedCupRemoved = true;
const short lightThreshold = 30;
short cupCount = 0;
short count = 0;
bool powderLevel = false;
Cup cups[4];


void clearScreen()
{
	for(int i = 0; i < 14;i++)
	{
		displayTextLine(i,"");
	}
}

bool lightDetected(short threshold)
{
	if(SensorValue[S1] > threshold)
		return true;
	return false;
}



task liquid()//Lucas
{
	taskLiquid = false;
	motor[motorA] = 100;
	wait1Msec(65000);
	motor[motorA] = 0;
	taskLiquid = true;

}

task powder()//Henon
{
	taskPowder = false;
  const int HIGH_POWDER_LVL = 1.5;
  const int LOW_POWDER_LVL = 0.75;
  const int CONVFACTOR = 360;
  int powderLVL = 0;
  if (powderLevel)
  	powderLVL = HIGH_POWDER_LVL;
 	else
 		powderLVL = LOW_POWDER_LVL;

 	nMotorEncoder[motorB] = 0;
  motor[motorB] = 20;
	wait1Msec(50); //Slight delay to take load off CPU
  while (abs(nMotorEncoder[motorB]) < powderLVL*CONVFACTOR) {}
	motor[motorB] = 0;  // Stop the motor
	motor[motorB] = -20;
	wait1Msec(500);
	motor[motorB] = 0;
	taskPowder = true;

}

task stir()//Devin
{
	taskStir = false;
	nMotorEncoder[motorC] = 0;
	motor[motorC] = -75;
	while(abs(nMotorEncoder[motorC])<(360*18))
	{}
	motor[motorC] = 0;
	wait1Msec(100);

	for (int i = 0; i < 7; i++) //up down once bottomed out
	{
		motor[motorC] = 100;
		wait1Msec(1000);
		motor[motorC] = 0;
		wait1Msec(100);
		motor[motorC] = -100;
		wait1Msec(1000);
		motor[motorC] = 0;
		wait1Msec(100);
	}

	motor[motorC] = 75;
	nMotorEncoder[motorC] = 0;
	while(abs(nMotorEncoder[motorC])<(360*22))
	{}
	motor[motorC] = 0;
	taskStir = true;
}

task fCup()//polls for whether the cup has been removed or not
{

	finishedCupRemoved = false;
	clearScreen();
	displayBigTextLine(2, "Please remove");
	displayBigTextLine(4, "your cup");
	wait1Msec(1000);
	while(lightDetected(lightThreshold))
	{}
	clearScreen();
	finishedCupRemoved = true;
}



void configureAllSensors()//make sure everything aligns with sensor
{
	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Reflected;
	wait1Msec(50);
	nMotorEncoder[motorA] = nMotorEncoder[motorB] = nMotorEncoder[motorC] = nMotorEncoder[motorD] = 0;
	wait1Msec(50);
	clearTimer(T1);
}

void rotate()//Devin
{
	nMotorEncoder[motorD] = 0;
	motor[motorD] = -30;
	while((abs(nMotorEncoder[motorD])<(90*5)))
	{
		motor[motorD] = -30+ (30*(abs(nMotorEncoder[motorD])/(90*5)));
	}
	motor[motorD] = 0;
	for(int i = 0; i < 4; i++)
	{
		if(cups[i].curSpot!= -1)
		{
			cups[i].curSpot++;
		}
	}
}

void receiptPrint (short cupCount, float time, bool powd)
{
	string cup = "Drink ";
	string took = " took ";
	string sec = " seconds to make a ";
	string level;
	if(powd)
		level = "strong drink.";
	else
		level = "weak drink.";

	writeTextPC(reciept, cup);

	writeLongPC(reciept, cupCount);

	writeTextPC(reciept, took);

	writeLongPC(reciept, time);

	writeTextPC(reciept, sec);

	writeTextPC(reciept, level);

	writeEndlPC(reciept);
}


bool receivePowderInput()
{
	bool amount = false;
	bool amountChosen = false;
	bool confirm = false;
	clearScreen();
	displayBigTextLine(3, "Enter the");
	displayBigTextLine (5, "powder amount");
	displayBigTextLine(7, "Up for strong");
	displayBigTextLine(9, "Down for weak");
	while(!confirm)
	{
		if(getButtonPress(buttonUp))
		{
			clearScreen();
			displayBigTextLine(2, "Press Enter");
			displayBigTextLine (4, "to confirm");
			displayBigTextLine(6, "Press other");
			displayBigTextLine(8, "to cancel");
			amount = true;
			amountChosen = true;
			while(getButtonPress(buttonUp)){}
		}
		else if(getButtonPress(buttonDown))
		{
			clearScreen();
			displayBigTextLine(2, "Press Enter");
			displayBigTextLine (4, "to confirm");
			displayBigTextLine(6, "Press other");
			displayBigTextLine(8, "to cancel");
			amount = false;
			amountChosen = true;
			while(getButtonPress(buttonDown))
			{}
		}

		while(amountChosen)
		{
			if(getButtonPress(buttonEnter))
			{
				clearScreen();
				while(getButtonPress(buttonEnter)){}
				return amount;
			}
			else if(getButtonPress(buttonAny) && !getButtonPress(buttonEnter))
			{
				clearScreen();
				amountChosen = false;
				while(getButtonPress(buttonAny)){}
				displayBigTextLine(3, "Enter the");
				displayBigTextLine (5, "powder amount");
				displayBigTextLine(7, "Up for strong");
				displayBigTextLine(9, "Down for weak");
			}
		}
	}
	return false;
}

void initializeCup(int count)
{
	int index = count%4;
	Cup c;
	c.curSpot = 0;
	bool x = false;

	x = receivePowderInput();
	c.highPowder = x;
	cups[index]	= c;
	cups[index].highPowder = c.highPowder;
	cups[index].time = time1[T1];
}




void initializeTasks()
{
	for(int i = 0; i < 4; i++)
	{

		if (cups[i].curSpot == 1)//cup at liquid station
			startTask(liquid);
		else if(cups[i].curSpot  == 2)//cup at powder station
		{
			powderLevel = cups[i].highPowder;
			startTask(powder);
		}
		else if(cups[i].curSpot  == 3) //cup at stirring station
			startTask(stir);
		else if(cups[i].curSpot == 4)//cup is at loading station
		{
			cupCount++;
			float timeSec = (time1[T1] - cups[i].time)/1000;
			startTask(fCup);//constantly polls for whether the cup has been removed, then makes fCup true
			receiptPrint(cupCount,timeSec, cups[i].highPowder);
			cups[i].curSpot = -1;//reset the curSpot attribute of that cup object
		}
		else
		{}

	}

}

void populateCups()
{

	for(int i = 0; i < 4; i++)
	{
		Cup c;
		c.curSpot = -1;

		cups[i] = c;
	}
}

bool cupsOnTray()
{

	for(int i = 0; i<4; i++)
	{
		if(cups[i].curSpot != -1)
		{
			return true;
		}
	}
	return false;
}


bool cupsExist()

{

	for(int i = 0; i<4; i++)

	{
		if(cups[i].curSpot != -1)
		{
			return true;
		}
	}

	clearTimer(T2);
	clearScreen();
	displayBigTextLine(2, "Enter a new");
	displayBigTextLine(4, "drink or");
	displayBigTextLine(6, "process will");
	displayBigTextLine(8,"terminate");
	while(time1[T2] < 15000)
	{
		if(lightDetected(lightThreshold))
		{
			clearScreen();
			return true;
		}
	}
	return false;
}

void initialCupSequence()
{
	configureAllSensors();
	populateCups();
	displayBigTextLine(2,"Please put your");
	displayBigTextLine(4,"cup in the");
	displayBigTextLine(6,"receptacle");
	while(!lightDetected(lightThreshold)){}//wait for cup to be placed
	clearScreen();
	displayBigTextLine(2,"Press the ");
	displayBigTextLine(4,"center button");
	displayBigTextLine(6,"to begin! :)");
	while(!getButtonPress(buttonEnter))
	{}
	while(getButtonPress(buttonEnter))
	{}
	clearScreen();
}

//To do: start localize variables that aren't the cup array
task main()
{
	initialCupSequence();




	clearTimer(T1);

	float curTime = time1[T1]*100;
	do{
			clearScreen();
			initializeTasks();
			wait1Msec(1000);
			curTime = time100[T1];
			while(!taskLiquid || !taskPowder || !taskStir || !finishedCupRemoved)//while the tasks are false and the watch dog hasn't times out
			{}

			wait1Msec(500);
			if(lightDetected(lightThreshold))//if we detect reflected light
			{
				initializeCup(count);
			}
			count++;
			stopTask(liquid);
			stopTask(powder);
			stopTask(fCup);
			stopTask(stir);
			if(cupsOnTray())
			{
				rotate();
			}
		}
		while(cupsExist());
	closeFilePC(reciept);
	stopAllTasks();
}
