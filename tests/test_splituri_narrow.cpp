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

TEST(helpers_splituri_narrow, test_1a)
{
    using namespace siddiqsoft::splituri_literals;
    auto uri = "http://search.msn.com:8080/results.asp?RS=CHECKED&FORM=MSNH&v=1&q=wininet"_Uri;
    EXPECT_EQ("search.msn.com", uri.authority.host);
    EXPECT_EQ(8080, uri.authority.port);
    EXPECT_EQ("/results.asp?RS=CHECKED&FORM=MSNH&v=1&q=wininet", uri.urlPart);
    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
    EXPECT_EQ("results.asp", doc.value("/path/0"_json_pointer, ""));
}

TEST(helpers_splituri_narrow, test_1b)
{
    using namespace siddiqsoft::splituri_literals;
    auto uri = "https://YOURDBNAME.documents.azure.com:443/"_Uri;
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
}

TEST(helpers_splituri_narrow, test_3a)
{
    using namespace siddiqsoft::splituri_literals;
    auto uri = "http://search.msn.com"_Uri;
    EXPECT_EQ("search.msn.com", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ("", uri.urlPart);
}

TEST(helpers_splituri_narrow, test_3b)
{
    auto uri = siddiqsoft::SplitUri("http://search.msn.com:65536/");
    EXPECT_EQ("search.msn.com", uri.authority.host);
    EXPECT_EQ(0, uri.authority.port);
    EXPECT_EQ("/", uri.urlPart);
}

TEST(helpers_splituri_narrow, test_4a)
{
    auto uri = siddiqsoft::SplitUri("http://m.co");
    EXPECT_EQ("m.co", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme);
}

TEST(helpers_splituri_narrow, test_4b)
{
    auto uri = siddiqsoft::SplitUri("https://m.co");
    EXPECT_EQ("m.co", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
}

TEST(helpers_splituri_narrow, test_4c)
{
    auto uri = siddiqsoft::SplitUri("http://localhost");
    EXPECT_EQ("localhost", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme);
}

TEST(helpers_splituri_narrow, test_5a)
{
    auto uri = siddiqsoft::SplitUri("http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/"
                                    "model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5");
    EXPECT_EQ("<servername>", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    nlohmann::json doc = uri;
    EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri_narrow, test_5b)
{
    auto uri = siddiqsoft::SplitUri("http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/"
                                    "model/Charts('Chart%201')/?Ranges('Sheet1!A1')=5.5");
    EXPECT_EQ("<servername>", uri.authority.host);
    EXPECT_EQ("/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')/", uri.pathPart);
    nlohmann::json doc = uri;
    EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri_narrow, test_5c)
{
    auto uri = siddiqsoft::SplitUri("http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/"
                                    "model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5");
    EXPECT_EQ("<servername>", uri.authority.host);
    EXPECT_EQ("/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')", uri.pathPart);
    nlohmann::json doc = uri;
    EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri_narrow, test_6a)
{
    auto uri = siddiqsoft::SplitUri("https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top");
    EXPECT_EQ("www.example.com", uri.authority.host);
    EXPECT_EQ("john.doe", uri.authority.userInfo);
    EXPECT_EQ("", uri.authority.password);
    EXPECT_EQ(123, uri.authority.port);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ("top", uri.fragment);
}

TEST(helpers_splituri_narrow, test_6b)
{
    auto uri = siddiqsoft::SplitUri(std::string("https://john.doe@www.example.com:123/forum/questions?tag=networking&order=newest#top"));
    EXPECT_EQ("www.example.com", uri.authority.host);
    EXPECT_EQ("john.doe", uri.authority.userInfo);
    EXPECT_EQ(123, uri.authority.port);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ("top", uri.fragment);
    nlohmann::json doc = uri;
    EXPECT_EQ("questions", doc.value("/path/1"_json_pointer, ""));
}

TEST(helpers_splituri_narrow, test_6c)
{
    auto uri = siddiqsoft::SplitUri(std::string("https://john.doe@www.example.com/forum/questions?tag=networking&order=newest#top"));
    EXPECT_EQ("www.example.com", uri.authority.host);
    EXPECT_EQ("john.doe", uri.authority.userInfo);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ("top", uri.fragment);
    nlohmann::json doc = uri;
    EXPECT_EQ("questions", doc.value("/path/1"_json_pointer, ""));
}

TEST(helpers_splituri_narrow, test7a)
{
    auto uri = siddiqsoft::SplitUri(std::string("https://a00s-engr.servicebus.windows.net:9098"));
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ("a00s-engr.servicebus.windows.net", uri.authority.host);
    EXPECT_EQ(9098, uri.authority.port);
    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
}

TEST(helpers_splituri_narrow, test7b)
{
    auto uri = siddiqsoft::SplitUri(std::string("https://www.yahoo.com:9098/path?x=u&n=0"));
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ("www.yahoo.com", uri.authority.host);
    EXPECT_EQ(9098, uri.authority.port);
    EXPECT_EQ(2, uri.query.size());
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ("path", uri.path.at(0));
}

TEST(helpers_splituri_narrow, test8a)
{
    auto uri = siddiqsoft::SplitUri(std::string(
            "https://www.bing.com/search?q=siddiqsoft&go=Search&qs=n&form=QBRE&sp=-1&pq=siddiqsoft&sc=8-10&sk=&cvid=90463834E5F74231B327D1158C16C5EE"));
    EXPECT_EQ(9, uri.query.size());
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ("search", uri.path.at(0));
    EXPECT_EQ("siddiqsoft", uri.query.at("q"));
    EXPECT_EQ("", uri.query.at("sk"));
    EXPECT_EQ("90463834E5F74231B327D1158C16C5EE", uri.query.at("cvid"));
}

TEST(helpers_splituri_narrow, test8b)
{
    std::string endpoint {"https://www.google.com/search?q=siddiqsoft&rlz=1C5CHFA_enUS880US881&oq=siddiqsoft&aqs=chrome..69i57j69i60l4.5894j0j15&sourceid=chrome&ie=UTF-8"};
    auto uri = siddiqsoft::SplitUri(endpoint);
    EXPECT_EQ(6, uri.query.size());
    EXPECT_EQ(endpoint, std::format("{}://{}{}", uri.scheme, uri.authority.host, uri.urlPart));
    EXPECT_EQ("search", uri.path.at(0));
}

TEST(helpers_splituri_narrow, test_9a) { using namespace siddiqsoft::splituri_literals; auto uri = "http://n.co:6553/"_Uri; EXPECT_EQ("n.co", uri.authority.host); EXPECT_EQ(6553, uri.authority.port); EXPECT_EQ("/", uri.urlPart); }
TEST(helpers_splituri_narrow, test_9b) { using namespace siddiqsoft::splituri_literals; auto uri = "http://n.co:6553"_Uri; EXPECT_EQ("n.co", uri.authority.host); EXPECT_EQ(6553, uri.authority.port); EXPECT_EQ("", uri.urlPart); }
TEST(helpers_splituri_narrow, test_9c) { using namespace siddiqsoft::splituri_literals; auto uri = "http://n.co:65536/"_Uri; EXPECT_EQ("n.co", uri.authority.host); EXPECT_EQ(0, uri.authority.port); EXPECT_EQ("/", uri.urlPart); }
TEST(helpers_splituri_narrow, test_9d) { using namespace siddiqsoft::splituri_literals; auto uri = "http://n.co:65535"_Uri; EXPECT_EQ("n.co", uri.authority.host); EXPECT_EQ(65535, uri.authority.port); EXPECT_EQ("", uri.urlPart); }

TEST(helpers_splituri_narrow, test_10)
{
    using namespace siddiqsoft::splituri_literals;
    auto u = "https://www.google.com/search/?emptyFlag&Char{55}&q=siddiqsoft#v1"_Uri;
    EXPECT_EQ("www.google.com", u.authority.host);
    EXPECT_EQ(443, u.authority.port);
    EXPECT_EQ("/search/?emptyFlag&Char{55}&q=siddiqsoft#v1", u.urlPart);
    EXPECT_EQ("emptyFlag&Char{55}&q=siddiqsoft", u.queryPart);
    EXPECT_EQ("v1", u.fragment);
    EXPECT_EQ(1, u.path.size());
    EXPECT_EQ("search", u.path.at(0));
    EXPECT_EQ(3, u.query.size());
    EXPECT_EQ("siddiqsoft", u.query.at("q"));
    ASSERT_TRUE(u.query.contains("emptyFlag"));
    ASSERT_TRUE(u.query.contains("Char{55}"));
    EXPECT_TRUE(u.query.at("emptyFlag").empty());
    EXPECT_TRUE(u.query.at("Char{55}").empty());
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
    EXPECT_EQ("www.google.com:443", std::format("{}", u.authority));
    EXPECT_EQ("https://www.google.com/search/?emptyFlag&Char{55}&q=siddiqsoft#v1", std::format("{}", u));
}

TEST(helpers_splituri_narrow, test_99a)
{
    auto endpoint = std::string("https://www.bing.com/?toWww=1&redig=https://www.bing.com/search?q=117244609&form=QBLH&sp=-1&pq=19983711434&sc=0-11&qs=n&sk=&cvid=46160ADDF1247EBA6FD76A4F6314D8B");
    auto uri = siddiqsoft::SplitUri(endpoint);
    EXPECT_EQ("www.bing.com", uri.authority.host);
    EXPECT_EQ(443, uri.authority.port);
    EXPECT_EQ(endpoint, std::format("{}://{}{}", uri.scheme, uri.authority.host, uri.urlPart));
    EXPECT_EQ(0, uri.path.size());
    nlohmann::json doc = uri;
    EXPECT_TRUE(doc.is_object());
}

// ============================================================================
// Default constructor and split() method
// ============================================================================

TEST(splituri_construction, default_constructor)
{
    siddiqsoft::Uri<> uri;
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme);
    EXPECT_EQ("", uri.authority.host);
    EXPECT_EQ("", uri.authority.userInfo);
    EXPECT_EQ("", uri.authority.password);
    EXPECT_EQ(0, uri.authority.port);
    EXPECT_TRUE(uri.path.empty());
    EXPECT_TRUE(uri.query.empty());
    EXPECT_EQ("", uri.fragment);
    EXPECT_EQ("", uri.urlPart);
    EXPECT_EQ("", uri.pathPart);
    EXPECT_EQ("", uri.queryPart);
}

TEST(splituri_construction, split_method_direct)
{
    siddiqsoft::Uri<> uri;
    uri.split("https://example.com:8443/api/v1?key=abc#section");
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ(8443, uri.authority.port);
    EXPECT_EQ(2, uri.path.size());
    EXPECT_EQ("api", uri.path.at(0));
    EXPECT_EQ("v1", uri.path.at(1));
    EXPECT_EQ("abc", uri.query.at("key"));
    EXPECT_EQ("section", uri.fragment);
}

TEST(splituri_construction, const_char_ptr_overload)
{
    auto uri = siddiqsoft::SplitUri("http://example.com/path");
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ(80, uri.authority.port);
    EXPECT_EQ(1, uri.path.size());
    EXPECT_EQ("path", uri.path.at(0));
}

// ============================================================================
// Empty and invalid input
// ============================================================================

TEST(splituri_edge_cases, empty_string) { auto uri = siddiqsoft::SplitUri(std::string("")); EXPECT_EQ("", uri.authority.host); EXPECT_TRUE(uri.path.empty()); }
TEST(splituri_edge_cases, garbage_input) { auto uri = siddiqsoft::SplitUri("not-a-url-at-all"); EXPECT_EQ("", uri.authority.host); EXPECT_TRUE(uri.path.empty()); }
TEST(splituri_edge_cases, scheme_only_no_authority) { auto uri = siddiqsoft::SplitUri("https://"); EXPECT_EQ("", uri.authority.host); EXPECT_EQ(443, uri.authority.port); }
TEST(splituri_edge_cases, only_whitespace) { auto uri = siddiqsoft::SplitUri("   "); EXPECT_EQ("", uri.authority.host); EXPECT_TRUE(uri.path.empty()); }

// ============================================================================
// WHATWG: Scheme detection (case-insensitive)
// ============================================================================

TEST(splituri_whatwg_schemes, http_uppercase) { auto uri = siddiqsoft::SplitUri("HTTP://EXAMPLE.COM/"); EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme); EXPECT_EQ("example.com", uri.authority.host); }
TEST(splituri_whatwg_schemes, http_mixed_case) { auto uri = siddiqsoft::SplitUri("hTtP://example.com/"); EXPECT_EQ(siddiqsoft::UriScheme::WebHttp, uri.scheme); }
TEST(splituri_whatwg_schemes, https_mixed_case) { auto uri = siddiqsoft::SplitUri("HtTpS://example.com/"); EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme); }
TEST(splituri_whatwg_schemes, ws_basic) { auto uri = siddiqsoft::SplitUri("ws://example.com/chat"); EXPECT_EQ(siddiqsoft::UriScheme::WebSocket, uri.scheme); EXPECT_EQ(80, uri.authority.port); }
TEST(splituri_whatwg_schemes, ws_uppercase) { auto uri = siddiqsoft::SplitUri("WS://EXAMPLE.COM/chat"); EXPECT_EQ(siddiqsoft::UriScheme::WebSocket, uri.scheme); EXPECT_EQ("example.com", uri.authority.host); }
TEST(splituri_whatwg_schemes, wss_basic) { auto uri = siddiqsoft::SplitUri("wss://example.com/chat"); EXPECT_EQ(siddiqsoft::UriScheme::WebSocketSecure, uri.scheme); EXPECT_EQ(443, uri.authority.port); }
TEST(splituri_whatwg_schemes, wss_uppercase) { auto uri = siddiqsoft::SplitUri("WSS://EXAMPLE.COM/chat"); EXPECT_EQ(siddiqsoft::UriScheme::WebSocketSecure, uri.scheme); EXPECT_EQ("example.com", uri.authority.host); }
TEST(splituri_whatwg_schemes, ftp_basic) { auto uri = siddiqsoft::SplitUri("ftp://files.example.com/pub/readme.txt"); EXPECT_EQ(siddiqsoft::UriScheme::Ftp, uri.scheme); EXPECT_EQ(21, uri.authority.port); EXPECT_EQ(2, uri.path.size()); }
TEST(splituri_whatwg_schemes, ftp_uppercase) { auto uri = siddiqsoft::SplitUri("FTP://FILES.EXAMPLE.COM/pub/"); EXPECT_EQ(siddiqsoft::UriScheme::Ftp, uri.scheme); EXPECT_EQ("files.example.com", uri.authority.host); }

