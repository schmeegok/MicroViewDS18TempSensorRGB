#include <SoftwareSerial.h>
#include <MicroView.h>
#include <DallasTemperature.h>
#include <OneWire.h>



#define ONE_WIRE_BUS 0

SoftwareSerial mySerial(0, 1); // RX, TX


MicroViewWidget *widget1;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//const int button1Pin = 2;  // pushbutton 1 pin
const byte button1Pin = 2; // pushbutton 1 pin

// RGB LED Pins
const int RED_PIN   = 6; // Common Anode Pinout
const int GREEN_PIN = 5; // Common Anode Pinout
const int BLUE_PIN  = 3; // Common Anode Pinout


// Constants for the upper and lower thresholds for LED
const float TEMP_LIM_LO = 0;
const float TEMP_LIM_HI = 90;



// Temp Variables
float degreesF_1, maxDegreesF_1, minDegreesF_1;
float degreesF_2, maxDegreesF_2, minDegreesF_2;
float degreesF_3, maxDegreesF_3, minDegreesF_3;

//int button1State;  // variable to hold the pushbutton states
//int lastButton1State = HIGH;

volatile byte mode = 1; // Variable to hold the display mode
const byte tempSensor1Mode = 1;
const byte tempSensor2Mode = 2;
const byte tempSensor3Mode = 3;
const byte tempSensor4Mode = 4;
const byte LED1Mode        = 5;
const byte numModes = 5;


void setup() 
{
  // put your setup code here, to run once:
  mySerial.begin(9600);
  uView.begin();
  uView.clear(PAGE);
  uView.display();
  uView.setFontType(0);
  uView.setCursor(0,0);
  uView.print("MicroView\nDS18B20\nDual\nTemp\nSensor");
  uView.display();
  delay(3000);
  uView.clear(PAGE);
  uView.setFontType(0);
  uView.setCursor(0,0);
  uView.print("schmeegok\n@gmail.com");
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
      showTempRGB(i, 0.0, 0.0);
      delay(10);
  }
  /*
  analogWrite(RED_PIN, 255);
  analogWrite(BLUE_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  delay(1000);
  analogWrite(RED_PIN, 0);
  analogWrite(BLUE_PIN, 255);
  analogWrite(GREEN_PIN, 0);
  delay(1000);
  analogWrite(RED_PIN, 0);
  analogWrite(BLUE_PIN, 0);
  analogWrite(GREEN_PIN, 255);
  delay(3000);
  analogWrite(RED_PIN, 0);
  analogWrite(BLUE_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  */
  
  maxDegreesF_1 = -20;
  minDegreesF_1 = 130;
  maxDegreesF_2 = -20;
  minDegreesF_2 = 130;
  maxDegreesF_3 = -20;
  minDegreesF_3 = 130;

  
  // Set up the pushbutton pins to be an input:
  pinMode(button1Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button1Pin), modeChange, RISING);
  // Initialize the Temp Sensor Library
  sensors.begin();
  mode = tempSensor1Mode;
}

