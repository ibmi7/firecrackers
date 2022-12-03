/*!
   file getVoltageCurrentPower.ino
   SEN0291 Gravity: I2C Digital Wattmeter
   The module is connected in series between the power supply and the load to read the voltage, current and power
   The module has four I2C, these addresses are:
   INA219_I2C_ADDRESS1  0x40   A0 = 0  A1 = 0
   INA219_I2C_ADDRESS2  0x41   A0 = 1  A1 = 0
   INA219_I2C_ADDRESS3  0x44   A0 = 0  A1 = 1
   INA219_I2C_ADDRESS4  0x45   A0 = 1  A1 = 1

   Copyright    [DFRobot](https://www.dfrobot.com), 2016
   Copyright    GNU Lesser General Public License
   version  V0.1
   date  2019-2-27
*/

#include <Wire.h>
#include "DFRobot_INA219.h"

/**
 * @fn DFRobot_INA219_IIC
 * @brief pWire I2C controller pointer
 * @param i2caddr  I2C address
 * @n INA219_I2C_ADDRESS1  0x40   A0 = 0  A1 = 0
 * @n INA219_I2C_ADDRESS2  0x41   A0 = 1  A1 = 0
 * @n INA219_I2C_ADDRESS3  0x44   A0 = 0  A1 = 1
 * @n INA219_I2C_ADDRESS4  0x45   A0 = 1  A1 = 1	 
  */
DFRobot_INA219_IIC ina219(&Wire, INA219_I2C_ADDRESS4);

// Revise the following two paramters according to actula reading of the INA219 and the multimeter
// for linearly calibration
float ina219Reading_mA = 1000;
float extMeterReading_mA = 1000;

//Example data:
int data[64] = { 14, 30, 35, 34, 34, 40, 46, 45, 30, 4, -26, -48, -55, -49, -37,
                 -28, -24, -22, -13, 6, 32, 55, 65, 57, 38, 17, 1, -6, -11, -19, -34,
                 -51, -61, -56, -35, -7, 18, 32, 35, 34, 35, 41, 46, 43, 26, -2, -31, -50,
                 -55, -47, -35, -27, -24, -21, -10, 11, 37, 58, 64, 55, 34, 13, -1, -7 };
byte sine_data[91] = {
  0,
  4, 9, 13, 18, 22, 27, 31, 35, 40, 44,
  49, 53, 57, 62, 66, 70, 75, 79, 83, 87,
  91, 96, 100, 104, 108, 112, 116, 120, 124, 127,
  131, 135, 139, 143, 146, 150, 153, 157, 160, 164,
  167, 171, 174, 177, 180, 183, 186, 189, 192, 195,  //Paste this at top of program
  198, 201, 204, 206, 209, 211, 214, 216, 219, 221,
  223, 225, 227, 229, 231, 233, 235, 236, 238, 240,
  241, 243, 244, 245, 246, 247, 248, 249, 250, 251,
  252, 253, 253, 254, 254, 254, 255, 255, 255, 255
};

float amp[1024];
float freq[1024];
float s_bin098_146Hz = 0;
float s_bin146_195Hz = 0;
float s_bin195_244Hz = 0;
float s_bin244_293Hz = 0;
float s_bin293_342Hz = 0;
float s_bin342_391Hz = 0;
float s_bin391_439Hz = 0;
float s_bin439_488Hz = 0;
float s_bin488_537Hz = 0;
float s_bin537_586Hz = 0;
int j_bin098_146Hz = 0;
int j_bin146_195Hz = 0;
int j_bin195_244Hz = 0;
int j_bin244_293Hz = 0;
int j_bin293_342Hz = 0;
int j_bin342_391Hz = 0;
int j_bin391_439Hz = 0;
int j_bin439_488Hz = 0;
int j_bin488_537Hz = 0;
int j_bin537_586Hz = 0;
int size = 0;
// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "HX711.h"
#define MAXIMWIRE_EXTERNAL_PULLUP