// ============================================================================
// WHATWG: Utility functions
// ============================================================================

TEST(splituri_whatwg_utils, default_port_table)
{
    EXPECT_EQ(21, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::Ftp));
    EXPECT_EQ(80, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::WebHttp));
    EXPECT_EQ(443, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::WebHttps));
    EXPECT_EQ(80, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::WebSocket));
    EXPECT_EQ(443, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::WebSocketSecure));
    EXPECT_EQ(0, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::File));
    EXPECT_EQ(0, siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::Unknown));
}

TEST(splituri_whatwg_utils, is_special_scheme)
{
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::Ftp));
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::File));
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::WebHttp));
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::WebHttps));
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::WebSocket));
    EXPECT_TRUE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::WebSocketSecure));
    EXPECT_FALSE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::Ldap));
    EXPECT_FALSE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::Mailto));
    EXPECT_FALSE(siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::Unknown));
}

TEST(splituri_whatwg_utils, to_string_all_schemes)
{
    EXPECT_EQ("http", siddiqsoft::to_string<char>(siddiqsoft::UriScheme::WebHttp));
    EXPECT_EQ("https", siddiqsoft::to_string<char>(siddiqsoft::UriScheme::WebHttps));
    EXPECT_EQ("ws", siddiqsoft::to_string<char>(siddiqsoft::UriScheme::WebSocket));
    EXPECT_EQ("wss", siddiqsoft::to_string<char>(siddiqsoft::UriScheme::WebSocketSecure));
    EXPECT_EQ("ftp", siddiqsoft::to_string<char>(siddiqsoft::UriScheme::Ftp));
    EXPECT_EQ("file", siddiqsoft::to_string<char>(siddiqsoft::UriScheme::File));
    EXPECT_EQ("Unknown", siddiqsoft::to_string<char>(siddiqsoft::UriScheme::Unknown));
}

