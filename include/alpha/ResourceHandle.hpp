/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_RESOURCE_HANDLE_HPP
#define __ALPHA_RESOURCE_HANDLE_HPP

#include <thallium.hpp>
#include <memory>
#include <chrono>
#include <span>
#include <unordered_set>
#include <alpha/Client.hpp>
#include <alpha/Exception.hpp>
#include <alpha/Future.hpp>
#include <alpha/BulkLocation.hpp>

namespace alpha {

// TUTORIAL
// ********
//
// The ResourceHandle is the client-side object that represents a remote Resource.
// Instances of this class can be created by the Client object. This ResourceHandle
// class provides a few example functions that call RPCs on the target Resource:
// computeSum, computeSumWithTimeout, computeSums, and computeSumsFromBulk.
// See src/ResourceHandle.cpp for their implementation.

class Client;
class ResourceHandleImpl;

/**
 * @brief A ResourceHandle object is a handle for a remote resource
 * on a server. It enables invoking the resource's functionalities.
 */
class ResourceHandle {

    friend class Client;

    public:

    /**
     * @brief Constructor. The resulting ResourceHandle handle will be invalid.
     */
    ResourceHandle();

    /**
     * @brief Copy-constructor.
     */
    ResourceHandle(const ResourceHandle&);

    /**
     * @brief Move-constructor.
     */
    ResourceHandle(ResourceHandle&&);

    /**
     * @brief Copy-assignment operator.
     */
    ResourceHandle& operator=(const ResourceHandle&);

    /**
     * @brief Move-assignment operator.
     */
    ResourceHandle& operator=(ResourceHandle&&);

    /**
     * @brief Destructor.
     */
    ~ResourceHandle();

    /**
     * @brief Returns the client this resource has been opened with.
     */
    Client client() const;


    /**
     * @brief Checks if the ResourceHandle instance is valid.
     */
    operator bool() const;

    /**
     * @brief Requests the target resource to compute the sum of two numbers.
     * If result is null, it will be ignored. If req is not null, this call
     * will be non-blocking and the caller is responsible for waiting on
     * the request.
     *
     * @param[in] x first integer
     * @param[in] y second integer
     *
     * @return a Future<int32_t> that can be awaited to get the result.
     */
    Future<int32_t> computeSum(int32_t x, int32_t y) const;

    /**
     * @brief Same as computeSum but allows specifying a timeout after which
     * the operation is considered to have failed.
     *
     * @param x first integer
     * @param y second integer
     * @param timeout Timeout (in milliseconds)
     *
     * @return a Future<int32_t> that can be awaited to get the result.
     */
    Future<int32_t> computeSumWithTimeout(int32_t x, int32_t y, std::chrono::milliseconds timeout) const;

    /**
     * @brief Computes the sums of two numbers in the x and y spans.
     * When the future completes, the results will be in the result span.
     * The three spans must have the same size.
     *
     * @param x X values
     * @param y Y values
     * @param result Result values
     *
     * @return a Future<void> that can be awaited.
     */
    Future<void> computeSums(std::span<const int32_t> x, std::span<const int32_t> y,
                             std::span<int32_t> result) const;

    /**
     * @brief Computes the sums of two numbers in the memory represented by
     * the BulkLocation instances. With this low-level function, one can
     * call computeSums, passing references to memories that belong to potentially
     * three different processes.
     *
     * @param x Bulk location of the X values
     * @param y Bulk location of the Y values
     * @param result Bulk location of the result values
     *
     * @return a Future<void> that can be awaited.
     */
    Future<void> computeSumsFromBulk(
        const BulkLocation& x,
        const BulkLocation& y,
        const BulkLocation& result) const;

    private:

    /**
     * @brief Constructor is private. Use a Client object
     * to create a ResourceHandle instance.
     *
     * @param impl Pointer to implementation.
     */
    ResourceHandle(const std::shared_ptr<ResourceHandleImpl>& impl);

    std::shared_ptr<ResourceHandleImpl> self;
};

}

#endif
