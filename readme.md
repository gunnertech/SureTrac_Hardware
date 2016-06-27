TODO:
    Update Instructables URL

# WheresMyGuns - aka SureTrac an Open Source GPS tracker for your web app

Here at Gunner Technology we needed a GPS tracker that could HTTP POST NMEA sentences to our server and let us use the GPS data in our web applications.

We decided to do this using the Onion Omega, particularly because we could use Javascript on the hardware component too so that we have one language across all our components, from Hardware to Client to Server.

However we ran into an issue during our prototype phase, we originally ordered the Onion GPS expansion and that did not arrive in time so we made due with what we had - an Adafruit Ultimate GPS module - so that we could prototype the project in time.
As a result we have two codebases the [Adafruit/](Adafruit/) which has Python and Arduino code, and the [Expansion/](Expansion/) base.
Depending on which you prefer to use we have two different instruction sets for setting the project up.

An Instructable for the Hardware setup: [Link] (http://www.instructables.com/editInstructable/edit/EM8T7HBIPQMB6O5)

An Example server source: [Link] (https://github.com/gunnertech/WheresMyGuns)

## Arduino/Atmega

There are two files of interest - the .ino file and the .py file.

### 1Hz or slower
If you want to refresh the GPS at 1Hz or slower here is the procedure:

1. Flash the Arduino/Arduino Dock with the .ino file
2. Move the .py file to /home/root (or just ~ if your logged in as root)
3. Create a "where_are_my_guns.cfg" file in the same directory as the .py file and modify the rates and URL. Here is an example file:
  
  ```cfg
  [Where Are My Guns]
  post_url: 'https://YOUR_URL_HERE.TLD/' # Where we'll HTTP POST the NMEA sentence to
  post_rate: 1 # The rate at which the Omega will push the NMEA sentence to the server
  ```
4. Install the dependencies

  ```bash
  opkg update
  opkg install python-light
  opkg install pyOnionI2C
  opkg install python-codecs
  ```
  
4. Make the script run at startup by adding the following to your /etc/rc.local file
 
  ```bash
  python /home/root/I2C_Read_And_Post.py &
  ```

### Faster than 1Hz
The procedure is similar to above except you need to change the Arduino code to also poll the GPS module faster - this is simple all you have to do is change the code at these two [lines](/Adafruit/WheresMyGuns/WheresMyGuns.ino#L24-25)

## Omega Expansion

Since the Omega Expansion has a rate-locked GPS, you shouldn't go faster than the GPS poll rate - I do not know what that rate is - but we use 1 Hz and that works great.

The procedure is simpler for the Omega Expansion:

1. Copy the gps.js file to /home/root (or just ~ if your logged in as root)
2. Create a .env file similar to below in the same directory as the gps.js file
  ```env
  URL=https://YOUR_HIP_URL_HERE.io
  RATE=1000 # In ms
  ```
3. Install Node

  ```bash
  opkg update
  opkg install node
  cd ~/
  npm install dotenv
  npm install needle
  ```
3. Make the script run at startup by adding the following to your /etc/rc.local file

  ```bash
  node /home/root/gps.js &
  ```