// ============================================================================
// WHATWG: Host lowercasing
// ============================================================================

TEST(splituri_whatwg_host, lowercase_for_special) { auto uri = siddiqsoft::SplitUri("https://EXAMPLE.COM/path"); EXPECT_EQ("example.com", uri.authority.host); }
TEST(splituri_whatwg_host, mixed_case_host) { auto uri = siddiqsoft::SplitUri("http://ExAmPlE.CoM/"); EXPECT_EQ("example.com", uri.authority.host); }
TEST(splituri_whatwg_host, host_with_numbers_hyphens) { auto uri = siddiqsoft::SplitUri("https://My-Host-123.Example.COM:8080/"); EXPECT_EQ("my-host-123.example.com", uri.authority.host); EXPECT_EQ(8080, uri.authority.port); }

// ============================================================================
// WHATWG: Input sanitization
// ============================================================================

TEST(splituri_whatwg_sanitize, strip_leading_spaces) { auto uri = siddiqsoft::SplitUri("   https://example.com/path"); EXPECT_EQ("example.com", uri.authority.host); EXPECT_EQ("/path", uri.urlPart); }
TEST(splituri_whatwg_sanitize, strip_trailing_spaces) { auto uri = siddiqsoft::SplitUri("https://example.com/path   "); EXPECT_EQ("example.com", uri.authority.host); EXPECT_EQ("/path", uri.urlPart); }
TEST(splituri_whatwg_sanitize, strip_both_spaces) { auto uri = siddiqsoft::SplitUri("  https://example.com/  "); EXPECT_EQ("example.com", uri.authority.host); EXPECT_EQ("/", uri.urlPart); }

