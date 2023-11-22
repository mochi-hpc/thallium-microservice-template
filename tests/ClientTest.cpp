/*
 * (C) 2020 The University of Chicago
 *
 * See COPYRIGHT in top-level directory.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>
#include "Ensure.hpp"
#include <alpha/Client.hpp>
#include <alpha/Provider.hpp>
#include <alpha/ResourceHandle.hpp>

TEST_CASE("Client test", "[client]") {

    auto engine = thallium::engine("na+sm", THALLIUM_SERVER_MODE);
    ENSURE(engine.finalize());
    // Initialize the provider
    const auto provider_config = R"(
    {
        "resource": {
            "type": "dummy",
            "config": {}
        }
    }
    )";

    alpha::Provider provider(engine, 42, provider_config);

    SECTION("Open resource") {

        alpha::Client client(engine);
        std::string addr = engine.self();

        alpha::ResourceHandle my_resource = client.makeResourceHandle(addr, 42);
        REQUIRE(static_cast<bool>(my_resource));

        REQUIRE_THROWS_AS(client.makeResourceHandle(addr, 55), alpha::Exception);
        REQUIRE_NOTHROW(client.makeResourceHandle(addr, 55, false));
    }
}
