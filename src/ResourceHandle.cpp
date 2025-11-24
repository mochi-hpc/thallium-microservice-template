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

Future<int32_t> ResourceHandle::computeSumWithTimeout(
        int32_t x, int32_t y, std::chrono::milliseconds timeout) const
{
    if(not self) throw Exception("Invalid alpha::ResourceHandle object");
    auto& rpc = self->m_client->m_compute_sum;
    auto& ph  = self->m_ph;
    auto async_response = rpc.on(ph).timed_async(timeout, x, y);
    return Future<int32_t>{std::move(async_response)};
}

Future<bool> ResourceHandle::computeSums(
    std::span<const int32_t> x, std::span<const int32_t> y,
    std::span<int32_t> result) const
{
    if(not self) throw Exception("Invalid alpha::ResourceHandle object");
    if(x.size() != y.size() || y.size() != result.size())
        throw Exception("span arguments must have the same size");
    auto n = x.size();
    auto& engine = self->m_client->m_engine;
    auto input_bulk = n == 0 ? thallium::bulk{} :
        engine.expose({{(void*)(x.data()), n*sizeof(int32_t)},
                       {(void*)(y.data()), n*sizeof(int32_t)}},
                      thallium::bulk_mode::read_only);
    auto engine_address = static_cast<std::string>(engine.self());
    auto x_bulk_location = BulkLocation{
        input_bulk,
        engine_address,
        0, sizeof(int32_t)*n
    };
    auto y_bulk_location = BulkLocation{
        input_bulk,
        engine_address,
        sizeof(int32_t)*n, sizeof(int32_t)*x.size()
    };
    auto result_bulk_location = BulkLocation{
        engine.expose({{(void*)(result.data()), n*sizeof(int32_t)}},
                      thallium::bulk_mode::write_only),
        engine_address,
        0, sizeof(int32_t)*n
    };
    return computeSumsFromBulk(x_bulk_location, y_bulk_location, result_bulk_location);
}

Future<bool> ResourceHandle::computeSumsFromBulk(
          const BulkLocation& x,
          const BulkLocation& y,
          const BulkLocation& result) const
{
    if(not self) throw Exception("Invalid alpha::ResourceHandle object");
    auto& rpc = self->m_client->m_compute_sum_bulk;
    auto& ph  = self->m_ph;
    auto async_response = rpc.on(ph).async(x, y, result);
    return Future<bool>{std::move(async_response)};
}

}
