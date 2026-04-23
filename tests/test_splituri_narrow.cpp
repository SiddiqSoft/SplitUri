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


TEST(helpers_splituri_narrow, test_1a)
{
    using namespace siddiqsoft::splituri_literals;

    auto uri = "http://search.msn.com:8080/results.asp?RS=CHECKED&FORM=MSNH&v=1&q=wininet"_Uri;
    EXPECT_EQ("search.msn.com", uri.authority.host);
    EXPECT_EQ(8080, uri.authority.port);
    EXPECT_EQ("/results.asp?RS=CHECKED&FORM=MSNH&v=1&q=wininet", uri.urlPart);

    std::cerr << "Re-serialized: " << uri.string() << std::endl;

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("results.asp", doc.value("/path/0"_json_pointer, ""));
}

TEST(helpers_splituri_narrow, test_1b)
{
    using namespace siddiqsoft::splituri_literals;

    auto uri = "https://YOURDBNAME.documents.azure.com:443/"_Uri;
    // WHATWG: host is lowercased for special schemes
    EXPECT_EQ("yourdbname.documents.azure.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ("/", uri.urlPart);
    EXPECT_EQ("", uri.queryPart);
    EXPECT_EQ("https://YOURDBNAME.documents.azure.com:443/", std::string(uri));
}

TEST(helpers_splituri_narrow, test_1c)
{
    using namespace siddiqsoft::splituri_literals;

    auto uri = "https://YOURDBNAME.documents.azure.com:1443/"_Uri;
    // WHATWG: host is lowercased for special schemes
    EXPECT_EQ("yourdbname.documents.azure.com", uri.authority.host);
    EXPECT_EQ(1443, uri.authority.port);
    EXPECT_EQ("/", uri.urlPart);
    EXPECT_EQ("", uri.queryPart);
    EXPECT_EQ("https://YOURDBNAME.documents.azure.com:1443/", std::string(uri));
}

TEST(helpers_splituri_narrow, test_2)
{
    auto uri = siddiqsoft::SplitUri("http://search.msn.com:8080");
    EXPECT_EQ("search.msn.com", uri.authority.host);
    EXPECT_EQ(8080, uri.authority.port);
    EXPECT_EQ("", uri.urlPart);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}

TEST(helpers_splituri_narrow, test_3a)
{
    using namespace siddiqsoft::splituri_literals;

    auto uri = "http://search.msn.com"_Uri;
    EXPECT_EQ("search.msn.com", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ("", uri.urlPart);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri_narrow, test_3b)
{
    auto uri = siddiqsoft::SplitUri("http://search.msn.com:65536/");
    EXPECT_EQ("search.msn.com", uri.authority.host);
    EXPECT_EQ(0, uri.authority.port); // WHATWG: port-out-of-range validation error
    EXPECT_EQ("/", uri.urlPart);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri_narrow, test_4a)
{
    auto uri = siddiqsoft::SplitUri("http://m.co");
    EXPECT_EQ("m.co", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ("", uri.urlPart);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri_narrow, test_4b)
{
    auto uri = siddiqsoft::SplitUri("https://m.co");
    EXPECT_EQ("m.co", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ("", uri.urlPart);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri_narrow, test_4c)
{
    auto uri = siddiqsoft::SplitUri("http://localhost");
    EXPECT_EQ("localhost", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ("", uri.urlPart);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri_narrow, test_5a)
{
    auto uri = siddiqsoft::SplitUri("http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/"
                                    "model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5");
    // WHATWG: host is lowercased for special schemes
    EXPECT_EQ("<servername>", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ("/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5",
              uri.urlPart);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri_narrow, test_5b)
{
    auto uri = siddiqsoft::SplitUri("http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/"
                                    "model/Charts('Chart%201')/?Ranges('Sheet1!A1')=5.5");
    // WHATWG: host is lowercased for special schemes
    EXPECT_EQ("<servername>", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ("/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')/?Ranges('Sheet1!A1')=5.5",
              uri.urlPart);
    EXPECT_EQ("/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')/", uri.pathPart);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri_narrow, test_5c)
{
    auto uri = siddiqsoft::SplitUri("http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/"
                                    "model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5");
    // WHATWG: host is lowercased for special schemes
    EXPECT_EQ("<servername>", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ("/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5",
              uri.urlPart);
    EXPECT_EQ("/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')", uri.pathPart);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri_narrow, test_6a)
{
    auto uri = siddiqsoft::SplitUri("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top");
    EXPECT_EQ("www.example.com", uri.authority.host);
    EXPECT_EQ("john.doe", uri.authority.userInfo);
    EXPECT_EQ(123, uri.authority.port);
    EXPECT_EQ("/forum/questions/?tag=networking&order=newest#top", uri.urlPart);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ("top", uri.fragment);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri_narrow, test_6b)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri("https://john.doe@www.example.com:123/forum/questions?tag=networking&order=newest#top"s);
    EXPECT_EQ("www.example.com", uri.authority.host);
    EXPECT_EQ("john.doe", uri.authority.userInfo);
    EXPECT_EQ(123, uri.authority.port);
    EXPECT_EQ("/forum/questions?tag=networking&order=newest#top", uri.urlPart);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ("top", uri.fragment);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("questions", doc.value("/path/1"_json_pointer, ""));
}


TEST(helpers_splituri_narrow, test_6c)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri("https://john.doe@www.example.com/forum/questions?tag=networking&order=newest#top"s);
    EXPECT_EQ("www.example.com", uri.authority.host);
    EXPECT_EQ("john.doe", uri.authority.userInfo);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ("/forum/questions?tag=networking&order=newest#top", uri.urlPart);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ("top", uri.fragment);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
    EXPECT_EQ("questions", doc.value("/path/1"_json_pointer, ""));
}


TEST(helpers_splituri_narrow, test7a)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri("https://a00s-engr.servicebus.windows.net:9098"s);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ("a00s-engr.servicebus.windows.net", uri.authority.host);
    EXPECT_EQ(9098, uri.authority.port);

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
}

TEST(helpers_splituri_narrow, test7b)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri("https://www.yahoo.com:9098/path?x=u&n=0"s);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ("www.yahoo.com", uri.authority.host);
    EXPECT_EQ(9098, uri.authority.port);
    EXPECT_EQ("/path?x=u&n=0", uri.urlPart);
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ("path", uri.path.at(0));

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
}

TEST(helpers_splituri_narrow, test8a)
{
    using namespace std;

    auto uri = siddiqsoft::SplitUri(
            "https://www.bing.com/search?q=siddiqsoft&go=Search&qs=n&form=QBRE&sp=-1&pq=siddiqsoft&sc=8-10&sk=&cvid=90463834E5F74231B327D1158C16C5EE"s);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ("www.bing.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ("/search?q=siddiqsoft&go=Search&qs=n&form=QBRE&sp=-1&pq=siddiqsoft&sc=8-10&sk=&cvid=90463834E5F74231B327D1158C16C5EE",
              uri.urlPart);
    EXPECT_EQ(9, uri.query.size());
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ("search", uri.path.at(0));

    EXPECT_EQ("siddiqsoft", uri.query.at("q"));
    EXPECT_EQ("Search", uri.query.at("go"));
    EXPECT_EQ("n", uri.query.at("qs"));
    EXPECT_EQ("QBRE", uri.query.at("form"));
    EXPECT_EQ("-1", uri.query.at("sp"));
    EXPECT_EQ("siddiqsoft", uri.query.at("pq"));
    EXPECT_EQ("8-10", uri.query.at("sc"));
    EXPECT_EQ("", uri.query.at("sk"));
    EXPECT_EQ("90463834E5F74231B327D1158C16C5EE", uri.query.at("cvid"));

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
}

TEST(helpers_splituri_narrow, test8b)
{
    using namespace std;

    std::string endpoint {"https://www.google.com/"
                          "search?q=siddiqsoft&rlz=1C5CHFA_enUS880US881&oq=siddiqsoft&aqs=chrome..69i57j69i60l4.5894j0j15&sourceid="
                          "chrome&ie=UTF-8"};
    auto        uri = siddiqsoft::SplitUri(endpoint);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ("www.google.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ("/search?q=siddiqsoft&rlz=1C5CHFA_enUS880US881&oq=siddiqsoft&aqs=chrome..69i57j69i60l4.5894j0j15&sourceid=chrome&ie="
              "UTF-8",
              uri.urlPart);
    EXPECT_EQ(6, uri.query.size());

    EXPECT_EQ("q=siddiqsoft&rlz=1C5CHFA_enUS880US881&oq=siddiqsoft&aqs=chrome..69i57j69i60l4.5894j0j15&sourceid=chrome&ie=UTF-8",
              uri.queryPart);

    // We should match the initial endpoint--ensures we did not drop anything.
    EXPECT_EQ(endpoint, std::format("{}://{}{}", uri.scheme, uri.authority.host, uri.urlPart));

    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ("search", uri.path.at(0));

    EXPECT_EQ("siddiqsoft", uri.query.at("q"));
    EXPECT_EQ("1C5CHFA_enUS880US881", uri.query.at("rlz"));
    EXPECT_EQ("siddiqsoft", uri.query.at("oq"));
    EXPECT_EQ("chrome..69i57j69i60l4.5894j0j15", uri.query.at("aqs"));
    EXPECT_EQ("chrome", uri.query.at("sourceid"));
    EXPECT_EQ("UTF-8", uri.query.at("ie"));

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
}

TEST(helpers_splituri_narrow, test_9a)
{
    using namespace siddiqsoft::splituri_literals;

    auto uri = "http://n.co:6553/"_Uri;
    EXPECT_EQ("n.co", uri.authority.host);
    EXPECT_EQ(6553, uri.authority.port);
    EXPECT_EQ("/", uri.urlPart);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}

TEST(helpers_splituri_narrow, test_9b)
{
    using namespace siddiqsoft::splituri_literals;

    auto uri = "http://n.co:6553"_Uri;
    EXPECT_EQ("n.co", uri.authority.host);
    EXPECT_EQ(6553, uri.authority.port);
    EXPECT_EQ("", uri.urlPart);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}

TEST(helpers_splituri_narrow, test_9c)
{
    using namespace siddiqsoft::splituri_literals;

    auto uri = "http://n.co:65536/"_Uri;
    EXPECT_EQ("n.co", uri.authority.host);
    EXPECT_EQ(0, uri.authority.port); // WHATWG: port-out-of-range validation error
    EXPECT_EQ("/", uri.urlPart);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}

TEST(helpers_splituri_narrow, test_9d)
{
    using namespace siddiqsoft::splituri_literals;

    auto uri = "http://n.co:65535"_Uri;
    EXPECT_EQ("n.co", uri.authority.host);
    EXPECT_EQ(65535, uri.authority.port);
    EXPECT_EQ("", uri.urlPart);

    std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri_narrow, test_10)
{
    using namespace siddiqsoft::splituri_literals;

    auto u = "https://www.google.com/search/?emptyFlag&Char{55}&q=siddiqsoft#v1"_Uri;

    EXPECT_EQ("www.google.com", u.authority.host);
    std::cerr << u.authority.host << std::endl;

    EXPECT_EQ(443, u.authority.port);
    std::cerr << u.authority.port << std::endl;

    EXPECT_EQ("/search/?emptyFlag&Char{55}&q=siddiqsoft#v1", u.urlPart);
    std::cerr << u.urlPart << std::endl;

    EXPECT_EQ("emptyFlag&Char{55}&q=siddiqsoft", u.queryPart);
    std::cerr << u.queryPart << std::endl;

    EXPECT_EQ("v1", u.fragment);
    std::cerr << u.fragment << std::endl;

    EXPECT_EQ(1, u.path.size());
    EXPECT_EQ("search", u.path.at(0));
    std::cerr << nlohmann::json(u.path).dump() << std::endl;

    EXPECT_EQ(3, u.query.size());
    EXPECT_EQ("siddiqsoft", u.query.at("q"));
    ASSERT_TRUE(u.query.contains("emptyFlag"));
    ASSERT_TRUE(u.query.contains("Char{55}"));
    EXPECT_TRUE(u.query.at("emptyFlag").empty());
    EXPECT_TRUE(u.query.at("Char{55}").empty());
    std::cerr << nlohmann::json(u.query).dump() << std::endl;

    // Checks that both serializers are available (caught at compile-time)
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
    std::cerr << std::format("{}", u.scheme) << "...." << nlohmann::json(u.scheme).dump() << std::endl;

    // Note that despite the initial uri string skipping the port, the SplitUri decodes and stores the port
    EXPECT_EQ("www.google.com:443", std::format("{}", u.authority));
    std::cerr << std::format("{}", u.authority) << std::endl;

    // The "rebuilt" endpoint
    EXPECT_EQ("https://www.google.com/search/?emptyFlag&Char{55}&q=siddiqsoft#v1", std::format("{}", u));
    std::cerr << std::format("{}", u) << std::endl;
}


TEST(helpers_splituri_narrow, test_99a)
{
    using namespace std;

    auto endpoint {
            "https://www.bing.com/?toWww=1&redig=https://www.bing.com/search?q=117244609&form=QBLH&sp=-1&pq=19983711434&sc=0-11&qs=n&sk=&cvid=46160ADDF1247EBA6FD76A4F6314D8B"s};
    auto uri = siddiqsoft::SplitUri(endpoint);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ("www.bing.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ("/?toWww=1&redig=https://www.bing.com/"
              "search?q=117244609&form=QBLH&sp=-1&pq=19983711434&sc=0-11&qs=n&sk=&cvid=46160ADDF1247EBA6FD76A4F6314D8B",
              uri.urlPart);
    EXPECT_EQ("toWww=1&redig=https://www.bing.com/"
              "search?q=117244609&form=QBLH&sp=-1&pq=19983711434&sc=0-11&qs=n&sk=&cvid=46160ADDF1247EBA6FD76A4F6314D8B",
              uri.queryPart);
    // We should match the initial endpoint--ensures we did not drop anything.
    EXPECT_EQ(endpoint, std::format("{}://{}{}", uri.scheme, uri.authority.host, uri.urlPart));
    EXPECT_EQ(0, uri.path.size());

    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    std::cerr << doc.dump(3) << std::endl;
}


// ============================================================================
// WHATWG URL Standard conformance tests
// ============================================================================

TEST(helpers_splituri_narrow_whatwg, test_special_schemes_ws)
{
    // WHATWG: ws:// is a special scheme with default port 80
    auto uri = siddiqsoft::SplitUri("ws://example.com/chat");
    EXPECT_EQ(siddiqsoft::UriScheme::WebSocket, uri.scheme);
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ("/chat", uri.urlPart);
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(uri.scheme));
    EXPECT_EQ(80, siddiqsoft::defaultPortForScheme(uri.scheme));
}

TEST(helpers_splituri_narrow_whatwg, test_special_schemes_wss)
{
    // WHATWG: wss:// is a special scheme with default port 443
    auto uri = siddiqsoft::SplitUri("wss://example.com/chat");
    EXPECT_EQ(siddiqsoft::UriScheme::WebSocketSecure, uri.scheme);
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ("/chat", uri.urlPart);
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(uri.scheme));
    EXPECT_EQ(443, siddiqsoft::defaultPortForScheme(uri.scheme));
}

TEST(helpers_splituri_narrow_whatwg, test_special_schemes_ftp)
{
    // WHATWG: ftp:// is a special scheme with default port 21
    auto uri = siddiqsoft::SplitUri("ftp://files.example.com/pub/readme.txt");
    EXPECT_EQ(siddiqsoft::UriScheme::Ftp, uri.scheme);
    EXPECT_EQ("files.example.com", uri.authority.host);
    EXPECT_EQ(21, uri.authority.port);
    EXPECT_EQ("/pub/readme.txt", uri.urlPart);
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(uri.scheme));
    EXPECT_EQ(21, siddiqsoft::defaultPortForScheme(uri.scheme));
}

TEST(helpers_splituri_narrow_whatwg, test_case_insensitive_scheme)
{
    // WHATWG: Schemes are case-insensitive
    auto uri1 = siddiqsoft::SplitUri("HTTPS://example.com/");
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri1.scheme);
    EXPECT_EQ("example.com", uri1.authority.host);

    auto uri2 = siddiqsoft::SplitUri("HtTpS://example.com/");
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri2.scheme);
    EXPECT_EQ("example.com", uri2.authority.host);
}

