set(name tap-bridge)

set(source_files model/tap-bridge.cc model/tap-encode-decode.cc helper/tap-bridge-helper.cc)

set(header_files model/tap-bridge.h model/tap-encode-decode.h helper/tap-bridge-helper.h)

set(libraries_to_link ${libcore} ${libinternet} ${libnetwork})

set(test_sources)

build_lib("${name}" "${source_files}" "${header_files}" "${libraries_to_link}" "${test_sources}")

set(tap_creator tap-creator)
if(WIN32)
  set(tap_creator ${tap_creator}.exe)
endif()

add_definitions(-DTAP_CREATOR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/src/tap-bridge/${tap_creator}")

add_executable(tap-creator model/tap-creator.cc model/tap-encode-decode.cc)
set_runtime_outputdirectory(tap-creator ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/src/tap-bridge/)
