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
    struct static_partitioner
    {
        static_partitioner(std::size_t num_partitions = 2,
                std::size_t min_partition_size = 1)
          : num_partitions_(num_partitions),
            min_partition_size_(min_partition_size)
        {
            BOOST_ASSERT(num_partitions_ != 0);
            BOOST_ASSERT(min_partition_size_ != 0);
        }

        template <typename Iterator>
        std::vector<std::pair<Iterator, Iterator> >
        operator()(Iterator begin, Iterator end) const
        {
            std::vector<std::pair<Iterator, Iterator> > result;

            std::size_t num_localities = std::distance(begin, end);
            std::size_t partition_size = get_partition_size(num_localities);

            for (Iterator it = begin; it != end; /**/)
            {
                std::size_t num_entries_left = std::distance(it, end);
                std::size_t next_size = (std::min)(num_entries_left, partition_size);

                Iterator partition_end = it + next_size;
                result.push_back(std::make_pair(it, partition_end));

                it = partition_end;
            }

            return boost::move(result);
        }

        std::size_t get_partition_size(std::size_t range_size) const
        {
            std::size_t max_num_partitions = range_size / min_partition_size_;
            std::size_t partition_size =
                range_size / (std::min)(num_partitions_, max_num_partitions);
            return partition_size;
        }

        std::size_t get_num_partitions(std::size_t range_size) const
        {
            std::size_t partition_size = get_partition_size(range_size);
            return range_size / partition_size + 1;
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