TEST(splituri_whatwg_sanitize, strip_c0_control_chars)
{
    std::string input = std::string("\x01\x02") + "https://example.com/" + std::string("\x1F");
    auto uri = siddiqsoft::SplitUri(input);
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ("/", uri.urlPart);
}

TEST(splituri_whatwg_sanitize, remove_tab_in_host) { auto uri = siddiqsoft::SplitUri("https://exa\tmple.com/path"); EXPECT_EQ("example.com", uri.authority.host); }
TEST(splituri_whatwg_sanitize, remove_newline_in_path) { auto uri = siddiqsoft::SplitUri("https://example.com/pa\nth"); EXPECT_EQ("/path", uri.urlPart); }
TEST(splituri_whatwg_sanitize, remove_carriage_return) { auto uri = siddiqsoft::SplitUri("https://example.com/pa\rth"); EXPECT_EQ("/path", uri.urlPart); }
TEST(splituri_whatwg_sanitize, remove_mixed_tab_newline_cr) { auto uri = siddiqsoft::SplitUri("ht\ttps://ex\namp\rle.com/p\ta\nth"); EXPECT_EQ("example.com", uri.authority.host); EXPECT_EQ("/path", uri.urlPart); }

// ============================================================================
// WHATWG: Credentials (username and password)
// ============================================================================

