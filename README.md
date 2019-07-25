# d6t-2jcieev01-raspberrypi
It is a sample projects for D6T OMRON MEMS Thermal Sensors with
evaluation kit **2JCIE-EV01-AR1**,
**2JCIE-EV01-FT1** and some Arduino boards.

D6T sensor series are High Sensitivity Enables Detection
of Stationary Human Presence,

- OMRON's unique MEMS and ASIC technology achieve a high SNR.
- Superior noise immunity with a digital output.
- High-precision area temperature detection with low cross-talk field of
    view characteristics


## Description
this Arduino sample projects for acquiring data from sensors on 2JCIE-EV01.
sample projects output the sensor data to USB-Serial ports.

| example | description                | baord |
|:-------:|:---------------------------|:--------------------|
| d6t-1a  | for the element type 1x1   | Raspberry-Pi ZERO-W |
| d6t-8l  | for the element type 1x8   | Raspberry-Pi ZERO-W |
| d6t-44l | for the element type 4x4   | Raspberry-Pi ZERO-W |
| d6t-32l | for the element type 32x32 | Raspberry-Pi ZERO-W |


## DEMO
sample output from D6T-44L

```
PTAT:   27.2[degC]
27.5,27.3,27.3,27.3 [degC]
27.2,27.1,27.1,27.2 [degC]
27.1,27.1,27.3,27.3 [degC]
27.0,27.3,27.3,27.0 [degC]
```


## Installation
### Prerequisite
- gcc (pre-installed in standard Raspbian)


### Procedure
this procedure shows the procedure to use the sensors on
**2JCIE-EV01-RP1** .

1. download the archive from github [releases](releases) or
    [master](archive/2jcieev01-raspberrypi-master.zip)

    ```shell
    $ unzip 2jcieev01-raspberrypi-master.zip
    ```

    or, with git:

    ```shell
    $ git clone https://github.com/omron-devhub/d6t-2jcieev01-raspberrypi
    ```

2. make

    ```shell
    $ cd d6t-2jcieev01-raspberrypi-master
    $ make all
    ```

3. run

    ```shell
    $ ./d6t-1a
    ```

    ```shell
    $ ./d6t-8l
    ```

    ```shell
    $ ./d6t-44l
    ```

    ```shell
    $ ./d6t-32l
    ```


## Links
- [Arduino samples for 2JCIE-01-AR1/FT1](https://github.com/omron-devhub/2jcieev01-arduino)
- [RaspberryPi samples for 2JCIE-01-RP1](https://github.com/omron-devhub/2jcieev01-raspberrypi)
- [Arduino sample for D6T on 2JCIE-01-AR1/FT1](https://github.com/omron-devhub/d6t-2jcieev01-arduino)
- [Arduino sample for D6F on 2JCIE-01-AR1/FT1](https://github.com/omron-devhub/d6f-2jcieev01-arduino)
- [RaspberryPi sample for D6F on 2JCIE-01-RP1](https://github.com/omron-devhub/d6f-2jcieev01-raspberrypi)
- [Arduino sample for B5W on 2JCIE-01-AR1/FT1](https://github.com/omron-devhub/b5w-2jcieev01-arduino)

projects by another authors.

- [d6t-grove-tinkerboard project](https://github.com/omron-devhub/d6t-grove-tinkerboard)
- [d6t-grove-m5stack project](https://github.com/omron-devhub/d6t-grove-m5stack)
- [d6t-grove-arduino project](https://github.com/omron-devhub/d6t-grove-arduino)  
    only for element 4x4 type, but libralized.


## Licence
Copyright (c) OMRON Corporation. All rights reserved.

Licensed under the MIT License.

