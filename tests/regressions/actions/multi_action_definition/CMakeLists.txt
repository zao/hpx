# Copyright (c) 2007-2013 Hartmut Kaiser
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(test multi_action_definition)
set(sources main.cpp a1.cpp a2.cpp a3.cpp)

###############################################################################
source_group("Source Files" FILES ${sources})

# add example executable
add_hpx_executable(${test}_test
                   SOURCES ${sources}
                   ${${test}_FLAGS}
                   FOLDER "Tests/Regressions/Actions")

add_hpx_regression_test("agas" ${test} ${${test}_PARAMETERS})

add_hpx_pseudo_target(tests.regressions.actions.${test})
add_hpx_pseudo_dependencies(tests.regressions.actions tests.regressions.actions.${test})
add_hpx_pseudo_dependencies(tests.regressions.actions.${test} ${test}_test_exe)

