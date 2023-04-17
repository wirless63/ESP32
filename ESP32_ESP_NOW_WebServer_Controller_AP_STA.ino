/*Russ Sveda 2023: ESP32 ESP-NOW Web Server Controller AP STA Mode - This example controls the on-board LED of three reciever boards.  This can be adapted to control other GPIOs to operate relays, etc.
Credits:
  ESP32: ESP-NOW Web Server Sensor Dashboard (ESP-NOW + Wi-Fi) :   https://randomnerdtutorials.com/esp32-esp-now-wi-fi-web-server/
  ESP-NOW with ESP32: Send Data to Multiple Boards (one-to-many) : https://randomnerdtutorials.com/esp-now-one-to-many-esp32-esp8266/
  ESP32 Async Web Server – Control Outputs with Arduino IDE (ESPAsyncWebServer library) : https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/
  Uteh Str Video: Arduino IDE + ESP32 + LEDs + ESP-NOW + Web Server | ESP32 ESP-NOW and Web Server (CONTROLLING) : https://www.youtube.com/watch?v=6O0rVC3zD_I 
 */

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Uses a User Interface Web page, store in the same folder as the .ino file
#include "PageIndex.h" 

// Defines the Digital Pin of the On-Board LEDs.
#define ON_Board_LED 2  

// Replace with your WiFi Router network credentials.
const char* wifi_network_ssid = "Guest-WiFi";
const char* wifi_network_password = "12345678abcde";

//ESP32 Controller Soft Access Point Config.
const char* soft_ap_ssid = "ESP32_Controller";  // name
const char* soft_ap_password = "12345678abcde"; // password
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

// Edit this section if you have more (add them) or less (delete them) Rcvrs
// REPLACE WITH THE MAC ADDRESS OF RECIEVE ESP32s.
uint8_t broadcastAddressESP32Rcvr1[] = {0xC0, 0x49, 0xEF, 0x6B, 0xDF, 0xA0}; // Rcvr 1 Board B
uint8_t broadcastAddressESP32Rcvr2[] = {0xC8, 0xF0, 0x9E, 0xF3, 0x1E, 0xBC}; // Rcvr 2 Board C
uint8_t broadcastAddressESP32Rcvr3[] = {0xC8, 0xF0, 0x9E, 0xF1, 0xF8, 0xB8}; // Rcvr 3 Board D

// The variables used to check the parameters passed in the URL - Look in the "PageIndex.h" file.
// "set_LED?board="+board+"&gpio_output="+gpio+"&val="+value
const char* PARAM_INPUT_1 = "board";
const char* PARAM_INPUT_2 = "gpio_output";
const char* PARAM_INPUT_3 = "val";

// Structure example to send data that must match the receiver structure
typedef struct struct_message_send {
  int send_GPIO_num;
  int send_Val;
} struct_message_send;

struct_message_send send_Data; // Create a struct_message to send data named "send_Data".

// Create a variable of type "esp_now_peer_info_t" to store information about the peer.
esp_now_peer_info_t peerInfo;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  digitalWrite(ON_Board_LED, HIGH); //--> Turn on controller ON_Board_LED.
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  digitalWrite(ON_Board_LED, LOW); //--> Turn off controller ON_Board_LED.
}