TEST(helpers_splituri_narrow_whatwg, test_host_lowercasing)
{
    // WHATWG Section 3.5: Hosts are lowercased for special schemes
    auto uri = siddiqsoft::SplitUri("https://EXAMPLE.COM/path");
    EXPECT_EQ("example.com", uri.authority.host);
}

TEST(helpers_splituri_narrow_whatwg, test_strip_leading_trailing_whitespace)
{
    // WHATWG Section 4.4 Step 1: Strip leading/trailing C0 control or space
    auto uri = siddiqsoft::SplitUri("  https://example.com/  ");
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ("/", uri.urlPart);
}

TEST(helpers_splituri_narrow_whatwg, test_remove_tab_newline)
{
    // WHATWG Section 4.4 Step 3: Remove ASCII tab and newline
    auto uri = siddiqsoft::SplitUri("https://exa\tmple.com/pa\nth");
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ("/path", uri.urlPart);
}

TEST(helpers_splituri_narrow_whatwg, test_password_parsing)
{
    // WHATWG: username:password are separate fields
    auto uri = siddiqsoft::SplitUri("https://user:pass@example.com/");
    EXPECT_EQ("user", uri.authority.userInfo);
    EXPECT_EQ("pass", uri.authority.password);
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
}

TEST(helpers_splituri_narrow_whatwg, test_password_empty)
{
    // WHATWG: username without password
    auto uri = siddiqsoft::SplitUri("https://user@example.com/");
    EXPECT_EQ("user", uri.authority.userInfo);
    EXPECT_EQ("", uri.authority.password);
    EXPECT_EQ("example.com", uri.authority.host);
}

