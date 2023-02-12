# Unreal Engine Plugin: Elementus Modules
![HighresScreenshot00001](https://user-images.githubusercontent.com/77353979/218318475-cb5c7f62-8ec8-43a3-a4a1-6b4b5104a362.png)

An Unreal Engine plugin containing all modules and dependencies that were created for [Project Elementus](https://github.com/lucoiso/UEProject_Elementus).

# About
[Project Elementus](https://github.com/lucoiso/UEProject_Elementus) is a third person template that can be used to start projects that will use some new features that come with Unreal Engine 5 as well as powerful existing features like the Gameplay Ability System and others. and now, all it's contents were turned into a plugin containing modules accordingly to it's objective.

## Modules
* **Elementus Ability System:** Adds custom classes, structures and functions related to Gameplay Ability System.
* **Elementus Actors:** Adds custom actors to be used in other projects.
* **Elementus Common:** Contains code that will be shared between the modules.
* **Elementus Core:** Contains code related to the core of the game framework such as character, controllers, etc.
* **Elementus Epic Online Services:** This module is focused in the integration of the Epic Online Services and contains some functions and a Game Instance class that provides EOS access directly via Unreal's APIs ([EOSSDKHandler](https://github.com/lucoiso/UEEOSSDKHandler)).
* **Elementus Inventory System:** Contains custom classes and interfaces to enable the use of an inventory system with multiplayer compatibility.

# Dependencies
## Submodules
* [Modular Features - Extra Actions:](https://github.com/lucoiso/UEModularFeatures_ExtraActions) Provides Modular integration of GAS and Enhanced Input.
* [Elementus Inventory:](https://github.com/lucoiso/UEElementusInventory) Provides a Data-Driven Inventory & Items system based on FPrimaryAssetIds and the Asset Manager.
* [EOS SDK Handler:](https://github.com/lucoiso/UEEOSSDKHandler) Handle the EOS SDK, define the use of EOS tools and give access to some private headers.
* [FSR 1.0 (Custom):](https://github.com/lucoiso/UEFSR) Adapted version of the original AMD FSR 1.0 plugin for UE4 to properly work in Unreal Engine 5.

## Requirements
* Unreal Engine 5.1+.
* Visual Studio 2019 or 2022 with the module Game Development with C++.
* Git w/ Git LFS.

# Installation
## Download as Zip
1. Navigate to the [release page](https://github.com/lucoiso/UEElementusModules/releases) and download the .zip file of the most recent version.
2. After the download complete, go to your project's directory and check if there's a folder named "Plugins". Create if doesn't exists.
3. Inside the 'Plugins' directory, create a new folder 'ElementusModules'.
4. Unzip the (.zip) content inside the new directory 'ElementusModules'.
5. Go to your project's root directory, right click the .uproject file and select: Generate Visual Studio Project Files.
6. Open the generated .sln file and compile your project.
7. Open your project and let's dev! :)

## Cloning: As submodule
As this repository containg only plugins, the ideal is to use as a submodule in your project.

Run these commands to add this plugin as a submodule:
```
git submodule add -b main https://github.com/lucoiso/UEElementusModules.git Plugins/UEElementusModules
git submodule update --init --recursive
```

## Cloning: As repository
Run these commands inside your project's Plugins directory

```
git clone https://github.com/lucoiso/UEElementusModules.git
cd UEElementusModules
git submodule update --init --recursive
```
