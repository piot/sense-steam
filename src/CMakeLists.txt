cmake_minimum_required(VERSION 3.16.3)
project(sense-steam C)

add_subdirectory(deps/piot/clog/src/lib)
add_subdirectory(deps/piot/tiny-libc/src/lib)
add_subdirectory(deps/piot/steam-input-c/src/lib)
add_subdirectory(deps/piot/steam-api-c/src/lib)
add_subdirectory(deps/piot/atheneum/src/lib)
add_subdirectory(deps/piot/basal-c/src/lib)
add_subdirectory(deps/piot/sense/src/lib)
add_subdirectory(deps/piot/imprint/src/lib)



add_subdirectory(lib)



