# Introduction #

This is the sketch that drives a small photovore,
# Details #

Source is in this repository

Photos and assembly instructions are documented at http://insanegiantrobots.com/photovore

# Parts per robot #
  * 1 Mini breadboard
  * 2 Continuous rotation micro servos (I got mine from robotshop.com)
  * 2 CDS Photosensors
  * 1 9V Battery Pack, modified to terminate in a breadboard-friendly 2 pin connector
  * 1 AT Tiny 85 microcontroller
  * 2 Resistors (the same value, between 1KOhm and 10KOhm)
  * 1 LM7805 5 volt voltage regulator
  * 1 breadboard friendly piezo speaker element
  * 2 extra long 3 pin headers
  * 9 short jumper wires
  * 7 medium (3") jumper wires
  * 9V battery
  * 2 1.5" diameter wheels, I laser cut mine from 1/8" acrylic, glued to servo horns
  * 2 0.3"x3" legs with a rounded end - I laser cut mine from 1/8" acrylic

# And to prepare, you'll need #
  * Glue to hold servo horns to acrylic. Crazy Glue never fails to disappoint, I used "Loctite Go2 Glue" but a hot glue gun would probably work.
  * Strong double sided sticky tape (I used  0.5" 3M VHB)
  * A programmer to burn sketches onto the AT Tiny.  [Mine](http://insanegiantrobots.com/photovore#burning) is a Mintduino and a breadboard with a ZIF socket, I keep a 5V Trinket for quick testing of sketches before I put them onto the bare Tinys.
  * A computer with the Arduino IDE modified to include AT Tiny 85 support