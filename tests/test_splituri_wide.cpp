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

// ============================================================================
// Original regression tests (updated for WHATWG conformance)
// ============================================================================

TEST(helpers_splituri_wide, test_1a)
{
    auto uri = siddiqsoft::SplitUri(std::wstring(L"http://search.msn.com:8080/results.asp?RS=CHECKED&FORM=MSNH&v=1&q=wininet"));
    EXPECT_EQ(L"search.msn.com", uri.authority.host);
    EXPECT_EQ(8080, uri.authority.port);
    EXPECT_EQ(L"/results.asp?RS=CHECKED&FORM=MSNH&v=1&q=wininet", uri.urlPart);
    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    EXPECT_EQ("results.asp", doc.value("/path/0"_json_pointer, ""));
}

TEST(helpers_splituri_wide, test_1b)
{
    using namespace siddiqsoft::splituri_literals;
    auto uri = L"https://YOURDBNAME.documents.azure.com:443/"_Uri;
    EXPECT_EQ(L"yourdbname.documents.azure.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(L"/", uri.urlPart);
    EXPECT_EQ(L"", uri.queryPart);
    EXPECT_EQ(L"https://YOURDBNAME.documents.azure.com:443/", std::wstring(uri));
}

TEST(helpers_splituri_wide, test_1c)
{
    using namespace siddiqsoft::splituri_literals;
    auto uri = L"https://YOURDBNAME.documents.azure.com:1443/"_Uri;
    EXPECT_EQ(L"yourdbname.documents.azure.com", uri.authority.host);
    EXPECT_EQ(1443, uri.authority.port);
    EXPECT_EQ(L"/", uri.urlPart);
    EXPECT_EQ(L"", uri.queryPart);
    EXPECT_EQ(L"https://YOURDBNAME.documents.azure.com:1443/", std::wstring(uri));
}

TEST(helpers_splituri_wide, test_2) { auto uri = siddiqsoft::SplitUri(L"http://search.msn.com:8080"); EXPECT_EQ(L"search.msn.com", uri.authority.host); EXPECT_EQ(8080, uri.authority.port); EXPECT_EQ(L"", uri.urlPart); }
TEST(helpers_splituri_wide, test_3a) { using namespace siddiqsoft::splituri_literals; auto uri = L"http://search.msn.com"_Uri; EXPECT_EQ(L"search.msn.com", uri.authority.host); EXPECT_EQ(80, uri.authority.port); }
TEST(helpers_splituri_wide, test_3b) { auto uri = siddiqsoft::SplitUri(L"http://search.msn.com:65536/"); EXPECT_EQ(L"search.msn.com", uri.authority.host); EXPECT_EQ(0, uri.authority.port); EXPECT_EQ(L"/", uri.urlPart); }
TEST(helpers_splituri_wide, test_4a) { auto uri = siddiqsoft::SplitUri(L"http://m.co"); EXPECT_EQ(L"m.co", uri.authority.host); EXPECT_EQ(80, uri.authority.port); EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme); }
TEST(helpers_splituri_wide, test_4b) { auto uri = siddiqsoft::SplitUri(L"https://m.co"); EXPECT_EQ(L"m.co", uri.authority.host); EXPECT_EQ(443, uri.authority.port); EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme); }
TEST(helpers_splituri_wide, test_4c) { auto uri = siddiqsoft::SplitUri(L"http://localhost"); EXPECT_EQ(L"localhost", uri.authority.host); EXPECT_EQ(80, uri.authority.port); EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme); }

