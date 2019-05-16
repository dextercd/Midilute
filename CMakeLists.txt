cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

find_package(ALSA REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GUILE REQUIRED guile-2.2)

add_library(guilemidi SHARED
	guile-midi/midi.cpp
)

target_compile_features(guilemidi PUBLIC cxx_std_17)

target_link_libraries(guilemidi ALSA::ALSA)
target_link_libraries(guilemidi ${GUILE_LIBRARIES})
target_include_directories(guilemidi PRIVATE ${GUILE_INCLUDE_DIRS})
target_compile_options(guilemidi PUBLIC ${GUILE_CFLAGS_OTHER})
