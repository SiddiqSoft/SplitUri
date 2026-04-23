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
#include "../include/siddiqsoft/SplitUri.hpp"


TEST(helpers_splituri_wide_nojson, test_10)
{
    using namespace siddiqsoft::splituri_literals;

    auto u = L"https://www.google.com/search/?emptyFlag&Char{55}&q=siddiqsoft#v1"_Uri;

    EXPECT_EQ(L"www.google.com", u.authority.host);
    EXPECT_EQ(443, u.authority.port);
    EXPECT_EQ(L"/search/?emptyFlag&Char{55}&q=siddiqsoft#v1", u.urlPart);
    EXPECT_EQ(L"emptyFlag&Char{55}&q=siddiqsoft", u.queryPart);
    EXPECT_EQ(L"v1", u.fragment);
    EXPECT_EQ(1, u.path.size());
    EXPECT_EQ(L"search", u.path.at(0));
    EXPECT_EQ(3, u.query.size());
    EXPECT_EQ(L"siddiqsoft", u.query.at(L"q"));
    ASSERT_TRUE(u.query.contains(L"emptyFlag"));
    ASSERT_TRUE(u.query.contains(L"Char{55}"));
    EXPECT_TRUE(u.query.at(L"emptyFlag").empty());
    EXPECT_TRUE(u.query.at(L"Char{55}").empty());
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
    EXPECT_EQ(L"www.google.com:443", std::format(L"{}", u.authority));
    EXPECT_EQ(L"https://www.google.com/search/?emptyFlag&Char{55}&q=siddiqsoft#v1", std::format(L"{}", u));
}

TEST(helpers_splituri_wide_nojson, test_99a)
{
    using namespace std;

    auto endpoint {
            L"https://www.bing.com/?toWww=1&redig=https://www.bing.com/search?q=117244609&form=QBLH&sp=-1&pq=19983711434&sc=0-11&qs=n&sk=&cvid=46160ADDF1247EBA6FD76A4F6314D8B"s};
    auto uri = siddiqsoft::SplitUri(endpoint);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ(L"www.bing.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(endpoint, std::format(L"{}://{}{}", uri.scheme, uri.authority.host, uri.urlPart));
    EXPECT_EQ(0, uri.path.size());
}

// ============================================================================
// No-JSON: WHATWG conformance tests (wide, no nlohmann dependency)
// ============================================================================

TEST(helpers_splituri_wide_nojson, ws_scheme)
{
    auto uri = siddiqsoft::SplitUri(L"ws://example.com/chat");
    EXPECT_EQ(siddiqsoft::UriScheme::WebSocket, uri.scheme);
    EXPECT_EQ(L"example.com", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ(L"/chat", uri.urlPart);
}

TEST(helpers_splituri_wide_nojson, wss_scheme)
{
    auto uri = siddiqsoft::SplitUri(L"wss://example.com/chat");
    EXPECT_EQ(siddiqsoft::UriScheme::WebSocketSecure, uri.scheme);
    EXPECT_EQ(443, uri.authority.port);
}

TEST(helpers_splituri_wide_nojson, ftp_scheme)
{
    auto uri = siddiqsoft::SplitUri(L"ftp://files.example.com/pub/readme.txt");
    EXPECT_EQ(siddiqsoft::UriScheme::Ftp, uri.scheme);
    EXPECT_EQ(21, uri.authority.port);
    EXPECT_EQ(2, uri.path.size());
}

TEST(helpers_splituri_wide_nojson, host_lowercasing)
{
    auto uri = siddiqsoft::SplitUri(L"https://EXAMPLE.COM/path");
    EXPECT_EQ(L"example.com", uri.authority.host);
}

TEST(helpers_splituri_wide_nojson, password_parsing)
{
    auto uri = siddiqsoft::SplitUri(L"https://user:pass@example.com/");
    EXPECT_EQ(L"user", uri.authority.userInfo);
    EXPECT_EQ(L"pass", uri.authority.password);
    EXPECT_EQ(L"example.com", uri.authority.host);
}

TEST(helpers_splituri_wide_nojson, dot_segment_resolution)
{
    auto uri = siddiqsoft::SplitUri(L"https://example.com/a/b/../c");
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(L"a", uri.path.at(0));
    EXPECT_EQ(L"c", uri.path.at(1));
}

TEST(helpers_splituri_wide_nojson, ipv6_basic)
{
    auto uri = siddiqsoft::SplitUri(L"http://[::1]:8080/path");
    EXPECT_EQ(L"[::1]", uri.authority.host);
    EXPECT_EQ(8080, uri.authority.port);
    EXPECT_EQ(L"/path", uri.urlPart);
}

TEST(helpers_splituri_wide_nojson, default_constructor)
{
    siddiqsoft::Uri<wchar_t> uri;
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme);
    EXPECT_EQ(L"", uri.authority.host);
    EXPECT_EQ(0, uri.authority.port);
    EXPECT_TRUE(uri.path.empty());
}

TEST(helpers_splituri_wide_nojson, format_all_new_schemes)
{
    EXPECT_EQ(L"ws", std::format(L"{}", siddiqsoft::UriScheme::WebSocket));
    EXPECT_EQ(L"wss", std::format(L"{}", siddiqsoft::UriScheme::WebSocketSecure));
    EXPECT_EQ(L"ftp", std::format(L"{}", siddiqsoft::UriScheme::Ftp));
    EXPECT_EQ(L"file", std::format(L"{}", siddiqsoft::UriScheme::File));
}

TEST(helpers_splituri_wide_nojson, authority_serialization_with_password)
{
    siddiqsoft::AuthorityHttp<wchar_t> a;
    a.userInfo = L"admin";
    a.password = L"secret";
    a.host = L"example.com";
    a.port = 443;
    EXPECT_EQ(L"admin:secret@example.com:443", std::wstring(a));
    EXPECT_EQ(L"admin:secret@example.com:443", std::format(L"{}", a));
}
