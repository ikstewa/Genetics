/*
* Gui Class
* Handles the overiding functionality of the gui as well as rendering it to the screen.
*/

#ifndef GUI_H
#define GUI_H

class GameEngine;
class RenderManager;

class Gui
{
public:
  Gui(GameEngine *engine);

  void render(int width, int height);
  void setupView(int width, int height);
  void handleInputs();

private:
  void setup();

  bool isOver(float mx, float maxx, float my, float maxy);

  int width;
  int height;
  bool leftButton;

  // CUTSCENE STUFF
  void renderCutscene();
  void cutsceneInputs();
  double currentCutsceneFrame;
  double endCutsceneFrame;
  bool cutscenePlaying;

  // MAIN MENU STUFF
  void renderMainMenu();
  void menuInputs();
  bool mainMenu;

  // CONTROLS STUFF
  void renderControls();
  void controlsInputs();
  bool controls;

  GameEngine *theEngine;
};

#endif