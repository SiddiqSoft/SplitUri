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
    std::wcerr << u.authority.host << std::endl;

    EXPECT_EQ(443, u.authority.port);
    std::wcerr << u.authority.port << std::endl;

    EXPECT_EQ(L"/search/?emptyFlag&Char{55}&q=siddiqsoft#v1", u.urlPart);
    std::wcerr << u.urlPart << std::endl;

    EXPECT_EQ(L"emptyFlag&Char{55}&q=siddiqsoft", u.queryPart);
    std::wcerr << u.queryPart << std::endl;

    EXPECT_EQ(L"v1", u.fragment);
    std::wcerr << u.fragment << std::endl;

    EXPECT_EQ(1, u.path.size());
    EXPECT_EQ(L"search", u.path.at(0));

    EXPECT_EQ(3, u.query.size());
    EXPECT_EQ(L"siddiqsoft", u.query.at(L"q"));
    ASSERT_TRUE(u.query.contains(L"emptyFlag"));
    ASSERT_TRUE(u.query.contains(L"Char{55}"));
    EXPECT_TRUE(u.query.at(L"emptyFlag").empty());
    EXPECT_TRUE(u.query.at(L"Char{55}").empty());

    // Checks that both serializers are available (caught at compile-time)
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
    std::wcerr << std::format(L"{}", u.scheme) << "....";

    // Note that despite the initial uri string skipping the port, the SplitUri decodes and stores the port
    EXPECT_EQ(L"www.google.com:443", std::format(L"{}", u.authority));
    std::wcerr << std::format(L"{}", u.authority) << std::endl;

    // The "rebuilt" endpoint
    EXPECT_EQ(L"https://www.google.com/search/?emptyFlag&Char{55}&q=siddiqsoft#v1", std::format(L"{}", u));
    std::wcerr << std::format(L"{}", u) << std::endl;
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
    EXPECT_EQ(L"/?toWww=1&redig=https://www.bing.com/"
              "search?q=117244609&form=QBLH&sp=-1&pq=19983711434&sc=0-11&qs=n&sk=&cvid=46160ADDF1247EBA6FD76A4F6314D8B",
              uri.urlPart);
    EXPECT_EQ(L"toWww=1&redig=https://www.bing.com/"
              "search?q=117244609&form=QBLH&sp=-1&pq=19983711434&sc=0-11&qs=n&sk=&cvid=46160ADDF1247EBA6FD76A4F6314D8B",
              uri.queryPart);
    // We should match the initial endpoint--ensures we did not drop anything.
    EXPECT_EQ(endpoint, std::format(L"{}://{}{}", uri.scheme, uri.authority.host, uri.urlPart));
    EXPECT_EQ(0, uri.path.size());
}
