# What an absurd name for a repo

Anyway, this will be the start of an attendance logger in an attempt to make the classroom 'smarter'

The premise is that using biometrics, we will take the attendance of students... that's it

So the current idea is to use a fingreprint reader. 

I currently am testing this model:
  - [GT-521F32 - Fingerprint Sensor Evaluation Board](https://www.sparkfun.com/products/14518)

I intend to use an ESP-32 and whether it is genuine espressif or not doesn't matter to me
I am currently using these ESP-32 wroom models:
  - [Genuine ESP-32 wroom](https://www.aliexpress.com/item/32793415575.html)

The current outline is to make this both an access point and a client*
  - Access point
    - This will be done so that upon configuring in a new enviroment, it can quickly be done on any device with a browser and wifi
  - Client
    - This would be used so that after it is configured in a new enviroment, we can attach logs to a database or serve csv's, etc
  
  *Note that this is possible by setting the ESP-32 into APSTA mode

## The Access Point
The access point is basically going to be our initial setup so it will be important to have a script that can communicate via websockets
or http 2.0 so that there is bidirectional communication from devices and the ESP-32.

Additionally this is how we will activate registration of finger-prints, override attendance, and manage the device
I anticipate a bulk of the work exisiting here

## The Client connection
The client connection will be used so that when in a LAN it can talk to a more dedicated device, like a database.
It should commincate to other devices in a uni-directional mode and only need to serve as an http-server or database client.

## Setting up the ESP-32
Since I will be using the Arduino IDE, it will be important to share step-by-step what libraries, baud-rate, and any other device specific 
things that I will use. I will add more detail here when I get to it
