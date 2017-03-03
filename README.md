
# Nabto-ESP8266 Arduino Library
This is an example AppMyProduct device application on the ESP8266 Arduino platform simulating a heatpump. Currently supports the [Heat Control](https://github.com/nabto/ionic-starter-nabto) Ionic starter app.

Please also refer to [this article](https://blog.nabto.com/2017/03/03/esp8266-wifi-module-appmyproduct/) on the [Nabto blog](http://blog.nabto.com/) for more information.

## What is needed to run the example
The example is tested on an [Adafruit HUZZAH ESP8266 Breakout board](https://www.adafruit.com/products/2471) connected with a standard [USB to TTL Serial Cable](https://www.adafruit.com/products/954) in the following way:

![USB to TTL Serial Cable wiring](https://nabto.files.wordpress.com/2016/03/wiring.png?w=676)

To compile the example you need to install the [Arduino IDE](https://www.arduino.cc/en/Main/Software) and the ESP8266 Board Package. Adafruit provides a [nice tutorial](https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide) on how to setup your Arduino IDE accordingly.

We also need a device name and key, both of which can be created at [portal.appmyproduct.com](https://portal.appmyproduct.com/)

## Step 1: Download the ESP8266 Board Package
Follow the steps laid out [here](https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide) to install the ESP8266 Board Package into the Arduino IDE OR read it right here: 

Open up the Arduino IDE. 
Click `File` -> `Preferences` and then copy this
```shell
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```
into the `Additional Boards Manager URLs` input field. Click `OK`

Now click `Tools` --> `Board: ` -> `Boards Manager`

Here we search for `ESP8266` and click `Install`

The ESP8266 Board Package is now installed

## Step 2: Install the uNabto SDK library
There are two ways of getting the uNabto SDK library:

Using git
```shell
git clone --recursive https://github.com/nabto/unabto-esp8266-sdk
```
then change directory to the `unabto-esp8266-sdk` folder and run
```shell
copy-unabto.cmd # Windows
copy-unabto.sh  # Linux
```
to copy the files from the unabto submodule into the appropriate folders. Afterwards you can create a zip library with
```shell
make-lib.cmd # Windows
make-lib.sh  # Linux
```

OR

Download the release zip file [unabto-esp8266-sdk.zip](https://github.com/nabto/unabto-esp8266-sdk/releases)

No matter which way, we can now add the library to the Arduino IDE via `Sketch` -> `Include Library` -> `Add .ZIP Library...` and then browse to and add the `unabto-esp8266-sdk.zip` that was just created/downloaded.

## Run the example
We can now open the [HeatPump.ino](./examples/HeatPump/HeatPump.ino) example by going to `File` -> `Examples` -> `Nabto-ESP8266` -> `HeatPump`

Here we need to enter the SSID and password of the wireless network we want to attach to. Furthermore we need to input the device name and key we created at [portal.appmyproduct.com](https://portal.appmyproduct.com/).

When the example is compiled you set the device into flash mode by pressing the following button combination on the Adafruit HUZZAH ESP8266 Breakout board:

1. Hold down the GPIO0 button, the red LED will light up
2. While holding down GPIO0, click the RESET button
3. Release RESET, then release GPIO0
4. When you release the RESET button, the red LED should light up dimly

Now the sketch can be uploaded to the device. When finished it automatically connects to the specified wifi network and runs uNabto. 

In your serial monitor you should see the following printout:

```
Connecting to WiFi........done
Init Nabto...
Device id: 'n7j4qebq.hyr7o.appmyproduct.com'
Program Release 123.456
Application event framework using SYNC model
SECURE ATTACH: 1, DATA: 1
NONCE_SIZE: 32, CLEAR_TEXT: 0
Nabto was successfully initialized
Nabto v123.456 running
SECURE ATTACH: 1, DATA: 1
NONCE_SIZE: 32, CLEAR_TEXT: 0
State change from IDLE to WAIT_DNS
Resolving dns: esp8266test1.demo.nab.to
State change from WAIT_DNS to WAIT_BS
State change from WAIT_BS to WAIT_GSP
########    U_INVITE with LARGE nonce sent, version: - URL: -
State change from WAIT_GSP to ATTACHED
```

Now, connect to your device using the [Heat Control](https://github.com/nabto/ionic-starter-nabto) Ionic starter app and see the LED change its brightness according to the target heat.
