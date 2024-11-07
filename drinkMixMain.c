
typedef struct {
    byte curSpot;
    int time;
    bool highPowder;
} Cup;


bool taskLiquid = true, taskPowder= true, taskStir = true;
bool finishedCupRemoved = false;
short lightThreshold;
short cupCount = 0;
short count = 0;


/*
dfdf
*/



int lightDetected(short threshold)
{
	if(SensorValue[S3] > threshold)
		return true;
	return false;
}


Cup cups[4];
task liquid()//Lucas
{}

task powder()//Henon
{}

task stir()//Devin
{}
task fCup()//polls for whether the cup has been removed or not
{

	while(!finishedCupRemoved)
	{
		displayBigTextLine(5, "Please remove your cup");
		if(lightDetected(lightThreshold))
		{
			finishedCupRemoved = true;
		}

	}


}



void configureAllSensors()
{
	SensorType[S1] = sensorEV3_Color;
  SensorMode[S1] = modeEV3Color_Reflected;
	clearTimer(T1);

}

void rotate()//Free
{

}

void receiptPrint(int cupCount, int cupTime)
{}
void receivePowderInput(Cup &c)
{

}


void initializeCup(int count)
{
		Cup c;
		c.curSpot = 0;

		cups[count%4]	= c;
		receivePowderInput(c);
		cups[count%4].time = time1[T1];
}


void initializeTasks()
{
	for(int i = 0; i < 4; i++)
		{
				if(cups[i].curSpot == -1)
					continue;
				else if (cups[i].curSpot == 1)//cup at liquid station
					startTask(liquid);
				else if(cups[i].curSpot  == 2)//cup at powder station
					startTask(powder);
				else if(cups[i].curSpot  == 3) //cup at stirring station
					startTask(stir);
				else if(cups[i].curSpot == 4)//cup is at loading station
				{
					finishedCupRemoved = true;
					cups[i].curSpot = -1;
					cupCount++;
					receiptPrint(cupCount,cups[i].time);

					startTask(fCup);//constantly polls for whether the cup has been removed, then makes fCup true
					cups[i].curSpot = -1;//reset the curSpot attribute of that cup object

					continue;
				}
				cups[i].curSpot++;
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
//To do: start localize variables that aren't the cup array
task main()
{


	configureAllSensors();
	populateCups();
	while(!getButtonPress(buttonEnter))
	{
		if(lightDetected(lightThreshold))//if we detect reflected light
		{
			initializeCup(count);
		}
		count++;
		initializeTasks();

		while(!(((taskLiquid && taskPowder) && taskStir) && finishedCupRemoved))//while the tasks are false
		{}


		stopAllTasks();




		rotate();
	}

}
