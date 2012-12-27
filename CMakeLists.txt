cmake_minimum_required (VERSION 2.6)

project (TileSim)

include_directories (
  external/boost
  config
)

IF (WINDOWS)
  include_directories (
    external/msinttypes
  )
ENDIF (WINDOWS)

add_executable (TileSim
  src/Cache.cpp
  src/CoreBlock.cpp
  src/CPU.cpp
  src/GlobalMemoryController.cpp
  src/IniReader.cpp
  src/main.cpp
  src/Message.cpp
  src/MMU.cpp
  src/Processor.cpp
  src/Router.cpp
  src/SimConfig.cpp
  src/simutil.cpp
  src/TaskBlock.cpp
  src/Task.cpp
  src/Thread.cpp
  src/Tile.cpp
)
