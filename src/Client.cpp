/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include "alpha/Exception.hpp"
#include "alpha/Client.hpp"
#include "alpha/ResourceHandle.hpp"
#include "alpha/Result.hpp"

#include "ClientImpl.hpp"
#include "ResourceHandleImpl.hpp"

#include <thallium/serialization/stl/string.hpp>

namespace tl = thallium;

namespace alpha {

Client::Client() = default;

Client::Client(const tl::engine& engine)
: self(std::make_shared<ClientImpl>(engine)) {}

Client::Client(margo_instance_id mid)
: self(std::make_shared<ClientImpl>(mid)) {}

Client::Client(const std::shared_ptr<ClientImpl>& impl)
: self(impl) {}

Client::Client(Client&& other) = default;

Client& Client::operator=(Client&& other) = default;

Client::Client(const Client& other) = default;

Client& Client::operator=(const Client& other) = default;


Client::~Client() = default;

const tl::engine& Client::engine() const {
    return self->m_engine;
}

Client::operator bool() const {
    return static_cast<bool>(self);
}

ResourceHandle Client::makeResourceHandle(
        const std::string& address,
        uint16_t provider_id,
        bool check) const {
    auto endpoint  = self->m_engine.lookup(address);
    auto ph        = tl::provider_handle(endpoint, provider_id);
    if(check) {
        try {
            if(ph.get_identity() != "alpha") {
                throw Exception{"Address and provider ID do not point to a alpha provider"};
            }
        } catch(const std::exception& ex) {
            throw Exception{ex.what()};
        }
    }
    return std::make_shared<ResourceHandleImpl>(self, std::move(ph));
}

std::string Client::getConfig() const {
    return "{}";
}

}
