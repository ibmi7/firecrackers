//Override the default main function to remove USB CDC feature - @farome contribution
int main(void){
init();
initVariant();

//Disabling UART0 (saves around 300-500µA) - @Jul10199555 contribution
NRF_UART0->TASKS_STOPTX = 1;
NRF_UART0->TASKS_STOPRX = 1;
NRF_UART0->ENABLE = 0;

*(volatile uint32_t *)0x40002FFC = 0;
*(volatile uint32_t *)0x40002FFC;
*(volatile uint32_t *)0x40002FFC = 1; //Setting up UART registers again due to a library issue


  setup();
  for(;;){
    loop();
//If you won't be using serial communication comment next line
    if(arduino::serialEventRun) arduino::serialEventRun();
  }
  return 0;
}

#include "ArduinoLowPower_Adjusted.h"

static char recv_buf[512];
static bool is_exist = false;
static bool is_join = false;
static int led = 0;
int ret = 0;
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

float delay_min = 1;

void setup() {
  delay(10000);
  digitalWrite(PIN_ENABLE_SENSORS_3V3, HIGH); //PIN_ENABLE_I2C_PULLUP - @pert contribution
  digitalWrite(PIN_ENABLE_I2C_PULLUP, HIGH); //PIN_ENABLE_SENSORS_3V3 - @pert contribution
  Serial.begin(9600);
  // put your setup code here, to run once:
  //début du serial1 pour la communication LoRaWAN
  Serial1.begin(9600);

  if (at_send_check_response("+AT: OK", 100, "AT\r\n")) {
    is_exist = true;
    at_send_check_response("+ID: AppEui", 1000, "AT+ID\r\n");
    at_send_check_response("+MODE: LWOTAA", 1000, "AT+MODE=LWOTAA\r\n");
    at_send_check_response("+DR: EU868", 1000, "AT+DR=EU868\r\n");
    at_send_check_response("+CH: NUM", 1000, "AT+CH=NUM,0-2\r\n");
    at_send_check_response("+KEY: APPKEY", 1000,
                           "AT+KEY=APPKEY,\"154C75B5F00F87DDE9DF98B6AF5A6E2E\"\r\n");
    at_send_check_response("+KEY: DEVEUI", 1000, "AT+ID=DEVEUI,\"70B3D57ED0058111\"\r\n");
    at_send_check_response("+KEY: APPEUI", 1000, "AT+ID=APPEUI,\"0000000000000000\"\r\n");
    at_send_check_response("+CLASS: C", 1000, "AT+CLASS=A\r\n");
    ret = at_send_check_response("+PORT: 8", 1000, "AT+PORT=8\r\n");
    delay(200);
    is_join = true;
    //économie de batterie en éteignant la led PWR
  } else {
    is_exist = false;
  }
}


void loop() {
  delay(2000);

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
      sprintf(cmd, "AT+MSGHEX=%04X\r\n",-10);
      at_send_check_response("ACK Received", 5000, cmd);
      char * downlink = strstr(recv_buf,"\"");
      downlink = strtok(downlink,"\"");
      Serial.println(downlink);
      if (downlink) delay_min = strtol(downlink,NULL,16);
      Serial.println(downlink);
      LowPower.sleep(10000);
    }
  } else {
    delay(1000);
  }
}