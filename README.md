# 2jcieev01-raspberrypi
It is a sample projects for OMRON sensor evaluation kit **2JCIE-EV01-RP1**
with Raspberry-Pi boards.

2JCIE-EV01 sensor evaluation boards are Open Platforms by OMRON corporation and
prototype your ideas with variety environmental information.

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
T.B.D (console output)

## Installation
### Prerequisite
- WiringPi (pre-installed in standard Raspbian)
- gcc (pre-installed in standard Raspbian)
- (Optional) ALSA library for microphone.

### Procedure
this procedure shows the procedure to use the sensors on
**2JCIE-EV01-RP1** (except microphone).

1. download the archive from github [releases](releases) or
    [master](archive/2jcieev01-raspberrypi-master.zip)

    ```shell
    $ unzip 2jcieev01-raspberrypi-master.zip
    ```

    or, with git:

    ```shell
    $ git clone https://github.com/omron-devhub/2jcieev01-raspberrypi
    ```

2. make

    ```shell
    $ cd 2jcieev01-raspberrypi-master
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
pcm.!default {
    type plug
    slave {
        pcm i2s
    }
    ttable.0.0 1.0
    ttable.0.1 1.0
}

pcm.i2s {
    type hw
    card 1
}

ctl.!default {
    type hw
    card 0
}
^D
```

**^D is ctrl + D**


## Links
- [Arduino projects for 2JCIE-01-AR1, FT1](https://github.com/omron-devhub/2jcieev01-arduino)


## Licence
Copyright (c) OMRON Corporation. All rights reserved.

Licensed under the MIT License.