TEST(helpers_splituri_wide, test_5a)
{
    auto uri = siddiqsoft::SplitUri(L"http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5");
    EXPECT_EQ(L"<servername>", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    nlohmann::json doc = uri;
    EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri_wide, test_5b)
{
    auto uri = siddiqsoft::SplitUri(L"http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')/?Ranges('Sheet1!A1')=5.5");
    EXPECT_EQ(L"<servername>", uri.authority.host);
    nlohmann::json doc = uri;
    EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri_wide, test_6a)
{
    auto uri = siddiqsoft::SplitUri(L"https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top");
    EXPECT_EQ(L"www.example.com", uri.authority.host);
    EXPECT_EQ(L"john.doe", uri.authority.userInfo);
    EXPECT_EQ(L"", uri.authority.password);
    EXPECT_EQ(123, uri.authority.port);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ(L"top", uri.fragment);
}

TEST(helpers_splituri_wide, test_6b)
{
    auto uri = siddiqsoft::SplitUri(std::wstring(L"https://john.doe@www.example.com:123/forum/questions?tag=networking&order=newest#top"));
    EXPECT_EQ(L"www.example.com", uri.authority.host);
    EXPECT_EQ(L"john.doe", uri.authority.userInfo);
    EXPECT_EQ(123, uri.authority.port);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ(L"top", uri.fragment);
    nlohmann::json doc = uri;
    EXPECT_EQ("questions", doc.value("/path/1"_json_pointer, ""));
}

TEST(helpers_splituri_wide, test_6c)
{
    auto uri = siddiqsoft::SplitUri(std::wstring(L"https://john.doe@www.example.com/forum/questions?tag=networking&order=newest#top"));
    EXPECT_EQ(L"www.example.com", uri.authority.host);
    EXPECT_EQ(L"john.doe", uri.authority.userInfo);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ(L"top", uri.fragment);
    nlohmann::json doc = uri;
    EXPECT_EQ("questions", doc.value("/path/1"_json_pointer, ""));
}

TEST(helpers_splituri_wide, test7a) { auto uri = siddiqsoft::SplitUri(std::wstring(L"https://a00s-engr.servicebus.windows.net:9098")); EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme); EXPECT_EQ(L"a00s-engr.servicebus.windows.net", uri.authority.host); EXPECT_EQ(9098, uri.authority.port); }
TEST(helpers_splituri_wide, test7b) { auto uri = siddiqsoft::SplitUri(std::wstring(L"https://www.yahoo.com:9098/path?x=u&n=0")); EXPECT_EQ(L"www.yahoo.com", uri.authority.host); EXPECT_EQ(9098, uri.authority.port); EXPECT_EQ(2, uri.query.size()); EXPECT_EQ(1, uri.path.size()); EXPECT_EQ(L"path", uri.path.at(0)); }

TEST(helpers_splituri_wide, test8a)
{
    auto uri = siddiqsoft::SplitUri(std::wstring(L"https://www.bing.com/search?q=siddiqsoft&go=Search&qs=n&form=QBRE&sp=-1&pq=siddiqsoft&sc=8-10&sk=&cvid=90463834E5F74231B327D1158C16C5EE"));
    EXPECT_EQ(9, uri.query.size());
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ(L"search", uri.path.at(0));
    EXPECT_EQ(L"siddiqsoft", uri.query.at(L"q"));
    EXPECT_EQ(L"", uri.query.at(L"sk"));
}

TEST(helpers_splituri_wide, test8b)
{
    auto uri = siddiqsoft::SplitUri(std::wstring(L"https://www.google.com/search?q=siddiqsoft&rlz=1C5CHFA_enUS880US881&oq=siddiqsoft&aqs=chrome..69i57j69i60l4.5894j0j15&sourceid=chrome&ie=UTF-8"));
    EXPECT_EQ(6, uri.query.size());
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ(L"search", uri.path.at(0));
}

TEST(helpers_splituri_wide, test_9a) { using namespace siddiqsoft::splituri_literals; auto uri = L"http://n.co:6553/"_Uri; EXPECT_EQ(L"n.co", uri.authority.host); EXPECT_EQ(6553, uri.authority.port); EXPECT_EQ(L"/", uri.urlPart); }
TEST(helpers_splituri_wide, test_9b) { using namespace siddiqsoft::splituri_literals; auto uri = L"http://n.co:6553"_Uri; EXPECT_EQ(L"n.co", uri.authority.host); EXPECT_EQ(6553, uri.authority.port); EXPECT_EQ(L"", uri.urlPart); }
TEST(helpers_splituri_wide, test_9c) { using namespace siddiqsoft::splituri_literals; auto uri = L"http://n.co:65536/"_Uri; EXPECT_EQ(L"n.co", uri.authority.host); EXPECT_EQ(0, uri.authority.port); EXPECT_EQ(L"/", uri.urlPart); }
TEST(helpers_splituri_wide, test_9d) { using namespace siddiqsoft::splituri_literals; auto uri = L"http://n.co:65535"_Uri; EXPECT_EQ(L"n.co", uri.authority.host); EXPECT_EQ(65535, uri.authority.port); EXPECT_EQ(L"", uri.urlPart); }

TEST(helpers_splituri_wide, test_10)
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
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
    EXPECT_EQ(L"www.google.com:443", std::format(L"{}", u.authority));
    EXPECT_EQ(L"https://www.google.com/search/?emptyFlag&Char{55}&q=siddiqsoft#v1", std::format(L"{}", u));
}

