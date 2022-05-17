# Mace
_Mobile C++ Game Engine_

This project is for creating a game engine with an emphasis on mobile development. 
The first iteration of this project will focus on Android development, but the engine
is being designed so platform-specific behavior is abstracted into generic interfaces.

## Structure

The project is divided into the following modules

- **engine** - the core implementation
- **android_engine** - android implementation of platform-specific engine functionality
- **game** - (soon to be) a sample game made with the engine
- **android_app** - simple android native app that invokes the game code