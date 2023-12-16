# ESP32
ESP32 ESP-Now and Web Server.
The project implements ESP-Now in a one to many configuration. An ESP32 Controller operates as both a Soft AP for ESP-Now communications and STA for WiFi connectivity to a network. 
The ESP32 Controller web server user interface allows controlling GPIOs of receiver ESP32s from a smartphone or PC connected to the same network. In the example, the control is of the on-board LED of multiple receiver boards.
Both an ESP-Now default transmission security and encrypted transmission security examples are provided.
The code was created using Arduino IDE with a board selection of DOIT ESP32 DEVKIT V1 and the actual boards being NodeMCU ESP32 V1.1.

There are two branches below the Main.  One branch has the code for the default ESP-Now transmission security and one for the ESP-Now encrypted transmission security.
