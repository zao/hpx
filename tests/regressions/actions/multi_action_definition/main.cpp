//  Copyright (c) 2013 Eric Schnetter
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <hpx/hpx_main.hpp>
#include <hpx/util/lightweight_test.hpp>

bool a1();
bool a2();
bool a3();

int main()
{
    HPX_TEST(a1());
    HPX_TEST(a2());
    HPX_TEST(a3());

    return hpx::util::report_errors();
}

