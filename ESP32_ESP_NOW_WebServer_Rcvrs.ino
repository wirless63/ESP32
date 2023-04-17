/*Russ Sveda 2023: ESP32 ESP-NOW Web Server Receivers - This example allows the Controller to control the on-board LED of a reciever board.  This can be modified to control GPIOs to operate relays, etc. 
Credits:
  Rui Santos: ESP-NOW Web Server Sensor Dashboard (ESP-NOW + Wi-Fi) : https://randomnerdtutorials.com/esp32-esp-now-wi-fi-web-server/
  Rui Santos: Send Data to Multiple Boards (one-to-many) : https://randomnerdtutorials.com/esp-now-one-to-many-esp32-esp8266/
  Rui Santos: ESP32 Async Web Server – Control Outputs with Arduino IDE (ESPAsyncWebServer library) : https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/
  Uteh Str: Video - Arduino IDE + ESP32 + LEDs + ESP-NOW + Web Server | ESP32 ESP-NOW and Web Server (CONTROLLING) : https://www.youtube.com/watch?v=6O0rVC3zD_I 
 */
// Adrduino-IDE used with Board set to DOIT ESP32 DEVKIT V1 and actual board used is NodeMCU ESP-32S V1.1 
#include <esp_now.h>  
#include <esp_wifi.h> 
#include <WiFi.h>     

// Defines the Digital Pin of the "On Board LED".
#define ON_Board_LED 2 

// Insert your Controller ESP32 AP SSID (controller code "soft_ap-ssid") 
constexpr char WIFI_SSID[] = "ESP32_Controller";

// Variable for Void Loop WiFi Channel Scan timer (in millis).
unsigned long previousMillisScan = 0;
const long intervalScan = 60000;  //every 60 sec

// Structure to receive data.
// Must match the sender structure in ESP32 Controller code
typedef struct struct_message_receive {
  int receive_GPIO_num;           //in Controller, send struct it is "send_GPIO_num"
  int receive_Val;                //in Controller, send struct it is "send_Val"
} struct_message_receive;

// Create a struct_message to receive data and name it as "receive_Data".
struct_message_receive receive_Data;

// Callback when data is received.
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {

// Get the MAC ADDRESS of the Controller.
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  memcpy(&receive_Data, incomingData, sizeof(receive_Data)); //Copies "incomingData" into "receive_Data"  variable.

  // Prints the MAC ADDRESS of controller and Bytes received
  Serial.print("Packet received from: ");
  Serial.println(macStr);
  Serial.print("Bytes received: ");
  Serial.println(len);
 
  // Handling the assigned GPIO based on the gpio num(ber) and on/off Val(ue) recieved from the Controller
  if (receive_Data.receive_GPIO_num == 2) digitalWrite(ON_Board_LED, receive_Data.receive_Val);
   
  // Prints the data received from the controller
  Serial.print("Receive Data: ");
  Serial.println(receive_Data.receive_GPIO_num);
  Serial.print("Val Data: ");
  Serial.println(receive_Data.receive_Val);
}

//  Function to scan your network and get the channel.
// In this project, the ESP32 Controller and the ESP32 Rcvrs must use the same Wi-Fi channel
// When the ESP32 Controller is connected to the Wi-Fi router, its Wi-Fi channel may be assigned automatically or changed by the Wi-Fi router.
// This function is used so that the ESP32 Rcvr knows what channel that the ESP32 Controller is using. 
int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}
// Scan_and_set_Wifi_Channel
void scan_and_set_Wifi_Channel() {
  Serial.println();
  Serial.print("Scanning SSID: ");
  Serial.println(WIFI_SSID);
  
  // Get the Wi-Fi channel of the ESP32 Controller
  int32_t channel = getWiFiChannel(WIFI_SSID);

  // Get the Wi-Fi channel on this board (ESP32 Rcvr)
  int cur_WIFIchannel = WiFi.channel();

  if (channel == 0) {
    Serial.print("SSID: ");
    Serial.print(WIFI_SSID);
    Serial.println(" not found !");
    Serial.println();
  } else {
    Serial.print("SSID: ");
    Serial.print(WIFI_SSID);
    Serial.print(" found. (Channel : ");
    Serial.print(channel);
    Serial.println(")");
    
  //If the ESP32 Rcvr channel is different then the ESP32 Controller's then its channel is set to the Controller channel
    if (cur_WIFIchannel != channel) {
      Serial.println("Set Wi-Fi channel");
      esp_wifi_set_promiscuous(true);
      esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
      esp_wifi_set_promiscuous(false);
    }
  }
  Serial.print("Wi-Fi channel: ");
  Serial.println(WiFi.channel());
}

void setup() {
  
  Serial.begin(115200);

  pinMode(ON_Board_LED,OUTPUT);    // On Board LED (GPIO 2) set to an output
  digitalWrite(ON_Board_LED, LOW); // Turns off On Board LED (GPIO 2)

  // Set Wi-Fi Mode as Wifi Station
  WiFi.mode(WIFI_STA);
 
  // Scan & Set the Wi-Fi channel
  scan_and_set_Wifi_Channel();

  // Init ESP-NOW and if failed, restart
  Serial.println("Start initializing ESP-NOW");
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    Serial.println("Restarting ESP32");
    delay(1000);
    ESP.restart();
  }
  Serial.println("ESP-NOW Initialized");
 
  // Register for a callback function that will be called when data is received
  Serial.println("Register for a callback function will be called when data is received");
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // put your main code here, to run repeatedly

  // Timer to scan for the ESP32 Controller WiFi channel every xx millis (see "intervalScan" variable).
  unsigned long currentMillisScan = millis();
  if (currentMillisScan - previousMillisScan >= intervalScan) {
    previousMillisScan = currentMillisScan;
    scan_and_set_Wifi_Channel();
  }
}