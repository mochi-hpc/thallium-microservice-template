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

TEST_CASE("Resource test", "[resource]") {
    auto engine = thallium::engine("na+sm", THALLIUM_SERVER_MODE);
    ENSURE(engine.finalize());
    const auto provider_config = R"(
    {
        "resource": {
            "type": "dummy",
            "config": {}
        }
    }
    )";
    alpha::Provider provider(engine, 42, provider_config);

    SECTION("Create ResourceHandle") {
        alpha::Client client(engine);
        std::string addr = engine.self();

        auto rh = client.makeResourceHandle(addr, 42);

        SECTION("Send Sum RPC") {
            int32_t result;
            REQUIRE_NOTHROW([&]() { result = rh.computeSum(42, 51).wait(); }());
            REQUIRE(result == 93);
        }

        SECTION("Send Sum RPC with timeout") {
            int32_t result;
            REQUIRE_NOTHROW([&]() {
                result = rh.computeSumWithTimeout(42, 51, std::chrono::milliseconds{500}).wait(); }()
            );
            REQUIRE(result == 93);
        }

        SECTION("Send Sum RPC for spans") {
            std::vector<int32_t> x{1,2,3};
            std::vector<int32_t> y{4,5,6};
            std::vector<int32_t> r(3);

            REQUIRE_NOTHROW(rh.computeSums(x, y, r).wait());

            REQUIRE(r[0] == 5);
            REQUIRE(r[1] == 7);
            REQUIRE(r[2] == 9);
        }
    }
}
