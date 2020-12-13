# R-Engine

R-Engine is a game engine made in C and C++ for Videogame Engine subject by **Oscar Royo Moreno** and **Carlos Redolar Torres**. We are currently studying Videogame Design and Development in CITM, Terrassa.

Github page: https://github.com/carlosredolar/R-Engine

Github profiles:
* https://github.com/oscarroyo4 - Oscar Royo
* https://github.com/carlosredolar - Carlos Redolar



## Controls:

**W** - Move Forward

**A** - Move Left

**S** - Move Backwards

**D** - Move Right

**Z** - Move Up

**X** - Move Down

**Hold Shift** - Duplicate move speed

**Mouse Wheel** - Zoom In/Out

**Alt + Left Click** - Orbit camera around a reference point

**F** - Focus the camera on the selected object

**Middle Mouse** - Drag to freely move perpendicularly

You can move the camera in the scene only if the scene window is selected.


## Engine Functionality

FBX models can be imported dragging and dropping them onto the engine from the assets window or from any other folder. Textures (png and jpg) can be imported dragging and dropping them onto the texture image on inspector and applied to the object selected.

*There is an issue when an asset has been already imported and is avaliable on assets window but you try to import it from your computer floder, just import it from the asset window of the program.

*Sometimes when you try to import many assets in a short period of time the program crashes, but just sometimes.

## Inspector

**Name:** selected object name

**Transform:** translate, rotate and scale the object

**Mesh:** can enable/disable vertex and face normals and shows the amount of vertices and indices of the object

**Texture:** can enable/discable the object or checkers texture and shows texture name, path and size


## Hierarchy

In this window we can see the GameObjects and their hierarchy, being able to select each one of them and see all its information on inspector.

You can also delete a selected object pressing **SUPR**.

## Console

In this window we can see al the logs of the engine. Errors will appear on red.

## Configuration

In this window we can see all the configuration options:

* Application

* Window

* Renderer

* Camera

* Hardware

* Input

* Time

## Toolbars

### File

* **Save:** save the current scene

* **Load:** load a saved scene

* **Quit:** close the program

### View

* **Inspector:** enable/disable the inspector window

* **Hierarchy:** enable/disable the hierarchy window

* **Scene:** enable/disable the scene window

* **Assets:** enable/disable the assets window

* **Console:** enable/disable the console window

* **Configuration:** enable/disable the configuration window

* **Resources:** enable/disable the resources window

### Add

* **Empty Object:** you can create an empty object on hierarchy

* **Primitives:** you can choose those primitives and load them on the scene as GameObjects

### About

Here you can find some information about us (Github links) and about the project (license and software versions)
