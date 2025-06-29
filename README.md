## Let's Rock!!

Let's Rock implements basic buttplug.io support for Guilty Gear XX Accent Core +R (GGXXACPR).

It connects to Intiface Central at `localhost:12345`, scans for devices, and interacts with the first connected device.

The vibration intensity is directly tied to the current combo meter, maxing out at 15 hits.

### Dependencies

- [Intiface Central](https://intiface.com/central)
- [A compatible device](https://buttplug.io/)
  - An *extremely* wide variety of devices are supported by the framework
- [Gear-DLL-Loader](https://github.com/MoonSquaredd/Gear-DLL-Loader)
  - This is the modloader for the game as well as the source for `GG.h`
- [buttplugCpp](https://github.com/dumbowumbo/buttplugCpp/tree/master)
  - This is the source for `buttplugclient.h`