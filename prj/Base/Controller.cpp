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

// a struct for function pointers
struct buttonFunctions {
  void (*BUTTON_A)();
  void (*BUTTON_B)();
  void (*BUTTON_X)();
  void (*BUTTON_Y)();
  void (*BUTTON_BACK)();
  void (*BUTTON_GUIDE)();
  void (*BUTTON_START)();
  void (*BUTTON_LEFTSTICK)();
  void (*BUTTON_RIGHTSTICK)();
  void (*BUTTON_LEFTSHOULDER)();
  void (*BUTTON_RIGHTSHOULDER)();
  void (*BUTTON_DPAD_UP)();
  void (*BUTTON_DPAD_DOWN)();
  void (*BUTTON_DPAD_LEFT)();
  void (*BUTTON_DPAD_RIGHT)();
};

// A class for initializing controllers and running the pollEvent loop
// Holds a list of structs that are linked to the positions of the controllers
// in the list <<<<<<<<<<<<<<<<<<<<<<<
class ControllerHolder {

  // class for joysticks (they are generic and can be applied to any controller)
  class Stick {
  public:
    // identifiers for the axes
    // funny enough, triggers also operate with axes
    SDL_GameControllerAxis X_AXIS;
    SDL_GameControllerAxis Y_AXIS;

    // positions values from -32768 - 32767 (note up and left are negative)
    float m_posX = 0;
    float m_posY = 0;

    // raw values for the use of moving deadzone comparison
    float m_rawX = 0;
    float m_rawY = 0;

  public:
    // joysticks are identified by their axes (ex. right stick has a rightX and
    // a rightY axis)
    Stick(SDL_GameControllerAxis xID, SDL_GameControllerAxis yID) {
      X_AXIS = xID;
      Y_AXIS = yID;
    }
  };

  // class for controllers
  class Controller {

  public:
    // the identifier (a pointer to the controller object and its instance ID)
    SDL_GameController *m_pointerID;
    SDL_JoystickID m_instanceID;

    // the values used to initialize sticks just identify a generic left or
    // right joystick
    Stick m_leftStick =
        Stick(SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY);
    Stick m_rightStick =
        Stick(SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY);

    // getter funcs
    SDL_GameController *getPointerID() { return m_pointerID; };
    SDL_JoystickID getInstanceID() { return m_instanceID; };

    // making a completely blank controller object
    Controller() {
      m_pointerID = nullptr;

      // invalid joystick ID
      m_instanceID = -1;
    }

    // assigns the controller's identifier upon initializing a controller object
    Controller(SDL_GameController *identifier) {
      m_pointerID = identifier;

      // joystick / controller instance ID <- joystick pointer <- controller
      // pointer
      m_instanceID =
          SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(identifier));
    }
  };

public:
  // Holding the controller objects
  Controller m_controllerList[2] = {Controller(), Controller()};

  // Holding the structs to map to each controller
  buttonFunctions m_buttonList[2];

  // Attempting to initialize controllers when initializing the controller
  // holder Eventually put the button structs in here

  ControllerHolder(buttonFunctions functionStruct1,
                   buttonFunctions functionStruct2) {
    // storing the structs in the list
    m_buttonList[0] = functionStruct1;
    m_buttonList[1] = functionStruct2;

    // initializing SDL sub systems`
    // This combo works, it is a little strange tho
    // -> SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
      fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
      SDL_Quit();
    }

    // Check for game controllers
    if (SDL_NumJoysticks() < 1) {
      std::cerr << "No controllers connected!" << std::endl
                << SDL_NumJoysticks();
      SDL_Quit();
    }
  }

  void eventLoop() {

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

          std::cout << "\nAxis motion\n";

          // axis index has to be converted to be an int to match the enum for
          // the axes value gets stuck sometimes, can be adjusted with the
          // deadzone
          std::cout << event.cdevice.which << std::endl
                    << (int)event.jaxis.axis << std::endl
                    << event.jaxis.value << std::endl;
          break;

        case SDL_CONTROLLERBUTTONDOWN:
          std::cout << "\nButton press\n";
          std::cout << getButtonName(event.cbutton.button) << std::endl;
          if (event.cbutton.button == SDL_CONTROLLER_BUTTON_X) {
            quit = true;
          }

        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            quit = true;
          }
          break;
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
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// detecting inital controller and opening it
int main(int argc, char *argv[]) {

  buttonFunctions example;

  buttonFunctions example2;

  // making the controller holder
  ControllerHolder cHolder = ControllerHolder(example, example2);

  cHolder.eventLoop();

  return 0;
}
