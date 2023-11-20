check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/mmwave-energy-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME mmwave-energy
    SOURCE_FILES 
                model/mmwave-energy.cc
                model/mmwave-radio-energy-model-enb.cc
                # helper/mmwave-energy-helper.cc
                # helper/mmwave-radio-energy-model-enb-helper.cc
    HEADER_FILES 
                model/mmwave-energy.h
                model/mmwave-radio-energy-model-enb.h
                # helper/mmwave-energy-helper.h
                # helper/mmwave-radio-energy-model-enb-helper.h
    LIBRARIES_TO_LINK 
                ${libcore}
                ${libenergy}
    TEST_SOURCES 
                test/mmwave-energy-test-suite.cc
                ${examples_as_tests_sources}
)

