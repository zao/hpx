// Copyright (c) 2017 Lars Viklund
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config.hpp>

#if defined(__FreeBSD__) || defined(__DragonFly__)

#include <hpx/exception.hpp>
#include <boost/format.hpp>

#include <cstdint>

#include <kvm.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <unistd.h>

#include <memory>

namespace hpx { namespace performance_counters { namespace memory
{
    ///////////////////////////////////////////////////////////////////////////
    // returns virtual memory value
    std::uint64_t read_psm_virtual(bool)
    {
        std::shared_ptr<kvm_t> kvm(kvm_open(nullptr, "/dev/null", nullptr, 0, nullptr), &kvm_close);
        if (!kvm)
        {
            HPX_THROW_EXCEPTION(kernel_error,
                "hpx::performance_counters::memory::read_psm_virtual",
                "kvm_open failed");
            return std::uint64_t(-1);
        }
        int proc_count = 0;
        struct kinfo_proc* pp = kvm_getprocs(kvm.get(), KERN_PROC_PID, getpid(), &proc_count);
        if (!pp)
        {
            HPX_THROW_EXCEPTION(kernel_error,
                "hpx::performance_counters::memory::read_psm_virtual",
                "kvm_getprocs failed");

            return std::uint64_t(-1);
        }

#if defined(__FreeBSD__)
        std::uint64_t virtual_size = static_cast<std::uint64_t>(pp->ki_size);
#elif defined(__DragonFly__)
        std::uint64_t virtual_size = static_cast<std::uint64_t>(pp->kp_vm_map_size);
#endif

        return virtual_size;
    }

    ///////////////////////////////////////////////////////////////////////////
    // returns resident memory value
    std::uint64_t read_psm_resident(bool)
    {
        std::shared_ptr<kvm_t> kvm(kvm_open(nullptr, "/dev/null", nullptr, 0, nullptr), &kvm_close);
        if (!kvm)
        {
            HPX_THROW_EXCEPTION(kernel_error,
                "hpx::performance_counters::memory::read_psm_resident",
                "kvm_open failed");
            return std::uint64_t(-1);
        }
        int proc_count = 0;
        struct kinfo_proc* pp = kvm_getprocs(kvm.get(), KERN_PROC_PID, getpid(), &proc_count);
        if (!pp)
        {
            HPX_THROW_EXCEPTION(kernel_error,
                "hpx::performance_counters::memory::read_psm_resident",
                "kvm_getprocs failed");

            return std::uint64_t(-1);
        }

#if defined(__FreeBSD__)
        std::uint64_t resident_size = static_cast<std::uint64_t>(pp->ki_rssize) * PAGE_SIZE;
#elif defined(__DragonFly__)
        std::uint64_t resident_size = static_cast<std::uint64_t>(pp->kp_vm_rssize) * PAGE_SIZE;
#endif

        return resident_size;
    }
}}}

#endif