TEST(splituri_whatwg_credentials, username_only) { auto uri = siddiqsoft::SplitUri("https://user@example.com/"); EXPECT_EQ("user", uri.authority.userInfo); EXPECT_EQ("", uri.authority.password); }
TEST(splituri_whatwg_credentials, username_and_password) { auto uri = siddiqsoft::SplitUri("https://user:pass@example.com/"); EXPECT_EQ("user", uri.authority.userInfo); EXPECT_EQ("pass", uri.authority.password); EXPECT_EQ("example.com", uri.authority.host); }
TEST(splituri_whatwg_credentials, username_empty_password) { auto uri = siddiqsoft::SplitUri("https://user:@example.com/"); EXPECT_EQ("user", uri.authority.userInfo); EXPECT_EQ("", uri.authority.password); }
TEST(splituri_whatwg_credentials, password_with_special_chars) { auto uri = siddiqsoft::SplitUri("https://admin:p%40ss%3Aw0rd@example.com/"); EXPECT_EQ("admin", uri.authority.userInfo); EXPECT_EQ("p%40ss%3Aw0rd", uri.authority.password); }
TEST(splituri_whatwg_credentials, password_with_multiple_colons) { auto uri = siddiqsoft::SplitUri("https://user:pass:word:extra@example.com/"); EXPECT_EQ("user", uri.authority.userInfo); EXPECT_EQ("pass:word:extra", uri.authority.password); }
TEST(splituri_whatwg_credentials, no_credentials) { auto uri = siddiqsoft::SplitUri("https://example.com/"); EXPECT_EQ("", uri.authority.userInfo); EXPECT_EQ("", uri.authority.password); }
TEST(splituri_whatwg_credentials, ftp_with_credentials) { auto uri = siddiqsoft::SplitUri("ftp://admin:secret@files.example.com:2121/pub/"); EXPECT_EQ("admin", uri.authority.userInfo); EXPECT_EQ("secret", uri.authority.password); EXPECT_EQ("files.example.com", uri.authority.host); EXPECT_EQ(2121, uri.authority.port); }

// ============================================================================
// WHATWG: Port validation
// ============================================================================

TEST(splituri_whatwg_port, explicit_default_port) { auto uri = siddiqsoft::SplitUri("https://example.com:443/"); EXPECT_EQ(443, uri.authority.port); }
TEST(splituri_whatwg_port, non_default_port) { auto uri = siddiqsoft::SplitUri("https://example.com:8443/"); EXPECT_EQ(8443, uri.authority.port); }
TEST(splituri_whatwg_port, port_zero) { auto uri = siddiqsoft::SplitUri("http://example.com:0/"); EXPECT_EQ(0, uri.authority.port); }
TEST(splituri_whatwg_port, port_max_valid) { auto uri = siddiqsoft::SplitUri("http://example.com:65535/"); EXPECT_EQ(65535, uri.authority.port); }
TEST(splituri_whatwg_port, port_out_of_range_65536) { auto uri = siddiqsoft::SplitUri("http://example.com:65536/"); EXPECT_EQ(0, uri.authority.port); }
TEST(splituri_whatwg_port, port_out_of_range_large) { auto uri = siddiqsoft::SplitUri("https://example.com:70000/"); EXPECT_EQ(0, uri.authority.port); }
TEST(splituri_whatwg_port, port_out_of_range_very_large) { auto uri = siddiqsoft::SplitUri("https://example.com:999999/"); EXPECT_EQ(0, uri.authority.port); }
TEST(splituri_whatwg_port, no_port_http_default) { auto uri = siddiqsoft::SplitUri("http://example.com/"); EXPECT_EQ(80, uri.authority.port); }
TEST(splituri_whatwg_port, no_port_https_default) { auto uri = siddiqsoft::SplitUri("https://example.com/"); EXPECT_EQ(443, uri.authority.port); }
TEST(splituri_whatwg_port, no_port_ws_default) { auto uri = siddiqsoft::SplitUri("ws://example.com/"); EXPECT_EQ(80, uri.authority.port); }
TEST(splituri_whatwg_port, no_port_wss_default) { auto uri = siddiqsoft::SplitUri("wss://example.com/"); EXPECT_EQ(443, uri.authority.port); }
TEST(splituri_whatwg_port, no_port_ftp_default) { auto uri = siddiqsoft::SplitUri("ftp://example.com/"); EXPECT_EQ(21, uri.authority.port); }

