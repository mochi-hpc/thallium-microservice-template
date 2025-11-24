/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_CLIENT_IMPL_H
#define __ALPHA_CLIENT_IMPL_H

#include <thallium.hpp>
#include <thallium/serialization/stl/unordered_set.hpp>
#include <thallium/serialization/stl/unordered_map.hpp>
#include <thallium/serialization/stl/string.hpp>

namespace alpha {

namespace tl = thallium;

class ClientImpl {

    public:

    tl::engine           m_engine;
    tl::remote_procedure m_compute_sum;
    tl::remote_procedure m_compute_sum_bulk;

    ClientImpl(const tl::engine& engine)
    : m_engine(engine)
    , m_compute_sum(m_engine.define("alpha_compute_sum"))
    , m_compute_sum_bulk(m_engine.define("alpha_compute_sum_bulk"))
    {}

    ClientImpl(margo_instance_id mid)
    : ClientImpl(tl::engine(mid)) {}

    ~ClientImpl() {}
};

}

#endif
