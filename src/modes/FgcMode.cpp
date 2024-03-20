#include "modes/FgcMode.hpp"

#define ANALOG_STICK_MIN 28
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 228

FgcMode::FgcMode(socd::SocdType horizontal_socd, socd::SocdType vertical_socd) {
    _socd_pair_count = 6;
    _socd_pairs = new socd::SocdPair[_socd_pair_count]{
        socd::SocdPair{&InputState::left,   &InputState::right, horizontal_socd         },
 /* Mod X override C-Up input if both are pressed. Without this, neutral SOCD doesn't work
  properly if Down and both Up buttons are pressed, because it first resolves Down + Mod X
  to set both as unpressed, and then it sees C-Up as pressed but not Down, so you get an up
  input instead of neutral. */
        socd::SocdPair{ &InputState::mod_x, &InputState::c_up,  socd::SOCD_DIR1_PRIORITY},
        socd::SocdPair{ &InputState::mod_x, &InputState::up2,   socd::SOCD_DIR1_PRIORITY},
        socd::SocdPair{ &InputState::down,  &InputState::mod_x, vertical_socd           },
        socd::SocdPair{ &InputState::down,  &InputState::up2,   vertical_socd           },
        socd::SocdPair{ &InputState::down,  &InputState::c_up,  vertical_socd           },
    };
}

void FgcMode::UpdateDigitalOutputs(InputState &inputs, OutputState &outputs) {
    // Directions
    outputs.dpadLeft = inputs.left;
    outputs.dpadRight = inputs.right;
    outputs.dpadDown = inputs.down;
    outputs.dpadUp = inputs.mod_x || inputs.up2 || inputs.c_up;

    // L3 & R3
    outputs.leftStickClick = inputs.l || inputs.c_left; 
    outputs.rightStickClick = inputs.a;                       

    // Menu keys
    outputs.start = inputs.start;
    outputs.select = inputs.select;
    outputs.home = inputs.home;

    // Activate select/home by holding Mod Y
    if ((inputs.mod_y)) {
        outputs.select = inputs.midshield;
        outputs.home = inputs.start;
    }

    // Right hand bottom row
    outputs.a = inputs.b;
    outputs.b = inputs.x;
    outputs.triggerRDigital = inputs.z;
    outputs.triggerLDigital = inputs.up;

    // Right hand top row
    outputs.x = inputs.r;
    outputs.y = inputs.y;
    outputs.buttonR = inputs.lightshield;
    outputs.buttonL = inputs.midshield;

    // Turns off Start / LB when holding Mod Y
    if ((inputs.mod_y)) {
        outputs.start = false;
        outputs.buttonL = false;
        // Turn off DPad 
        outputs.dpadLeft = false;
        outputs.dpadRight = false;
        outputs.dpadDown = false;
        outputs.dpadUp = false;
        outputs.leftStickClick = false;
    }    
}

void FgcMode::UpdateAnalogOutputs(InputState &inputs, OutputState &outputs) {
    outputs.leftStickX = 128;
    outputs.leftStickY = 128;
    outputs.rightStickX = 128;
    outputs.rightStickY = 128;
    outputs.triggerLAnalog = outputs.triggerLDigital ? 255 : 0;
    outputs.triggerRAnalog = outputs.triggerRDigital ? 255 : 0;

    //Hold Mod Y to use analog input.
    if ((inputs.mod_y)) {
    UpdateDirections(
    inputs.left,
    inputs.right,
    inputs.down,
    inputs.up2,
    inputs.c_left,
    inputs.c_right,
    inputs.c_down,
    inputs.c_up,
    ANALOG_STICK_MIN,
    ANALOG_STICK_NEUTRAL,
    ANALOG_STICK_MAX,
    outputs);
    }
}
