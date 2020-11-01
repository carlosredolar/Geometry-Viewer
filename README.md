# R-Engine

R-Engine is a game engine made in C and C++ for Videogame Engine subject by **Oscar Royo Moreno** and **Carlos Redolar Torres**. We are currently studying Videogame Design and Development in CITM, Terrassa.

Github page: https://github.com/carlosredolar/R-Engine

Github profiles:
* https://github.com/oscarroyo4 - Oscar Royo
* https://github.com/carlosredolar - Carlos Redolar



## Controls:

**W** - Move Up

**A** - Move Left

**S** - Move Down

**D** - Move Right

**Shift + WASD** - Duplicate move speed

**Mouse Wheel** - Zoom In/Out

**Alt + Left Click** - Orbit camera around a reference point

**Right Click** - Look around

**F** - Focus the camera on the selected object


## Engine Functionality

FBX models can be imported dragging and dropping them onto the engine. Textures (png and jpg) can be imported dragging and dropping them onto the engine and applied to an object if a GameObject is selected.

## Inspector

**Name:** selected object name

**Transform:** shows selected object information (position, rotation and scale)

**Mesh:** can enable/disable vertex and face normals and shows the amount of vertices and indices of the object

**Texture:** can enable/discable the object texture and shows texture name, path and size


## Hierarchy

In this window we can see the GameObjects and their hierarchy, being able to select each one of them and see all its information on inspector.

## Console

In this window we can see al the logs of the engine.

## Configuration

In this window we can see all the configuration options:

* Application

* Window

* 3D Renderer

* File System

* Input

* Hardware

## Toolbars

### File

* **Quit:** close the program

### View

* **Console:** enable/disable the console window

* **Configuration:** enable/disable the configuration window

* **Inspector:** enable/disable the inspector window

* **Hierarchy:** enable/disable the hierarchy window

### Add

* **Primitives:** you can choose those primitives and load them on the scene as GameObjects

### About

Here you can find some information about us (Github links) and about the project (license and software versions)