#include <MaximWire.h>

#define PIN_BUS 9

MaximWire::Bus bus(PIN_BUS);
MaximWire::DS18B20 device;
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;
#define DHTPIN D11  // Digital pin connected to the DHT sensor
#define DHTPIN2 D7
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE DHT22  // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
#define T 64

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);
DHT_Unified dht2(DHTPIN2, DHTTYPE);
uint32_t delayMS;

static char recv_buf[512];
static bool is_exist = false;
static bool is_join = false;
static int led = 0;
int ret = 0;
float tmp = 20;
float hum = 50;
float tmp2 = 20;
float hum2 = 50;
short batterie = 0;
float poids = 0.0;
float voltage = 0.0;
float temp[3];
char cpt = 0;
static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...) {
  int ch;
  int num = 0;
  int index = 0;
  int startMillis = 0;
  memset(recv_buf, 0, sizeof(recv_buf));
  Serial1.write(p_cmd);
  delay(200);
  startMillis = millis();
  do {
    while (Serial1.available() > 0) {
      ch = Serial1.read();
      recv_buf[index++] = ch;
      delay(2);
    }
  } while (millis() - startMillis < timeout_ms);
  if (strstr(recv_buf, p_ack) != NULL) {
    return 1;
  } else return 0;
}
void setup() {
  delay(10000);
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(12,OUTPUT);
  digitalWrite(12,HIGH);
  //début du serial1 pour la communication LoRaWAN
  Serial1.begin(9600);

  //connexion I2C pour l'intensité du panneau solaire
  int cptINA = 0;
  while (ina219.begin() != true) {
    delay(2000);
    cptINA++;
    if (cptINA==5) break;
  }
  ina219.linearCalibrate(ina219Reading_mA, extMeterReading_mA);

  //connexion LoRaWAN
  if (at_send_check_response("+AT: OK", 100, "AT\r\n")) {
    is_exist = true;
    at_send_check_response("+ID: AppEui", 1000, "AT+ID\r\n");
    at_send_check_response("+MODE: LWOTAA", 1000, "AT+MODE=LWOTAA\r\n");
    at_send_check_response("+DR: EU868", 1000, "AT+DR=EU868\r\n");
    at_send_check_response("+CH: NUM", 1000, "AT+CH=NUM,0-2\r\n");
    at_send_check_response("+KEY: APPKEY", 1000,
                           "AT+KEY=APPKEY,\"5096ECB9BD2700F2C039F5A04351B770\"\r\n");
    at_send_check_response("+KEY: DEVEUI", 1000, "AT+ID=DEVEUI,\"ABC3E554E09DF7E6\"\r\n");
    at_send_check_response("+KEY: APPEUI", 1000, "AT+ID=APPEUI,\"0000000000000000\"\r\n");
    at_send_check_response("+CLASS: C", 1000, "AT+CLASS=A\r\n");
    ret = at_send_check_response("+PORT: 8", 1000, "AT+PORT=8\r\n");
    delay(200);
    is_join = true;
    //économie de batterie en éteignant la led PWR
    digitalWrite(LED_PWR, LOW);
  } else {
    is_exist = false;
  }


  // setup du capteur hum/temp
  dht.begin();
  dht2.begin();
  // setup du capteur poids
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(28395.61);
}

float delay_min = 20;

