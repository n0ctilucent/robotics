#!/usr/bin/env bash
#
# SPDX-FileCopyrightText: ©2026 franklin <smoooth.y62wj@passmail.net>
#
# SPDX-License-Identifier: MIT

# ChangeLog:

DEB_PKG=(imx500 libgpiod-dev)
LRED=$(tput setaf 1)

function main() {
  figlet -f "${HOME}/workspace/fonts/figlet/pagga" scrappy the robot && echo -e "\n"
  if [ -f "${HOME}/bin/common.sh" ]; then
    source "${HOME}/bin/common.sh"
  else
    echo -e "${LRED}can not find ${HOME}/bin/common.sh.${NC}"
    exit 1
  fi
  log_info "successfully sourced ${HOME}/bin/common.sh" && echo -e "\n"

  install_debian
  # gpiodetect
  autoreconf -ivf
  ./configure
  make all
}

main "$@"
