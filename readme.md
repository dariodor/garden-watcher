# Garden watcher

Garden watcher is a simple way to check your garden state for a better harvest.

### Usage

1. Download garden-watcher.ino and fill missing information like network SSID, xively feed ID...
2. Compile and upload garden-watcher.ino to your Arduino with WifiShield R3
3. Test with serial Monitor
4. Connect your sensors and power up your Arduino

### Build instructions

**Moisture sensor**

![Resistive_divider](http://upload.wikimedia.org/wikipedia/commons/d/db/Resistive_divider.png)

- R1 = 1kΩ
- R2 = moisture sensor
- Vin = 5v
- Vout to Arduino board, analogique pin

R2 = R1/((Vin/Vout)-1)

**Light sensor**

![Resistive_divider](http://upload.wikimedia.org/wikipedia/commons/d/db/Resistive_divider.png)

- R1 = 680Ω
- R2 = moisture sensor
- Vin = 5v
- Vout to Arduino board, analogique pin

R2 = R1/((Vout/Vin)-1)

### What to do next?

* Temp sensor
* External panel with full sensors history
* Electric water valve
* ADC with I2C for more than 5 sensors
* PH sensor

### Like it?

Tell me your thoughts! I'm on [twitter](https://twitter.com/dariodor) and [google+](https://plus.google.com/+DarioDorfelli/about)

### Similar projects

* [http://gardenbot.org/](http://gardenbot.org/about)

### Author

garden-watcher.ino is written by [Dario Dorfelli](https://plus.google.com/+DarioDorfelli/about).

### Known issues

* WifiShield R3 doesn't work with IDE 1.0.5, 1.0.3 - [1840](https://github.com/arduino/Arduino/issues/1840) - Switch to 1.0.4

### Further reading

* [Cheap moisture sensor](http://www.cheapvegetablegardener.com/how-to-make-cheap-soil-moisture-sensor-2/)