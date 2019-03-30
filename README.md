# 2jcieev01-raspberrypi
It is a sample projects for OMRON sensor evaluation kit **2JCIE-EV01-RP1**
with Raspberry-Pi boards.

2JCIE-EV01 sensor evaluation kits are ...T.B.D...
(see 2SMPB-02E projects abstract)

## Description
Sample programs for acquiring data from sensors on 2JCIE-EV01.
there samples output the sensor data to console.

| example        | description                     | baord |
|:--------------:|:--------------------------------|:--------------------|
| 2jcieev01-baro | The Barometer sensor sample     | Raspberry-Pi ZERO-W |
| 2jcieev01-humi | The Illuminance sensor sample   | Raspberry-Pi ZERO-W |
| 2jcieev01-illm | The Humidity sensor sample      | Raspberry-Pi ZERO-W |
| 2jcieev01-accl | The Accelerometer sensor sample | Raspberry-Pi ZERO-W |

## DEMO
T.B.D.

## Installation
### Prerequisite
- WiringPi (pre-installed in standard Raspbian)
- gcc (pre-installed in standard Raspbian)
- (Optional) ALSA library for microphone.

### Procedure
this procedure shows the procedure to use the sensors on
**2JCIE-EV01-RP1** (except microphone).

1. download this repo

    ```shell
    $ git clone https://github.com/omron-devhub/2jcieev01-raspberrypi
    ```

2. make

    ```shell
    $ cd 2jcieev01-raspberrypi
    $ make all
    ```

3. run for barometer

    ```shell
    $ ./2jcieev01-baro
    ```

4. run for illuminance sensor

    ```shell
    $ ./2jcieev01-illm
    ```

5. run for humidity sensor

    ```shell
    $ ./2jcieev01-humi
    ```

6. run for accerelometer

    ```shell
    $ ./2jcieev01-accl
    ```

### Procedure for microphone
microphone on **2JCIE-EV01-RP1** is wired with
standard Raspberry-Pi I2S connections.

For enbale it, please refer below site:

* [Adafruit Raspberry Pi Wiring & Test](https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/raspberry-pi-wiring-and-test)

* [Using the ICS43432 MEMS microphone on a Raspberry Pi with i2s](https://github.com/nejohnson2/rpi-i2s)

microphone on **2JCIE-EV01-RP1** is not same on above site,
but don't worry I2S microphones will work same procedure
with high compatibility.

#### Small output(volume) from microphone
In some I2S reasons, microphone was treated as streao device
and mixed the output by 50%(left) and 50%(right).

- left: peseudo ouput and always output 0.
- right: real ouput.

then microphone output is small.
you can fix the volume via `.asoundrc` like this::

```shell
$ cd $HOME
$ cat > .asoundrc
T.B.D.
```

## Licence
Copyright (c) OMRON Corporation. All rights reserved.

Licensed under the MIT License.

