/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <inttypes.h>
#include <sense-steam/stik_input_manager.h>
#include <sense/sense_input.h>

static int checkForNewGamepads(SenseStikInputManager* self)
{
    if (self->boundGamepadsCount == 0) {
        InputHandle_t handles[STIK_INPUT_MAX_COUNT];
        int controllerCount = stikGetConnectedControllers(&self->stik, handles);
        if (controllerCount > 0) {
            InputHandle_t selectedControllerHandle = handles[0];
            ESteamInputType inputDeviceType = stikGetInputTypeForHandle(&self->stik, selectedControllerHandle);
            CLOG_OUTPUT_STDERR("you have device type %d %s", inputDeviceType, stikGetInputTypeName(inputDeviceType))
            self->boundGamepadsCount = 1;
            self->boundGamepads[0].isUsed = 1;
            self->boundGamepads[0].inputHandle = selectedControllerHandle;

            int worked = stikActivateActionSet(&self->stik, selectedControllerHandle, self->actionSetHandle);
            if (worked < 0) {
                CLOG_SOFT_ERROR("could not set action set")
                return worked;
            }
        } else {
            CLOG_SOFT_ERROR("No controllers attached")
        }
    }
    /*
		knownState->isBound = true;
		const char* name = StikGetGamepadName(joystickId);
		CLOG_INFO("detected and bound gamepad %d %s", joystickId, name)
		if (self->boundGamepadsCount >= self->boundGamepadsMax) {
			CLOG_ERROR("too many gamepads")
			return;
		}
		BoundGamepad* boundGamepad = &self->boundGamepads[self->boundGamepadsCount++];
		boundGamepad->joystickId = joystickId;
		boundGamepad->isUsed = true;
	*/

    return 0;
}

static void setDigitalFromData(int* target, InputDigitalActionData_t data)
{
    //CLOG_WARN("Setting digital action %d %d", data.state, data.active)
    *target = data.state == 0 ? 0 : 1;
}

static int setDigital(Stik* stik, int* target, InputHandle_t controllerHandle, const char* digitalActionName)
{
    InputDigitalActionHandle_t digitalActionHandle = stikGetDigitalActionHandle(stik, digitalActionName);
    if (digitalActionHandle == 0) {
        CLOG_SOFT_ERROR("stik digital action failed '%s' %" PRIX64, digitalActionName, digitalActionHandle)
        return -5;
    }

    InputDigitalActionData_t data = stikGetDigitalActionData(stik, controllerHandle, digitalActionHandle);
    setDigitalFromData(target, data);

    return 0;
}

static void setIntFromFloat(int* target, float data)
{
    *target = (int)(data * 1000.0f); // SWAMP_FIXED_FACTOR
}

static void setAnalogPairFromData(int* targetX, int* targetY, InputAnalogActionData_t data)
{
    //CLOG_WARN("setting target  %f %f %d %d", data.x, data.y, data.active, data.sourceMode);
    setIntFromFloat(targetX, data.x);
    setIntFromFloat(targetY, data.y);
}

static int setAnalog(
    Stik* stik, int* targetX, int* targetY, InputHandle_t controllerHandle, const char* analogActionName)
{
    InputAnalogActionHandle_t analogActionHandle = stikGetAnalogActionHandle(stik, analogActionName);
    if (analogActionHandle == 0) {
        CLOG_SOFT_ERROR("stik analog action failed '%s' %" PRIX64, analogActionName, analogActionHandle)
        return -5;
    }

    InputAnalogActionData_t data = stikGetAnalogActionData(stik, controllerHandle, analogActionHandle);
    setAnalogPairFromData(targetX, targetY, data);

    return 0;
}

static int getGamepadState(Stik* stik, InputHandle_t inputHandle, SenseNamedButtons* button)
{
    setDigital(stik, &button->rightShoulder, inputHandle, "Interact");
    setDigital(stik, &button->menu, inputHandle, "Menu");

    setDigital(stik, &button->a, inputHandle, "Ability1");
    setDigital(stik, &button->b, inputHandle, "Ability2");
    setDigital(stik, &button->x, inputHandle, "Ability3");
    setDigital(stik, &button->y, inputHandle, "Ability4");

    setAnalog(stik, &button->horizontal, &button->vertical, inputHandle, "Move");

    return 0;
}

static void scanGamepads(SenseStikInputManager* self, SenseButtons gamepadStates[8])
{
    for (int i = 0; i < self->boundGamepadsCount; ++i) {
        BoundGamepad* gamepad = &self->boundGamepads[i];
        if (!gamepad->isUsed) {
            tc_mem_clear_type(&gamepadStates[i]);
            continue;
        }

        int worked = getGamepadState(&self->stik, gamepad->inputHandle, &gamepadStates[i].named);
        if (worked < 0) {
            tc_mem_clear_type(&gamepadStates[i]);
            continue;
        }
    }
}

int senseStikInputManagerInit(SenseStikInputManager* self, Atheneum* atheneum)
{
    tc_mem_clear_type(self);
    self->boundGamepadsMax = 16;

    int stikResult = stikInit(&self->stik, atheneum);
    if (stikResult < 0) {
        return stikResult;
    }

    stikUpdate(&self->stik);

    return 0;
}

void senseStikInputManagerUpdate(SenseStikInputManager* self, SenseInput* input)
{
    tc_mem_clear_type(input);

    stikUpdate(&self->stik);

    if (self->actionSetHandle == 0) {
        self->actionSetHandle = stikGetActionSetHandle(&self->stik, "InGame");
        if (self->actionSetHandle == 0) {
            CLOG_SOFT_ERROR("sense: action set not found %" PRIX64, self->actionSetHandle)
            return;
        }
        CLOG_WARN("sense: action set found:%ld", self->actionSetHandle)
    }

    int worked = checkForNewGamepads(self);
    if (worked < 0) {
        CLOG_ERROR("sense: can not check for gamepads %d", worked)
    }
    scanGamepads(self, &input->devices[0]);
}