void loop() {
  digitalWrite(12,HIGH);
  delay(2000);

  //FFT du son

  //acquisition du micro
  int tab[T];
  for (int i = 0; i < T; i++) {
    tab[i] = analogRead(A0);
    delayMicroseconds(500);
  }
  Q_FFT(tab, T, 2000);
  
  
  //capteur de Temp/Hum
  sensors_event_t event;
  sensors_event_t event2;
  dht.temperature().getEvent(&event);
  dht2.temperature().getEvent(&event2);
  tmp = event.temperature;
  tmp2 = event2.temperature;
  dht.humidity().getEvent(&event);
  dht2.humidity().getEvent(&event2);
  hum = event.relative_humidity;
  hum2 = event2.relative_humidity;

  //capteur de poids
  scale.power_up();
  poids = scale.get_units(2);
  scale.power_down();  //capteur en mode repos
  
  //capteur de voltage
  ina219.setMode(ina219.eIna219SAndBVolCon);
  voltage = ina219.getBusVoltage_V();
  ina219.setMode(ina219.eIna219PowerDown);
  //voltage = ina219.getPower_mW()*1000*10/0.01;
  //ina219.setMode(ina219.eIna219PowerDown);

  //mesure niveau batterie
  batterie = (short)(analogRead(A7) * 100 * 3.3 / (2.1 * 1023));  //en %

  // capteurs de temp exterieurs
  MaximWire::Discovery discovery = bus.Discover();
  do {
    MaximWire::Address address;
    if (discovery.FindNextDevice(address)) {
      if (address.GetModelCode() == MaximWire::DS18B20::MODEL_CODE) {
        MaximWire::DS18B20 device(address);
        temp[cpt] = device.GetTemperature<float>(bus);
        device.Update(bus);
      }
    }
    if (cpt == 2) cpt = 0;
    else cpt++;
  } while (discovery.HaveMore());

  //vérification de la validité des valeurs
  // début envoi
  if (is_exist) {
    int ret = 0;
    if (is_join) {
      ret = at_send_check_response("+JOIN: Network joined", 12000, "AT+JOIN\r\n");
      if (ret) {
        is_join = false;
      } else {
        at_send_check_response("+ID: AppEui", 1000, "AT+ID\r\n");
        delay(5000);
      }
    } else {
      char cmd[128];
      sprintf(cmd, "AT+MSGHEX=%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X%04X\r\n", short(poids * 100), short(tmp * 100), short(voltage * 100), (batterie), short(hum * 100), (short)(temp[0] * 100), (short)(temp[1] * 100),short(tmp2 * 100),short(hum2 * 100), (short)(100 * s_bin098_146Hz), (short)(100 * s_bin146_195Hz), (short)(100 * s_bin195_244Hz), (short)(100 * s_bin244_293Hz), (short)(100 * s_bin293_342Hz), (short)(100 * s_bin342_391Hz), (short)(100 * s_bin391_439Hz), (short)(100 * s_bin439_488Hz), (short)(100 * s_bin488_537Hz), (short)(100 * s_bin537_586Hz),(short)(temp[2] * 100));
      at_send_check_response("ACK Received", 5000, cmd);
      char * downlink = strstr(recv_buf,"\"");
      downlink = strtok(downlink,"\"");
      if (downlink) delay_min = strtol(downlink,NULL,16);
      delay(delay_min*60000);
    }
  } else {
    delay(1000);
  }
  digitalWrite(12,LOW);
}












//-----------------------------FFT Function----------------------------------------------//
/*
Code to perform High speed (5-7 times faster) and low accuracy FFT on arduino,
This code compromises accuracy for speed,
setup:

1. in[]     : Data array, 
2. N        : Number of sample (recommended sample size 2,4,8,16,32,64,128...)
3. Frequency: sampling frequency required as input (Hz)

It will by default return frequency with max aplitude,

If sample size is not in power of 2 it will be clipped to lower side of number. 
i.e, for 150 number of samples, code will consider first 128 sample, remaining sample  will be omitted.
For Arduino nano, FFT of more than 256 sample not possible due to mamory limitation 
Code by ABHILASH
Contact: abhilashpatel121@gmail.com 
Documentation & deatails: https://www.instructables.com/member/abhilash_patel/instructables/
*/


