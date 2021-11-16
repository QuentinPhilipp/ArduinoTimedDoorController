
# Arduino timed door controler

This project allows the control of a door with a linear motor. It is built to run independently without any actions.

## Features

 - The board contains an I2C real time clock that store the current time. It will stay on time even if the board is not powered for a long time. Just plug it in again and it will set to the correct time. 
 - Setup the time with `setupHour == true` and then flash it with `setupHour == false`
 - If the closing and opening command are sent at the same time because of a wrong configuration, the default action is to **close** the door. It is a security feature on this project.
 - Connect to the serial port for debugging only. It will print all the status

## Files
data.h file contains the hours of opening and closing of the door. 
main.c file contains the code of the Arduino mini board
