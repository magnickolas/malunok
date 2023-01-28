set(DEMOS sin sin_wall)

find_package(SDL2 REQUIRED)
find_package(X11 REQUIRED)

function(buildDemo DEMO_NAME)
  SET(DEMO_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_NAME})
  message(STATUS "Generating project file for demo in ${DEMO_FOLDER}")
  file(GLOB SOURCE ../src/*.cc ${DEMO_FOLDER}/*.cc)
  add_executable(${DEMO_NAME} ${SOURCE})
  target_include_directories(${DEMO_NAME} PRIVATE
    ../include ${SDL2_INCLUDE_DIRS} ${X11_INCLUDE_DIR})
  target_link_libraries(${DEMO_NAME} PRIVATE
    ${SDL2_LIBRARIES} ${X11_LIBRARIES})
  target_compile_features(
    ${DEMO_NAME} PRIVATE
    cxx_std_20)
  target_compile_options(
    ${DEMO_NAME} PRIVATE
    -Wall -Wextra -pedantic-errors)
endfunction(buildDemo)

function(buildDemos)
  foreach(DEMO ${DEMOS})
    buildDemo(${DEMO})
  endforeach(DEMO)
endfunction(buildDemos)

buildDemos()
