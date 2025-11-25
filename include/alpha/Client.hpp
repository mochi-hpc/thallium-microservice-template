/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_CLIENT_HPP
#define __ALPHA_CLIENT_HPP

#include <alpha/ResourceHandle.hpp>
#include <thallium.hpp>
#include <memory>

namespace alpha {

class ClientImpl;
class ResourceHandle;

// TUTORIAL
// ********
//
// The client library is composed of the Client and ResourceHandle classes.
// It uses the Pimpl idiom to better hide implementation details, so the
// inner state of these classes are held in ClientImpl and ResourceHandleImpl
// respectively, which are not exposed to users.
//
// The Client is an object that registers RPCs with the Margo instance or
// Thallium engine. It can then be used create ResourceHandle instances
// using makeResourceHandle. Since ResourceHandle instances keep a shared
// pointer to the ClientImpl that created them, it is safe for the Client
// object that created them to get out of scope. It is also safe for multiple
// Client objects to be created by the program.

/**
 * @brief The Client object is the main object used to establish
 * a connection with a Alpha service.
 */
class Client {

    friend class ResourceHandle;

    public:

    /**
     * @brief Default constructor.
     */
    Client();

    /**
     * @brief Constructor using a margo instance id.
     *
     * @param mid Margo instance id.
     */
    Client(margo_instance_id mid);

    /**
     * @brief Constructor.
     *
     * @param engine Thallium engine.
     */
    Client(const thallium::engine& engine);

    /**
     * @brief Copy constructor.
     */
    Client(const Client&);

    /**
     * @brief Move constructor.
     */
    Client(Client&&);

    /**
     * @brief Copy-assignment operator.
     */
    Client& operator=(const Client&);

    /**
     * @brief Move-assignment operator.
     */
    Client& operator=(Client&&);

    /**
     * @brief Destructor.
     */
    ~Client();

    /**
     * @brief Returns the thallium engine used by the client.
     */
    const thallium::engine& engine() const;

    /**
     * @brief Creates a handle to a remote resource and returns.
     * You may set "check" to false if you know for sure that the
     * corresponding resource exists, which will avoid one RPC.
     *
     * @param address Address of the provider holding the database.
     * @param provider_id Provider id.
     * @param check Checks if the Resource exists by issuing an RPC.
     *
     * @return a ResourceHandle instance.
     */
    ResourceHandle makeResourceHandle(const std::string& address,
                                      uint16_t provider_id,
                                      bool check = true) const;

    /**
     * @brief Checks that the Client instance is valid.
     */
    operator bool() const;

    /**
     * @brief Get internal configuration as a JSON-formatted string.
     *
     * @return configuration string.
     */
    std::string getConfig() const;

    private:

    Client(const std::shared_ptr<ClientImpl>& impl);

    std::shared_ptr<ClientImpl> self;
};

}

#endif
