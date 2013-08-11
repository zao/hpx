//  Copyright (c) 2013 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_LCOS_DETAIL_STATIC_LOCALITY_PARTITIONER_AU_06_2013_1104AM)
#define HPX_LCOS_DETAIL_STATIC_LOCALITY_PARTITIONER_AU_06_2013_1104AM

#include <hpx/hpx_fwd.hpp>

#include <algorithm>
#include <vector>

#include <boost/serialization/serialization.hpp>

namespace hpx { namespace lcos
{
    struct static_locality_partitioner
    {
        static_locality_partitioner(std::size_t num_partitions = 2,
                std::size_t min_partition_size = 1)
          : num_partitions_(num_partitions),
            min_partition_size_(min_partition_size)
        {
            BOOST_ASSERT(num_partitions_ != 0);
            BOOST_ASSERT(min_partition_size_ != 0);
        }

        std::vector<std::vector<naming::id_type> >
        operator()(std::vector<naming::id_type>::const_iterator begin,
            std::vector<naming::id_type>::const_iterator end) const
        {
            std::vector<std::vector<naming::id_type> > result;

            typedef std::vector<naming::id_type>::const_iterator iterator;

            std::size_t num_localities = std::distance(begin, end);
            std::size_t max_num_partitions = num_localities / min_partition_size_;
            std::size_t partition_size =
                num_localities / (std::min)(num_partitions_, max_num_partitions);


            for (iterator it = begin; it != end; /**/)
            {
                std::size_t num_entries_left = std::distance(it, end);
                std::size_t next_size = (std::min)(num_entries_left, partition_size);

                iterator partition_end = it + next_size;
                result.push_back(std::vector<naming::id_type>(it, partition_end));
                it = partition_end;
            }

            return boost::move(result);
        }

        std::size_t num_partitions_;
        std::size_t min_partition_size_;

    private:
        friend class boost::serialization::access;

        template <class Archive>
        void save(Archive & ar, const unsigned int version) const
        {
            ar.save(num_partitions_);
            ar.save(min_partition_size_);
        }

        template <class Archive>
        void load(Archive & ar, const unsigned int version)
        {
            ar.load(num_partitions_);
            ar.load(min_partition_size_);
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };
}}

#endif
