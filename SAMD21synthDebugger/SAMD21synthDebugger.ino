 /**
 * @file SAMD21synthDebugger.ino
 * @author Turi Scandurra
 * @date 18.12.2022
 *
 * @brief This software can be used to print out the address of knobs and sliders
 * of Midi controllers, which can then be mapped to specific synth parameters.
 * 
 */

#include <Arduino.h>
#include <usbh_midi.h>
#include <usbhub.h>

#define SerialDebug Serial1

USBHost UsbH;
USBHub Hub(&UsbH);
USBH_MIDI  Midi(&UsbH);

void MIDI_poll();
uint16_t pid, vid;

void setup(void) {
  vid = pid = 0;
  SerialDebug.begin(115200);

  if (UsbH.Init()) {
    SerialDebug.println("USB host did not start");
    while (1); //halt
  }
  while (!Serial);
  SerialDebug.println("SerialDebug started");
  //delay( 200 );

}

void loop(void) {
  UsbH.Task();
  if ( Midi ) {
    MIDI_poll();
  }
}

// Poll USB MIDI Controller and send to serial MIDI
void MIDI_poll()
{
  char buf[20];
  uint8_t bufMidi[64];
  uint16_t  rcvd;

  if (Midi.idVendor() != vid || Midi.idProduct() != pid) {
    vid = Midi.idVendor();
    pid = Midi.idProduct();
    sprintf(buf, "%04X:%04X", vid, pid);
    SerialDebug.println(buf);
  }
  if (Midi.RecvData(&rcvd,  bufMidi) == 0 ) {
    uint32_t time = (uint32_t)millis();
    sprintf(buf, "%04X%04X: ", (uint16_t)(time >> 16), (uint16_t)(time & 0xFFFF)); // Split variable to prevent warnings on the ESP8266 platform
    SerialDebug.print(buf);
    SerialDebug.print(rcvd);
    SerialDebug.print(':');
    for (int i = 0; i < 64; i++) {
      sprintf(buf, " %02X", bufMidi[i]);
      SerialDebug.print(buf);
    }
    SerialDebug.println();
  }
}
