cmake_minimum_required(VERSION 3.10)
project(JetpackJoyridePC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Znajdź SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

# Znajdź SDL2_image - ręcznie ustawiamy ścieżki
set(SDL2_IMAGE_INCLUDE_DIRS /opt/homebrew/include/SDL2)
set(SDL2_IMAGE_LIBRARIES /opt/homebrew/lib/libSDL2_image.dylib)

include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
link_directories(${SDL2_IMAGE_LIBRARIES})

# Znajdź SDL2_ttf - ręcznie ustawiamy ścieżki
set(SDL2_TTF_INCLUDE_DIRS /opt/homebrew/include/SDL2)
set(SDL2_TTF_LIBRARIES /opt/homebrew/lib/libSDL2_ttf.dylib)

include_directories(${SDL2_TTF_INCLUDE_DIRS})
link_directories(${SDL2_TTF_LIBRARIES})

# Dodaj katalogi z plikami nagłówkowymi
include_directories(include)

# Dodaj katalogi z plikami źródłowymi
add_executable(JetpackJoyridePC src/main.cpp src/Game.cpp)

# Linkowanie bibliotek SDL2, SDL2_image i SDL2_ttf
target_link_libraries(JetpackJoyridePC ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
