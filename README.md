## AMB Demo Application

### Hardware Assembly
* Connect the Audio Mezzanine Board to the DragonBoard 410c. (The DragonBoard 410c that comes with the Arrow DragonBoard 410c Audio Kit should already include the audio pin header.)
* Insert a headset to headphone/microphone splitter into the headset jack on the Audio Mezzanine Board.
* Connect a microphone to the microphone jack on the splitter.
* Connect a HDMI monitor to the DragonBoard 410c HDMI jack.
* Connect a USB keyboard to one of the DragonBoard 410c USB jacks.
* Connect a USB mouse to one of the DragonBoard 410c USB jacks.
* Connect the Grove color sensor to the Grove I2C cable.
* Connect the Grove I2C cable to the I2C0 port on the Audio Mezzanine Board.
* Set Switch 1 *(SW1)* on the Audio Mezzanine Board to the ON position.
* Set Switch 2 *(SW2)* on the Audio Mezzanine Board to the 5V position.
* Set Switch 6-3 *(SW6-3)* underneath the DragonBoard 410c to the ON position *(USB HOST)*.
* Insert the power cable to the DragonBoard 410c.

### Installation
* Follow the directions at [96boards.org](http://www.96boards.org/documentation/ConsumerEdition/DragonBoard-410c/Downloads/Debian.md/) to install the Debian OS on the DragonBoard 410c.
   * Be sure to use version [16.06](http://builds.96boards.org/releases/dragonboard410c/linaro/debian/16.06/).
   * If using the SD Card method, download the [boot image](http://builds.96boards.org/releases/dragonboard410c/linaro/debian/16.06/dragonboard410c_sdcard_install_debian-110.zip), and be sure to set the Switch 6-2 (SW6-2) on the back of the DragonBoard 410c to OFF before rebooting.
   * The DragonBoard 410c included in the Audio Kit from Arrow should already have the correct version of Linux installed for you.
* Boot the DragonBoard 410c to get to the desktop on the monitor.
* Connect to a WiFi network by clicking on the Networking icon in the taskbar.
* Open a terminal by navigating: **\[Start\] -> \[System Tools\] -> \[LXTerminal\]**
* Download the necessary supporting packages by running the following commands in the LXTerminal:
```
sudo apt-get update
sudo apt-get -y install espeak
sudo apt-get -y install libttspico0
sudo apt-get -y install libttspico-utils
sudo apt-get -y install libpulse-dev
sudo apt-get -y install vlc
```
* Add the i2c group to the linaro user to access I2C devices:
```
sudo usermod -a -G i2c linaro
```
* Download and build the AMB Demo project by running the following commands in the LXTerminal:
```
mkdir Projects
cd Projects
git clone https://github.com/DBOpenSource/amb_demo.git
cd amb_demo
make clean && make
```
* Install the MP3 songs (probably from a USB thumbdrive) into the AMB Demo project:
```
mkdir songs
cp /media/linaro/<USB_thumbdrive_name>/<song_name>.mp3 songs/Red.mp3
cp /media/linaro/<USB_thumbdrive_name>/<song_name>.mp3 songs/Blue.mp3
cp /media/linaro/<USB_thumbdrive_name>/<song_name>.mp3 songs/Green.mp3
cp /media/linaro/<USB_thumbdrive_name>/<song_name>.mp3 songs/Yellow.mp3
```
* Install the Amazon AWS client under the ~/Projects directory by following the instructions in the [Amazon Alexa Voice Control Services Setup Instructions for the DragonBoard410c](/docs/Alexa-Service-on-DragonBoard410c.pdf).
* Reboot the DragonBoard 410c.

### Running
* Open a terminal by navigating to:
> **\[Start\] -> \[System Tools\] -> \[LXTerminal\]**
* Go to the AMB Demo project directory:
```
cd ~/Projects/amb_demo
```
* Pair a Bluetooth speaker to the DragonBoard 410c by clicking on the BlueTooth icon in the taskbar.
* Select the BlueTooth speaker:
   * Navigate to **\[Start\] -> \[Sound & Video\] -> \[PulseAudio Volume Control\]**.
   * Click on the **\[Output Devices\]** tab.
   * Click on the Enable button of the BlueTooth speaker.
   * **NOTE:** The Enable button is the one with the icon of a checkmark in a green circle.
   * Close the Volume Control window.
* Run the AMB Demo program:
```
./amb_demo
```
* Follow the directions in section 2.10 of the [Amazon Alexa Voice Control Services Setup Instructions for the DragonBoard410c](/docs/Alexa-Service-on-DragonBoard410c.pdf) to enable the AWS service.
* Select one of the five available demos by selecting the number preceding it and pressing Enter.
  * **NOTE:** See the individual demo descriptions below for details on how to run them.
* Press **a** and **Enter** to start the demo.
* Press **b** and **Enter** to stop the demo.
* Press **x** and **Enter** to exit the AMB Demo program.
* After stopping a demo, a new demo can be selected.

#### Color to Music Demo
* Start the demo.
* Place a [blue](/colors/Blue.jpg), [red](/colors/Red.jpg), [green](/colors/Green.jpg), or [yellow](/colors/Yellow.jpg) paper flush against the cyclinder of the Grove sensor and wait for several seconds.
* The song corresponding to the color will begin to play over the BlueTooth speaker.
* After removing the paper, the song will continue to play.
* Place a different colored paper to change the song.
* Stop the demo.

#### Color to Local TTS Demo
* Start the demo.
* Place a [blue](/colors/Blue.jpg), [red](/colors/Red.jpg), [green](/colors/Green.jpg), or [yellow](/colors/Yellow.jpg) paper flush against the cyclinder of the Grove sensor and wait for several seconds.
* The color of the paper will be identified over the BlueTooth speaker.
* Place a different colored paper to repeat.
* Stop the demo.

#### Color to Pico TTS Demo
* Start the demo.
* Place a [blue](/colors/Blue.jpg), [red](/colors/Red.jpg), [green](/colors/Green.jpg), or [yellow](/colors/Yellow.jpg) paper flush against the cyclinder of the Grove sensor and wait for several seconds.
* The color of the paper will be identified over the BlueTooth speaker.
* Place a different colored paper to repeat.
* Stop the demo.

#### AWS Voice Recognition Demo
* Start the demo.
* Press the **\[Start Recognition\]** button.
* Speak into the microphone with a request for AWS.  Ignore the audible tone that emits over the BlueTooth speaker.
* The response will come over the BlueTooth speaker.
* Press the **\[Start Recognition\]** button to repeat.
* Stop the demo.

#### Voice Test Demo
* Start the demo.
* Speak into the microphone.
* The audio will be looped back to the BlueTooth speaker.
* Stop the demo.