TEST(helpers_splituri_wide, test_99a)
{
    auto endpoint = std::wstring(L"https://www.bing.com/?toWww=1&redig=https://www.bing.com/search?q=117244609&form=QBLH&sp=-1&pq=19983711434&sc=0-11&qs=n&sk=&cvid=46160ADDF1247EBA6FD76A4F6314D8B");
    auto uri = siddiqsoft::SplitUri(endpoint);
    EXPECT_EQ(L"www.bing.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(endpoint, std::format(L"{}://{}{}", uri.scheme, uri.authority.host, uri.urlPart));
    EXPECT_EQ(0, uri.path.size());
    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
}

// ============================================================================
// WHATWG: Wide string special scheme tests
// ============================================================================

TEST(splituri_wide_whatwg, ws_basic) { auto uri = siddiqsoft::SplitUri(L"ws://example.com/chat"); EXPECT_EQ(siddiqsoft::UriScheme::WebSocket, uri.scheme); EXPECT_EQ(L"example.com", uri.authority.host); EXPECT_EQ(80, uri.authority.port); }
TEST(splituri_wide_whatwg, wss_basic) { auto uri = siddiqsoft::SplitUri(L"wss://example.com/chat"); EXPECT_EQ(siddiqsoft::UriScheme::WebSocketSecure, uri.scheme); EXPECT_EQ(443, uri.authority.port); }
TEST(splituri_wide_whatwg, ftp_basic) { auto uri = siddiqsoft::SplitUri(L"ftp://files.example.com/pub/readme.txt"); EXPECT_EQ(siddiqsoft::UriScheme::Ftp, uri.scheme); EXPECT_EQ(21, uri.authority.port); EXPECT_EQ(2, uri.path.size()); }

TEST(splituri_wide_whatwg, host_lowercasing) { auto uri = siddiqsoft::SplitUri(L"https://EXAMPLE.COM/path"); EXPECT_EQ(L"example.com", uri.authority.host); }
TEST(splituri_wide_whatwg, case_insensitive_scheme) { auto uri = siddiqsoft::SplitUri(L"HTTPS://example.com/"); EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme); }

TEST(splituri_wide_whatwg, password_parsing) { auto uri = siddiqsoft::SplitUri(L"https://user:pass@example.com/"); EXPECT_EQ(L"user", uri.authority.userInfo); EXPECT_EQ(L"pass", uri.authority.password); EXPECT_EQ(L"example.com", uri.authority.host); }
TEST(splituri_wide_whatwg, username_only) { auto uri = siddiqsoft::SplitUri(L"https://user@example.com/"); EXPECT_EQ(L"user", uri.authority.userInfo); EXPECT_EQ(L"", uri.authority.password); }

TEST(splituri_wide_whatwg, dot_segment_resolution) { auto uri = siddiqsoft::SplitUri(L"https://example.com/a/b/../c"); EXPECT_EQ(2, uri.path.size()); EXPECT_EQ(L"a", uri.path.at(0)); EXPECT_EQ(L"c", uri.path.at(1)); }
TEST(splituri_wide_whatwg, single_dot_removed) { auto uri = siddiqsoft::SplitUri(L"https://example.com/a/./b"); EXPECT_EQ(2, uri.path.size()); EXPECT_EQ(L"a", uri.path.at(0)); EXPECT_EQ(L"b", uri.path.at(1)); }

TEST(splituri_wide_whatwg, ipv6_basic) { auto uri = siddiqsoft::SplitUri(L"http://[::1]/path"); EXPECT_EQ(L"[::1]", uri.authority.host); EXPECT_EQ(80, uri.authority.port); }
TEST(splituri_wide_whatwg, ipv6_with_port) { auto uri = siddiqsoft::SplitUri(L"http://[::1]:8080/path"); EXPECT_EQ(L"[::1]", uri.authority.host); EXPECT_EQ(8080, uri.authority.port); }

TEST(splituri_wide_whatwg, port_out_of_range) { auto uri = siddiqsoft::SplitUri(L"https://example.com:70000/"); EXPECT_EQ(0, uri.authority.port); }

// ============================================================================
// Wide: Default constructor
// ============================================================================

TEST(splituri_wide_construction, default_constructor)
{
    siddiqsoft::Uri<wchar_t> uri;
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme);
    EXPECT_EQ(L"", uri.authority.host);
    EXPECT_EQ(L"", uri.authority.userInfo);
    EXPECT_EQ(L"", uri.authority.password);
    EXPECT_EQ(0, uri.authority.port);
    EXPECT_TRUE(uri.path.empty());
    EXPECT_TRUE(uri.query.empty());
}

// ============================================================================
// Wide: Authority serialization
// ============================================================================

TEST(splituri_wide_authority, user_password_host_port)
{
    siddiqsoft::AuthorityHttp<wchar_t> a;
    a.userInfo = L"admin";
    a.password = L"secret";
    a.host = L"example.com";
    a.port = 443;
    EXPECT_EQ(L"admin:secret@example.com:443", std::wstring(a));
}

TEST(splituri_wide_authority, host_only)
{
    siddiqsoft::AuthorityHttp<wchar_t> a;
    a.host = L"example.com";
    EXPECT_EQ(L"example.com", std::wstring(a));
}

