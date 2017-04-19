# MicroViewDS18TempSensorRGB

Summary:
A program to connect a MicroView to two DS18B20 Temp sensors, an RGB LED, and a push button.

The Idea:
Built from a handful of components I purchased from SparkFun.  The original idea came from checking out my brother's fishing kayak.
He purchased a Bluetooth fishfinder that displays data on his Android phone.  It is a neat idea, but the problem is, the fish finder
only lasts as long as your phone battery lasts.  He solved this by building a waterproof battery case, and a 12V high capacity battery.
The battery case can be disconnected and removed from the kayak for recharging back at home.  Cabling from the case behind where he sits
runs to the front of his kayak and powers  waterproof covered USB outlets (for charging the phone), a voltage meter (to monitor the battery in the case),
and a switch to turn the backup on or off.  His kayak looked a bit like a jet cockpit with gauges and knobs.  I was very impressed and inspired
to come up with ideas of other things he could want.

That's when I found the MicroView OLED on SparkFun.  A bit expensive for a reduced I/O Arduino Uno, but the display and the fact that it IS
the microcontroller was appealing to me.  I showed my MicroView to him and he is thinking about getting one.  In the mean-time, I wanted to 
put together a project that could possibly go in his kayak, but combines another idea I had for an RGB LED and a temp-sensor with an Arduino.

The thought was to create a dual temp-sensor circuit that could monitor the temperature in two places.  One temp-sensor to monitor below water
(using a waterproof version of the DS18B20 sensor that sells for about $10 on SparkFun... or make my own with one of the two much cheaper
DS18B20s I already purchased).  The other temp sensor would monitor the temperature above water (either straight from the PCB, or with another 
waterproof cabled DS18B20).  The RGB LED would simply light to a color of the spectrum that represents the temperature (Blue for cold, and shifting
all the way to Red for hot).

Currently:
The project for now does use both temp-sensors and the RGB LED.  The mode of the display is controlled by push-button (to toggle modes), and 
the RGB LED works differenty for each mode.  Additionally, I have added a humidity sensor and T5403 barometric pressure sensor.

The MicroView weather station has been connected to an older Raspberry Pi 1 with WIFI adapter.  The Raspberry Pi runs a python script at boot to control the power up sequence of the MicroView, read the data off of the serial port, and log the sensor data to https://data.sparkfun.com/streams/NJXyDLYX2ns5G5nG8pZz

Also, the sensor data is sent to Weather Underground to provide weather data to their Personal Weather Station Network.  You can see the station data broadcast here: https://www.wunderground.com/personal-weather-station/dashboard?ID=KCOCOLOR604#history
