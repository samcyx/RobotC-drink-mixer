

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

/*
dfdf
*/

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
	motor[motorA] = 10;
	wait1Msec(9000);
	motor[motorA] = 0;
	taskLiquid = true;

}

task powder()//Henon
{

	taskPowder = false;
	if(powderLevel)
	{
		setLEDColor(ledRedFlash);
	}
	else
	{
		setLEDColor(ledOrangeFlash);
	}
	wait1Msec(5000);
	setLEDColor(ledOff);

	taskPowder = true;

}

task stir()//Devin
{
	taskStir = false;
	nMotorEncoder[motorC] = 0;
	motor[motorC] = -50;
	while(abs(nMotorEncoder[motorC])<(360*17.5))
	{}
	motor[motorC] = 0;
	wait1Msec(100);
	motor[motorC] = 50;
	nMotorEncoder[motorC] = 0;
	while(abs(nMotorEncoder[motorC])<(360*17.6))
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

void receiptPrint(int cupCount, int cupTime)
{}

bool receivePowderInput()
{
	bool amount = false;
	bool amountChosen = false;
	bool confirm = false;
	clearScreen();
	displayTextLine(5, "Enter the amount of powder");
	displayTextLine(7, "Press up for strong");
	displayTextLine(8, "Press down for weak");
	while(!confirm)
	{
		if(getButtonPress(buttonUp))
		{
			clearScreen();
			displayTextLine(5, "Press Enter to continue");
			amount = true;
			amountChosen = true;
		}
		else if(getButtonPress(buttonDown))
		{
			clearScreen();
			displayTextLine(5, "Press Enter to continue");
			amount = false;
			amountChosen = true;
		}
		while(getButtonPress(buttonUp)||getButtonPress(buttonDown))
		{}
		while(amountChosen)
		{
			if(getButtonPress(buttonEnter))
			{
				clearScreen();
				while(getButtonPress(buttonEnter)){}
				return amount;
			}
			else if(getButtonPress(buttonUp) || getButtonPress(buttonDown))
			{
				amountChosen = false;
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
			int num = 0;
			if(cups[i].highPowder)
				num = 5;
			displayBigTextLine(2, "%d", num);
			wait1Msec(2000);
			powderLevel = cups[i].highPowder;
			startTask(powder);
		}
		else if(cups[i].curSpot  == 3) //cup at stirring station
			startTask(stir);
		else if(cups[i].curSpot == 4)//cup is at loading station
		{
			cupCount++;
			receiptPrint(cupCount,cups[i].time);
			startTask(fCup);//constantly polls for whether the cup has been removed, then makes fCup true
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
	displayBigTextLine(8,"end");
	while(time1[T2] < 30000)

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
	do{//TODO add watchdog timer to kill program if tasks take too long to execute
			clearScreen();
			initializeTasks();
			wait1Msec(1000);
			curTime = time100[T1];
			while(!taskLiquid || !taskPowder || !taskStir || !finishedCupRemoved)//while the tasks are false and the watch dog hasn't times out
			{}
/*			if(time100[T1] < curTime + 300 && count!=0)
			{
				displayBigTextLine(0,"Unexpected");
				displayBigTextLine(2,"Failure!");
				stopTask(liquid);
				stopTask(powder);
				stopTask(fCup);
				stopTask(stir);
				wait1Msec(5000);
				stopAllTasks();
			}
	*/
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

	stopAllTasks();
}
