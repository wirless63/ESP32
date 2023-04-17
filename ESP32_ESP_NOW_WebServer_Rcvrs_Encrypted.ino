/*Russ Sveda 2023: ESP32 ESP-NOW Web Server Receivers Encrypted - This example allows the Controller to control the on-board LED of a reciever board.  This can be modified to control GPIOs to operate relays, etc. 
Credits:
  Rui Santos: ESP-NOW Web Server Sensor Dashboard (ESP-NOW + Wi-Fi) : https://randomnerdtutorials.com/esp32-esp-now-wi-fi-web-server/
  Rui Santos: Send Data to Multiple Boards (one-to-many) : https://randomnerdtutorials.com/esp-now-one-to-many-esp32-esp8266/
  Rui Santos: ESP32 Async Web Server – Control Outputs with Arduino IDE (ESPAsyncWebServer library) : https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/
  Uteh Str: Video - Arduino IDE + ESP32 + LEDs + ESP-NOW + Web Server | ESP32 ESP-NOW and Web Server (CONTROLLING) : https://www.youtube.com/watch?v=6O0rVC3zD_I
  Unknown: ESP32 ESP-NOW Send and Receive Encrypted Messages - https://microcontrollerslab.com/esp32-esp-now-encrypted-messages
 */
// Arduino-IDE 2.0.4 used with a board selection of DOIT ESP32 DEVKIT V1 and board used is a NodeMCU ESP-32S V1.1

#include <esp_now.h>  
#include <esp_wifi.h> 
#include <WiFi.h>     

// Define the MAC Address of the Controller
uint8_t masterMacAddress[] = {0x3C, 0x71, 0xBF, 0x84, 0xD1, 0x7C};

// Define the PMK and LMK encryption keys
static const char* PMK_KEY = "123hjk9i0ii67bte";
static const char* LMK_KEY = "0ii67bte123hjk9i";

// Defines the Digital Pin of the Rcvr On-Board LED
#define ON_Board_LED 2 

// Insert your Controller ESP32 AP SSID (controller code variable "soft_ap-ssid") 
constexpr char WIFI_SSID[] = "ESP32_Controller";

// Variable for Void Loop WiFi Channel Scan timer (in millis).
unsigned long previousMillisScan = 0;
const long intervalScan = 120000;  //every 120 sec

// Structure to receive data.
// Must match the sender structure in ESP32 Controller code
typedef struct struct_message_receive {
  int receive_GPIO_num;           //in Controller, send struct is "send_GPIO_num"
  int receive_Val;                //in Controller, send struct is "send_Val"
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
  Serial.print("LMK Key used: ");
  Serial.println(LMK_KEY);
  Serial.println("");  
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

// Create a variable of type "esp_now_peer_info_t" to store information about the peer
esp_now_peer_info_t peerInfo;

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

  // Set the encryption keys
  esp_now_set_pmk((uint8_t *)PMK_KEY);

  Serial.println("ESP-NOW Initialized");

  memcpy(peerInfo.peer_addr, masterMacAddress, 6);
  for (uint8_t i = 0; i < 16; i++) {
    peerInfo.lmk[i] = LMK_KEY[i];
  }
  peerInfo.encrypt = true;
       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

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