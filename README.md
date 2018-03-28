
# ESP32 DVM - ESP32 Elektor Design Contest 2018 #

https://www.elektormagazine.com/labs/contest/esp32-design-contest-2018
 
FIRST low-cost ESP32 Microcontrolled Digital Voltmeter that REALLY measures Negative Voltages, using old style ADC conversion method ! With accuracy !

High precision, good stability , negative and positive Voltages !

Voltage reading range from -2V to +2V . Method of Voltage measurement - Dual slope integrating ADC conversion.

-Can measure voltages from -2.000V to +2.000V with accuracy and stability,

-True Differential Input,

-conversion reliability with ±1 in 20,000 count accuracy,

-auto-zero to less than 10μV,

-Guaranteed Zero Reading for 0V Input,

-Indication of Over Voltage, 

-Use only 5 GPIO pins of ESP32,

-Low power ( uses 3.3V Regulator of ESP32 - Current consumption only 3 mA aproximately  ! )

DVM ESP32 using the Blynk IOT PLataform :

I choose Blynk because it is easier to implement IOT with ESP32. Not the best, but it works!
My ESP32 DVM sent voltage values to my Tablet (Blynk application) through the Blynk Cloud Server.
The sending data rate must be at least 1 value per second. Restriction of the free Blynk server.

Youtube Video - ESP32 DVM7135 Blynk IOT Tests :
https://youtu.be/8PL2q0f5Bow

You must configure the router's SSID and password in that Sketch.
The Token Key is what you can get from the Blynk Application configuration.

Blynk Getting Started Guide ->https://www.blynk.cc/getting-started

Blynk Documentation -> http://docs.blynk.cc/

Blynk Sketch generator -> https://examples.blynk.cc/
