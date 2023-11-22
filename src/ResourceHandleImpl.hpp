/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_RESOURCE_HANDLE_IMPL_H
#define __ALPHA_RESOURCE_HANDLE_IMPL_H

#include "ClientImpl.hpp"

namespace alpha {

class ResourceHandleImpl {

    public:

    std::shared_ptr<ClientImpl> m_client;
    tl::provider_handle         m_ph;

    ResourceHandleImpl() = default;

    ResourceHandleImpl(std::shared_ptr<ClientImpl> client,
                       tl::provider_handle&& ph)
    : m_client(std::move(client))
    , m_ph(std::move(ph)) {}
};

}

#endif
