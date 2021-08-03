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

# Requirements
- C++20 compliant toolchain: We use `<concepts>`, `<format>`.
- The `nlohmann.json` library.
- The build and tests are for Visual Studio 2019 under x64.

# Usage
- Use the nuget [SiddiqSoft.SplitUri](https://www.nuget.org/packages/SiddiqSoft.SplitUri/)
- Copy paste..whatever works.

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
