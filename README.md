# raspberrypi_lab

raspberrypiでROSを使ってad7147と通信する。

## Getting Started
## raspberrypi

WiringPi
```
git clone  https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build
```

gpio setting
```
./prog/setup.bash
```

## note
which + 12 =
raspberry pi
$ gpio readall
のBCM = raspberry pi のGPIO x

BCM | 12 13 14 15 16 17 18 19 20 21 22 23
wpi | 26 23 15 16 27 0  1  24 28 29 3  4   ←setup.bash
