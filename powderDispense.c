void taskPowder() { 
	taskPowder = false;
    	nMotorEncoder[motorC] = 0;
	
    	const int HIGH_POWDER_LVL = 20;
    	const int LOW_POWDER_LVL = 10;
    	const int MEDIUM_POWDER_LVL = 15;

    	int powderLVL;
    	// Set encoder rotation levels based on powder level
    	if (powderLevel == 1) {
    	    powderLVL = HIGH_POWDER_LVL;
    	} else if (powderLevel == 0) {
    	    powderLVL = LOW_POWDER_LVL;
    	} else {
    		powderLVL = MEDIUM_POWDER_LVL;
    	}

   	// Start motor
   	motor[motorC] = 20;  
	wait1MS(50); //Slight delay to take load off CPU
   	while (abs(nMotorEncoder[motorC]) < powderLVL) {}
	motor[motorC] = 0;  // Stop the motor

	
   	taskPowder = true;
}
