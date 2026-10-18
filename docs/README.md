# docs

```sh
autoreconf -i
```

## wiring pi

```sh
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build debian
cd debian-template
sudo apt install ./wiringpi_3.14_arm64.deb
```

```sh
NOTE: To compile programs with wiringPi, you need to add:
    -lwiringPi
  to your compile line(s) To use the Gertboard, MaxDetect, etc.
  code (the devLib), you need to also add:
    -lwiringPiDev
  to your compile line(s).
```
## GPIO

```sh
gpio -v
```

## camera

- [rpicam-apps](https://www.raspberrypi.com/documentation/computers/camera_software.html)
