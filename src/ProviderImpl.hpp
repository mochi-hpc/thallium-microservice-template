/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#ifndef __ALPHA_PROVIDER_IMPL_H
#define __ALPHA_PROVIDER_IMPL_H

#include "alpha/ResourceInterface.hpp"

#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>
#include <thallium/serialization/stl/vector.hpp>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <tuple>

namespace alpha {

using namespace std::string_literals;
namespace tl = thallium;

class ProviderImpl : public tl::provider<ProviderImpl> {

    auto id() const { return get_provider_id(); } // for convenience

    using json = nlohmann::json;

    #define DEF_LOGGING_FUNCTION(__name__)                         \
    template<typename ... Args>                                    \
    void __name__(Args&&... args) {                                \
        auto msg = fmt::format(std::forward<Args>(args)...);       \
        spdlog::__name__("[alpha:{}] {}", get_provider_id(), msg); \
    }

    DEF_LOGGING_FUNCTION(trace)
    DEF_LOGGING_FUNCTION(debug)
    DEF_LOGGING_FUNCTION(info)
    DEF_LOGGING_FUNCTION(warn)
    DEF_LOGGING_FUNCTION(error)
    DEF_LOGGING_FUNCTION(critical)

    #undef DEF_LOGGING_FUNCTION

    public:

    tl::engine           m_engine;
    tl::pool             m_pool;
    // Client RPC
    tl::auto_remote_procedure m_compute_sum;
    // FIXME: other RPCs go here ...
    // ResourceInterfaces
    std::shared_ptr<ResourceInterface> m_backend;

    ProviderImpl(const tl::engine& engine, uint16_t provider_id, const std::string& config, const tl::pool& pool)
    : tl::provider<ProviderImpl>(engine, provider_id, "alpha")
    , m_engine(engine)
    , m_pool(pool)
    , m_compute_sum(define("alpha_compute_sum",  &ProviderImpl::computeSumRPC, pool))
    {
        trace("Registered provider with id {}", get_provider_id());
        json json_config;
        try {
            json_config = json::parse(config);
        } catch(json::parse_error& e) {
            error("Could not parse provider configuration: {}", e.what());
            return;
        }
        if(!json_config.is_object()) return;
        if(!json_config.contains("resource")) return;
        auto& resource = json_config["resource"];
        if(!resource.is_object()) return;
        if(resource.contains("type") && resource["type"].is_string()) {
            auto& resource_type = resource["type"].get_ref<const std::string&>();
            auto resource_config = resource.contains("config") ? resource["config"] : json::object();
            auto result = createResource(resource_type, resource_config);
            result.check();
        }
    }

    ~ProviderImpl() {
        trace("Deregistering provider");
        if(m_backend) {
            m_backend->destroy();
        }
    }

    std::string getConfig() const {
        auto config = json::object();
        if(m_backend) {
            config["resource"] = json::object();
            auto resource_config = json::object();
            resource_config["type"] = m_backend->name();
            resource_config["config"] = json::parse(m_backend->getConfig());
            config["resource"] = std::move(resource_config);
        }
        return config.dump();
    }

    Result<bool> createResource(const std::string& resource_type,
                                const json& resource_config) {

        Result<bool> result;

        try {
            m_backend = ResourceFactory::createResource(resource_type, get_engine(), resource_config);
        } catch(const std::exception& ex) {
            result.success() = false;
            result.error() = ex.what();
            error("Error when creating resource of type {}: {}",
                  resource_type, result.error());
            return result;
        }

        if(not m_backend) {
            result.success() = false;
            result.error() = "Unknown resource type "s + resource_type;
            error("Unknown resource type {}", resource_type);
            return result;
        }

        trace("Successfully created resource of type {}", resource_type);
        return result;
    }

    void computeSumRPC(const tl::request& req,
                       int32_t x, int32_t y) {
        trace("Received computeSum request");
        Result<int32_t> result;
        tl::auto_respond<decltype(result)> response{req, result};
        if(!m_backend) {
            result.success() = false;
            result.error() = "Provider has no resource attached";
        } else {
            result = m_backend->computeSum(x, y);
        }
        trace("Successfully executed computeSum");
    }

};

}

#endif
