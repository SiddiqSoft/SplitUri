SplitUri : Parse Uri Utility
-------------------------------------------
<!-- badges -->
[![CodeQL](https://github.com/SiddiqSoft/SplitUri/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/SiddiqSoft/SplitUri/actions/workflows/codeql-analysis.yml)
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
- Literals support so you can use `auto u= "https://www.siddiqsoft.com/"_Uri;`
- Formatters for `std::format` as well as serializers for `nlohmann.json` library.
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

auto u = "https://www.google.com/search?flag&q=siddiqsoft#v1"_Uri;

std::cerr << u.authority.host << std::endl;
std::cerr << u.authority.port << std::endl;
std::cerr << u.urlPart << std::endl;
std::cerr << u.queryPart << std::endl;
std::cerr << u.fragment << std::endl;
std::cerr << nlohmann::json(u.path).dump() << std::endl;
std::cerr << nlohmann::json(u.query).dump() << std::endl;
std::cerr << std::format("{}", u.scheme) << "...." << nlohmann::json(u.scheme).dump() << std::endl;
std::cerr << std::format("{}", u.authority) << std::endl;
std::cerr << std::format("{}", u) << std::endl;
```

And the corresponding output

```json
www.google.com
443
/search?flag&q=siddiqsoft#v1
flag&q=siddiqsoft
v1
["search"]
{"flag":"", "q":"siddiqsoft"}
https...."https"
www.google.com:443
https://www.google.com/search?flag&q=siddiqsoft#v1
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
`queryPart` | string | Section of the uri past the `?` and upto the fragment delimiter
`url` | string | Short-cut of the path, query and fragment section
`urlPart` | string | Section of the uri past the host until end of string
`fragment` | string | Section of the uri containing the fragment


### SplitUri()

```cpp
template <class CharT = char, class Auth = AuthorityHttp<CharT>>
  requires std::same_as<char, CharT> || std::same_as<wchar_t, CharT>
Uri<CharT, Auth> static SplitUri(const std::basic_string<CharT>& aEndpoint);
```

<hr/>

# Roadmap

- Support the other URI schemes.

<p align="right">
&copy; 2021 Siddiq Software LLC. All rights reserved.
</p>