// ============================================================================
// Wide: std::format support
// ============================================================================

TEST(splituri_wide_format, format_scheme) { EXPECT_EQ(L"https", std::format(L"{}", siddiqsoft::UriScheme::WebHttps)); }
TEST(splituri_wide_format, format_scheme_ws) { EXPECT_EQ(L"ws", std::format(L"{}", siddiqsoft::UriScheme::WebSocket)); }
TEST(splituri_wide_format, format_scheme_ftp) { EXPECT_EQ(L"ftp", std::format(L"{}", siddiqsoft::UriScheme::Ftp)); }

// ============================================================================
// Wide: JSON serialization
// ============================================================================

TEST(splituri_wide_json, basic_json)
{
    auto uri = siddiqsoft::SplitUri(L"https://user:pass@example.com:8443/api?key=val#sec");
    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    EXPECT_EQ("https", doc.at("scheme").get<std::string>());
    EXPECT_EQ("user", doc.at("authority").at("userInfo").get<std::string>());
    EXPECT_EQ("pass", doc.at("authority").at("password").get<std::string>());
    EXPECT_EQ("example.com", doc.at("authority").at("host").get<std::string>());
    EXPECT_EQ(8443, doc.at("authority").at("port").get<int>());
    EXPECT_EQ("api", doc.at("path").at(0).get<std::string>());
    EXPECT_EQ("val", doc.at("query").at("key").get<std::string>());
    EXPECT_EQ("sec", doc.at("fragment").get<std::string>());
}

// ============================================================================
// Wide: to_string
// ============================================================================

TEST(splituri_wide_tostring, all_schemes)
{
    EXPECT_EQ(L"http", siddiqsoft::to_string<wchar_t>(siddiqsoft::UriScheme::WebHttp));
    EXPECT_EQ(L"https", siddiqsoft::to_string<wchar_t>(siddiqsoft::UriScheme::WebHttps));
    EXPECT_EQ(L"ws", siddiqsoft::to_string<wchar_t>(siddiqsoft::UriScheme::WebSocket));
    EXPECT_EQ(L"wss", siddiqsoft::to_string<wchar_t>(siddiqsoft::UriScheme::WebSocketSecure));
    EXPECT_EQ(L"ftp", siddiqsoft::to_string<wchar_t>(siddiqsoft::UriScheme::Ftp));
    EXPECT_EQ(L"file", siddiqsoft::to_string<wchar_t>(siddiqsoft::UriScheme::File));
    EXPECT_EQ(L"Unknown", siddiqsoft::to_string<wchar_t>(siddiqsoft::UriScheme::Unknown));
}

// ============================================================================
// Wide: Literal operator
// ============================================================================

TEST(splituri_wide_literal, basic)
{
    using namespace siddiqsoft::splituri_literals;
    auto uri = L"https://example.com/"_Uri;
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ(L"example.com", uri.authority.host);
}

TEST(splituri_wide_literal, complex)
{
    using namespace siddiqsoft::splituri_literals;
    auto uri = L"ws://chat.example.com:9090/room?id=42#messages"_Uri;
    EXPECT_EQ(siddiqsoft::UriScheme::WebSocket, uri.scheme);
    EXPECT_EQ(L"chat.example.com", uri.authority.host);
    EXPECT_EQ(9090, uri.authority.port);
    EXPECT_EQ(L"room", uri.path.at(0));
    EXPECT_EQ(L"42", uri.query.at(L"id"));
    EXPECT_EQ(L"messages", uri.fragment);
}

// ============================================================================
// Wide: Edge cases
// ============================================================================

TEST(splituri_wide_edge, empty_string) { auto uri = siddiqsoft::SplitUri(std::wstring(L"")); EXPECT_EQ(L"", uri.authority.host); EXPECT_TRUE(uri.path.empty()); }
TEST(splituri_wide_edge, garbage_input) { auto uri = siddiqsoft::SplitUri(L"not-a-url-at-all"); EXPECT_EQ(L"", uri.authority.host); EXPECT_TRUE(uri.path.empty()); }

// ============================================================================
// Wide: Rebuild
// ============================================================================

TEST(splituri_wide_rebuild, parsed_returns_original)
{
    std::wstring original = L"https://example.com:8080/path?q=1#frag";
    auto uri = siddiqsoft::SplitUri(original);
    EXPECT_EQ(original, uri.string());
}

TEST(splituri_wide_rebuild, rebuild_from_components)
{
    siddiqsoft::Uri<wchar_t> uri;
    uri.scheme = siddiqsoft::UriScheme::WebHttps;
    uri.authority.host = L"example.com";
    uri.authority.port = 443;
    uri.urlPart = L"/path";
    EXPECT_EQ(L"https://example.com:443/path", uri.string());
}
