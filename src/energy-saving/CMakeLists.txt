check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/energy-saving-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME energy-saving
    SOURCE_FILES model/energy-saving.cc
                 helper/energy-saving-helper.cc
    HEADER_FILES model/energy-saving.h
                 helper/energy-saving-helper.h
    LIBRARIES_TO_LINK ${libcore}
    TEST_SOURCES test/energy-saving-test-suite.cc
                 ${examples_as_tests_sources}
)

# # set(source_files
# #     # helper/cc-helper.cc
# #     # model/a2-a4-rsrq-handover-algorithm.cc
# # )

# set(header_files
#     # helper/cc-helper.h
#     # model/a2-a4-rsrq-handover-algorithm.h
#     model/json.hpp
# )

# # set(test_sources
# #     test/es-topology-test.cc
# # )

# build_lib(
#   LIBNAME energy-saving
#   LIBRARIES_TO_LINK
#                       ${libnetwork}
#                       ${liblte}
#                       ${libcore}
#                       ${libmobility}
#                       # ${libpoint-to-point}
#                     #   ${libinternet}
#                     #   ${libenergy}
#                     #   ${libwifi}
#   # TEST_SOURCES ${test_sources}
#   # SOURCE_FILES ${source_files}
#   HEADER_FILES ${header_files}
# )