// Subroutine to prepare data and send it to the Rcvrs.
void send_data_to_rcvr(int Rcvr_number, int gpio_number, int value) {
  Serial.println();
  Serial.println("Send data");

  send_Data.send_GPIO_num = gpio_number;
  send_Data.send_Val = value;

  esp_err_t result;
  // Edit this section if you have more (add them) or less (delete them) Rcvrs
  // Sending data to Rcvr 1
  if (Rcvr_number == 1) {
    result = esp_now_send(broadcastAddressESP32Rcvr1, (uint8_t *) &send_Data, sizeof(send_Data));
  }
  // Sending data to Rcvr 2
  if (Rcvr_number == 2) {
    result = esp_now_send(broadcastAddressESP32Rcvr2, (uint8_t *) &send_Data, sizeof(send_Data));
  }
    // Sending data to Rcvr 3
  if (Rcvr_number == 3) {
    result = esp_now_send(broadcastAddressESP32Rcvr3, (uint8_t *) &send_Data, sizeof(send_Data));
  }
    if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED set to output
  digitalWrite(ON_Board_LED, LOW); //--> Turn off Led On Board

  // Set Wifi to AP+STA mode
  Serial.println("WIFI mode: AP & STA");
  WiFi.mode(WIFI_AP_STA);
 
  // Access Point Settings
  Serial.println("Setting up ESP32 to be an Access Point for Rcvrs");
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  delay(1000);
  Serial.println("Setting up ESP32 softAPConfig");
  WiFi.softAPConfig(local_ip, gateway, subnet);

  // Connect to Wi-Fi Router
  Serial.print("Connecting to : ");
  Serial.println(wifi_network_ssid);
  WiFi.begin(wifi_network_ssid, wifi_network_password);
  
  // Connecting ESP32 Controller to the WiFi Router.
  // The process timeout of connecting is 20 seconds and if the ESP32 isn't connected by then the ESP32 will restart.

  int connecting_process_timed_out = 20; //20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }
  
  digitalWrite(ON_Board_LED, LOW);
  Serial.println("");
  Serial.println("WiFi connected");

  // Init ESP-NOW
  Serial.println("Initializing ESP-NOW");
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    Serial.println("Restarting ESP32");
    delay(1000);
    ESP.restart();
  }
  Serial.println("ESP-NOW Initialization successful");
  
  // Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasnmitted packet
  Serial.println("Get the status of Trasnmitted packet");
  esp_now_register_send_cb(OnDataSent);
  
  // Register Peers
  Serial.println("Registering peers");
  peerInfo.encrypt = false;

  // Edit this section if you have more (add them) or less (delete them) Rcvrs
   // Register 1st Rcvr
  Serial.println("Register 1st peer (ESP32 Rcvr 1)");
  memcpy(peerInfo.peer_addr, broadcastAddressESP32Rcvr1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add 1st peer");
    return;
  }
   // Register 2nd Rcvr
  Serial.println("Register 2nd peer (ESP32 Rcvr 2)");
  memcpy(peerInfo.peer_addr, broadcastAddressESP32Rcvr2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add 2nd peer");
    return;
  }
   // Register 3rd Rcvr
  Serial.println("Register 3rd peer (ESP32 Rcvr 3)");
  memcpy(peerInfo.peer_addr, broadcastAddressESP32Rcvr3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add 3rd peer");
    return;
  }

  //Handle Web Server
  Serial.println("Setting Up the Main Page on the Server.");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", MAIN_page);
  });
  
  // Send a GET request to <ESP_IP>/set_LED?board=<inputMessage1>&gpio_output=<inputMessage2>&val=<inputMessage3>
  server.on("/set_LED", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // Board = PARAM_INPUT_1 = inputMessage1
    // LED_gpio_num = PARAM_INPUT_2 = inputMessage2
    // LED_val = PARAM_INPUT_3 = inputMessage3

    String Board;
    String LED_gpio_num;
    String LED_val;
    
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2) && request->hasParam(PARAM_INPUT_3)) {
      Board = request->getParam(PARAM_INPUT_1)->value();
      LED_gpio_num = request->getParam(PARAM_INPUT_2)->value();
      LED_val = request->getParam(PARAM_INPUT_3)->value();

      String Rslt = "Board : " + Board + " || GPIO : " + LED_gpio_num + " || Set to :" + LED_val;
      Serial.println();
      Serial.println(Rslt);
      // Edit this section if you have more or less Rcvrs
      // Conditions for sending data to Rcvr 1. 
      if (Board == "ESP32Rcvr1") send_data_to_rcvr(1, LED_gpio_num.toInt(), LED_val.toInt());
      // Conditions for sending data to Rcvr 2.
      if (Board == "ESP32Rcvr2") send_data_to_rcvr(2, LED_gpio_num.toInt(), LED_val.toInt());
      // Conditions for sending data to Rcvr 3.
      if (Board == "ESP32Rcvr3") send_data_to_rcvr(3, LED_gpio_num.toInt(), LED_val.toInt());
    }
    else {
      Board = "No message sent";
      LED_gpio_num = "No message sent";
      LED_val = "No message sent";
    }
    request->send(200, "text/plain", "OK");
  });
  
  Serial.println("Starting the Server");
  server.begin();

  Serial.print("ESP32 Controller IP address of Soft AP: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("ESP32 Controller IP address on the WiFi network: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel Used: ");
  Serial.println(WiFi.channel());
  // If you wish connect to ESP32 controller directly then use the Soft APIP Address or via WiFi then use the WiFi network IP Address.
}

void loop() {
  // put code here to run repeatedly
}
