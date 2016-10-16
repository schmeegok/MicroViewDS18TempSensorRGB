#include <SoftwareSerial.h>
#include <MicroView.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>
#include <t5403.h>
#include "SparkFun_Si7021_Breakout_Library.h"

#define ONE_WIRE_BUS A0

SoftwareSerial mySerial(0, 1); // RX, TX


MicroViewWidget *widget1;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
T5403 barometer(MODE_I2C);
Weather humSensor;
//const int button1Pin = 2;  // pushbutton 1 pin
const byte button1Pin = 2; // pushbutton 1 pin

// RGB LED Pins
const int RED_PIN   = 6; // Common Anode Pinout
const int GREEN_PIN = 5; // Common Anode Pinout
const int BLUE_PIN  = 3; // Common Anode Pinout

// Temp Variables
float degF_Out, maxDegF_Out, minDegF_Out;
float degF_In, maxDegF_In, minDegF_In;

float relHumidity, relHumidityMax, relHumidityMin;
float humTempf, humTempfMax, humTempfMin;

// Baro Variables
double relPress, relPressMax, relPressMin, absPress;
float baroTemp, baroTempMax, baroTempMin;
double baseAltitude_m = 2591.11; //8501 feet
double calFactor = 0.07;

//int button1State;  // variable to hold the pushbutton states
//int lastButton1State = HIGH;

volatile byte mode = 1; // Variable to hold the display mode
const byte baroPressMode   = 1;
const byte outsideTempMode = 2;
const byte insideTempMode  = 3;
const byte baroTempMode    = 4;
const byte humTempMode     = 5;
const byte numModes = 5;


void setup() 
{
  // put your setup code here, to run once:
  mySerial.begin(9600);

  // Initialize the Temp Sensor Library
  sensors.begin();
  barometer.begin();
  humSensor.begin();
  
  uView.begin();
  uView.clear(PAGE);
  uView.display();
  uView.setFontType(0);
  uView.setCursor(0,0);
  uView.print(F("MicroView\nDS18B20\nDual\nTemp\nSensor"));
  uView.display();
  delay(3000);
  uView.clear(PAGE);
  uView.setFontType(0);
  uView.setCursor(0,0);
  uView.print(F("schmeegok\n@gmail.com"));
  uView.display();
  delay(3000);
  uView.clear(PAGE);
  uView.display();

  // configure RGB LED Pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Test RGB LED using Temp scale
  for (float i=-1.0; i<= 101.0; i+= 0.5)
  {
      showTempRGB(i);//, 0.0, 0.0);
      delay(5);
  }

  resetStatistics();
  
  // Set up the pushbutton pins to be an input:
  pinMode(button1Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button1Pin), modeChange, RISING);
  
  // Initialize the Temp Sensor Library
  //sensors.begin();
  //barometer.begin();
  
  mode = outsideTempMode;
  //mode = baroPressMode;
}

