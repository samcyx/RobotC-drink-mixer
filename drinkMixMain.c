
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
	if(SensorValue[S3] > threshold)
		return true;
	return false;
}



task liquid()//Lucas
{
	taskLiquid = false;
	motor[motorA] = 10;
	wait1Msec(9000);
	motor[motorA] = 10;
	taskLiquid = true;

}

task powder()//Henon
{

	taskPowder = false;
	wait1Msec(5000);
	setLEDColor(ledRedFlash);

	taskPowder = true;

}

task stir()//Devin
{
	taskStir = false;
	nMotorEncoder[motorC] = 0;
	motor[motorC] = -50;
	while(abs(nMotorEncoder[motorC])<(360*18))
	{}
	motor[motorC] = 0;
	wait1Msec(100);
	motor[motorC] = 50;
	nMotorEncoder[motorC] = 0;
	while(abs(nMotorEncoder[motorC])<(360*18))
	{}
	motor[motorC] = 0;
	taskStir = true;
}
task fCup()//polls for whether the cup has been removed or not
{

	finishedCupRemoved = false;
	while(!finishedCupRemoved)
	{
		displayBigTextLine(5, "Please remove your cup");
		if(lightDetected(lightThreshold))
		{
			finishedCupRemoved = true;
		}

	}


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
	while((abs(nMotorEncoder[motorD])<(90*5-20)))
	{
		motor[motorD] = -30+ (30*(abs(nMotorEncoder[motorD])/(90*5)));
	}
	motor[motorD] = 0;
	for(int i = 0; i < 4; i++)
	{
		if(!cups[i].curSpot!= -1)
			cups[i].curSpot++;
	}
}

void receiptPrint(int cupCount, int cupTime)
{}
bool receivePowderInput()
{
	bool amount = false;
	bool amountChosen = false;
	bool confirm = false;
	displayTextLine(5, "Enter the amount of powder");
	displayTextLine(7, "Press up for strong");
	displayTextLine(8, "Press down for weak");
	while(!confirm)
	{
		if(getButtonPress(buttonUp))
		{
			displayTextLine(5, "Press Enter to continue");
			amount = true;
			amountChosen = true;
		}
		else if(getButtonPress(buttonDown))
		{
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
	Cup c;
	c.curSpot = 0;


	c.highPowder = receivePowderInput();
	cups[count%4]	= c;
	cups[count%4].time = time1[T1];
}




void initializeTasks()
{
	for(int i = 0; i < 4; i++)
	{

		if (cups[i].curSpot == 1)//cup at liquid station
			startTask(liquid);
		else if(cups[i].curSpot  == 2)//cup at powder station
			startTask(powder);
		else if(cups[i].curSpot  == 3) //cup at stirring station
			startTask(stir);
		else if(cups[i].curSpot == 4)//cup is at loading station
		{
			finishedCupRemoved = false;
			cups[i].curSpot = -1;
			cupCount++;
			receiptPrint(cupCount,cups[i].time);

			startTask(fCup);//constantly polls for whether the cup has been removed, then makes fCup true
			cups[i].curSpot = -1;//reset the curSpot attribute of that cup object
		}

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

	while(time1[T2] < 30000)

	{

		if(lightDetected(lightThreshold))

		{

			return true;

		}

	}
	return false;
}

void initialCupSequence()
{
	configureAllSensors();
	populateCups();
	displayBigTextLine(2,"Please put your\n cup in the\n receptacle");
	displayBigTextLine(4,"cup in the");
	displayBigTextLine(6,"receptacle");
	while(!lightDetected(lightThreshold)){}//wait for cup to be placed
	clearScreen();
	initializeCup(count);//initialize the cup we have detected
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




	//TO-DO: add display text line



	while(cupsExist()){//TODO add watchdog timer to kill program if tasks take too long to execute

		count++;
		initializeTasks();

		while(!(((taskLiquid && taskPowder) && taskStir) && finishedCupRemoved))//while the tasks are false
		{}
		displayBigTextLine(6,"enter %d", count);
		rotate();

		if(lightDetected(lightThreshold))//if we detect reflected light
		{
			initializeCup(count);
		}

		stopTask(liquid);
		stopTask(powder);
		stopTask(fCup);
		stopTask(stir);




		rotate();
	}

}
