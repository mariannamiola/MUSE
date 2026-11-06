# Include after find_package(cinolib).
set(_muse_fp_default "")
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _muse_processor)
if(_muse_processor MATCHES "^(aarch64|arm64)$")
    set(_muse_fp_default "-ffp-contract=off")
endif()
set(MUSE_FP_FLAGS "${_muse_fp_default}" CACHE STRING
    "Space-separated compiler flags for robust geometric predicates")

if(NOT MUSE_FP_FLAGS STREQUAL "")
    separate_arguments(_muse_fp_options UNIX_COMMAND "${MUSE_FP_FLAGS}")
    # Cover both cinolib header implementations and compiled predicate libraries.
    target_compile_options(cinolib INTERFACE ${_muse_fp_options})
    foreach(_muse_dependency tetgen triangle shewchuk_predicates)
        if(TARGET ${_muse_dependency})
            target_compile_options(${_muse_dependency} PRIVATE ${_muse_fp_options})
        endif()
    endforeach()
endif()
