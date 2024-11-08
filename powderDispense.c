task powder() { 
		finishCupRemoval = false;
    clearTimer(timer3);
    nMotorEncoder[motorC] = 0;


    while (!finishCupRemoval) {}

   
    if (time1[timer3] >= time && SensorValue[S4] == 1) {
        motor[motorC] = 10;   // Start motor
        clearTimer(timer3);    // Reset timer3
    }


    while (nMotorEncoder[motorC] < 20 && time1[timer1] < time) {}

    motor[motorC] = 0;  // Stop the motor
  

    turntableFunction();
}
