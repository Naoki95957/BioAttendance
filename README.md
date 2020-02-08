# What an absurd name for a repo

Anyway, this will be the start of an attendance logger in an attempt to make the classroom 'smarter'

The premise is that using biometrics, we will take the attendance of students... that's it.

So the current idea is to use a fingreprint reader. 

I currently am testing this model:
  - [GT-521F32 - Fingerprint Sensor Evaluation Board](https://www.sparkfun.com/products/14518)

I intend to use an ESP-32 and whether it is genuine espressif or not doesn't matter to me. I am going on the assumption that we will need as much space as possible and will be testing with the 32D module:
  - [Genuine ESP-32 (Amazon)](https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z)
  - [Genuine ESP-32 (Aliexpress)](https://www.aliexpress.com/item/4000090132509.html)

The current outline is to make this both an access point and a client*
  - Access point
    - This will be done so that upon configuring in a new enviroment, it can quickly be done on any device with a browser and wifi
  - Client
    - This would be used so that after it is configured in a new enviroment, we can attach logs to a database or serve csv's, etc
  
  *Note that this is possible by setting the ESP-32 into APSTA mode.

## The Access Point
The access point is basically going to be our initial setup so it will be important to have a script that can communicate via websockets
or http 2.0 so that there is bidirectional communication from devices and the ESP-32.

Additionally this is how we will activate registration of finger-prints, override attendance, and manage the device.
I anticipate a bulk of the work exisiting here

## The Client connection
The client connection will be used so that when in a LAN it can talk to a more dedicated device, like a database.
It should commincate to other devices in a uni-directional mode and only need to serve as an http-server or database client.

## Setting up the ESP-32
Since I will be using the Arduino IDE, it will be important to share step-by-step what libraries, frequencies, baud-rate, etc
and any other device specific things that I will use. I will add more detail here when I get to it

USB Driver for my particular board/s
  - Silicon Labs CP210x USB to UART Bridge Driver Ver. 10.1.8.2466
  - buad 9600, 8 data bits (this is different from serial comms on Arudino!)
  - for windows, check the device manager to ensure your device is comming up under 'Ports (COM & PT)'
  
Testing HTTP webservicing
  - Use incognito or any browser that doesn't chache 
    - This is neccessary since the communication is primarily going to be done by indexing the url header

### [Arduino IDE](https://www.arduino.cc/en/Main/Software)
  - **When uploading, don't forget to hold boot!**
    - Then release when it establishes connection
  - Current version is 1.8.11
  - Adding board manager
    - File > Preferences
      - insert this literal text-link into 'Additional Board Manager URLs:'
       "https://dl.espressif.com/dl/package_esp32_index.json"
     - Tools > Board > Boards Manager
      - Search for ESP32 and install the manager
  - I'm currently using **ESP32 DEV MODULE** but the **DOIT ESP32 DEVKIT V1** and a few others should work too. Just be sure to pick the board you have and if you don't see it, go with **ESP32 DEV MODULE**
  
  - Adding libraries
    - A bulk of the libraries used will be from [espressif](https://github.com/espressif/arduino-esp32)
      - Necessary librarys TBD
      - Sketch > Include Library > Add .ZIP library

  - Board settings (under tools)
    - Upload Speed: 921600
    - CPU Frequency: 240MHz (WiFi/BT)
    - Flash Frequency: 80MHz
    - Flash Mode: QIO
    - Flash Size: 16MB (128Mb)
    - Partition Scheme: 16MB Flash (3MB APP/9MB FATFS)
    - Core Debug Level: None
    - PSRAM: Disabled
  
  - Uploading a sketch
    - Ensure that the device is connected and showing up in device manager
      - if not, you may have to check for correct drivers
      - sometimes its also just a bad cable
    - Select the port in Arduino IDE under tools
    - Programmer should be left as default (AVRISP mkII)
    - Click upload and hold the boot button on the ESP-32 until you see it connects
    - Once it's finished, you're done!

##Guide-lines
Since this is my first attempt to collaborate an Arduino project, I'm not exactly sure how to split the work-load.
I would ask that branching is done for now. I know that the master file must be .ino, but from there on out we should be able to add
.c and .h files for arduino to use. I might right off everything except the html currently to some classes? We'll see what we need when it gets there. 

Pinout Reference
  ![ESP32_pinout](https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2018/08/ESP32-DOIT-DEVKIT-V1-Board-Pinout-30-GPIOs-Copy.png)
  
  
## Extras
I might look into OTA options for this project. Having that capability with the device once it is already packaged up will be nice. This is entirely conditional however since the project is not done and I do not know the size constraints for things like program memory, file systems, etc.
