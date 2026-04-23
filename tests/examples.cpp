/*
    SplitUri

    BSD 3-Clause License

    Copyright (c) 2021, Siddiq Software LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gtest/gtest.h"
#include <iostream>

#include "nlohmann/json.hpp"
#include "../include/siddiqsoft/SplitUri.hpp"


namespace siddiqsoft
{
    TEST(examples, example_1_narrow)
    {
        using namespace siddiqsoft::splituri_literals;

        auto u = "https://www.google.com/search?q=siddiqsoft&from=example_1_narrow#v1"_Uri;

        EXPECT_EQ("www.google.com", u.authority.host);
        EXPECT_EQ(443, u.authority.port);
        EXPECT_EQ("/search?q=siddiqsoft&from=example_1_narrow#v1", u.urlPart);
        EXPECT_EQ("q=siddiqsoft&from=example_1_narrow", u.queryPart);
        EXPECT_EQ("v1", u.fragment);

        EXPECT_EQ("search", u.path.at(0));
        nlohmann::json dp = u.path;
        EXPECT_EQ("search", dp[0].get<std::string>());

        EXPECT_EQ("siddiqsoft", u.query.at("q"));
        nlohmann::json dq = u.query;
        EXPECT_EQ("siddiqsoft", dq.value("q", ""));
        EXPECT_EQ("example_1_narrow", dq.value("from", ""));

        EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
        EXPECT_EQ("www.google.com:443", std::format("{}", u.authority));
        EXPECT_EQ("https://www.google.com/search?q=siddiqsoft&from=example_1_narrow#v1", std::format("{}", u));
    }


    TEST(examples, example_1_wide)
    {
        using namespace siddiqsoft::splituri_literals;

        auto u = L"https://www.google.com/search?q=siddiqsoft&from=example_1_wide#v1"_Uri;

        EXPECT_EQ(L"www.google.com", u.authority.host);
        EXPECT_EQ(443, u.authority.port);
        EXPECT_EQ(L"/search?q=siddiqsoft&from=example_1_wide#v1", u.urlPart);
        EXPECT_EQ(L"q=siddiqsoft&from=example_1_wide", u.queryPart);
        EXPECT_EQ(L"v1", u.fragment);

        EXPECT_EQ(L"search", u.path.at(0));
        nlohmann::json dp;
        to_json(dp, u.path);
        EXPECT_EQ("search", dp[0].get<std::string>());

        EXPECT_EQ(L"siddiqsoft", u.query.at(L"q"));
        nlohmann::json dq;
        to_json(dq, u.query);
        EXPECT_EQ("siddiqsoft", dq.value("q", ""));
        EXPECT_EQ("example_1_wide", dq.value("from", ""));

        EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
        EXPECT_EQ(L"www.google.com:443", std::format(L"{}", u.authority));
        EXPECT_EQ(L"https://www.google.com/search?q=siddiqsoft&from=example_1_wide#v1", std::format(L"{}", u));
    }


    TEST(examples, example_websocket)
    {
        // WHATWG: WebSocket scheme support
        auto uri = siddiqsoft::SplitUri("ws://chat.example.com:9090/room?id=42#messages");

        EXPECT_EQ(siddiqsoft::UriScheme::WebSocket, uri.scheme);
        EXPECT_EQ("chat.example.com", uri.authority.host);
        EXPECT_EQ(9090, uri.authority.port);
        EXPECT_EQ("room", uri.path.at(0));
        EXPECT_EQ("42", uri.query.at("id"));
        EXPECT_EQ("messages", uri.fragment);

        // Verify it's a special scheme
        EXPECT_TRUE(siddiqsoft::isSpecialScheme(uri.scheme));
        EXPECT_EQ(80, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::WebSocket));
    }


    TEST(examples, example_ftp_with_credentials)
    {
        // WHATWG: FTP with username:password
        auto uri = siddiqsoft::SplitUri("ftp://admin:secret@files.example.com:2121/pub/readme.txt");

        EXPECT_EQ(siddiqsoft::UriScheme::Ftp, uri.scheme);
        EXPECT_EQ("admin", uri.authority.userInfo);
        EXPECT_EQ("secret", uri.authority.password);
        EXPECT_EQ("files.example.com", uri.authority.host);
        EXPECT_EQ(2121, uri.authority.port);
        EXPECT_EQ(2, uri.path.size());
        EXPECT_EQ("pub", uri.path.at(0));
        EXPECT_EQ("readme.txt", uri.path.at(1));

        // Authority serialization includes credentials
        EXPECT_EQ("admin:secret@files.example.com:2121", std::format("{}", uri.authority));
    }


    TEST(examples, example_dot_segment_resolution)
    {
        // WHATWG: Path dot-segment resolution
        auto uri = siddiqsoft::SplitUri("https://example.com/a/b/c/../../d/./e");

        EXPECT_EQ("example.com", uri.authority.host);
        // a, b, c, .., .., d, ., e -> a, d, e
        EXPECT_EQ(3, uri.path.size());
        EXPECT_EQ("a", uri.path.at(0));
        EXPECT_EQ("d", uri.path.at(1));
        EXPECT_EQ("e", uri.path.at(2));
    }


    TEST(examples, example_case_insensitive_scheme)
    {
        // WHATWG: Schemes are case-insensitive
        auto uri = siddiqsoft::SplitUri("HTTPS://EXAMPLE.COM/Path");

        EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
        EXPECT_EQ("example.com", uri.authority.host); // host lowercased
        EXPECT_EQ(443, uri.authority.port);
        EXPECT_EQ(1, uri.path.size());
        EXPECT_EQ("Path", uri.path.at(0)); // path segments are NOT lowercased
    }

} // namespace siddiqsoft
