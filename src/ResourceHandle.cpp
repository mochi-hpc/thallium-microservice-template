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
    // TUTORIAL
    // ********
    //
    // This version of computeSum calls into the "async" method of the RPC.
    // The returned async_response is then moved into a Future for the caller
    // to wait on.
    if(not self) throw Exception("Invalid alpha::ResourceHandle object");
    auto& rpc = self->m_client->m_compute_sum;
    auto& ph  = self->m_ph;
    auto async_response = rpc.on(ph).async(x, y);
    return Future<int32_t>{std::move(async_response)};
}

Future<int32_t> ResourceHandle::computeSumWithTimeout(
        int32_t x, int32_t y, std::chrono::milliseconds timeout) const
{
    // TUTORIAL
    // ********
    //
    // To add a timeout, simply use timed_async instead of async.
    // timed_async expects an std::chrono::duration as first argument.
    // If it times out, an exception will be raised.
    if(not self) throw Exception("Invalid alpha::ResourceHandle object");
    auto& rpc = self->m_client->m_compute_sum;
    auto& ph  = self->m_ph;
    auto async_response = rpc.on(ph).timed_async(timeout, x, y);
    return Future<int32_t>{std::move(async_response)};
}

Future<void> ResourceHandle::computeSums(
    std::span<const int32_t> x, std::span<const int32_t> y,
    std::span<int32_t> result) const
{
    // TUTORIAL
    // ********
    //
    // This version of compueSums will compute pair-wise sums of numbers
    // from the x and y spans. It relies on RDMA to transfer these (potentially
    // large) memory regions. The engine.expose function takes a vector or
    // pairs (pointer, size) of the memory to expose for RDMA, as well as either
    // thallium::bulk_mode::read_only, thallium::bulk_mode::write_only, or
    // thallium::bulk_mode::readwrite, representing the type of access we expect
    // the server to perform. In our case, x and y are read only from the server's
    // point of view, while result is write only.
    //
    // We rely on BulkLocation to transfer the information about each bulk handle.
    // Note how we exposed both x and y in a single expose call, then created
    // two BulkLocation objects with different offset values to distinguish them.
    // We could have also used a single expose call to expose x, y, and result, using
    // readwrite as access mode, or used three expose calls.
    //
    // The BulkLocation instances are then passed to computeSumsFromBulk.
    // Note that the content of the spans must remain valid until the returned future
    // completes, since the server will perform RDMA operations on them.

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

Future<void> ResourceHandle::computeSumsFromBulk(
          const BulkLocation& x,
          const BulkLocation& y,
          const BulkLocation& result) const
{
    // TUTORIAL
    // ********
    //
    // This function simply invokes the compute_sum_bulk RPC in a non-blocking way,
    // passing the x, y, and result BulkLocation objects. These objects have a serialize
    // method so they can be passed as RPC arguments.
    //
    // Note that this method could be used to forward bulk handles that don't belong
    // to the calling process. Note also that the data referenced to by the BulkLocation
    // instances is not serialized. It will be transferred via RDMA by the server.

    if(not self) throw Exception("Invalid alpha::ResourceHandle object");
    auto& rpc = self->m_client->m_compute_sum_bulk;
    auto& ph  = self->m_ph;
    auto async_response = rpc.on(ph).async(x, y, result);
    return Future<void>{std::move(async_response)};
}

}
