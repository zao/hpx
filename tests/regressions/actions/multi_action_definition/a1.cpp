//  Copyright (c) 2013 Eric Schnetter
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <vector>

bool called_a1_f = false;
extern bool called_a2_f;
extern bool called_a3_f;

void a1_f()
{
    called_a1_f = true;
}
HPX_PLAIN_ACTION(a1_f, f_action);

void a1_g()
{
    std::vector<hpx::id_type> const locs = hpx::find_all_localities();
    std::size_t const nlocs = locs.size();
    hpx::wait(hpx::async(f_action(), locs[2 % nlocs]));
}
HPX_PLAIN_ACTION(a1_g, g_action);

bool a1()
{
    called_a1_f = false;
    called_a2_f = false;
    called_a3_f = false;

    std::vector<hpx::id_type> const locs = hpx::find_all_localities();
    std::size_t const nlocs = locs.size();
    hpx::wait(hpx::async(g_action(), locs[1 % nlocs]));

    return called_a1_f && !called_a2_f && !called_a3_f;
}
