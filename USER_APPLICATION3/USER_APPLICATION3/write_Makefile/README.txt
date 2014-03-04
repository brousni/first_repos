# This folder must be placed in the root of your project
  (The folder containing the: "Debug" and "src" folders).

# Run "WRITE_MAKEFILE.bat" - (You will be asked to enter
  the project name here) - This sets up the environment
  (../ext_make) for the makefile and writes the makefile
  in that folder.
  Gnu make alongside "MAKE_ALL" and "MAKE_CLEAN" are in the
  folder src_files/gnu_make. Copy these to the ext_make folder

# Open your project in Atmel studio. Choose the tab:
  "Project" -> Properties
  Choose the tab: "Build" and check out the "Use External Makefile"
  Now browse for the folder in which the make file is generated
  eg. C:\your_work_folder\project\project\ext_make\Makefile