TEST(helpers_splituri_narrow_whatwg, test_port_out_of_range)
{
    // WHATWG Section 4.4 port state: port > 65535 is a validation error
    auto uri = siddiqsoft::SplitUri("https://example.com:70000/");
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ(0, uri.authority.port); // port-out-of-range
}

TEST(helpers_splituri_narrow_whatwg, test_dot_segment_resolution)
{
    // WHATWG Section 4.4 path state: "." and ".." segments are resolved
    auto uri = siddiqsoft::SplitUri("https://example.com/a/b/../c");
    EXPECT_EQ("example.com", uri.authority.host);
    // Path should resolve: a, b, .., c -> a, c
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ("a", uri.path.at(0));
    EXPECT_EQ("c", uri.path.at(1));
}

TEST(helpers_splituri_narrow_whatwg, test_single_dot_segment_resolution)
{
    // WHATWG: Single dot segments are removed
    auto uri = siddiqsoft::SplitUri("https://example.com/a/./b/./c");
    EXPECT_EQ(3, uri.path.size());
    EXPECT_EQ("a", uri.path.at(0));
    EXPECT_EQ("b", uri.path.at(1));
    EXPECT_EQ("c", uri.path.at(2));
}

TEST(helpers_splituri_narrow_whatwg, test_double_dot_to_root)
{
    // WHATWG: ".." at root stays at root
    auto uri = siddiqsoft::SplitUri("https://example.com/../path");
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ("path", uri.path.at(0));
}

