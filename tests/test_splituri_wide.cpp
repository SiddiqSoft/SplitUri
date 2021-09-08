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
#include "../src/SplitUri.hpp"


TEST(helpers_splituri_wide, test_1a)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri(L"http://search.msn.com:8080/results.asp?RS=CHECKED&FORM=MSNH&v=1&q=wininet"s);
    EXPECT_EQ(L"search.msn.com", uri.authority.host);
    EXPECT_EQ(8080, uri.authority.port);
    EXPECT_EQ(L"/results.asp?RS=CHECKED&FORM=MSNH&v=1&q=wininet", uri.urlPart);

    std::wcerr << L"Re-serialized: " << uri.string() << std::endl;

    nlohmann::json doc = uri; // always utf8
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("results.asp", doc.value("/path/0"_json_pointer, ""));
}


TEST(helpers_splituri_wide, test_1b)
{
    using namespace siddiqsoft::literals;

    auto uri = L"https://YOURDBNAME.documents.azure.com:443/"_Uri;
    EXPECT_EQ(L"YOURDBNAME.documents.azure.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(L"/", uri.urlPart);
    EXPECT_EQ(L"", uri.queryPart);
    EXPECT_EQ(L"https://YOURDBNAME.documents.azure.com:443/", std::wstring(uri));
}


TEST(helpers_splituri_wide, test_1c)
{
    using namespace siddiqsoft::literals;

    auto uri = L"https://YOURDBNAME.documents.azure.com:1443/"_Uri;
    EXPECT_EQ(L"YOURDBNAME.documents.azure.com", uri.authority.host);
    EXPECT_EQ(1443, uri.authority.port);
    EXPECT_EQ(L"/", uri.urlPart);
    EXPECT_EQ(L"", uri.queryPart);
    EXPECT_EQ(L"https://YOURDBNAME.documents.azure.com:1443/", std::wstring(uri));
}


TEST(helpers_splituri_wide, test_2)
{
    auto uri = siddiqsoft::SplitUri(L"http://search.msn.com:8080");
    EXPECT_EQ(L"search.msn.com", uri.authority.host);
    EXPECT_EQ(8080, uri.authority.port);
    EXPECT_EQ(L"", uri.urlPart);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}

TEST(helpers_splituri_wide, test_3a)
{
    using namespace siddiqsoft::literals;

    auto uri = L"http://search.msn.com"_Uri;
    EXPECT_EQ(L"search.msn.com", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ(L"", uri.urlPart);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}


TEST(helpers_splituri_wide, test_3b)
{
    auto uri = siddiqsoft::SplitUri(L"http://search.msn.com:65536/");
    EXPECT_EQ(L"search.msn.com", uri.authority.host);
    EXPECT_EQ(0, uri.authority.port); // max uint16_t is 65535 so anything more will roll it over to 0
    EXPECT_EQ(L"/", uri.urlPart);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}


TEST(helpers_splituri_wide, test_4a)
{
    auto uri = siddiqsoft::SplitUri(L"http://m.co");
    EXPECT_EQ(L"m.co", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ(L"", uri.urlPart);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}


TEST(helpers_splituri_wide, test_4b)
{
    auto uri = siddiqsoft::SplitUri(L"https://m.co");
    EXPECT_EQ(L"m.co", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(L"", uri.urlPart);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}


TEST(helpers_splituri_wide, test_4c)
{
    auto uri = siddiqsoft::SplitUri(L"http://localhost");
    EXPECT_EQ(L"localhost", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ(L"", uri.urlPart);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}


TEST(helpers_splituri_wide, test_5a)
{
    auto uri = siddiqsoft::SplitUri(L"http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/"
                                               "model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5");
    EXPECT_EQ(L"<ServerName>", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ(L"/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5",
              uri.urlPart);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}


TEST(helpers_splituri_wide, test_5b)
{
    auto uri = siddiqsoft::SplitUri(L"http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/"
                                               "model/Charts('Chart%201')/?Ranges('Sheet1!A1')=5.5");
    EXPECT_EQ(L"<ServerName>", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ(L"/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')/?Ranges('Sheet1!A1')=5.5",
              uri.urlPart);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri_wide, test_6a)
{
    auto uri = siddiqsoft::SplitUri(
            L"https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top");
    EXPECT_EQ(L"www.example.com", uri.authority.host);
    EXPECT_EQ(L"john.doe", uri.authority.userInfo);
    EXPECT_EQ(123, uri.authority.port);
    EXPECT_EQ(L"/forum/questions/?tag=networking&order=newest#top", uri.urlPart);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ(L"top", uri.fragment);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}


TEST(helpers_splituri_wide, test_6b)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri(
            L"https://john.doe@www.example.com:123/forum/questions?tag=networking&order=newest#top"s);
    EXPECT_EQ(L"www.example.com", uri.authority.host);
    EXPECT_EQ(L"john.doe", uri.authority.userInfo);
    EXPECT_EQ(123, uri.authority.port);
    EXPECT_EQ(L"/forum/questions?tag=networking&order=newest#top", uri.urlPart);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ(L"top", uri.fragment);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("questions", doc.value("/path/1"_json_pointer, ""));
}


TEST(helpers_splituri_wide, test_6c)
{
    using namespace std;

    auto uri =
            siddiqsoft::SplitUri(L"https://john.doe@www.example.com/forum/questions?tag=networking&order=newest#top"s);
    EXPECT_EQ(L"www.example.com", uri.authority.host);
    EXPECT_EQ(L"john.doe", uri.authority.userInfo);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(L"/forum/questions?tag=networking&order=newest#top", uri.urlPart);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ(L"top", uri.fragment);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("questions", doc.value("/path/1"_json_pointer, ""));
}


TEST(helpers_splituri_wide, test7a)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri(L"https://a00s-engr.servicebus.windows.net:9098"s);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ(L"a00s-engr.servicebus.windows.net", uri.authority.host);
    EXPECT_EQ(9098, uri.authority.port);

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
}


TEST(helpers_splituri_wide, test7b)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri(L"https://www.yahoo.com:9098/path?x=u&n=0"s);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ(L"www.yahoo.com", uri.authority.host);
    EXPECT_EQ(9098, uri.authority.port);
    EXPECT_EQ(L"/path?x=u&n=0", uri.urlPart);
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ(L"path", uri.path.at(0));

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
}


TEST(helpers_splituri_wide, test8a)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri(
            L"https://www.bing.com/search?q=siddiqsoft&go=Search&qs=n&form=QBRE&sp=-1&pq=siddiqsoft&sc=8-10&sk=&cvid=90463834E5F74231B327D1158C16C5EE"s);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ(L"www.bing.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(
            L"/search?q=siddiqsoft&go=Search&qs=n&form=QBRE&sp=-1&pq=siddiqsoft&sc=8-10&sk=&cvid=90463834E5F74231B327D1158C16C5EE",
            uri.urlPart);
    EXPECT_EQ(9, uri.query.size());
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ(L"search", uri.path.at(0));

    EXPECT_EQ(L"siddiqsoft", uri.query.at(L"q"));
    EXPECT_EQ(L"Search", uri.query.at(L"go"));
    EXPECT_EQ(L"n", uri.query.at(L"qs"));
    EXPECT_EQ(L"QBRE", uri.query.at(L"form"));
    EXPECT_EQ(L"-1", uri.query.at(L"sp"));
    EXPECT_EQ(L"siddiqsoft", uri.query.at(L"pq"));
    EXPECT_EQ(L"8-10", uri.query.at(L"sc"));
    EXPECT_EQ(L"", uri.query.at(L"sk"));
    EXPECT_EQ(L"90463834E5F74231B327D1158C16C5EE", uri.query.at(L"cvid"));

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
}


TEST(helpers_splituri_wide, test8b)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri(
            L"https://www.google.com/search?q=siddiqsoft&rlz=1C5CHFA_enUS880US881&oq=siddiqsoft&aqs=chrome..69i57j69i60l4.5894j0j15&sourceid=chrome&ie=UTF-8"s);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ(L"www.google.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(L"/search?q=siddiqsoft&rlz=1C5CHFA_enUS880US881&oq=siddiqsoft&aqs=chrome..69i57j69i60l4.5894j0j15&sourceid=chrome&ie="
              "UTF-8",
              uri.urlPart);
    EXPECT_EQ(6, uri.query.size());

    EXPECT_EQ(L"q=siddiqsoft&rlz=1C5CHFA_enUS880US881&oq=siddiqsoft&aqs=chrome..69i57j69i60l4.5894j0j15&sourceid=chrome&ie=UTF-8",
              uri.queryPart);

    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ(L"search", uri.path.at(0));

    EXPECT_EQ(L"siddiqsoft", uri.query.at(L"q"));
    EXPECT_EQ(L"1C5CHFA_enUS880US881", uri.query.at(L"rlz"));
    EXPECT_EQ(L"siddiqsoft", uri.query.at(L"oq"));
    EXPECT_EQ(L"chrome..69i57j69i60l4.5894j0j15", uri.query.at(L"aqs"));
    EXPECT_EQ(L"chrome", uri.query.at(L"sourceid"));
    EXPECT_EQ(L"UTF-8", uri.query.at(L"ie"));

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
}

TEST(helpers_splituri_wide, test_9a)
{
    using namespace siddiqsoft::literals;

    auto uri = L"http://n.co:6553/"_Uri;
    EXPECT_EQ(L"n.co", uri.authority.host);
    EXPECT_EQ(6553, uri.authority.port);
    EXPECT_EQ(L"/", uri.urlPart);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}

TEST(helpers_splituri_wide, test_9b)
{
    using namespace siddiqsoft::literals;

    auto uri = L"http://n.co:6553"_Uri;
    EXPECT_EQ(L"n.co", uri.authority.host);
    EXPECT_EQ(6553, uri.authority.port);
    EXPECT_EQ(L"", uri.urlPart);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}

TEST(helpers_splituri_wide, test_9c)
{
    using namespace siddiqsoft::literals;

    auto uri = L"http://n.co:65536/"_Uri;
    EXPECT_EQ(L"n.co", uri.authority.host);
    EXPECT_EQ(0, uri.authority.port); // we're just above the max uint16_t
    EXPECT_EQ(L"/", uri.urlPart);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}

TEST(helpers_splituri_wide, test_9d)
{
    using namespace siddiqsoft::literals;

    auto uri = L"http://n.co:65535"_Uri;
    EXPECT_EQ(L"n.co", uri.authority.host);
    EXPECT_EQ(65535, uri.authority.port);
    EXPECT_EQ(L"", uri.urlPart);

    std::wcerr << L"Re-serialized: " << std::wstring(uri) << std::endl;
}

TEST(helpers_splituri_wide, test_10)
{
    using namespace siddiqsoft::literals;

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
    std::cerr << nlohmann::json(u.path).dump() << std::endl;

    EXPECT_EQ(3, u.query.size());
    EXPECT_EQ(L"siddiqsoft", u.query.at(L"q"));
    ASSERT_TRUE(u.query.contains(L"emptyFlag"));
    ASSERT_TRUE(u.query.contains(L"Char{55}"));
    EXPECT_TRUE(u.query.at(L"emptyFlag").empty());
    EXPECT_TRUE(u.query.at(L"Char{55}").empty());
    std::cerr << nlohmann::json(u.query).dump() << std::endl;

    // Checks that both serializers are available (caught at compile-time)
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
    std::wcerr << std::format(L"{}", u.scheme) << "....";
    std::cerr << nlohmann::json(u.scheme).dump() << std::endl;

    // Note that despite the initial uri string skipping the port, the SplitUri decodes and stores the port
    EXPECT_EQ(L"www.google.com:443", std::format(L"{}", u.authority));
    std::wcerr << std::format(L"{}", u.authority) << std::endl;

    // The "rebuilt" endpoint
    EXPECT_EQ(L"https://www.google.com/search/?emptyFlag&Char{55}&q=siddiqsoft#v1", std::format(L"{}", u));
    std::wcerr << std::format(L"{}", u) << std::endl;
}

TEST(helpers_splituri_wide, test_99a)
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

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
}