void loop(void) 
{
    // put your main code here, to run repeatedly:
    int ledIntensity;    

    // Take a temperature every time
    // Take a temperature reading from the DS18B20 Sensor
    sensors.requestTemperatures(); // Send the command to get temperatures
    //degF_Out = 70.12; // Simulated Value
    degF_Out = sensors.getTempFByIndex(0);  // Device 1 is index 0
    degF_In = sensors.getTempFByIndex(1);  // Device 1 is index 0
    //degreesF_3 = (degF_Out + degF_In)/2; // Average between both sensors

    // Get the pressure and temperature from the barometer
    baroTemp = barometer.getTemperature(FAHRENHEIT)/100.00;
    absPress = barometer.getPressure(MODE_ULTRA);

    // Sometime this may return negative values so we only want good values
    relPress = sealevel_inhg(absPress, baseAltitude_m) - calFactor;
    //relPress = 30.01; // Simulated Value
    while (relPress < 28.00 || relPress > 31.00)
    {
        mySerial.print(F("Pressure measurement out of range (28.00 to 31.00): "));
        mySerial.print(relPress);
        mySerial.println(F(" in-hg"));
        relPress = sealevel_inhg(absPress, baseAltitude_m) - calFactor;
    }

    // Measure Relative Humidity from the HTU21D or Si7021
    relHumidity = humSensor.getRH();
 
    // Measure Temperature from the HTU21D or Si7021
    humTempf = humSensor.getTempF();
    // Temperature is measured every time RH is requested.
    // It is faster, therefore, to read it from previous RH
    // measurement with getTemp() instead with readTemp()
     
    // Update the maxs
    if (degF_Out > maxDegF_Out)
    {
        maxDegF_Out = degF_Out;
    }
    if (degF_In > maxDegF_In)
    {
        maxDegF_In = degF_In;
    }
    if (baroTemp > baroTempMax)
    {
        baroTempMax = baroTemp;
    }
    if (relPress > relPressMax)
    {
        relPressMax = relPress;
    }
    if (relHumidity > relHumidityMax)
    {
        relHumidityMax = relHumidity;
    }
            
    // Update the mins
    if (degF_Out < minDegF_Out)
    {
        minDegF_Out = degF_Out;
    }
    if (degF_In < minDegF_In)
    {
        minDegF_In = degF_In;
    }
    if (baroTemp < baroTempMin)
    {
        baroTempMin = baroTemp;
    }
    if (relPress < relPressMin)
    {
        relPressMin = relPress;
    }
    if (relHumidity < relHumidityMin)
    {
        relHumidityMin = relHumidity;
    }

    // Print to Serial Port
    sendToSerial();
    
   
    // Mode selection
    switch(mode)
    {
        /* Working version of the "regular" guage
        // Barometric Pressure Mode: Display the barometric Pressure information
        case baroPressMode:
            // Update the microview display
            uView.clear(PAGE);
            //widget1 = new MicroViewGauge(35, 17, -200, 1300, WIDGETSTYLE0 + WIDGETNOVALUE);
            widget1 = new MicroViewSlider(18, 20, relPressMin*10, relPressMax*10, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw a fixed "inhg" text
            uView.setCursor(widget1->getX() + 6, widget1->getY() + 10);
            uView.print("inhg");

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print("BP");
            
            customGauge0(relPress*10, relPressMin*10, relPressMax*10);
            uView.display();
            delete widget1;
            showTempRGB(relPress, TEMP_LIM_LO, TEMP_LIM_HI);
            break;
        */

        // Barometric Pressure Mode: Display the barometric Pressure information
        case baroPressMode:
            // Update the microview display
            uView.clear(PAGE);
            widget1 = new MicroViewGauge(31, 18, 280, 310, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw the fixed "inhg" text
            uView.setCursor(widget1->getX() - 11, widget1->getY() + 11);
            uView.print(F("in-hg"));

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print(F("BP"));

            customGauge0(relPress*10, relPressMin*10, relPressMax*10, 0);
            //update1widget(relPress*10);
            uView.display();
            delete widget1;
            showTempRGB(relPress);//, TEMP_LIM_LO, TEMP_LIM_HI);
            break;
        
        // Outside Temp Sensor Mode (Sensor 1)
        case outsideTempMode: // Outdoor Sensor
            // Update the microview display
            uView.clear(PAGE);
            //widget1 = new MicroViewGauge(35, 17, -200, 1300, WIDGETSTYLE0 + WIDGETNOVALUE);
            widget1 = new MicroViewSlider(18, 20, minDegF_Out*10, maxDegF_Out*10, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw a fixed "F" text
            uView.setCursor(widget1->getX() + 13, widget1->getY() + 10);
            uView.print(F("F"));

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print(F("TO"));
            
            customGauge0(degF_Out*10, minDegF_Out*10, maxDegF_Out*10, 1);
            uView.display();
            delete widget1;

            showTempRGB(degF_Out);//, TEMP_LIM_LO, TEMP_LIM_HI);
            break;

        // Inside Temp Sensor Mode (sensor 2)
        case insideTempMode: // Inside Sensor
            // Update the microview display
            uView.clear(PAGE);
            //widget1 = new MicroViewGauge(35, 17, -200, 1300, WIDGETSTYLE0 + WIDGETNOVALUE);
            widget1 = new MicroViewSlider(18, 20, minDegF_In*10, maxDegF_In*10, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw a fixed "F" text
            uView.setCursor(widget1->getX() + 13, widget1->getY() + 10);
            uView.print(F("F"));

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print(F("TI"));
            
            customGauge0(degF_In*10, minDegF_In*10, maxDegF_In*10, 1);
            uView.display();
            delete widget1;
            
            showTempRGB(degF_In);//, TEMP_LIM_LO, TEMP_LIM_HI);
            break;

        // Barometer Temp Sensor Mode
        case baroTempMode: // Barometer built in temp sensor
            // Update the microview display
            uView.clear(PAGE);
            //widget1 = new MicroViewGauge(35, 17, -200, 1300, WIDGETSTYLE0 + WIDGETNOVALUE);
            widget1 = new MicroViewSlider(18, 20, baroTempMin*10, baroTempMax*10, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw a fixed "F" text
            uView.setCursor(widget1->getX() + 13, widget1->getY() + 10);
            uView.print(F("F"));

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print(F("TB"));
            
            customGauge0(baroTemp*10, baroTempMin*10, baroTempMax*10, 1);
            uView.display();
            delete widget1;
            
            showTempRGB(baroTemp);//, TEMP_LIM_LO, TEMP_LIM_HI);
            break;

        // Relative Humidity Mode: Display the barometric Pressure information
        case humTempMode:
            // Update the microview display
            uView.clear(PAGE);
            widget1 = new MicroViewGauge(31, 18, 0, 1000, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw the fixed "inhg" text
            uView.setCursor(widget1->getX() - 11, widget1->getY() + 11);
            uView.print(F("%"));

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print(F("RH"));

            customGauge0(relHumidity*10, relHumidityMin*10, relHumidityMax*10, 0);
            //update1widget(relPress*10);
            uView.display();
            delete widget1;
            showTempRGB(relHumidity);//, TEMP_LIM_LO, TEMP_LIM_HI);
            break;

        
    }// End Of Switch Case
}// End of Main

// Update function for Temp Sensor Screen (Demo 12?)
void customGauge0(int16_t val, int16_t minVal, int16_t maxVal, uint8_t mainFontSize) {
  widget1->setValue(val);
  
  uView.setCursor(widget1->getX() - 0, widget1->getY() - 18);
  uView.setFontType(mainFontSize);
  // add leading space if necessary, to right justify.
  // only 2 digit (plus decimal) numbers are supported.
  if (val < 100 && val > 0) 
  {
    uView.print(' ');
  }
  uView.print((float)val / 10, 1);
  uView.setFontType(0);

  // Print out the min/max in the appropriate places
  uView.setCursor(0,40);
  uView.print((float)minVal / 10, 1);
  if (maxVal > 1000)
  {
    uView.setCursor(35,40);
  }
  else
  {
    uView.setCursor(40,40);
  }
  //uView.setCursor(40,40);
  uView.print((float)maxVal /10, 1);
}

/*// Function to update widget1
void update1widget(int16_t val) {
  widget1->setValue(val);
}*/

void showTempRGB(float currentTemp)//, float tempThresholdLo, float tempThresholdHi)
{
  int redIntensity;
  int greenIntensity;
  int blueIntensity;

  float slopeRed;
  float slopeBlue;
  float slopeGreen;
  
  // Python equation: RedColorValue = slopeRed1*temp + (256 - slopeRed1*(t=0.0))
  // Python equation: BlueColorValue = 255;
  if (currentTemp <= 32)          // zone 1
  {
    slopeRed   = (0.00-255.00)/(32.00-0.00);
    slopeBlue  = 0;
    slopeGreen = 0;
    redIntensity = (int) (slopeRed*currentTemp + (256 - slopeRed*0));    // As Temp increases, Red Decreases
    blueIntensity = 255;           // blue is always on
    greenIntensity = 0;        // green is always off
  }
  
  // Python equation: RedColorValue = Off
  // y1.append(mb2*t[i] + (256.00 - mb2*t[t.index(32.0)]))
  // y2.append(mg1*t[i] + (0.0 - mg1*t[t.index(32.0)]))
  
  else if (currentTemp > 32.0 && currentTemp <= 70.0)          // zone 2
  {
    slopeRed   = 0;
    slopeBlue  = (0.00-255.00)/(70.00-32.00);
    slopeGreen = (255.00-0.00)/(70.00-32.00);
    redIntensity = 0;                                                       // As Temp increases, Keep Zero
    blueIntensity = (int) (slopeBlue*currentTemp + (256 - slopeBlue*32.0));           // As Temp increases, blue fades out
    greenIntensity = (int) (slopeGreen*currentTemp + (0.0 - slopeGreen*32.0));        // As Temp decreases, green fades in
  }

  // mr2*t[i] +(0.0 - mr2*t[t.index(70.0)])
  // Blue OFF
  // mg2*t[i] + (256.00 - mg2*t[t.index(70.0)])
  
  else if (currentTemp > 70.0 && currentTemp <= 90.0)          // zone 2
  {
    slopeRed   = (255.00-0.00)/(90.00-70.00);
    slopeBlue  = 0;
    slopeGreen = (0.00-255.00)/(90.00-70.00);
    redIntensity = (int) (slopeRed*currentTemp + (0.0 - slopeRed*70.0));              // As Temp increases, red fades in
    blueIntensity = 0;                                                        // As Temp increases, blue stays off
    greenIntensity = (int) (slopeGreen*currentTemp + (256 - slopeGreen*70.0));        // As Temp decreases, green fades out
  }

  
  else if (currentTemp > 90.0 && currentTemp <= 100.0)          // zone 2
  {
    slopeRed   = 0;
    slopeBlue  = (255.00-0.00)/(100.00-90.00);
    slopeGreen = (255.00-0.00)/(100.00-90.00);
    redIntensity = 255;              // As Temp increases, red fades in
    blueIntensity = (int) (slopeBlue*currentTemp + (0.0 - slopeBlue*90.0));                                                        // As Temp increases, blue stays off
    greenIntensity = (int) (slopeGreen*currentTemp + (0.0 - slopeGreen*90.0));        // As Temp decreases, green fades out
  }

  else if (currentTemp > 100.0)
  {
    redIntensity = 255;
    blueIntensity = 255;
    greenIntensity = 255;  
  }

  // Now that the brightness values have been set, command the LED
  // to those values

  mySerial.print(F("R="));
  mySerial.print(redIntensity);
  mySerial.print(F("; "));
  mySerial.print(F("G="));
  mySerial.print(greenIntensity);
  mySerial.print(F("; "));
  mySerial.print(F("B="));
  mySerial.print(blueIntensity);
  mySerial.println(F(";"));
  
  analogWrite(RED_PIN, redIntensity);
  analogWrite(BLUE_PIN, blueIntensity);
  analogWrite(GREEN_PIN, greenIntensity);
}

void sendToSerial()
{
    mySerial.print(F("T1_Min="));
    mySerial.print(minDegF_Out);
    mySerial.print(F("; "));
    mySerial.print(F("T1_Max="));
    mySerial.print(maxDegF_Out);
    mySerial.print(F("; "));
    mySerial.print(F("T1="));
    mySerial.print(degF_Out);
    mySerial.print(F("; "));
    mySerial.print(F("T2_Min="));
    mySerial.print(minDegF_In);
    mySerial.print(F("; "));
    mySerial.print(F("T2_Max="));
    mySerial.print(maxDegF_In);
    mySerial.print(F("; "));
    mySerial.print(F("T2="));
    mySerial.print(degF_In);
    mySerial.print(F("; "));
    mySerial.print(F("RH_Min="));
    mySerial.print(relHumidityMin);
    mySerial.print(F("; "));
    mySerial.print(F("RH_Max="));
    mySerial.print(relHumidityMax);
    mySerial.print(F("; "));
    mySerial.print(F("RH="));
    mySerial.print(relHumidity);
    mySerial.print(F("; "));
    mySerial.print(F("TRh_Min="));
    mySerial.print(humTempfMin);
    mySerial.print(F("; "));
    mySerial.print(F("TRh_Max="));
    mySerial.print(humTempfMax);
    mySerial.print(F("; "));
    mySerial.print(F("TRh="));
    mySerial.print(humTempf);
    mySerial.print(F("; "));
    mySerial.print(F("TBr_Min="));
    mySerial.print(baroTempMin);
    mySerial.print(F("; "));
    mySerial.print(F("TBr_Max="));
    mySerial.print(baroTempMax);
    mySerial.print(F("; "));
    mySerial.print(F("TBr="));
    mySerial.print(baroTemp);
    mySerial.print(F("; "));
    mySerial.print(F("P="));
    mySerial.print(relPress);
    mySerial.print(F("; "));
    mySerial.print(F("P_Min="));
    mySerial.print(relPressMin);
    mySerial.print(F("; "));
    mySerial.print(F("P_Max="));
    mySerial.print(relPressMax);
    mySerial.println(F(";"));
}

void resetStatistics()
{
    maxDegF_Out = -20;
    minDegF_Out = 130;
    maxDegF_In = -20;
    minDegF_In = 130;
    //maxDegreesF_3 = -20;
    //minDegreesF_3 = 130;
    baroTempMax = -20;
    baroTempMin = 130;
    relPressMax = 28.00;
    relPressMin = 31.00;
    relHumidityMax = 0.0;
    relHumidityMin = 100.0;
    humTempfMax = -20.0;
    humTempfMin = 130.0;
}

/*void setRGBColor(int redIntensity, int greenIntensity, int blueIntensity)
{
  analogWrite(RED_PIN, redIntensity);
  analogWrite(GREEN_PIN, greenIntensity);
  analogWrite(BLUE_PIN, blueIntensity);
}*/

void modeChange()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce
  if (interrupt_time - last_interrupt_time > 400)
  {
    // Do your thing
    if (mode < numModes)
    {
      mode += 1;
    }
    else if (mode == numModes)
    {
        mode = 1;
    }
  }
  last_interrupt_time = interrupt_time;
}

double sealevel_mb(double P_Pa, double A_m)
// Given Pressure P (Pa)taken at altitude A (meters)
// return equivalent pressure (mb or hPa) at sea level
{
  return ( (P_Pa/100)/pow(1-(A_m/44330.0),5.255));
}

double sealevel_inhg(double P_Pa, double A_m)
// Same as above, but return in inhg units
{
  double pressVar_mb;
  pressVar_mb = sealevel_mb(P_Pa, A_m);
  return ( pressVar_mb * 0.02953); // 1 mb = 0.02953 inhg
}

