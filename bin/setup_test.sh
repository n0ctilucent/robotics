#!/usr/bin/env bash
#
# SPDX-FileCopyrightText: ©2026 franklin <smoooth.y62wj@passmail.net>
#
# SPDX-License-Identifier: MIT

LRED='\033[1;31m'

function main() {

  if [ -d "${HOME}/workspace/fonts/figlet" ] && [ "$(which figlet)" ]; then
    echo -e "\n\n" && figlet -f "${HOME}/workspace/fonts/figlet/pagga" rover && echo -e "\n"
  fi

  if [ -f "./common.sh" ]; then
    source "./common.sh"
  else
    echo -e "${LRED}can not find bin/common.sh.${NC}"
    exit 1
  fi
  
  log_info "successfully sourced bin/common.sh" && echo -e "\n"

  sudo apt update
  sudo apt install -y python3 python3-pip python3-serial minicom 

  sudo usermod -a -G dialout $(whoami)

  # Raspberry Pi
  # sudo usermod -a -G gpio $(whoami)
  # gpiodetect

  sudo dmesg | grep tty | grep -i usb
  # lsusb -> "Bus 005 Device 011: ID 10c4:ea60 Silicon Labs CP210x UART Bridge"
  log_info "Plug in your ESP-32. Look for ttyUSB0 as USB ACMP device or similar."
  sudo dmesg | grep tty | grep -i usb
 
}

main "$@"