/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_BULK_LOCATION_HPP
#define __ALPHA_BULK_LOCATION_HPP

#include <thallium.hpp>

namespace alpha {

// TUTORIAL
// ********
//
// It is often useful to encapsulate information about some process' memory region
// when sending to a provider. The BulkLocation structure encapsulates a bulk handle
// with the owner's address, as well as where in the bulk handle (offset, size) the
// server should pull from or push into.
//
// The serialize function allows BulkLocation instances to be passed as arguments
// to RPCs.

/**
 * @brief Structure encapsulating a bulk handle and the
 * address of the process where the memory is located,
 * as well as the offset and size in the bulk that are
 * relevant for the operation.
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
