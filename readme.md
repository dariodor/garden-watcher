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
![Transistor](http://upload.wikimedia.org/wikipedia/en/3/3b/BJT_bias.png)

- R1 = 1kΩ
- R2 = moisture sensor
- Vin = 5v
- Vout to Arduino board, analogique pin
- Transistor BC237A
- Rb = 150kΩ

R2[Ω] = R1/((Vin/Vout)-1)

**Light sensor**

![Resistive_divider](http://upload.wikimedia.org/wikipedia/commons/d/db/Resistive_divider.png)

- R1 = light sensor
- R2 = 680Ω
- Vin = 5v
- Vout to Arduino board, analogique pin

R1[Ω] = (((Vin*R2)/Vout)-R2)

R1[Ω] = (((vIn*R2)/((analogValue*vIn)/1024))-R2)

**Temp sensor**

![Resistive_divider](http://www.learningaboutelectronics.com/images/LM35-temperature-sensor-pinout.png)

- Vin = 5v
- R1 = 2kΩ
- Vout to Arduino board, analogique pin

Temp[°C] = ((Vin*analogValue)/1024)*100

### What to do next?

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