// ============================================================================
// WHATWG: IPv6 addresses
// ============================================================================

TEST(splituri_whatwg_ipv6, basic_ipv6) { auto uri = siddiqsoft::SplitUri("http://[::1]/path"); EXPECT_EQ("[::1]", uri.authority.host); EXPECT_EQ(80, uri.authority.port); EXPECT_EQ("/path", uri.urlPart); }
TEST(splituri_whatwg_ipv6, ipv6_with_port) { auto uri = siddiqsoft::SplitUri("http://[::1]:8080/path"); EXPECT_EQ("[::1]", uri.authority.host); EXPECT_EQ(8080, uri.authority.port); EXPECT_EQ("/path", uri.urlPart); }
TEST(splituri_whatwg_ipv6, full_ipv6) { auto uri = siddiqsoft::SplitUri("https://[2001:db8::7]:443/c=GB?objectClass=one"); EXPECT_EQ("[2001:db8::7]", uri.authority.host); EXPECT_EQ(443, uri.authority.port); EXPECT_EQ("objectClass=one", uri.queryPart); }
TEST(splituri_whatwg_ipv6, ipv6_no_port_no_path) { auto uri = siddiqsoft::SplitUri("http://[::1]"); EXPECT_EQ("[::1]", uri.authority.host); EXPECT_EQ(80, uri.authority.port); EXPECT_EQ("", uri.urlPart); }

// ============================================================================
// WHATWG: Path dot-segment resolution
// ============================================================================

TEST(splituri_whatwg_path, single_dot_removed) { auto uri = siddiqsoft::SplitUri("https://example.com/a/./b/./c"); EXPECT_EQ(3, uri.path.size()); EXPECT_EQ("a", uri.path.at(0)); EXPECT_EQ("b", uri.path.at(1)); EXPECT_EQ("c", uri.path.at(2)); }
TEST(splituri_whatwg_path, double_dot_pops_parent) { auto uri = siddiqsoft::SplitUri("https://example.com/a/b/../c"); EXPECT_EQ(2, uri.path.size()); EXPECT_EQ("a", uri.path.at(0)); EXPECT_EQ("c", uri.path.at(1)); }
TEST(splituri_whatwg_path, double_dot_at_root) { auto uri = siddiqsoft::SplitUri("https://example.com/../path"); EXPECT_EQ(1, uri.path.size()); EXPECT_EQ("path", uri.path.at(0)); }
TEST(splituri_whatwg_path, multiple_double_dots) { auto uri = siddiqsoft::SplitUri("https://example.com/a/b/c/../../d"); EXPECT_EQ(2, uri.path.size()); EXPECT_EQ("a", uri.path.at(0)); EXPECT_EQ("d", uri.path.at(1)); }
TEST(splituri_whatwg_path, all_dots_collapse) { auto uri = siddiqsoft::SplitUri("https://example.com/a/../b/../c/.."); EXPECT_EQ(0, uri.path.size()); }
TEST(splituri_whatwg_path, mixed_dots) { auto uri = siddiqsoft::SplitUri("https://example.com/a/./b/../c/./d"); EXPECT_EQ(3, uri.path.size()); EXPECT_EQ("a", uri.path.at(0)); EXPECT_EQ("c", uri.path.at(1)); EXPECT_EQ("d", uri.path.at(2)); }
TEST(splituri_whatwg_path, no_path) { auto uri = siddiqsoft::SplitUri("https://example.com"); EXPECT_TRUE(uri.path.empty()); EXPECT_EQ("", uri.urlPart); }
TEST(splituri_whatwg_path, root_path_only) { auto uri = siddiqsoft::SplitUri("https://example.com/"); EXPECT_TRUE(uri.path.empty()); EXPECT_EQ("/", uri.urlPart); }
TEST(splituri_whatwg_path, deep_path) { auto uri = siddiqsoft::SplitUri("https://example.com/a/b/c/d/e/f"); EXPECT_EQ(6, uri.path.size()); EXPECT_EQ("a", uri.path.at(0)); EXPECT_EQ("f", uri.path.at(5)); }

