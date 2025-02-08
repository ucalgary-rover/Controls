#include <Controller.h>
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

// A class for initializing controllers and running the pollEvent loop
// Holds a list of structs that are linked to the positions of the controllers
// in the list <<<<<<<<<<<<<<<<<<<<<<<
class ControllerHolder {

  // class for joysticks (they are generic and can be applied to any controller)
  class Stick {
  private:
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

  private:
    // the identifier (a pointer to the controller object and its instance ID)
    SDL_GameController* m_pointerID;
    SDL_JoystickID m_instanceID;

    // the values used to initialize sticks just identify a generic left or
    // right joystick
    Stick m_leftStick
      = Stick(SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY);
    Stick m_rightStick
      = Stick(SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY);

    // struct for button functions
    buttonFunctions m_buttonFuncs;

  public:
    // getter/setter funcs

    // these are never changed unless there is a new controller anyways so no
    // setter needed
    SDL_GameController* getPointerID() { return m_pointerID; };
    SDL_JoystickID getInstanceID() { return m_instanceID; };

    Stick getLeftStick() { return m_leftStick; };
    Stick getRightStick() { return m_rightStick; };
    void setLeftStick(Stick lStick) { m_leftStick = lStick; };
    void setRightStick(Stick rStick) { m_rightStick = rStick; };

    void setButtonFuncs(buttonFunctions funcsStruct) {
      m_buttonFuncs = funcsStruct;
    };

    // Default constructor for making a completely blank controller object
    Controller() {
      m_pointerID = nullptr;

      // invalid joystick ID
      m_instanceID = -1;
    }

    // assigns the controller's identifier upon initializing a controller object
    Controller(SDL_GameController* identifier) {
      m_pointerID = identifier;

      // joystick / controller instance ID <- joystick pointer <- controller
      // pointer
      m_instanceID
        = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(identifier));
    }

    void buttonResponse(Uint8 buttonID) {
      switch (buttonID) {
      case SDL_CONTROLLER_BUTTON_A:
        m_buttonFuncs.BUTTON_A;
      case SDL_CONTROLLER_BUTTON_B:
        m_buttonFuncs.BUTTON_B;
      case SDL_CONTROLLER_BUTTON_X:
        m_buttonFuncs.BUTTON_X;
      case SDL_CONTROLLER_BUTTON_Y:
        m_buttonFuncs.BUTTON_Y;
      case SDL_CONTROLLER_BUTTON_BACK:
        m_buttonFuncs.BUTTON_BACK;
      case SDL_CONTROLLER_BUTTON_GUIDE:
        m_buttonFuncs.BUTTON_GUIDE;
      case SDL_CONTROLLER_BUTTON_START:
        m_buttonFuncs.BUTTON_START;
      case SDL_CONTROLLER_BUTTON_LEFTSTICK:
        m_buttonFuncs.BUTTON_LEFTSTICK;
      case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        m_buttonFuncs.BUTTON_RIGHTSTICK;
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        m_buttonFuncs.BUTTON_LEFTSHOULDER;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        m_buttonFuncs.BUTTON_RIGHTSHOULDER;
      case SDL_CONTROLLER_BUTTON_DPAD_UP:
        m_buttonFuncs.BUTTON_DPAD_UP;
      case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        m_buttonFuncs.BUTTON_DPAD_DOWN;
      case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        m_buttonFuncs.BUTTON_DPAD_LEFT;
      case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        m_buttonFuncs.BUTTON_DPAD_RIGHT;

      // yet to trigger for some reason
      default:
        std::cout << "Unknown Button" << std::endl;
      }
    }
  };

private:
  // Holding the controller objects
  Controller m_controllerList[2] = { Controller(), Controller() };

public:
  // getter and setter function
  Controller getControllerFromList(int i) {
    return (i == 1 || i == 0) ? m_controllerList[i] : m_controllerList[0];
  };

  void setControllerFromList(int i, Controller controller) {
    i = (i == 1 || i == 0) ? i : 0;
    m_controllerList[i] = controller;
  };

  // Attempting to initialize controllers when initializing the controller
  // holder Eventually put the button structs in here

  ControllerHolder(buttonFunctions functionStruct1,
                   buttonFunctions functionStruct2) {
    // storing the structs in the controller objects
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
              m_controllerList[i]
                = Controller(SDL_GameControllerOpen(event.cdevice.which));
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
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// detecting inital controller and opening it
int main(int argc, char* argv[]) {

  buttonFunctions example;

  buttonFunctions example2;

  // making the controller holder
  ControllerHolder cHolder = ControllerHolder(example, example2);

  cHolder.eventLoop();

  return 0;
}
