#include "Controller.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <string>

// Helper function to get button name
std::string getButtonName(Uint8 button) {
  switch (button) {
  case SDL_CONTROLLER_BUTTON_A:
    return "Cross (A)";
  case SDL_CONTROLLER_BUTTON_B:
    return "Circle (B)";
  case SDL_CONTROLLER_BUTTON_X:
    return "Square (X)";
  case SDL_CONTROLLER_BUTTON_Y:
    return "Triangle (Y)";
  case SDL_CONTROLLER_BUTTON_BACK:
    return "Share";
  case SDL_CONTROLLER_BUTTON_GUIDE:
    return "PS Button";
  case SDL_CONTROLLER_BUTTON_START:
    return "Options";
  case SDL_CONTROLLER_BUTTON_LEFTSTICK:
    return "Left Stick Button";
  case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
    return "Right Stick Button";
  case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
    return "L1";
  case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
    return "R1";
  case SDL_CONTROLLER_BUTTON_DPAD_UP:
    return "D-Pad Up";
  case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
    return "D-Pad Down";
  case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
    return "D-Pad Left";
  case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
    return "D-Pad Right";

  // yet to trigger for some reason
  default:
    return "Unknown Button";
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Stick definitions------------------------------------------------------
Stick::Stick(SDL_GameControllerAxis xID, SDL_GameControllerAxis yID) {
  X_AXIS = xID;
  Y_AXIS = yID;
}

float Stick::stickUpdate(Sint16 axisValue, int axisID) {
  // any number less than ~30000
  // 5000 is a moderate rate
  int MOVING_DEAD_ZONE_RADIUS = 5000;

  // ideally greater than moving dead zone
  // used for zones where a specific value should be set (ex. central
  // deadzone and values close to maximum)
  int STATIC_ZONE_RADIUS = MOVING_DEAD_ZONE_RADIUS * 2;

  // lists for pointers to position variables
  int *posList[2] = {&m_posX, &m_posY};
  float *rawPosList[2] = {&m_rawX, &m_rawY};

  // converts from Sint16 to float (float conversion may be superfluous)
  float Pos = axisValue;

  // based on ID, sets other values
  float *rawPos = rawPosList[axisID];

  // kind of gross way to switch to the other index value
  // otherPos is used to normalize Pos to circular coordinates
  float otherPos = *rawPosList[(int)!((bool)axisID)] / INT16_MAX;

  //------------------------------------------------------------------------------------

  // If near the maximum value, adjusts the value to the maximum
  if (abs(Pos) > (INT16_MAX - STATIC_ZONE_RADIUS)) {

    // quick and dirty way to adjust the sign of the value
    Pos = (INT16_MAX + 1) * ((Pos < 0) ? -1 : 1);
    // std::cout << "\033[31m" << "\nX Max!\n"
    //           << "\033[0m"; //==========================================

  }
  // if close to zero, adjusts to zero
  else if (abs(Pos) < STATIC_ZONE_RADIUS) {

    Pos = 0;
    // std::cout << "\033[31m" << "\nX Zero\n"
    //           << "\033[0m"; //==========================================
  }
  // Checks if it is outside of the moving deadzone, if so, changes value
  else if (abs(Pos - *rawPos) > MOVING_DEAD_ZONE_RADIUS) {
    Pos = Pos;
    // std::cout << "\033[31m" << "\nX Deadzone\n"
    //           << "\033[0m"; //==========================================

    // if none apply, keep the value the same
  } else {

    Pos = *rawPos;
  }

  // Saving new raw value
  // this is important for the moving deadzone
  *rawPos = Pos;

  // adjusting to circular values -255 - 255
  Pos /= INT16_MAX;

  // setting new value
  *posList[axisID] = Pos * SDL_sqrt((1 - (SDL_pow(otherPos, 2) / 2))) * 255;

  return *posList[axisID];
}

// Controller definitions---------------------------------------------------

Controller::Controller() {
  m_pointerID = nullptr;

  // invalid joystick ID
  m_instanceID = -1;
}

Controller::Controller(SDL_GameController *identifier) {
  m_pointerID = identifier;

  // joystick / controller instance ID <- joystick pointer <- controller
  // pointer
  m_instanceID =
      SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(identifier));

  m_rightStick = Stick(SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY);
  m_leftStick = Stick(SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY);
}

void Controller::buttonResponse(Uint8 buttonID) {
  switch (buttonID) {
  case SDL_CONTROLLER_BUTTON_A:
    m_buttonFuncs.BUTTON_A;
    break;
  case SDL_CONTROLLER_BUTTON_B:
    m_buttonFuncs.BUTTON_B;
    break;
  case SDL_CONTROLLER_BUTTON_X:
    m_buttonFuncs.BUTTON_X;
    break;
  case SDL_CONTROLLER_BUTTON_Y:
    m_buttonFuncs.BUTTON_Y;
    break;
  case SDL_CONTROLLER_BUTTON_BACK:
    m_buttonFuncs.BUTTON_BACK;
    break;
  case SDL_CONTROLLER_BUTTON_GUIDE:
    m_buttonFuncs.BUTTON_GUIDE;
    break;
  case SDL_CONTROLLER_BUTTON_START:
    m_buttonFuncs.BUTTON_START;
    break;
  case SDL_CONTROLLER_BUTTON_LEFTSTICK:
    m_buttonFuncs.BUTTON_LEFTSTICK;
    break;
  case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
    m_buttonFuncs.BUTTON_RIGHTSTICK;
    break;
  case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
    m_buttonFuncs.BUTTON_LEFTSHOULDER;
    break;
  case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
    m_buttonFuncs.BUTTON_RIGHTSHOULDER;
    break;
  case SDL_CONTROLLER_BUTTON_DPAD_UP:
    m_buttonFuncs.BUTTON_DPAD_UP;
    break;
  case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
    m_buttonFuncs.BUTTON_DPAD_DOWN;
    break;
  case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
    m_buttonFuncs.BUTTON_DPAD_LEFT;
    break;
  case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
    m_buttonFuncs.BUTTON_DPAD_RIGHT;
    break;

  // yet to trigger for some reason
  default:
    std::cout << "Unknown Button" << std::endl;
  }
}

void Controller::stickResponse(Sint16 axisValue, int axisID) {

  // an axis ID to pass into the stick update function
  int stickAxisID = (axisID == 1 || axisID == 3) ? 1 : 0;

  // identifies if left stick
  if (axisID < 2) {
    m_leftStick.stickUpdate(axisValue, stickAxisID);
    m_buttonFuncs.LEFT_JOYSTICK(m_leftStick.getPosX(), m_leftStick.getPosY());

    // otherwise it is right stick
  } else {
    m_rightStick.stickUpdate(axisValue, stickAxisID);
    m_buttonFuncs.RIGHT_JOYSTICK(m_rightStick.getPosX(), m_rightStick.getPosY());
  }
}

// ControllerHolder definitions -----------------------------------------
ControllerHolder::ControllerHolder(buttonFunctions functionStruct1,
                                   buttonFunctions functionStruct2) {
                                    
  // storing the structs in the controller objects and the list
  m_buttonFuncList[0] = functionStruct1;
  m_buttonFuncList[1] = functionStruct2;

  m_controllerList[0].setButtonFuncs(functionStruct1);
  m_controllerList[1].setButtonFuncs(functionStruct2);

  // initializing SDL sub systems`
  // This combo works, it is a little strange tho
  // -> SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
    fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
    SDL_Quit();
  }

  // Check for game controllers
  if (SDL_NumJoysticks() < 1) {
    std::cerr << "No controllers connected!" << std::endl << SDL_NumJoysticks();
    SDL_Quit();
  }
}

