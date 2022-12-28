/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


 /**
 * @file portami.ino
 * @author Turi Scandurra
 * @date 18.12.2022
 *
 * @brief This file contains the project code of Portami, a fork of the original "Lo-Fi SAMD21 based mini chip tune synthesizer" by Marcel Licence:
 * https://github.com/marcel-licence/samd21_mini_synth
 * 
 */


#ifdef __CDT_PARSER__
#include "cdt.h"
#endif

#include "config.h"

#include <Usb.h>
#include <Arduino.h>
#include <usbh_midi.h>
#include <usbhub.h>
#include <Button.h>


#define MUL_I16(a, b) ((int16_t)((((int32_t)a) * ((int32_t)(b))) / 0x8000))

int8_t current_waveform = 0;
Button wave_sel_button(10);

void setup()
{

    pinMode(DAC0, OUTPUT);

    wave_sel_button.begin();
    pinMode(1, OUTPUT); // R LED
    pinMode(2, OUTPUT); // G LED
    pinMode(3, OUTPUT); // B LED
    digitalWrite(1, HIGH);

#if defined MIDI_USB_HOST_ENABLED
    USBHost_setup();
 #endif

#if 1
    Midi_Setup();
    Synth_Init();
    Delay_Init();
    SAMD21_Synth_Init();
#endif

    Serial.printf("Setup done\n");
}

static uint32_t cnt = 0;

inline
void ProcessAudio(uint16_t *buff, size_t len)
{
    int32_t u32buf[SAMPLE_BUFFER_SIZE];
    Synth_Process_Buff(u32buf, len);
    Delay_Process(u32buf, len);
#if 1
    /* convert from u16 to u10 */
    for (size_t i = 0; i < len; i++)
    {
        buff[i] = (uint16_t)(0x200 + (u32buf[i] / 512));
    }
#endif
    cnt += SAMPLE_BUFFER_SIZE;
    Midi_Process();

  USBHost_loop();
}

void updateLEDs(){
switch (current_waveform) {
  case 0:
    analogWrite(1, 256);
    analogWrite(2, 0);
    analogWrite(3, 0);
  break;
  case 1:
    analogWrite(1, 0);
    analogWrite(2, 256);
    analogWrite(3, 0);
  break;
  case 2:
    analogWrite(1, 0);
    analogWrite(2, 0);
    analogWrite(3, 256);
  break;
  case 3:
    analogWrite(1, 128);
    analogWrite(2, 32);
    analogWrite(3, 8);
  break;
  case 4:
    analogWrite(1, 128);
    analogWrite(2, 0);
    analogWrite(3, 128);
  break;
  case 5:
    analogWrite(1, 0);
    analogWrite(2, 128);
    analogWrite(3, 64);
  break;
  }
}

void loop_1Hz()
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void loop()
{
    if (cnt >= SAMPLE_RATE)
    {
        cnt = 0;
        loop_1Hz();
    }

    SAMD21_Synth_Process(ProcessAudio);
    if (wave_sel_button.pressed())
		  changeWaveform();

}
