// ESP32 - DVM7135
// ESP32 DEVKIT - ARDUINO IDE 1.8.5 - TLC7135 - PCF8574
// Gustavo Murta 19/02/2018
// Use I2C pullup Resistors 3K3 ohms

/*Blynk Getting Started Guide -> https://www.blynk.cc/getting-started
  Blynk Documentation -> http://docs.blynk.cc/
  Blynk Sketch generator -> https://examples.blynk.cc/
  Youtube video -> https://youtu.be/8PL2q0f5Bow

  BSD 3-Clause Open Source License
  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
  following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
  products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES,INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
  PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
  OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <Wire.h>               // ESP32 I2C Interface Library
#include <WiFi.h>               // ESP32 WIFI Library 
#include <WiFiClient.h>         // ESP32 WIFI Client Library 
#include <BlynkSimpleEsp32.h>   // Blynk Library 

# define BLYNK_PRINT Serial
# define CLOCK 2       // TCL7135 pin22 CLKIN     => GIPO_2 
# define STB  4        // TCL7135 pin26 -STB      => GPIO_4
# define RHLD 5        // TCL7135 pin25 RUN/-HOLD => GPIO_5
# define SDApin 21     // PCF8574 SDA pin15       => GPIO_21
# define SCLpin 22     // PCF8574 SCL pin14       => GPIO_22
# define PCF8574 0x20  // PCF8574 Address

char auth[] = "123456789ABCDEFGHIJKLMN";     // Blynk Application Token key
char ssid[] = "Router SSID";                 // Wifi Router name
char pass[] = "Router Password";             // Wifi Router password

byte Digit5, Digit4, Digit3, Digit2, Digit1 = 0;  // Digits of TLC7135
byte pcfData = 0;
int DigitCount = 0 ;
char Signal ;
float Voltage = 0;


void setup()
{
  pinMode (STB, INPUT);                    // Interrupt - ESP32 GPIO_4
  pinMode (RHLD, OUTPUT);                  // RUN DVM   - ESP32 GIPO_5

  // TLC7135 pin22 CLKIN => GPIO_2 generate CLOCK 480 KHz

  pinMode(CLOCK, OUTPUT);       // GPIO_2 as Output
  ledcAttachPin(CLOCK, 0);      // GPIO_2 attached to PWM Channel 0
  ledcSetup(0, 480000, 2);      // Channel 0 , freq 480 KHz , 2 bit resolution
  ledcWrite(0, 2);              // Enable frequency with duty cycle 50%

  digitalWrite(RHLD, LOW);                // Reset DVM TLC7135
  delay(10);
  digitalWrite(RHLD, HIGH);               // Run DVM TLC7135

  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(STB), TestData, FALLING); // External Interrupt GPIO_4
  Wire.begin (SDApin, SCLpin);           // sda= GPIO_21 /scl= GPIO_22

  Blynk.begin(auth, ssid, pass);   // Connecting Blink Cloud Server
}

void ReadData ()
{
  Wire.begin();                   // Start I2C
  Wire.requestFrom(PCF8574, 1);   // Read One Byte from PCF8574
  pcfData = Wire.read();          // Byte Read
}

void PrintData ()
{
  Serial.print(" TLC7135 Data = ");  // Print Voltage to adjustments of VRef
  Serial.print(Signal);              // Signal Plus or Minus
  Serial.print(" ");
  Serial.print(Digit5, HEX);
  Serial.print(",");
  Serial.print(Digit4, HEX);
  Serial.print(Digit3, HEX);
  Serial.print(Digit2, HEX);
  Serial.print(Digit1, HEX);
}

void UnderVoltage ()
{
  if ( bitRead(pcfData, 7))           // Bit7 PCF8574 => Under Voltage
    Serial.println(" Under Voltage ");
}

void OverVoltage ()
{
  if ( bitRead(pcfData, 6))           // Bit6 PCF8574 => Over Voltage
    Serial.println(" Over Voltage ");
}

void Polarity ()
{
  if ( bitRead(pcfData, 5))          // Bit5 PCF8574 => Polarity + or -
  {
    Signal = '+' ;
  }
  else
  {
    Signal = '-' ;
  }
}

void PrintVoltage ()           // Print Voltage X,XXX
{
  Voltage = ((Digit5 * 10000) + (Digit4 * 1000) + (Digit3 * 100) + (Digit2 * 10) + Digit1) / 10000.0 ; ;
  Serial.print("   Voltage = ");
  Serial.print(Signal);
  Serial.println(Voltage, 3);
}

void TestData ()
{
  ReadData ();
  Polarity();
  if ( bitRead(pcfData, 4))             // Test if TLC7135 Digit 5 is on (bit 4 of PCF8574)
  {
    Digit5 = pcfData & 0x0F ;           // Read first Byte
    DigitCount = 0 ;                    // Reset Digit Count
    DigitCount = ++DigitCount ;         // Digit Count increment
  }
  if (! (bitRead(pcfData, 4)))          // Test If bit 4 of PCF8574 is Off - Not Digit 5
    switch (DigitCount)
    {
      case 1 :
        Digit4 = pcfData & 0x0F ;       // Read 2nd Byte - Digit 4
        DigitCount = ++DigitCount ;     // Digit Count increment
        break ;
      case 2 :
        Digit3 = pcfData & 0x0F ;       // Read 3rd Byte - Digit 3
        DigitCount = ++DigitCount ;     // Digit Count increment
        break ;
      case 3 :
        Digit2 = pcfData & 0x0F ;       // Read 4th Byte - Digit 2
        DigitCount = ++DigitCount ;     // Digit Count increment
        break ;
      case 4 :
        Digit1 = pcfData & 0x0F ;       // Read 5th Byte - Digit 1
        DigitCount = ++DigitCount ;     // Digit Count increment
        PrintData ();
        PrintVoltage ();
        break ;
      default :
        break;
    }
  OverVoltage ();
  //UnderVoltage ();
}

BLYNK_READ(V0)                        
{
  Blynk.virtualWrite(V0, Voltage);   // Send Voltage Value V0 to Blynk Cloud Server 
  delay(1000);                       // minimum delay MUST BE 1 second !
}


void loop()
{
  Blynk.run();    // Blynk  running
}
