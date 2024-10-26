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


namespace siddiqsoft
{
	TEST(examples, example_1_narrow)
	{
		using namespace siddiqsoft::splituri_literals;

		auto u = "https://www.google.com/search?q=siddiqsoft&from=example_1_narrow#v1"_Uri;

		EXPECT_EQ("www.google.com", u.authority.host);
		std::cerr << u.authority.host << std::endl;

		EXPECT_EQ(443, u.authority.port);
		std::cerr << u.authority.port << std::endl;

		EXPECT_EQ("/search?q=siddiqsoft&from=example_1_narrow#v1", u.urlPart);
		std::cerr << u.urlPart << std::endl;

		EXPECT_EQ("q=siddiqsoft&from=example_1_narrow", u.queryPart);
		std::cerr << u.queryPart << std::endl;

		EXPECT_EQ("v1", u.fragment);
		std::cerr << u.fragment << std::endl;

		EXPECT_EQ("search", u.path.at(0));
		nlohmann::json dp = u.path;
		std::cerr << dp.dump() << std::endl;
		EXPECT_EQ("search", dp[0].get<std::string>());

		EXPECT_EQ("siddiqsoft", u.query.at("q"));
		nlohmann::json dq = u.query;
		std::cerr << dq.dump() << std::endl;
		EXPECT_EQ("siddiqsoft", dq.value("q", ""));
		EXPECT_EQ("example_1_narrow", dq.value("from", ""));

		// Checks that both serializers are available (caught at compile-time)
		EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
		std::cerr << std::format("{}", u.scheme) << "....";
		std::cerr << nlohmann::json(u.scheme).dump() << std::endl;

		// Note that despite the initial uri string skipping the port, the SplitUri decodes and stores the port
		EXPECT_EQ("www.google.com:443", std::format("{}", u.authority));
		std::cerr << std::format("{}", u.authority) << std::endl;

		// The "rebuilt" endpoint
		EXPECT_EQ("https://www.google.com/search?q=siddiqsoft&from=example_1_narrow#v1", std::format("{}", u));
		std::cerr << std::format("{}", u) << std::endl;
	}


	TEST(examples, example_1_wide)
	{
		using namespace siddiqsoft::splituri_literals;

		auto u = L"https://www.google.com/search?q=siddiqsoft&from=example_1_wide#v1"_Uri;

		EXPECT_EQ(L"www.google.com", u.authority.host);
		std::wcerr << u.authority.host << std::endl;

		EXPECT_EQ(443, u.authority.port);
		std::wcerr << u.authority.port << std::endl;

		EXPECT_EQ(L"/search?q=siddiqsoft&from=example_1_wide#v1", u.urlPart);
		std::wcerr << u.urlPart << std::endl;

		EXPECT_EQ(L"q=siddiqsoft&from=example_1_wide", u.queryPart);
		std::wcerr << u.queryPart << std::endl;

		EXPECT_EQ(L"v1", u.fragment);
		std::wcerr << u.fragment << std::endl;

		EXPECT_EQ(L"search", u.path.at(0));
		nlohmann::json dp;   //= u.path; // does not work due as json lib does not support wstring
		to_json(dp, u.path); // we must explicity convert
		std::cerr << dp.dump() << std::endl;
		EXPECT_EQ("search", dp[0].get<std::string>());

		EXPECT_EQ(L"siddiqsoft", u.query.at(L"q"));
		nlohmann::json dq;    //= u.query; // does not work due as json lib does not support wstring
		to_json(dq, u.query); // we must explicity convert
		std::cerr << dq.dump() << std::endl;
		EXPECT_EQ("siddiqsoft", dq.value("q", ""));
		EXPECT_EQ("example_1_wide", dq.value("from", ""));

		// Checks that both serializers are available (caught at compile-time)
		EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
		std::wcerr << std::format(L"{}", u.scheme) << L"....";
		std::cerr << nlohmann::json(u.scheme).dump() << std::endl;

		// Note that despite the initial uri string skipping the port, the SplitUri decodes and stores the port
		EXPECT_EQ(L"www.google.com:443", std::format(L"{}", u.authority));
		std::wcerr << std::format(L"{}", u.authority) << std::endl;

		// The "rebuilt" endpoint
		EXPECT_EQ(L"https://www.google.com/search?q=siddiqsoft&from=example_1_wide#v1", std::format(L"{}", u));
		std::wcerr << std::format(L"{}", u) << std::endl;
	}
} // namespace siddiqsoft