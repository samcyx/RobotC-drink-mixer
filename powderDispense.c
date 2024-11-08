task powder() { 
	taskPowder = false;
	nMotorEncoder[motorC] = 0;
	wait1MS(1000);
	
	motor[motorC] = 10;  //start mot

   	while (nMotorEncoder[motorC] < 20 && time1[timer1] < time) {}

    	motor[motorC] = 0;  // Stop the motor
  	
	taskPowder = true;
	}
