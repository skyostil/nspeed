Project(N-Speed)
cmake_minimum_required(VERSION 2.6)

set (
  SOURCES
  src/BitmapFont.cpp
  src/Car.cpp
  src/Environment.cpp
  src/FixedPointMath.cpp
  src/FixedPointMatrix.cpp
  src/FixedPointVector.cpp
  src/GameEngine.cpp
  src/Land.cpp
  src/Menu.cpp
  src/Mesh.cpp
  src/MeshSet.cpp
  src/Mixer.cpp
  src/ModPlayer.cpp
  src/Object.cpp
  src/Rasterizer.cpp
  src/TagFile.cpp
  src/TestGame.cpp
  src/Texture.cpp
  src/Track.cpp
  src/View.cpp
  src/World.cpp
  src/engine/Engine.cpp
  src/engine/Video.cpp
  src/engine/Audio.cpp
  src/framework/sdl/SDLFramework.cpp
)
include_directories(src unix)

add_definitions(-DPREFIX=".")
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

link_libraries(
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
)

add_executable(
  nspeed
  WIN32
  MACOSX_BUNDLE
  ${SOURCES}
)

