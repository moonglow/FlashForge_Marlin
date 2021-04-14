#!/usr/bin/env bash
# Usage: ./build_marlin.sh <printer_type> <swap_extruder>
#
# ./build_marlin.sh dreamer_nx
# ./build_marlin.sh inventor <true/false>
# ./build_marlin.sh dreamer <true/false>

set -o errexit
set -o nounset
set -o pipefail

# global variables
PRINTER_TYPE="${1:-dreamer_nx}"
SWAP_EXTRUDER=${2:-false}

RELEASE_NAME="marlin_${PRINTER_TYPE}"
MARLIN_SOURCE_DIR=$PWD/Marlin
FF_ENC_DIR=$PWD/flashforge_firmware_tool
BUILD_DIR=$PWD/BUILD
CONFIG_FOLDER=$PWD/configs
FINAL_FW="${BUILD_DIR}/${RELEASE_NAME}.bin"
# log levels
ERROR=1
DEBUG=1
INFO=1


# helper functions
function __msg_error() {
    [[ "${ERROR}" == "1" ]] && echo -e "[ERROR]: $*"
}

function __msg_debug() {
    [[ "${DEBUG}" == "1" ]] && echo -e "[DEBUG]: $*"
}

function __msg_info() {
    [[ "${INFO}" == "1" ]] && echo -e "[INFO]: $*"
}

function check_command_present() {
  if ! [[ -x "$(command -v $1)" ]]; then
    __msg_error "$1 does not exists" >&2

    if [[ -n "$2" ]]; then
      __msg_error "$2"
    fi
    exit 1
  fi
}

function check_file_exists() {
  if ! [[ -f  "$1" ]]; then
    __msg_error "$1 does not exists" >&2
    exit 1
  else
    __msg_info "$1 exists"
  fi
}
# main functions
function build_ff_enc_tool() {
  cd "${FF_ENC_DIR}"
  __msg_info "Building FF firmware Encryption tool"
  gcc main.c -o ff_fw_tool
  __msg_info "...done"

  check_file_exists ff_fw_tool
  mv ff_fw_tool "${BUILD_DIR}/"

}

function backup_restore_config(){
   cd "${MARLIN_SOURCE_DIR}/Marlin"
  #backup
  if [[ $1 -eq 0 ]]; then
     cp  machine_config.h  machine_config.h.bkp
     cp  machine_config_adv.h machine_config_adv.h.bkp
  # restore
  else
    mv  machine_config.h.bkp  machine_config.h
    mv  machine_config_adv.h.bkp machine_config_adv.h
  fi
  cd -
}

function build_marlin() {
   __msg_info "Building Marlin firmware"
   backup_restore_config 0

   fw_file=".pio/build/FF_F407ZG/firmware.bin"

   cd "${MARLIN_SOURCE_DIR}/Marlin"
   cp "${CONFIG_FOLDER}/Configuration-${PRINTER_TYPE}.h"  machine_config.h
   cp "${CONFIG_FOLDER}/Configuration_adv-${PRINTER_TYPE}.h"  machine_config_adv.h

  if [ "${SWAP_EXTRUDER}" = true ]; then
     if [ "${PRINTER_TYPE}" == "inventor" ] || [ "${PRINTER_TYPE}" == "dreamer" ]; then
         cp "${CONFIG_FOLDER}/Configuration-${PRINTER_TYPE}-swap.h"  machine_config.h
    fi
  fi


  cd ${MARLIN_SOURCE_DIR}
  platformio run -e FF_F407ZG

  __msg_info "...done"

  check_file_exists "$fw_file"
  # restore the backup
  backup_restore_config 1
  mv "$fw_file" "${BUILD_DIR}/"
}

function encrypt_firmware() {
  __msg_info "Encrypting firmware"
  cd "${BUILD_DIR}"

  ./ff_fw_tool -e -i "firmware.bin" -o "${FINAL_FW}"
}
function cleanup() {
  __msg_info "cleaning up"
   cd "${BUILD_DIR}"

   rm firmware.bin
   rm ff_fw_tool

   cd "${MARLIN_SOURCE_DIR}"
   platformio run --target clean -e FF_F407ZG

}

function start_build_process() {
  __msg_info "Starting building process for ${PRINTER_TYPE}"
  mkdir -p "${BUILD_DIR}"

  check_command_present platformio "Head onto https://docs.platformio.org/en/latest/core/installation.html to install platformio"

  build_ff_enc_tool
  build_marlin
  encrypt_firmware

  __msg_info "Ending building process"

}

function main() {
  start_build_process
  cleanup

  echo "--------------"
  __msg_info "Firmware should be available @ ${FINAL_FW}"
}




# Run main
main