float Q_FFT(int in[], int N, float Frequency) {

  unsigned int Pow2[13] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };  // declaring this as global array will save 1-2 ms of time


  int a, c1, f, o, x;
  byte check = 0;
  a = N;

  for (int i = 0; i < 12; i++)  //calculating the levels
  {
    if (Pow2[i] <= a) { o = i; }
  }

  int out_r[Pow2[o]] = {};   //real part of transform
  int out_im[Pow2[o]] = {};  //imaginory part of transform

  x = 0;
  for (int b = 0; b < o; b++)  // bit reversal
  {
    c1 = Pow2[b];
    f = Pow2[o] / (c1 + c1);
    for (int j = 0; j < c1; j++) {
      x = x + 1;
      out_im[x] = out_im[j] + f;
    }
  }


  for (int i = 0; i < Pow2[o]; i++)  // update input array as per bit reverse order
  {
    out_r[i] = in[out_im[i]];
    out_im[i] = 0;
  }


  int i10, i11, n1, tr, ti;
  float e;
  int c, s;
  for (int i = 0; i < o; i++)  //fft
  {
    i10 = Pow2[i];                // overall values of sine/cosine
    i11 = Pow2[o] / Pow2[i + 1];  // loop with similar sine cosine
    e = 360 / Pow2[i + 1];
    e = 0 - e;
    n1 = 0;

    for (int j = 0; j < i10; j++) {
      c = e * j;
      while (c < 0) { c = c + 360; }
      while (c > 360) { c = c - 360; }

      n1 = j;

      for (int k = 0; k < i11; k++) {

        if (c == 0) {
          tr = out_r[i10 + n1];
          ti = out_im[i10 + n1];
        } else if (c == 90) {
          tr = -out_im[i10 + n1];
          ti = out_r[i10 + n1];
        } else if (c == 180) {
          tr = -out_r[i10 + n1];
          ti = -out_im[i10 + n1];
        } else if (c == 270) {
          tr = out_im[i10 + n1];
          ti = -out_r[i10 + n1];
        } else if (c == 360) {
          tr = out_r[i10 + n1];
          ti = out_im[i10 + n1];
        } else if (c > 0 && c < 90) {
          tr = out_r[i10 + n1] - out_im[i10 + n1];
          ti = out_im[i10 + n1] + out_r[i10 + n1];
        } else if (c > 90 && c < 180) {
          tr = -out_r[i10 + n1] - out_im[i10 + n1];
          ti = -out_im[i10 + n1] + out_r[i10 + n1];
        } else if (c > 180 && c < 270) {
          tr = -out_r[i10 + n1] + out_im[i10 + n1];
          ti = -out_im[i10 + n1] - out_r[i10 + n1];
        } else if (c > 270 && c < 360) {
          tr = out_r[i10 + n1] + out_im[i10 + n1];
          ti = out_im[i10 + n1] - out_r[i10 + n1];
        }

        out_r[n1 + i10] = out_r[n1] - tr;
        out_r[n1] = out_r[n1] + tr;
        if (out_r[n1] > 15000 || out_r[n1] < -15000) { check = 1; }

        out_im[n1 + i10] = out_im[n1] - ti;
        out_im[n1] = out_im[n1] + ti;
        if (out_im[n1] > 15000 || out_im[n1] < -15000) { check = 1; }

        n1 = n1 + i10 + i10;
      }
    }

    if (check == 1) {  // scale the matrics if value higher than 15000 to prevent varible from overloading
      for (int i = 0; i < Pow2[o]; i++) {
        out_r[i] = out_r[i] / 100;
        out_im[i] = out_im[i] / 100;
      }
      check = 0;
    }
  }

  /*
for(int i=0;i<Pow2[o];i++)
{
Serial.print(out_r[i]);
Serial.print("\t");                                     // un comment to print RAW o/p    
Serial.print(out_im[i]); Serial.println("i");      
}
*/

  //---> here onward out_r contains amplitude and our_in conntains frequency (Hz)
  int fout, fm, fstp;
  float fstep;
  fstep = Frequency / N;
  fstp = fstep;
  fout = 0;
  fm = 0;

  for (int i = 1; i < Pow2[o - 1]; i++)  // getting amplitude from compex number
  {
    if ((out_r[i] >= 0) && (out_im[i] >= 0)) {
      out_r[i] = out_r[i] + out_im[i];
    } else if ((out_r[i] <= 0) && (out_im[i] <= 0)) {
      out_r[i] = -out_r[i] - out_im[i];
    } else if ((out_r[i] >= 0) && (out_im[i] <= 0)) {
      out_r[i] = out_r[i] - out_im[i];
    } else if ((out_r[i] <= 0) && (out_im[i] >= 0)) {
      out_r[i] = -out_r[i] + out_im[i];
    }
    // to find peak sum of mod of real and imaginery part are considered to increase speed

    out_im[i] = out_im[i - 1] + fstp;
    if (fout < out_r[i]) {
      fm = i;
      fout = out_r[i];
    }
    amp[i] = out_r[i];
    freq[i] = out_im[i];
    size = Pow2[o - 1];
    /*
         Serial.print(out_im[i]);Serial.print("Hz");
         Serial.print("\t");                            // un comment to print freuency bin    
         Serial.println(out_r[i]); 
          */
  }
  for (int i = 0; i < size; i++) {
    if (freq[i] >= 98 && freq[i] < 146){
      s_bin098_146Hz += amp[i];
      j_bin098_146Hz ++;
    } 
    else if (freq[i] >= 146 && freq[i] < 195){
      s_bin146_195Hz += amp[i];
      j_bin146_195Hz++;
    } 
    else if (freq[i] >= 195 && freq[i] < 244){
      s_bin195_244Hz += amp[i];
      j_bin195_244Hz++;
    } 
    else if (freq[i] >= 244 && freq[i] < 293){
      s_bin244_293Hz += amp[i];
      j_bin244_293Hz++;
    } 
    else if (freq[i] >= 293 && freq[i] < 342){
      s_bin293_342Hz += amp[i];
      j_bin293_342Hz++;
    } 
    else if (freq[i] >= 342 && freq[i] < 391){
      s_bin342_391Hz += amp[i];
      j_bin342_391Hz++;
    } 
    else if (freq[i] >= 391 && freq[i] < 439){
      s_bin391_439Hz += amp[i];
      j_bin391_439Hz++;
    } 
    else if (freq[i] >= 439 && freq[i] < 488){
      s_bin439_488Hz += amp[i];
      j_bin439_488Hz++;
    } 
    else if (freq[i] >= 488 && freq[i] < 537){
      s_bin488_537Hz += amp[i];
      j_bin488_537Hz++;
    } 
    else if (freq[i] >= 537 && freq[i] < 586){
      s_bin537_586Hz += amp[i];
      j_bin537_586Hz++;
    } 
  }
  if (j_bin098_146Hz) s_bin098_146Hz /= j_bin098_146Hz;
  if (j_bin146_195Hz)   s_bin146_195Hz /= j_bin146_195Hz;
  if (j_bin195_244Hz)  s_bin195_244Hz /= j_bin195_244Hz;
  if (j_bin244_293Hz) s_bin244_293Hz /= j_bin244_293Hz;
  if (j_bin293_342Hz) s_bin293_342Hz /= j_bin293_342Hz;
  if (j_bin342_391Hz) s_bin342_391Hz /= j_bin342_391Hz;
  if (j_bin391_439Hz) s_bin391_439Hz /= j_bin391_439Hz;
  if (j_bin439_488Hz) s_bin439_488Hz /= j_bin439_488Hz;
  if (j_bin488_537Hz) s_bin488_537Hz /= j_bin488_537Hz;
  if (j_bin537_586Hz) s_bin537_586Hz /= j_bin537_586Hz;


  float fa, fb, fc;
  fa = out_r[fm - 1];
  fb = out_r[fm];
  fc = out_r[fm + 1];
  fstep = (fa * (fm - 1) + fb * fm + fc * (fm + 1)) / (fa + fb + fc);

  return (fstep * Frequency / N);
}


//------------------------------------------------------------------------------------//