void loop(void) 
{
    // put your main code here, to run repeatedly:
    int ledIntensity;

    /*
    int buttonPushed = 0;
    // First thing to do is handle the button push and mode
    button1State = digitalRead(button1Pin);
    if (button1State != HIGH)
    {
        buttonPushed = 1;
    }
    else
    {
        buttonPushed = 0;
    }

    if (buttonPushed == 1)
    {    
        if (mode < numModes)
        {
            mode += 1;
        }
        else if (mode == numModes)
        {
            mode = 1;
        }
    }*/

    // Take a temperature every time
    // Take a temperature reading from the DS18B20 Sensor
    sensors.requestTemperatures(); // Send the command to get temperatures
    degreesF_1 = sensors.getTempFByIndex(0);  // Device 1 is index 0
    degreesF_2 = sensors.getTempFByIndex(1);  // Device 1 is index 0
    degreesF_3 = (degreesF_1 + degreesF_2)/2; // Average between both sensors
    
    // Update the maxs
    if (degreesF_1 > maxDegreesF_1)
    {
        maxDegreesF_1 = degreesF_1;
    }
    if (degreesF_2 > maxDegreesF_2)
    {
        maxDegreesF_2 = degreesF_2;
    }
    if (degreesF_3 > maxDegreesF_3)
    {
        maxDegreesF_3 = degreesF_3;
    }
            
    // Update the mins
    if (degreesF_1 < minDegreesF_1)
    {
        minDegreesF_1 = degreesF_1;
    }
    if (degreesF_2 < minDegreesF_2)
    {
        minDegreesF_2 = degreesF_2;
    }
    if (degreesF_3 < minDegreesF_3)
    {
        minDegreesF_3 = degreesF_3;
    }

    // Print to Serial Port
    sendToSerial(minDegreesF_1, maxDegreesF_1, degreesF_1, minDegreesF_2, maxDegreesF_2, degreesF_2, minDegreesF_3, maxDegreesF_3, degreesF_3);
    
   
    // Mode selection
    switch(mode)
    {
        // Mode 1: Use both sensors, average the temperatures, set the RGB based on the min/max values
        case tempSensor1Mode:
            // Update the microview display
            uView.clear(PAGE);
            //widget1 = new MicroViewGauge(35, 17, -200, 1300, WIDGETSTYLE0 + WIDGETNOVALUE);
            widget1 = new MicroViewSlider(18, 20, minDegreesF_3*10, maxDegreesF_3*10, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw a fixed "F" text
            uView.setCursor(widget1->getX() + 13, widget1->getY() + 10);
            uView.print("F");

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print("1");
            
            customGauge0(degreesF_3*10, minDegreesF_3*10, maxDegreesF_3*10);
            uView.display();
            delete widget1;

            // Calculate the RGB Intensities: analogWrite(PIN, Value) where Value = 0-255
            ledIntensity = getLedIntensityFromTemp(degreesF_3, minDegreesF_3, maxDegreesF_3);
            showRGB(ledIntensity);

            //delay(500);// 1 s for temp
            break;

        // Mode 2: Use both sensors, average the temperatures, set the RGB based on the hard coded temp threshold values
        case tempSensor2Mode:
            // Update the microview display
            uView.clear(PAGE);
            //widget1 = new MicroViewGauge(35, 17, -200, 1300, WIDGETSTYLE0 + WIDGETNOVALUE);
            widget1 = new MicroViewSlider(18, 20, minDegreesF_3*10, maxDegreesF_3*10, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw a fixed "F" text
            uView.setCursor(widget1->getX() + 13, widget1->getY() + 10);
            uView.print("F");

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print("2");
            
            customGauge0(degreesF_3*10, minDegreesF_3*10, maxDegreesF_3*10);
            uView.display();
            delete widget1;

            // Calculate the RGB Intensities: analogWrite(PIN, Value) where Value = 0-255
            //ledIntensity = getLedIntensityFromTemp(degreesF_3, TEMP_LIM_LO, TEMP_LIM_HI);
            //showRGB(ledIntensity);
            showTempRGB(degreesF_3, TEMP_LIM_LO, TEMP_LIM_HI);

            //delay(500);// 1 s for temp
            break;
            
        case tempSensor3Mode:
            // Update the microview display
            uView.clear(PAGE);
            //widget1 = new MicroViewGauge(35, 17, -200, 1300, WIDGETSTYLE0 + WIDGETNOVALUE);
            widget1 = new MicroViewSlider(18, 20, minDegreesF_1*10, maxDegreesF_1*10, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw a fixed "F" text
            uView.setCursor(widget1->getX() + 13, widget1->getY() + 10);
            uView.print("F");

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print("3");
            
            customGauge0(degreesF_1*10, minDegreesF_1*10, maxDegreesF_1*10);
            uView.display();
            delete widget1;

            // Calculate the RGB Intensities: analogWrite(PIN, Value) where Value = 0-255
            ledIntensity = getLedIntensityFromTemp(degreesF_1, minDegreesF_1, maxDegreesF_1);
            showRGB(ledIntensity);

            //delay(500);// 1 s for temp
            break;

        case tempSensor4Mode:
            // Update the microview display
            uView.clear(PAGE);
            //widget1 = new MicroViewGauge(35, 17, -200, 1300, WIDGETSTYLE0 + WIDGETNOVALUE);
            widget1 = new MicroViewSlider(18, 20, minDegreesF_2*10, maxDegreesF_2*10, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw a fixed "F" text
            uView.setCursor(widget1->getX() + 13, widget1->getY() + 10);
            uView.print("F");

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print("4");
            
            customGauge0(degreesF_2*10, minDegreesF_2*10, maxDegreesF_2*10);
            uView.display();
            delete widget1;

            // Calculate the RGB Intensities: analogWrite(PIN, Value) where Value = 0-255
            ledIntensity = getLedIntensityFromTemp(degreesF_2, minDegreesF_2, maxDegreesF_2);
            showRGB(ledIntensity);

            //delay(500);// 1 s for temp
            break;

        case LED1Mode:
            // Update the microview display
            uView.clear(PAGE);
            //widget1 = new MicroViewGauge(35, 17, -200, 1300, WIDGETSTYLE0 + WIDGETNOVALUE);
            widget1 = new MicroViewSlider(18, 20, minDegreesF_3*10, maxDegreesF_3*10, WIDGETSTYLE0 + WIDGETNOVALUE);
            // draw a fixed "F" text
            uView.setCursor(widget1->getX() + 13, widget1->getY() + 10);
            uView.print("F");

            uView.setCursor(0,0);
            uView.setFontType(0);
            uView.print("5");
            
            customGauge0(degreesF_3*10, minDegreesF_3*10, maxDegreesF_3*10);
            uView.display();
            delete widget1;

            // Loop Through LED colors, (0,0,0) to (255,255,255) and back
            setRGBColor(0,0,0);
            //delay(20000);
            
            for (int r=0; r <= 255; r++)
            {
              for (int g=0; g <= 255; g++)
              {
                for (int b=0; b <= 255; b++)
                {
                  setRGBColor(r,g,b);
                  //delayMicroseconds(10);
                }
              }
            }
            for (int r=255; r >=0; r--)
            {
              for (int g=255; g >= 0; g--)
              {
                for (int b=255; b <= 0; b--)
                {
                  setRGBColor(r,g,b);
                  //delayMicroseconds(10);
                }
              }
            }
            
            //delay(20000);// 1 s for temp
            break;
    }
    //delay(1000);
}

// Update function for Temp Sensor Screen (Demo 12?)
void customGauge0(int16_t val, int16_t minVal, int16_t maxVal) {
  widget1->setValue(val);
  
  uView.setCursor(widget1->getX() - 0, widget1->getY() - 18);
  uView.setFontType(1);
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

// Function to update widget1
void update1widget(int16_t val) {
  widget1->setValue(val);
}


void showRGB(int color)
{
  int redIntensity;
  int greenIntensity;
  int blueIntensity;

  // Here we'll use an "if / else" statement to determine which
  // of the three (R,G,B) zones x falls into. Each of these zones
  // spans 255 because analogWrite() wants a number from 0 to 255.

  // In each of these zones, we'll calculate the brightness
  // for each of the red, green, and blue LEDs within the RGB LED.

  if (color <= 255)          // zone 1
  {
    redIntensity = 255 - color;    // red goes from on to off
    greenIntensity = color;        // green goes from off to on
    blueIntensity = 0;             // blue is always off
  }
  else if (color <= 511)     // zone 2
  {
    redIntensity = 0;                     // red is always off
    greenIntensity = 255 - (color - 256); // green on to off
    blueIntensity = (color - 256);        // blue off to on
  }
  else // color >= 512       // zone 3
  {
    redIntensity = (color - 512);         // red off to on
    greenIntensity = 0;                   // green is always off
    blueIntensity = 255 - (color - 512);  // blue on to off
  }

  // Now that the brightness values have been set, command the LED
  // to those values

  analogWrite(RED_PIN, redIntensity);
  analogWrite(BLUE_PIN, blueIntensity);
  analogWrite(GREEN_PIN, greenIntensity);
}

void showTempRGB(float currentTemp, float tempThresholdLo, float tempThresholdHi)
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

  mySerial.print("R=");
  mySerial.print(redIntensity);
  mySerial.print("; ");
  mySerial.print("G=");
  mySerial.print(greenIntensity);
  mySerial.print("; ");
  mySerial.print("B=");
  mySerial.print(blueIntensity);
  mySerial.println(";");
  
  analogWrite(RED_PIN, redIntensity);
  analogWrite(BLUE_PIN, blueIntensity);
  analogWrite(GREEN_PIN, greenIntensity);
}

int getLedIntensityFromTemp(float currentTemp, float tempThresholdLo, float tempThresholdHi)
{
  int currentLedIntensity;
  
  // There are two scales working here.  We want temperature to go between 60-80 degrees to determine LED color.
  // Anything less than 60 is just blue.  Anything more than 80 is just red.  Otherwise we want to choose the LED
  // based on the current temperature.
  if (currentTemp <= tempThresholdLo)
  {
    currentLedIntensity = 511; //Blue
  }
  else if (currentTemp >= tempThresholdHi)
  {
    currentLedIntensity = 0; // Red
  }
  else
  {
    currentLedIntensity = 511 - (currentTemp - tempThresholdLo)*(511/(tempThresholdHi - tempThresholdLo)); // Choose the color based on the scale
  }
  return (currentLedIntensity);
}

void sendToSerial(float t1Min, float t1Max, float t1, float t2Min, float t2Max, float t2, float t3Min, float t3Max, float t3)
{
    mySerial.print("T1_Min=");
    mySerial.print(t1Min);
    mySerial.print("; ");
    mySerial.print("T1_Max=");
    mySerial.print(t1Max);
    mySerial.print("; ");
    mySerial.print("T1=");
    mySerial.print(t1);
    mySerial.print("; ");
    mySerial.print("T2_Min=");
    mySerial.print(t2Min);
    mySerial.print("; ");
    mySerial.print("T2_Max=");
    mySerial.print(t2Max);
    mySerial.print("; ");
    mySerial.print("T2=");
    mySerial.print(t2);
    mySerial.print("; ");
    mySerial.print("T3_Min=");
    mySerial.print(t3Min);
    mySerial.print("; ");
    mySerial.print("T3_Max=");
    mySerial.print(t3Max);
    mySerial.print("; ");
    mySerial.print("T3=");
    mySerial.print(t3);
    mySerial.println(";");
}

void setRGBColor(int redIntensity, int greenIntensity, int blueIntensity)
{
  analogWrite(RED_PIN, redIntensity);
  analogWrite(GREEN_PIN, greenIntensity);
  analogWrite(BLUE_PIN, blueIntensity);
}

void modeChange()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce
  if (interrupt_time - last_interrupt_time > 200)
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
