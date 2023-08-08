/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef sense_stik_input_manager_h
#define sense_stik_input_manager_h


#include <stik/stik.h>
#include <stdbool.h>

struct SenseInput;

typedef struct GamepadState {
	bool isBound;
} GamepadState;

typedef struct BoundGamepad {
	InputHandle_t inputHandle;
    bool isUsed;
} BoundGamepad;

typedef struct SenseStikInputManager {
	GamepadState gamepadStates[16];
	BoundGamepad boundGamepads[16];
	int boundGamepadsMax;
	int boundGamepadsCount;
    Stik stik;
    InputActionSetHandle_t actionSetHandle;
} SenseStikInputManager;

struct Atheneum;

int senseStikInputManagerInit(SenseStikInputManager* self, struct Atheneum *atheneum);
void senseStikInputManagerUpdate(SenseStikInputManager* self, struct SenseInput* input);

#endif
