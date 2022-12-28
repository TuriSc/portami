/**
 * @file samd21_usbmidi.ino
 * @author Turi Scandurra
 * @date 18.12.2022
 *
 * @brief This file contains the implementation to use a Midi USB keyboard to control the synthesizer
 *
 */


#ifdef __CDT_PARSER__
#include "cdt.h"
#endif


#ifdef MIDI_USB_HOST_ENABLED

#include <KeyboardController.h>


#define SerialDebug Serial1


KeyboardController keyboard(UsbH);

void USBHost_setup()
{

    if (UsbH.Init())
    {
        SerialDebug.println("USB host did not start");
        while (1); //halt
    }
    SerialDebug.println("USB Host started");

    delay(200);
    
}

uint32_t lastUSBstate = 0;


void USBHost_loop()
{
    UsbH.Task();

    uint32_t currentUSBstate = UsbH.getUsbTaskState();
    if (lastUSBstate != currentUSBstate)
    {
        SerialDebug.print("USB state changed: 0x");
        SerialDebug.print(lastUSBstate, HEX);
        SerialDebug.print(" -> 0x");
        SerialDebug.println(currentUSBstate, HEX);
        switch (currentUSBstate)
        {
        case USB_ATTACHED_SUBSTATE_SETTLE:
            SerialDebug.println("Device Attached");
            break;
        case USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE:
            SerialDebug.println("Detached, waiting for Device");
            break;
        case USB_ATTACHED_SUBSTATE_RESET_DEVICE:
            SerialDebug.println("Resetting Device");
            break;
        case USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE:
            SerialDebug.println("Reset complete");
            break;
        case USB_STATE_CONFIGURING:
            SerialDebug.println("USB Configuring");
            break;
        case USB_STATE_RUNNING:
            SerialDebug.println("USB Running");
            break;
        }
        lastUSBstate = currentUSBstate;
    }
}


/*
 * Keyboard stuff
 */
void printKey();


struct
{
    uint8_t key;
    uint8_t note;
} keyMidiMapping[] =
{
    {4, 26},
    {5, 57},
    {6, 43},
    {7, 40},
    {8, 37},
    {9, 47},
    {10, 54},
    {11, 61},
    {12, 72},
    {13, 68},
    {14, 75},
    {15, 82},
    {16, 71},
    {17, 64},
    {18, 79},
    {19, 86},
    {20, 23},
    {21, 44},
    {22, 33},
    {23, 51},
    {24, 65},
    {25, 50},
    {26, 30},
    {27, 36},
    {28, 58},
    {29, 29},
    {30, 20},
    {31, 27},
    {32, 34},
    {33, 41},
    {34, 48},
    {35, 55},
    {36, 62},
    {37, 69},
    {38, 76},
    {39, 83},

    {45, 90},
    {46, 97},
    {47, 93},
    {48, 100},

    {50, 103},
    {51, 89},
    {52, 96},

    {54, 78},
    {55, 85},
    {56, 92},
};

struct
{
    uint8_t key;
    uint8_t cc;
} keyMidiCcMapping[] =
{
    /* F1 - F12 */
    {58, 58},
    {59, 59},
    {60, 60},
    {61, 61},
    {62, 62},
    {63, 63},
    {64, 64},
    {65, 65},
    {66, 66},
    {67, 67},
    {68, 68},
    {69, 69},

    {79, 79},
    {80, 80},
    {81, 81},
    {82, 82},
};

#define MAP_SIZE (sizeof(keyMidiMapping)/sizeof(keyMidiMapping[0]))
#define MAP_CC_SIZE (sizeof(keyMidiCcMapping)/sizeof(keyMidiCcMapping[0]))


// This function intercepts key press
void keyPressed()
{
    SerialDebug.print("Pressed:  ");
    printKey();
    for (int  i = 0; i < MAP_SIZE; i++)
    {
        if (keyMidiMapping[i].key == keyboard.getOemKey())
        {
            Midi_NoteOn(0, keyMidiMapping[i].note, 127);
        }
    }

    for (int  i = 0; i < MAP_CC_SIZE; i++)
    {
        if (keyMidiCcMapping[i].key == keyboard.getOemKey())
        {
            Midi_ControlChange(0, keyMidiCcMapping[i].cc, 127);
            SerialDebug.printf("cc: %d on\n",  keyMidiCcMapping[i].cc);
        }
    }
}

// This function intercepts key release
void keyReleased()
{
    SerialDebug.print("Released: ");
    printKey();

    for (int  i = 0; i < MAP_SIZE; i++)
    {
        if (keyMidiMapping[i].key == keyboard.getOemKey())
        {
            Midi_NoteOff(0, keyMidiMapping[i].note);
        }
    }

    for (int  i = 0; i < MAP_CC_SIZE; i++)
    {
        if (keyMidiCcMapping[i].key == keyboard.getOemKey())
        {
            Midi_ControlChange(0, keyMidiCcMapping[i].cc, 0);
            SerialDebug.printf("cc: %d off\n",  keyMidiCcMapping[i].cc);
        }
    }
}


void printKey()
{
    // getOemKey() returns the OEM-code associated with the key
    SerialDebug.print(" key:");
    SerialDebug.print(keyboard.getOemKey());

    SerialDebug.println();
}

#endif
