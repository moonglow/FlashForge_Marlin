#!/bin/bash

flags=""
name_postfix=""
encryption_key="flashforge790315"
build_env="FF_F407ZG"
project_dir="Marlin"
fw_path=$project_dir"/.pio/build/"$build_env
fw_tool_dir="flashforge_firmware_tool"
fw_version=$(awk '/fine SHORT_BUILD_VERSION/{ print substr($3,2,length($3)-2) }' Marlin/Marlin/Version.h)
build_silent="--silent"
SIZE=arm-none-eabi-size

function usage()
{
   cat << usage_info

   Usage: $(basename $0) -m <machine> [-s] [-l]

   arguments:
     -h           show this help message and exit
     -m           machine name ( nx/dreamer/inventor/powerspec )
     -s           swap extruders ( for dreamer and inventor machines )
     -o           use Dreamer old motherboard ( swap extruder DIR )
     -l           enable linear advance ( pressure control algo )
     -u           old style GUI
     -g           MKS GUI
     -v           verbose build
     
   example:
     $(basename $0) -m dreamer -s -l
usage_info
}

if ! hash -r python3; then
   echo "python3 is not installed"
   exit
fi

if ! hash -r platformio; then
   echo "platformio is not installed"
   exit
fi

if [ ! -x $fw_tool_dir/ff_fw_tool ]
then
   echo "Build FF FW tool..."
   gcc $fw_tool_dir/main.c -o $fw_tool_dir/ff_fw_tool
   if [[ $? -eq 0 ]]
   then
    echo "done"
   else
    echo "failed"
    exit
   fi
fi

while getopts "m:slohvug" opt
do
   case "$opt" in
      m ) machine="$OPTARG" ;;
      s ) flags+="-DFF_EXTRUDER_SWAP "
          name_postfix+="_swap"
         ;;
      l ) flags+="-DLIN_ADVANCE "
          name_postfix+="_la"
         ;;
      u ) flags+="-DUSE_OLD_MARLIN_UI "
          name_postfix+="_classic"
         ;;
      o ) flags+="-DFF_DREAMER_OLD_MB "
          name_postfix+="_dir"
         ;;
      g ) flags+="-DUSE_MKS_UI "
          name_postfix+="_mks"
         ;;
      v ) build_silent="" ;;
      ? | h ) usage; exit ;;
   esac
done

if [[ "$machine" == "nx" ]]
then
   flags+="-DFF_DREAMER_NX_MACHINE"
elif [[ "$machine" == "dreamer" ]]
then
   flags+="-DFF_DREAMER_MACHINE"
elif [[ "$machine" == "inventor" ]]
then
   flags+="-DFF_INVENTOR_MACHINE"
elif [[ "$machine" == "dremel" ]]
then
   flags+="-DFF_DREMEL_3D20_MACHINE"
   encryption_key="flashforge123456"
elif [[ "$machine" == "powerspec" ]]
then
   flags+="-DFF_DREAMER_MACHINE -DFF_SILVER_PULLEY -DFF_EXTRUDER_SWAP -DFF_DREAMER_OLD_MB"
else
   usage
   exit
fi

PLATFORMIO_BUILD_FLAGS="$flags"
export PLATFORMIO_BUILD_FLAGS
echo "Flags: " $PLATFORMIO_BUILD_FLAGS
echo "Clean..."
platformio run --project-dir $project_dir --target clean -e $build_env $build_silent
echo "Build..."
platformio run --project-dir $project_dir -e $build_env $build_silent

if [[ $? -eq 0 ]]
then
   echo -e "Build OK\nEncode firmware..."
   if hash -r $SIZE; then
      $SIZE --format=berkeley $fw_path/firmware.elf
   fi
   mkdir -p "build"
   $(pwd)/$fw_tool_dir/ff_fw_tool -k $encryption_key -e -i $fw_path/firmware.bin -o build/$machine"_"$fw_version$name_postfix"_"`date +"%m%d%Y"`".bin"
else
   echo "Build failed"
fi



