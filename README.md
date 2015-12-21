# esp8266WifiMatrix
# Build a internet connected dot matrix display
## See also on http://www.µc.net
## You need
* ESP-12 module (or other ESP8266 based module)
* At least one, better 5 or so MAX7219 based 8x8 LED matrix modules
* Breadboard, power supply
* The MAX7219 Arduino library: https://github.com/hermann-kurz/MAX7219LedMatrix

Steps:
* Install Arduino IDE with ESP8266 support: https://github.com/esp8266/Arduino
* Get patche MAX7219 library https://github.com/hermann-kurz/MAX7219LedMatrix and install it in arduino Sketchbook/library 
* Put the hardare together on a breadbord, see the fritzing files
* Compile esp8266WifiMatrix sketch
* Program via FTD232 adapter

# Have fun
