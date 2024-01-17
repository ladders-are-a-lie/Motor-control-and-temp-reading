




/*
        Arduino Brushless Motor Control
     by Dejan, https://howtomechatronics.com
     EDIT: Ethan Thomas 
     the thermo coupling reads temp in celsuis
     MAX31856 Data sheet:https://www.digikey.com/htmldatasheets/production/1746536/0/0/1/max31856.html#:~:text=readings%20as%20high%20as%20%2B1800%C2%B0C,and%20as%20low%20as%20-210%C2%B0C

*/

#include <Servo.h>
#include <Adafruit_MAX31856.h>
#include <SPI.h>
#include <SD.h>

float TEMPC;
float TEMPF;

Servo ESC;     // create servo object to control the ESC

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(10, 11, 12, 13);

int potValue =0;  // value from the analog pin
const int chipSelect = 4; //chip select for the sd logger 

// the logging file
File logfile;


const unsigned int CONSOLE_OUTPUT=1; // (0/1) serial console ON or OFF?



void setup() {
   // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // assign LED pins as digital outputs
  pinMode(LED_BUILTIN, OUTPUT);      // Arduino LED
  //pinMode(10,OUTPUT);//chip sellect for the Thermocouple
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  Serial.flush();

   // -----------------------------------------
  //   create a new log file with unique name
  // -----------------------------------------
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0'; // % is the mod() operator, modulo
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the for-loop!
    }
  }
  if (! logfile) {
    Serial.print("couldn't create file. stop.");
    Serial.flush();
    while (true) ; // stop here upon fail
  }
  
  Serial.print("logfile=");
  Serial.print(logfile);
  Serial.print(", filename: ");
  Serial.println(filename);
  Serial.flush();
  
  // Attach the ESC on pin 9
  ESC.attach(9,1000,2000); // (pin, min pulse width, max pulse width in microseconds) 
  pinMode(8,OUTPUT);//connected to the kill switch connected to the relay  

  //Serial.begin(115200);
  Serial.println("MAX31856 thermocouple testing the MAD2815t");
  logfile.println("MAX31856 thermocouple testing the MAD2815");

  maxthermo.begin();

  maxthermo.setThermocoupleType(MAX31856_TCTYPE_K);

  Serial.print("Thermocouple type: ");
  logfile.print("Thermocouple type: ");
  switch (maxthermo.getThermocoupleType()){
    case MAX31856_TCTYPE_B: Serial.println("B Type") & logfile.println("B Type"); break;
    case MAX31856_TCTYPE_E: Serial.println("E Type") & logfile.println("E Type"); break;
    case MAX31856_TCTYPE_J: Serial.println("J Type") & logfile.println("J Type"); break;
    case MAX31856_TCTYPE_K: Serial.println("K Type") & logfile.println("K Type"); break;
    case MAX31856_TCTYPE_N: Serial.println("N Type") & logfile.println("N Type"); break;
    case MAX31856_TCTYPE_R: Serial.println("R Type") & logfile.println("R Type"); break;
    case MAX31856_TCTYPE_S: Serial.println("S Type") & logfile.println("S Type"); break;
    case MAX31856_TCTYPE_T: Serial.println("T Type") & logfile.println("T Type"); break;
    case MAX31856_VMODE_G8: Serial.println("Voltage x8 Gain mode") & logfile.println("Voltage x8 Gain mode"); break;
    case MAX31856_VMODE_G32: Serial.println("Voltage x8 Gain mode") & logfile.println("Voltage x8 Gain mode"); break;
    default: Serial.println("Unknown") & logfile.println("Unknown"); break;
  }
}

void loop() {
  digitalWrite(8,LOW);//having a charge will send a charge to the normily open relay to close

  potValue = analogRead(A0);   // reads the value of the potentiometer (value between 0 and 1023)
  potValue = map(potValue, 0, 1023, 0, 180);   // scale it to use it with the servo library (value between 0 and 180)
  ESC.write(potValue);    // Send the signal to the ESC

  //Serial.print("Cold Junction Temp:, "); 
  //Serial.println(maxthermo.readCJTemperature());
  maxthermo.readCJTemperature();

  Serial.print("Thermocouple Temp[C]:, "); 
  logfile.print("Thermocouple Temp[C]:, "); 
  TEMPC = maxthermo.readThermocoupleTemperature();
  TEMPF = TEMPC; //1.8*TEMPC+32;
  Serial.println(TEMPF);
  logfile.print(TEMPF);
  logfile.println(",");
  logfile.flush();
  
  // Check and print any faults
  /*uint8_t fault = maxthermo.readFault();0
  if (fault) {
    if (fault & MAX31856_FAULT_CJRANGE) Serial.println("Cold Junction Range Fault");
    if (fault & MAX31856_FAULT_TCRANGE) Serial.println("Thermocouple Range Fault");
    if (fault & MAX31856_FAULT_CJHIGH)  Serial.println("Cold Junction High Fault");
    if (fault & MAX31856_FAULT_CJLOW)   Serial.println("Cold Junction Low Fault");
    if (fault & MAX31856_FAULT_TCHIGH)  Serial.println("Thermocouple High Fault");
    if (fault & MAX31856_FAULT_TCLOW)   Serial.println("Thermocouple Low Fault");
    if (fault & MAX31856_FAULT_OVUV)    Serial.println("Over/Under Voltage Fault");
    if (fault & MAX31856_FAULT_OPEN)    Serial.println("Thermocouple Open Fault");
  }
  */
  delay(1000);
  /*
    // red LED on SD shield blinks to show FAT activity
    digitalWrite(LED_BUILTIN, HIGH);
    logfile.flush();
    digitalWrite(LED_BUILTIN, LOW);
  }
  */
}
