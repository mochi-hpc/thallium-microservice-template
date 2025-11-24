/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_BULK_LOCATION_HPP
#define __ALPHA_BULK_LOCATION_HPP

#include <thallium.hpp>

namespace alpha {

/**
 * @brief Structure encapsulating a bulk handle and the
 * address of the process where the memory is located,
 * as well as the offset and size in the bulk.
 */
struct BulkLocation {

    thallium::bulk bulk;
    std::string    address;
    size_t         offset;
    size_t         size;

    template<typename Archive>
    void serialize(Archive& ar) {
        ar(bulk);
        ar(address);
        ar(offset);
        ar(size);
    }
};

}

#endif