// ============================================================================
// WHATWG: Backslash normalization for special schemes
// ============================================================================

TEST(splituri_whatwg_backslash, backslash_as_first_path_separator)
{
    auto uri = siddiqsoft::SplitUri("http://example.com\\path\\to\\file");
    EXPECT_EQ("example.com", uri.authority.host);
    EXPECT_EQ(3, uri.path.size());
    EXPECT_EQ("path", uri.path.at(0));
    EXPECT_EQ("to", uri.path.at(1));
    EXPECT_EQ("file", uri.path.at(2));
}

// ============================================================================
// Fragment and query edge cases
// ============================================================================

TEST(splituri_fragment_query, fragment_only_no_query) { auto uri = siddiqsoft::SplitUri("https://example.com/page#section"); EXPECT_EQ("section", uri.fragment); EXPECT_EQ("", uri.queryPart); EXPECT_TRUE(uri.query.empty()); EXPECT_EQ(1, uri.path.size()); }
TEST(splituri_fragment_query, query_only_no_fragment) { auto uri = siddiqsoft::SplitUri("https://example.com/page?key=val"); EXPECT_EQ("", uri.fragment); EXPECT_EQ("key=val", uri.queryPart); EXPECT_EQ("val", uri.query.at("key")); }
TEST(splituri_fragment_query, query_and_fragment) { auto uri = siddiqsoft::SplitUri("https://example.com/page?key=val#frag"); EXPECT_EQ("frag", uri.fragment); EXPECT_EQ("key=val", uri.queryPart); EXPECT_EQ("val", uri.query.at("key")); }
TEST(splituri_fragment_query, empty_query) { auto uri = siddiqsoft::SplitUri("https://example.com/page?"); EXPECT_EQ("", uri.queryPart); EXPECT_TRUE(uri.query.empty()); }
TEST(splituri_fragment_query, empty_fragment) { auto uri = siddiqsoft::SplitUri("https://example.com/page#"); EXPECT_EQ("", uri.fragment); }
TEST(splituri_fragment_query, query_with_no_value) { auto uri = siddiqsoft::SplitUri("https://example.com/?flag"); EXPECT_EQ(1, uri.query.size()); EXPECT_TRUE(uri.query.contains("flag")); EXPECT_EQ("", uri.query.at("flag")); }
TEST(splituri_fragment_query, multiple_query_params) { auto uri = siddiqsoft::SplitUri("https://example.com/?a=1&b=2&c=3&d=4&e=5"); EXPECT_EQ(5, uri.query.size()); EXPECT_EQ("1", uri.query.at("a")); EXPECT_EQ("5", uri.query.at("e")); }

// ============================================================================
// Authority serialization (operator string)
// ============================================================================

TEST(splituri_authority_serialization, host_only) { siddiqsoft::AuthorityHttp<char> a; a.host = "example.com"; EXPECT_EQ("example.com", std::string(a)); }
TEST(splituri_authority_serialization, host_and_port) { siddiqsoft::AuthorityHttp<char> a; a.host = "example.com"; a.port = 8080; EXPECT_EQ("example.com:8080", std::string(a)); }
TEST(splituri_authority_serialization, user_host_port) { siddiqsoft::AuthorityHttp<char> a; a.userInfo = "admin"; a.host = "example.com"; a.port = 443; EXPECT_EQ("admin@example.com:443", std::string(a)); }
TEST(splituri_authority_serialization, user_password_host_port) { siddiqsoft::AuthorityHttp<char> a; a.userInfo = "admin"; a.password = "secret"; a.host = "example.com"; a.port = 443; EXPECT_EQ("admin:secret@example.com:443", std::string(a)); }
TEST(splituri_authority_serialization, user_no_password) { siddiqsoft::AuthorityHttp<char> a; a.userInfo = "admin"; a.host = "example.com"; EXPECT_EQ("admin@example.com", std::string(a)); }

// ============================================================================
// std::format support
// ============================================================================

TEST(splituri_format, format_scheme_http) { EXPECT_EQ("http", std::format("{}", siddiqsoft::UriScheme::WebHttp)); }
TEST(splituri_format, format_scheme_https) { EXPECT_EQ("https", std::format("{}", siddiqsoft::UriScheme::WebHttps)); }
TEST(splituri_format, format_scheme_ws) { EXPECT_EQ("ws", std::format("{}", siddiqsoft::UriScheme::WebSocket)); }
TEST(splituri_format, format_scheme_wss) { EXPECT_EQ("wss", std::format("{}", siddiqsoft::UriScheme::WebSocketSecure)); }
TEST(splituri_format, format_scheme_ftp) { EXPECT_EQ("ftp", std::format("{}", siddiqsoft::UriScheme::Ftp)); }
TEST(splituri_format, format_scheme_file) { EXPECT_EQ("file", std::format("{}", siddiqsoft::UriScheme::File)); }

