#if ENABLED( FF_EXTRUDER_SWAP )
#if ENABLED( FF_DREAMER_MACHINE )
  #define MAIN_MENU_ITEM_3_GCODE \
"G90\n"\
"M117 Bed leveling started\n"\
"G28\n"\
"M117 Moving to next point\n"\
"G1 Z5 F420\n"\
"G1 X-17 Y-60 F3300\n"\
"G1 Z0 F420\n"\
"M0 Adjust the screw and press to continue\n"\
"M117 Moving to next point\n"\
"G1 Z5 F420\n"\
"G1 X-87 Y55 F3300\n"\
"G1 Z0 F420\n"\
"M0 Adjust the screw and press to continue\n"\
"M117 Moving to next point\n"\
"G1 Z5 F420\n"\
"G1 X53 Y55 F3300\n"\
"G1 Z0 F420\n"\
"M0 Adjust the screw and press to continue\n"\
"M117 Moving to next point\n"\
"G1 Z5 F420\n"\
"G1 X-17 Y0 F3300\n"\
"G1 Z0 F420\n"\
"M0 Press to finish\n"\
"M117 Leveling done\n"\
"G1 Z10 F420\n"\
"G28 X Y\n"\
"M84"
#elif ENABLED( FF_INVENTOR_MACHINE )
  #define MAIN_MENU_ITEM_3_GCODE \
"G90\n"\
"M117 Bed leveling started\n"\
"G28\n"\
"M117 Moving to next point\n"\
"G1 X-17 Y-60 F3300\n"\
"G1 Z0 F840\n"\
"M0 Adjust the screw and press to continue\n"\
"M117 Moving to next point\n"\
"G1 Z5 F420\n"\
"G1 X-87 Y55 F3300\n"\
"G1 Z0 F420\n"\
"M0 Adjust the screw and press to continue\n"\
"M117 Moving to next point\n"\
"G1 Z5 F420\n"\
"G1 X53 Y55 F3300\n"\
"G1 Z0 F420\n"\
"M0 Adjust the screw and press to continue\n"\
"M117 Moving to next point\n"\
"G1 Z5 F420\n"\
"G1 X-17 Y0 F3300\n"\
"G1 Z0 F420\n"\
"M0 Press to finish\n"\
"M117 Leveling done\n"\
"G1 Z10 F420\n"\
"G28 X Y\n"\
"M84"
#endif
#else
#if ENABLED( FF_DREMEL_3D20_MACHINE )
  #define MAIN_MENU_ITEM_3_GCODE \
"G90\n"\
"M117 Bed leveling started\n"\
"G28\n"\
"G1 Z10 F800\n"\
"G1 X-57.7 Y-67 F3000\n"\
"G1 Z0 F800\n"\
"M0 Adjust the screw and press to continue\n"\
"G1 Z10 F800\n"\
"G1 X41.3 Y-67 F3000\n"\
"G1 Z0 F800\n"\
"M0 Adjust the screw and press to continue\n"\
"G1 Z10 F800\n"\
"G1 X-6.6 Y60 F3000\n"\
"G1 Z0 F800\n"\
"M0 Adjust the screw and press to continue\n"\
"G1 Z10 F800\n"\
"G1 X0 Y0 F3000\n"\
"G1 Z0 F800\n"\
"M0 Press to finish\n"\
"M117 Leveling done\n"\
"G1 Z10 F800\n"\
"G28 X Y\n"\
"M84"
#else
/* original script for FlashForge NX/Dreamer/Inventor/ */
  #define MAIN_MENU_ITEM_3_GCODE \
"G90\n"\
"M117 Bed leveling started\n"\
"G28\n"\
"G1 Z10 F800\n"\
"G1 X-5.3 Y-71.1 F3000\n"\
"G1 Z0 F800\n"\
"M0 Adjust the screw and press to continue\n"\
"G1 Z10 F800\n"\
"G1 X-54.4 Y61.6 F3000\n"\
"G1 Z0 F800\n"\
"M0 Adjust the screw and press to continue\n"\
"G1 Z10 F800\n"\
"G1 X42.8 Y61.6 F3000\n"\
"G1 Z0 F800\n"\
"M0 Adjust the screw and press to continue\n"\
"G1 Z10 F800\n"\
"G1 X0 Y0 F3000\n"\
"G1 Z0 F800\n"\
"M0 Press to finish\n"\
"M117 Leveling done\n"\
"G1 Z10 F800\n"\
"G28 X Y\n"\
"M84"
#endif
#endif