TEST(helpers_splituri_narrow_whatwg, test_default_port_table)
{
    // Verify the WHATWG default port table
    EXPECT_EQ(21, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::Ftp));
    EXPECT_EQ(80, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::WebHttp));
    EXPECT_EQ(443, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::WebHttps));
    EXPECT_EQ(80, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::WebSocket));
    EXPECT_EQ(443, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::WebSocketSecure));
    EXPECT_EQ(0, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::File));
    EXPECT_EQ(0, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::Unknown));
}

TEST(helpers_splituri_narrow_whatwg, test_is_special_scheme)
{
    // Verify WHATWG special scheme classification
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::Ftp));
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::File));
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::WebHttp));
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::WebHttps));
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::WebSocket));
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::WebSocketSecure));
    EXPECT_FALSE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::Mailto));
    EXPECT_FALSE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::Unknown));
}

TEST(helpers_splituri_narrow_whatwg, test_ws_with_port)
{
    auto uri = siddiqsoft::SplitUri("ws://example.com:9090/chat?room=general");
    EXPECT_EQ(siddiqsoft::UriScheme::WebSocket, uri.scheme);
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ(9090, uri.authority.port);
    EXPECT_EQ("/chat?room=general", uri.urlPart);
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ("chat", uri.path.at(0));
    EXPECT_EQ(1, uri.query.size());
    EXPECT_EQ("general", uri.query.at("room"));
}

TEST(helpers_splituri_narrow_whatwg, test_ftp_with_credentials)
{
    auto uri = siddiqsoft::SplitUri("ftp://admin:secret@files.example.com:2121/pub/");
    EXPECT_EQ(siddiqsoft::UriScheme::Ftp, uri.scheme);
    EXPECT_EQ("admin", uri.authority.userInfo);
    EXPECT_EQ("secret", uri.authority.password);
    EXPECT_EQ("files.example.com", uri.authority.host);
    EXPECT_EQ(2121, uri.authority.port);
    EXPECT_EQ("/pub/", uri.urlPart);
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ("pub", uri.path.at(0));
}
