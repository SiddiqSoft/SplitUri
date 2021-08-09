SplitUri : Parse Uri Utility
-------------------------------------------
<!-- badges -->
<!--[![CodeQL](https://github.com/SiddiqSoft/SplitUri/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/SiddiqSoft/SplitUri/actions/workflows/codeql-analysis.yml)-->
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.SplitUri?branchName=main)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=14&branchName=main)
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.SplitUri?branchName=develop)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=14&branchName=develop)
![](https://img.shields.io/nuget/v/SiddiqSoft.SplitUri)
![](https://img.shields.io/github/v/tag/SiddiqSoft/SplitUri)
![](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/14)
<!--![](https://img.shields.io/azure-devops/coverage/siddiqsoft/siddiqsoft/14)-->
<!-- end badges -->

# Objective

Parse the Uri for HTTP resources with support for `format` and `nlohmann.json`.

While there are many, many libraries available, the goal of this implementation is to make *usage* easy along with a modern design.

## Features
- C++20 compliant toolchain: We use `<concepts>`, `<format>`.
- The `nlohmann.json` library.
- Support for `std::string` and `std::wstring`
- The build and tests are for Visual Studio 2019 under x64.

# Usage
- Use the nuget [SiddiqSoft.SplitUri](https://www.nuget.org/packages/SiddiqSoft.SplitUri/)
- Copy paste..whatever works.

## Examples

```cpp
#include "siddiqsoft/SplitUri.hpp"
..
..
using namespace siddiqsoft::literals;
// Use the literal operator helper.
auto uri= "https://www.google.com/search?q=siddiqsoft"_Uri;
// Outputs https://www.google.com/search?q=siddiqsoft
std::cout << std::format("{}", uri) << std::endl;
```
A more through example

```cpp
using namespace siddiqsoft::literals;

auto u = "https://www.google.com/search?q=siddiqsoft#v1"_Uri;

EXPECT_EQ("www.google.com", u.authority.host);
std::cerr << u.authority.host << std::endl;

EXPECT_EQ(443, u.authority.port);
std::cerr << u.authority.port << std::endl;

EXPECT_EQ("/search?q=siddiqsoft#v1", u.urlPart);
std::cerr << u.urlPart << std::endl;

EXPECT_EQ("q=siddiqsoft", u.queryPart);
std::cerr << u.queryPart << std::endl;

EXPECT_EQ("v1", u.fragment);
std::cerr << u.fragment << std::endl;

EXPECT_EQ("search", u.path.at(0));
std::cerr << nlohmann::json(u.path).dump() << std::endl;

EXPECT_EQ("siddiqsoft", u.query.at("q"));
std::cerr << nlohmann::json(u.query).dump() << std::endl;

// Checks that both serializers are available (caught at compile-time)
EXPECT_EQ(siddiqsoft::UriScheme::WebHttps, u.scheme);
std::cerr << std::format("{}", u.scheme) << "...." << nlohmann::json(u.scheme).dump() << std::endl;

// Note that despite the initial uri string skipping the port, the SplitUri decodes and stores the port
EXPECT_EQ("www.google.com:443", std::format("{}", u.authority));
std::cerr << std::format("{}", u.authority) << std::endl;

// The "rebuilt" endpoint
EXPECT_EQ("https://www.google.com/search?q=siddiqsoft#v1", std::format("{}", u));
std::cerr << std::format("{}", u) << std::endl;
```

And the corresponding output

```json
www.google.com
443
/search?q=siddiqsoft#v1
q=siddiqsoft
v1
["search"]
{"q":"siddiqsoft"}
https...."https"
www.google.com:443
https://www.google.com/search?q=siddiqsoft#v1
```

<hr/>

From [Wikipedia](https://en.wikipedia.org/wiki/Uniform_Resource_Identifier#Syntax) we have the following structure of Uri.

			  userinfo       host      port
			  ┌──┴───┐ ┌──────┴──────┐ ┌┴┐
	  https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top
	  └─┬─┘   └───────────┬──────────────┘└───────┬───────┘ └───────────┬─────────────┘ └┬┘
	  scheme          authority                  path                 query           fragment

## API

> Current implementation only supports the `http` and `https` scheme.


Uri       |
---------:|:-----------------
`scheme` | UriScheme | Enumeration of the Uri schemes. We currently only support http scheme.
`authority` | Auth  | template argument defaulting to AuthorityHttp
`path` | vector<> | Array of strings each containing the path element
`query` | map<> | Map of query elements
`url` | string | Short-cut of the path, query and fragment section

```cpp
template <class T = std::string, class Auth = AuthorityHttp<T>>
requires (std::same_as<std::string, T> || std::same_as<std::wstring, T>) && std::same_as<AuthorityHttp<T>, Auth>
struct Uri
{
    UriScheme  scheme;
    Auth       authority;
    vector<T>  path {};
    map<T, T>  query {};
    T          url {}; // contains the "balance" post Authority section

    operator T();

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Uri<T>, scheme, authority, url, path, query, fragment);
};
```

### SplitUri()

```cpp
template <class T = std::string, class Auth = AuthorityHttp<T>>
requires std::same_as<std::string, T> || std::same_as<std::wstring, T>
Uri<T, Auth> static SplitUri(const T& aEndpoint);
```


<hr/>

# Roadmap

- Update to handle std::wstring.
- Support the other URI schemes.

<p align="right">
&copy; 2021 Siddiq Software LLC. All rights reserved.
</p>
