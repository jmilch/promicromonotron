# promicromonotron
Code to control a Korg Monotron via an Arduino Pro Micro 3,3V

Pin mapping:

- If you want to power the Monotron directly from the Pro Micro (3,3V!) connect
  - GND to GND and
  - VCC to VCC

- Suboscilator via Aux in (Mono) Pin 6 and GND
- Pitch Pin 9
- Gate Pin 7
- Filter Cutoff Pin 10 (currently not used)


Todo:
- Control Filter Cutoff via MidiCC

Credits:
- Goetz Müller-Dürholt for Suboscilator via PWM Code: http://goetzmd.de/diy/korg-monotron-arduino-hack/teensy-usb-midi-tutorial-suboszillator-deutsch

- Jan Ostman for Midi2CV Code: https://janostman.wordpress.com/cheap-diy-usb-midi-to-cv-interface/

- Paulo Costa for MIDIUSB Code: https://github.com/arduino-libraries/MIDIUSB/blob/master/examples/MIDIUSB_buzzer/MIDIUSB_buzzer.ino
