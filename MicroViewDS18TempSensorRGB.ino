#include <MicroView.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 0

MicroViewWidget *widget1;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
const int button1Pin = 2;  // pushbutton 1 pin

// RGB LED Pins
const int RED_PIN   = 6; // Common Anode Pinout
const int GREEN_PIN = 5; // Common Anode Pinout
const int BLUE_PIN  = 3; // Common Anode Pinout


// Constants for the upper and lower thresholds for LED
const float TEMP_LIM_LO = 0;
const float TEMP_LIM_HI = 80;



// Temp Variables
float degreesF_1, maxDegreesF_1, minDegreesF_1;
float degreesF_2, maxDegreesF_2, minDegreesF_2;
float degreesF_3, maxDegreesF_3, minDegreesF_3;

int button1State;  // variable to hold the pushbutton states
int lastButton1State = HIGH;

int mode; // Variable to hold the display mode
const int tempSensor1Mode = 1;
const int tempSensor2Mode = 2;
const int tempSensor3Mode = 3;
const int tempSensor4Mode = 4;
const int numModes = 4;


void setup() 
{
  // put your setup code here, to run once:
  uView.begin();
  uView.clear(PAGE);
  uView.display();
  uView.setFontType(0);
  uView.setCursor(0,0);
  uView.print("MicroView\nDS18B20\nDual\nTemp\nSensor");
  uView.display();
  delay(5000);
  uView.clear(PAGE);
  uView.setFontType(0);
  uView.setCursor(0,0);
  uView.print("schmeegok\n@gmail.com");
  uView.display();
  delay(5000);
  uView.clear(PAGE);
  uView.display();

  // configure RGB LED Pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

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
  
  
  maxDegreesF_1 = -20;
  minDegreesF_1 = 130;
  maxDegreesF_2 = -20;
  minDegreesF_2 = 130;
  maxDegreesF_3 = -20;
  minDegreesF_3 = 130;

  
  // Set up the pushbutton pins to be an input:
  pinMode(button1Pin, INPUT);
  // Initialize the Temp Sensor Library
  sensors.begin();
  
  mode = tempSensor1Mode;
}

void loop(void) 
{
    // put your main code here, to run repeatedly:
    int ledIntensity;
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
    }

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
            ledIntensity = getLedIntensityFromTemp(degreesF_3, TEMP_LIM_LO, TEMP_LIM_HI);
            showRGB(ledIntensity);

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