TEST(splituri_format, format_authority_with_password)
{
    auto uri = siddiqsoft::SplitUri("https://user:pass@example.com:8443/");
    EXPECT_EQ("user:pass@example.com:8443", std::format("{}", uri.authority));
}

TEST(splituri_format, format_full_uri)
{
    auto uri = siddiqsoft::SplitUri("https://example.com/path?q=1#frag");
    EXPECT_EQ("https://example.com/path?q=1#frag", std::format("{}", uri));
}

// ============================================================================
// JSON serialization
// ============================================================================

TEST(splituri_json, basic_json_serialization)
{
    auto uri = siddiqsoft::SplitUri("https://user:pass@example.com:8443/api?key=val#sec");
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

TEST(splituri_json, json_scheme_enum_serialization)
{
    nlohmann::json j;
    j = siddiqsoft::UriScheme::WebHttp; EXPECT_EQ("http", j.get<std::string>());
    j = siddiqsoft::UriScheme::WebSocket; EXPECT_EQ("ws", j.get<std::string>());
    j = siddiqsoft::UriScheme::Ftp; EXPECT_EQ("ftp", j.get<std::string>());
    j = siddiqsoft::UriScheme::File; EXPECT_EQ("file", j.get<std::string>());
}

TEST(splituri_json, json_no_credentials)
{
    auto uri = siddiqsoft::SplitUri("https://example.com/");
    nlohmann::json doc = uri;
    EXPECT_EQ("", doc.at("authority").at("userInfo").get<std::string>());
    EXPECT_EQ("", doc.at("authority").at("password").get<std::string>());
}

// ============================================================================
// w2n helper
// ============================================================================

TEST(splituri_helpers, w2n_empty) { EXPECT_EQ("", siddiqsoft::w2n(L"")); }
TEST(splituri_helpers, w2n_basic) { EXPECT_EQ("hello", siddiqsoft::w2n(L"hello")); }
TEST(splituri_helpers, w2n_with_numbers) { EXPECT_EQ("test123", siddiqsoft::w2n(L"test123")); }

// ============================================================================
// Literal operator
// ============================================================================

TEST(splituri_literals_test, narrow_literal_basic)
{
    using namespace siddiqsoft::splituri_literals;
    auto uri = "https://example.com/"_Uri;
    EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, uri.scheme);
    EXPECT_EQ("example.com", uri.authority.host);
}

TEST(splituri_literals_test, narrow_literal_complex)
{
    using namespace siddiqsoft::splituri_literals;
    auto uri = "ws://chat.example.com:9090/room?id=42#messages"_Uri;
    EXPECT_EQ(siddiqsoft::UriScheme::WebSocket, uri.scheme);
    EXPECT_EQ("chat.example.com", uri.authority.host);
    EXPECT_EQ(9090, uri.authority.port);
    EXPECT_EQ("room", uri.path.at(0));
    EXPECT_EQ("42", uri.query.at("id"));
    EXPECT_EQ("messages", uri.fragment);
}

// ============================================================================
// Uri::string() rebuild
// ============================================================================

TEST(splituri_rebuild, rebuild_from_components)
{
    siddiqsoft::Uri<> uri;
    uri.scheme = siddiqsoft::UriScheme::WebHttps;
    uri.authority.host = "example.com";
    uri.authority.port = 443;
    uri.urlPart = "/path";
    auto result = uri.string();
    EXPECT_EQ("https://example.com:443/path", result);
}

TEST(splituri_rebuild, rebuild_with_credentials)
{
    siddiqsoft::Uri<> uri;
    uri.scheme = siddiqsoft::UriScheme::WebHttps;
    uri.authority.userInfo = "admin";
    uri.authority.password = "secret";
    uri.authority.host = "example.com";
    uri.authority.port = 443;
    uri.urlPart = "/";
    auto result = uri.string();
    EXPECT_EQ("https://admin:secret@example.com:443/", result);
}

TEST(splituri_rebuild, rebuild_user_no_password)
{
    siddiqsoft::Uri<> uri;
    uri.scheme = siddiqsoft::UriScheme::WebHttps;
    uri.authority.userInfo = "admin";
    uri.authority.host = "example.com";
    uri.authority.port = 443;
    uri.urlPart = "/";
    auto result = uri.string();
    EXPECT_EQ("https://admin@example.com:443/", result);
}

TEST(splituri_rebuild, parsed_uri_returns_original)
{
    std::string original = "https://example.com:8080/path?q=1#frag";
    auto uri = siddiqsoft::SplitUri(original);
    EXPECT_EQ(original, uri.string());
}
