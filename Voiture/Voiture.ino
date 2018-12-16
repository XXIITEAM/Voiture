//test 1:2015.08.15
#include <car_bluetooth.h>
#include <SoftwareSerial.h>   //Software Serial Port
#include <String.h>
#include <MotorDriver.h>
#include <stdlib.h>

#include <Ultrasonic.h>

#define TRIGGER_PIN  5//connect Trip of the Ultrasonic Sensor moudle to D5 of Arduino 
//and can be changed to other ports
#define ECHO_PIN     3

Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
// Only run 50 time so we can reburn the code easily.
int CYCLES = 50;
int CYCLES2 = 3;
int count = 0;
int count2 = 0;
bool test;
 float cmMsec;
#define RxD 2
#define TxD 4
CarBluetooth bluetooth(RxD, TxD);
#define CMD_INVALID     0XFF
#define CMD_FORWARD     'F'
#define CMD_RIGHT_FRONT 'R'
#define CMD_RIGHT_FORWARD 'I'
#define CMD_LEFT_FORWARD 'G'
#define CMD_RIGHT_BACK 'J'
#define CMD_LEFT_BACK 'H'
#define CMD_BACKWARD    'B'

#define CMD_LEFT_FRONT  'L'
#define CMD_STOP        'S'

#define SPEED_STEPS 20
uint8_t speed0 = 180;
void setup() {
	Serial.begin(9600);
	motordriver.init();
	motordriver.setSpeed(180, MOTORA);
	motordriver.setSpeed(180, MOTORB);
	//bluetooth.waitPairable();
	//bluetooth.waitConnected();
}
uint8_t bt_command;

#define CAR_STOP 0
#define CAR_FORWARD 1
#define CAR_BACK 2
uint8_t car_status = CAR_STOP;
uint8_t new_status = car_status;

void loop() {

  scanFrontCenter();
      count = 0;
  if(cmMsec > 150){
      motordriver.goRight();
      delay(100);


  }
  if(cmMsec <= 150 && cmMsec >= 50){
      motordriver.goForward();
      delay(100);


  }
  if(cmMsec <= 49 && cmMsec >= 30){
      motordriver.goLeft();
      delay(1000);

  }
  if(cmMsec <= 29 && cmMsec >= 1){
      motordriver.goBackward();
      delay(1000);

  }
  count++;
  //if (count2 == CYCLES2)
  //{


  /*  bt_command = bluetooth.readByte();
  //Serial.println(bt_command);
  if (bt_command != CMD_INVALID) {
    controlCar(bt_command);
        count2 = 0;

  }
  //}

  
  if (bluetooth.getStatus() == PAIRABLE) {
    motordriver.stop();
    bluetooth.waitConnected();
  }
  count++;
  count2++;
  
  */
}

/*
void controlCar(uint8_t cmd) {

		switch (cmd)
		{
		case CMD_FORWARD:    
			motordriver.goForward(); 
			break;
		case CMD_RIGHT_FORWARD:
			rightForward();
			motordriver.goForward();
			motordriver.setSpeed(speed0, MOTORB);
			break;
		case CMD_LEFT_FORWARD:
			leftForward();
			motordriver.goForward();
			motordriver.setSpeed(speed0, MOTORA);
			break;
		case CMD_RIGHT_BACK:
			rightForward();
			motordriver.goBackward();
			motordriver.setSpeed(speed0, MOTORB);
			break;
		case CMD_LEFT_BACK:
			leftForward();
			motordriver.goBackward();
			motordriver.setSpeed(speed0, MOTORA);
			break;
		case CMD_RIGHT_FRONT:
			//  if(car_status != CAR_STOP)new_status = CAR_FORWARD;
			motordriver.goRight();
			// delay(200); 
			break;
		case CMD_BACKWARD:    motordriver.goBackward(); break;
		case CMD_LEFT_FRONT:
			motordriver.goLeft();
			// delay(200);
			break;
		case CMD_STOP:        motordriver.stop(); break;
		default: break;
	}


	
/*if ((cmd >= '0') && (cmd <= '9'))
	{
		speed0 = cmd - 0x30;
		//Serial.print(speed0);
		//Serial.print(">");
		speed0 = map(speed0, 0, 9, 0, 255);
		//Serial.println(speed0);
		motordriver.setSpeed(speed0, MOTORA);
		motordriver.setSpeed(speed0, MOTORB);
	}
}*/
void scanFrontCenter() {
  


   
  long microsec = ultrasonic.timing();

  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
    Serial.print(cmMsec);
   Serial.println(" CM");
   Serial.println(bt_command);
}
/*void speedUp() {
	if (speed0 < 236)speed0 += SPEED_STEPS;
	else speed0 = 255;
	motordriver.setSpeed(speed0, MOTORA);
	motordriver.setSpeed(speed0, MOTORB);
}

void speedDown() {
	if (speed0 > 70)speed0 -= SPEED_STEPS;
	else speed0 = 50;
	motordriver.setSpeed(speed0, MOTORA);
	motordriver.setSpeed(speed0, MOTORB);
}*/

void rightForward() {
	motordriver.setSpeed(100, MOTORB);
}
void leftForward() {
	motordriver.setSpeed(100, MOTORA);
}
