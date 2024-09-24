/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include "alpha/ResourceHandle.hpp"
#include "alpha/Result.hpp"
#include "alpha/Exception.hpp"

#include "ClientImpl.hpp"
#include "ResourceHandleImpl.hpp"

#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/pair.hpp>

namespace alpha {

ResourceHandle::ResourceHandle() = default;

ResourceHandle::ResourceHandle(const std::shared_ptr<ResourceHandleImpl>& impl)
: self(impl) {}

ResourceHandle::ResourceHandle(const ResourceHandle&) = default;

ResourceHandle::ResourceHandle(ResourceHandle&&) = default;

ResourceHandle& ResourceHandle::operator=(const ResourceHandle&) = default;

ResourceHandle& ResourceHandle::operator=(ResourceHandle&&) = default;

ResourceHandle::~ResourceHandle() = default;

ResourceHandle::operator bool() const {
    return static_cast<bool>(self);
}

Client ResourceHandle::client() const {
    return Client(self->m_client);
}

Future<int32_t> ResourceHandle::computeSum(
        int32_t x, int32_t y) const
{
    if(not self) throw Exception("Invalid alpha::ResourceHandle object");
    auto& rpc = self->m_client->m_compute_sum;
    auto& ph  = self->m_ph;
    auto async_response = rpc.on(ph).async(x, y);
    return Future<int32_t>{std::move(async_response)};
}

}