void ControllerHolder::eventLoop() {

  bool quit = false;
  SDL_Event event;

  // the main update loop
  // reads events and updates accordingly per loop
  while (!quit) {

    // pulls an event from the queue (crucial for updating literally
    // anything!) this is the event loop yet to figure out why event loop has
    // to look like this
    while (SDL_PollEvent(&event)) {

      // checks what kind of event
      switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;

      case SDL_CONTROLLERDEVICEADDED:

        // checks for which controller object is not active and connects the
        // new controller there
        for (int i = 0; i < 3; i++) {

          // have to do this since nullptrs are special little snowflakes
          // they don't play nice with "!" I guess
          if (!((bool)m_controllerList[i].getPointerID())) {
            m_controllerList[i] =
                Controller(SDL_GameControllerOpen(event.cdevice.which));

            //sets buttonfuncs based on the position the connected controller is assigned
            m_controllerList[i].setButtonFuncs(m_buttonFuncList[i]);

            std::cout << i << std::endl;
            break;
          }
        }
        break;

      case SDL_CONTROLLERDEVICEREMOVED:

        // checks if there is a controller id and the controller id matches
        // the current controller
        for (int i = 0; i < 3; i++) {
          if (m_controllerList[i].getInstanceID() == event.cdevice.which) {
            SDL_GameControllerClose(m_controllerList[i].getPointerID());
            m_controllerList[i] = Controller();
            break;
          }
        }

        break;

      // Truly superficial!
      // Adding stuff here really lagged things out
      // Testing may be needed to see if updates can be placed here or I am
      // just dumb
      case SDL_JOYAXISMOTION:

        // std::cout << "\nAxis motion\n";

        // axis index has to be converted to be an int
        // std::cout << event.cdevice.which << std::endl
        //           << (int)event.jaxis.axis << std::endl
        //           << event.jaxis.value << std::endl;
        // executes the function according to what controller was used
        m_controllerList[event.cdevice.which].stickResponse(event.jaxis.value,
                                                            event.jaxis.axis);

        break;

      case SDL_CONTROLLERBUTTONDOWN:
        std::cout << event.cdevice.which << std::endl;
        std::cout << "Button press\n";
        std::cout << getButtonName(event.cbutton.button) << std::endl;

        // executes the function according to what controller was used
        m_controllerList[event.cdevice.which].buttonResponse(
            event.cbutton.button);
      }
    }
  }

  // Clean up >>>>>>>>>>>>>>>>>>>>>> important <<<<<<<<<<<<<<<<<<<<<<
  for (Controller controller : m_controllerList) {
    if (controller.getPointerID()) {
      SDL_GameControllerClose(controller.getPointerID());
    }
  }

  SDL_Quit();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// detecting inital controller and opening it
int main(int argc, char *argv[]) {

  buttonFunctions example;

  buttonFunctions example2;

  // making the controller holder
  ControllerHolder cHolder = ControllerHolder(example, example2);

  cHolder.eventLoop();

  return 0;
}