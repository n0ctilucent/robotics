## clone the esp-idf repo (build tools)
```
git clone --recursive https://github.com/espressif/esp-idf.git
```

## Copy the build tools to your system
```
sudp cp xtensa-esp32-elf /opt
sudo cp esp-idf /opt
```

## Set up  .bash_profile

So you can find the toolchain

```
export IDF_PATH=/opt/esp-idf
export PATH=$PATH:/opt/xtensa-esp32-elf/bin
```

## create config file
```
make menuconfig
```
