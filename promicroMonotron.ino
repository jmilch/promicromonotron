#include "MIDIUSB.h"

float standardPitchA = 440.00;
float step = 1.059463094359;
float frequencies[128];

//Adjust pin numbers to model
//Pro Micro pins 9 & 10 for timer 1
const int subOscPin = 6;
const int pitchPin = 9;
const int gatePin = 7;
const int cutoffPin = 10;
const uint8_t LOWEST_KEY = 24; // C2
const uint8_t HIGHEST_KEY = LOWEST_KEY + 40; // 1v per octave; 3,3v; 12 semitones per octave  --> 40
byte subOscTranspose = 0;

void setup() {
  standardPitchA = standardPitchA*pow(2,5);
  standardPitchA = standardPitchA/pow(step,2);
  for (int i = 127; i > -1; i--) {
    frequencies[i] = standardPitchA;
    standardPitchA = standardPitchA/step;
  }

  pinMode(subOscPin, OUTPUT);
  pinMode(pitchPin, OUTPUT);
  pinMode(gatePin, OUTPUT);
  pinMode(cutoffPin, OUTPUT);
 
//Setup Timer1 to do PWM DAC for Pitch adjust for atmega32u4
//Set 3 Parameters: Enable Fast PWM 8bit -> WGM12 to 1 & WGM10 to 1; clear on complete match -> set COM1A1 to 1; set prescaler to 1 -> set CS10 to 1 
TCCR1B = _BV(WGM12) | _BV(CS10);
TCCR1A = _BV(WGM10) | _BV(COM1A1); 
GTCCR = 0; //Set general timer counter 1 control register 
OCR1C = 239; //Set top value; 240 steps = 6 steps per semitone
OCR1A = 0; //Set initial Pitch to C2
digitalWrite(gatePin,LOW); //Set initial Gate to LOW;


  Serial.begin(115200); //remove if Serial.print is removed
}

void loop() {
  
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {
      Serial.print("Received: ");
      Serial.print(rx.header, HEX);
      Serial.print("-");
      Serial.print(rx.byte1, HEX);
      Serial.print("-");
      Serial.print(rx.byte2, HEX);
      Serial.print("-");
      Serial.println(rx.byte3, HEX);
    }
  } while (rx.header != 0);

  rx = MidiUSB.read();
  switch (rx.header) {
    case 0:
      break; //No pending events
      
    case 0x9:
      noteOn(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;
      
    case 0x8:
      noteOff(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;
      
    case 0xB:
      controlChange(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //control
        rx.byte3         //value
      );
      break; 
      
    default:
      Serial.print("Unhandled MIDI message: ");
      Serial.print(rx.header, HEX);
      Serial.print("-");
      Serial.print(rx.byte1, HEX);
      Serial.print("-");
      Serial.print(rx.byte2, HEX);
      Serial.print("-");
      Serial.println(rx.byte3, HEX);
  }


}

void noteOn(byte kanal, byte note, byte velocity) {
  if(velocity==0) {
    noTone(subOscPin);
    digitalWrite(gatePin,LOW);
  }
  else {
  if (note < LOWEST_KEY) note = LOWEST_KEY;
  if (note > HIGHEST_KEY) note = HIGHEST_KEY;
  
    tone(subOscPin, frequencies[note+subOscTranspose]);
    //Adjust timer and set tone
    digitalWrite(gatePin, HIGH);
    int vnote = note - LOWEST_KEY; 
    OCR1A = vnote*6; //Multiply note by 6 to set the voltage (1v/octave)
  }
}

void noteOff(byte kanal, byte note, byte velocity) {
 digitalWrite(gatePin, LOW);
  int vnote = note - LOWEST_KEY; 
OCR1A = vnote*6; //Multiply note by 6 to set the voltage (1v/octave)
  noTone(subOscPin);
}

void controlChange(byte channel, byte control, byte value) {
  Serial.print("Control change: control=");
  Serial.print(control);
  Serial.print(", value=");
  Serial.print(value);
  Serial.print(", channel=");
  Serial.println(channel);
}
