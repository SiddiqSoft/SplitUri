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


TEST(helpers_splituri, test_1)
{
	using namespace std;

	auto uri = siddiqsoft::SplitUri<>("http://search.msn.com:8080/results.asp?RS=CHECKED&FORM=MSNH&v=1&q=wininet"s);
	EXPECT_EQ("search.msn.com", uri.authority.host);
	EXPECT_EQ(8080, uri.authority.port);
	EXPECT_EQ("/results.asp?RS=CHECKED&FORM=MSNH&v=1&q=wininet", uri.url);

	std::cerr << "Re-serialized: " << std::string(uri) << std::endl;

	nlohmann::json doc = uri;
	EXPECT_TRUE(doc.is_object());
	std::cerr << doc.dump(3) << std::endl;
	EXPECT_EQ("results.asp", doc.value("/path/0"_json_pointer, ""));
}

TEST(helpers_splituri, test_2)
{
	auto uri = siddiqsoft::SplitUri<std::string>("http://search.msn.com:8080");
	EXPECT_EQ("search.msn.com", uri.authority.host);
	EXPECT_EQ(8080, uri.authority.port);
	EXPECT_EQ("", uri.url);

	std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}

TEST(helpers_splituri, test_3)
{
	auto uri = siddiqsoft::SplitUri<std::string>("http://search.msn.com");
	EXPECT_EQ("search.msn.com", uri.authority.host);
	EXPECT_EQ(80, uri.authority.port);
	EXPECT_EQ("", uri.url);

	std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri, test_4a)
{
	auto uri = siddiqsoft::SplitUri<std::string>("http://m.co");
	EXPECT_EQ("m.co", uri.authority.host);
	EXPECT_EQ(80, uri.authority.port);
	EXPECT_EQ("", uri.url);

	std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri, test_4b)
{
	auto uri = siddiqsoft::SplitUri<std::string>("https://m.co");
	EXPECT_EQ("m.co", uri.authority.host);
	EXPECT_EQ(443, uri.authority.port);
	EXPECT_EQ("", uri.url);

	std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri, test_5a)
{
	auto uri = siddiqsoft::SplitUri<std::string>("http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/"
	                                             "model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5");
	EXPECT_EQ("<ServerName>", uri.authority.host);
	EXPECT_EQ(80, uri.authority.port);
	EXPECT_EQ("/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')?Ranges('Sheet1!A1')=5.5",
	          uri.url);

	std::cerr << "Re-serialized: " << std::string(uri) << std::endl;

	nlohmann::json doc = uri;
	EXPECT_TRUE(doc.is_object());
	std::cerr << doc.dump(3) << std::endl;
	EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri, test_5b)
{
	auto uri = siddiqsoft::SplitUri<std::string>("http://<ServerName>/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/"
	                                             "model/Charts('Chart%201')/?Ranges('Sheet1!A1')=5.5");
	EXPECT_EQ("<ServerName>", uri.authority.host);
	EXPECT_EQ(80, uri.authority.port);
	EXPECT_EQ("/_vti_bin/ExcelRest.aspx/Docs/Documents/sampleWorkbook.xlsx/model/Charts('Chart%201')/?Ranges('Sheet1!A1')=5.5",
	          uri.url);

	std::cerr << "Re-serialized: " << std::string(uri) << std::endl;

	nlohmann::json doc = uri;
	EXPECT_TRUE(doc.is_object());
	std::cerr << doc.dump(3) << std::endl;
	EXPECT_EQ("Charts('Chart%201')", doc.value("/path/6"_json_pointer, ""));
}

TEST(helpers_splituri, test_6a)
{
	auto uri = siddiqsoft::SplitUri<std::string>(
			"https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top");
	EXPECT_EQ("www.example.com", uri.authority.host);
	EXPECT_EQ("john.doe", uri.authority.userInfo);
	EXPECT_EQ(123, uri.authority.port);
	EXPECT_EQ("/forum/questions/?tag=networking&order=newest#top", uri.url);
	EXPECT_EQ(2, uri.path.size());
	EXPECT_EQ(2, uri.query.size());
	EXPECT_EQ("top", uri.fragment);

	std::cerr << "Re-serialized: " << std::string(uri) << std::endl;
}


TEST(helpers_splituri, test_6b)
{
	using namespace std;

	auto uri = siddiqsoft::SplitUri(
			"https://john.doe@www.example.com:123/forum/questions?tag=networking&order=newest#top"s);
	EXPECT_EQ("www.example.com", uri.authority.host);
	EXPECT_EQ("john.doe", uri.authority.userInfo);
	EXPECT_EQ(123, uri.authority.port);
	EXPECT_EQ("/forum/questions?tag=networking&order=newest#top", uri.url);
	EXPECT_EQ(2, uri.path.size());
	EXPECT_EQ(2, uri.query.size());
	EXPECT_EQ("top", uri.fragment);

	std::cerr << "Re-serialized: " << std::string(uri) << std::endl;

	nlohmann::json doc = uri;
	EXPECT_TRUE(doc.is_object());
	std::cerr << doc.dump(3) << std::endl;
	EXPECT_EQ("questions", doc.value("/path/1"_json_pointer, ""));
}


TEST(helpers_splituri, test_6c)
{
	using namespace std;

	auto uri = siddiqsoft::SplitUri("https://john.doe@www.example.com/forum/questions?tag=networking&order=newest#top"s);
	EXPECT_EQ("www.example.com", uri.authority.host);
	EXPECT_EQ("john.doe", uri.authority.userInfo);
	EXPECT_EQ(443, uri.authority.port);
	EXPECT_EQ("/forum/questions?tag=networking&order=newest#top", uri.url);
	EXPECT_EQ(2, uri.path.size());
	EXPECT_EQ(2, uri.query.size());
	EXPECT_EQ("top", uri.fragment);

	std::cerr << "Re-serialized: " << std::string(uri) << std::endl;

	nlohmann::json doc = uri;
	EXPECT_TRUE(doc.is_object());
	std::cerr << doc.dump(3) << std::endl;
	EXPECT_EQ("questions", doc.value("/path/1"_json_pointer, ""));
}
