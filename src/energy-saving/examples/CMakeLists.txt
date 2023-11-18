build_lib_example(
    NAME energy-saving-example
    SOURCE_FILES energy-saving-example.cc
    LIBRARIES_TO_LINK ${libenergy-saving}
)

build_lib_example(
  NAME es-topology
  SOURCE_FILES es-topology.cc
  LIBRARIES_TO_LINK 
                    ${liblte}
                    ${libcore}
                    ${libnetwork}
                    ${libmobility}
)

# set(base_examples
#     es-topology
# )

# foreach(
#   example
#   ${base_examples}
# )
#   build_lib_example(
#     NAME ${example}
#     SOURCE_FILES ${example}.cc
#     LIBRARIES_TO_LINK 
#                       ${liblte}
#                       ${libcore}
#                       ${libnetwork}
#                       ${libmobility}
#                     #   ${libenergy}
#                     #   ${libwifi}
#                     #   ${libpoint-to-point}
#                     #   ${libinternet}
#                     #   ${fd-net-device} #In EMU case
#                     # ${libconfig-store}
#   )