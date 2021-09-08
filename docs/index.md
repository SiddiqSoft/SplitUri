# SiddiqSoft.SplitUri

Header-only C++20 library for parsing Uri.

<!-- badges -->
[![CodeQL](https://github.com/SiddiqSoft/SplitUri/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/SiddiqSoft/SplitUri/actions/workflows/codeql-analysis.yml)
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.SplitUri?branchName=main)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=14&branchName=main)
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.SplitUri?branchName=develop)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=14&branchName=develop)
![](https://img.shields.io/nuget/v/SiddiqSoft.SplitUri)
![](https://img.shields.io/github/v/tag/SiddiqSoft/SplitUri)
![](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/14)
<!--![](https://img.shields.io/azure-devops/coverage/siddiqsoft/siddiqsoft/14)-->
<!-- end badges -->

# Getting started

- This library does not use any Windows-specific code. However, use Visual Studio 2019 v16.11.2 or newer as support for `<format>` is [not present](https://en.cppreference.com/w/cpp/20) for GCC or Clang!
- On Windows with VisualStudio, use the Nuget package! 
- Make sure you use `c++latest` as the `<format>` is no longer in the `c++20` option pending ABI resolution.

# Dependencies

None. However, if you're using the [`nlohmann.json`](https://github.com/nlohmann.json/) library then we will enable the serializers for that library.

<hr/>


# API

The [anatomy of a Uri](https://en.wikipedia.org/wiki/Uniform_Resource_Identifier#Syntax) can be summarized as follows:

```
           userinfo       host      port
           ┌──┴───┐ ┌──────┴──────┐ ┌┴┐
   https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top
   └─┬─┘   └───────────┬──────────────┘└───────┬───────┘ └───────────┬─────────────┘ └┬┘
   scheme          authority                  path                 query           fragment
```

In this library, we focus on the `http` and `https` scheme.

<hr/>

## class `siddiqsoft::Uri`

### Signature

```cpp
template <typename CharT = char, class Auth = AuthorityHttp<CharT>>
    requires (same_as<char, CharT> || same_as<wchar_t, CharT>) &&
              same_as<Auth, AuthorityHttp<CharT>>
class Uri
{
private:
    basic_string<CharT>                           sourceUri {};

public:
    UriScheme                                     scheme {UriScheme::WebHttp};
    Auth                                          authority {};
    vector<std::basic_string<CharT>>              path {};
    map<basic_string<CharT>, basic_string<CharT>> query {};
    basic_string<CharT>                           fragment {};
    basic_string<CharT>                           urlPart {};
    basic_string<CharT>                           queryPart {};

    Uri();
    Uri(const basic_string<CharT>& aEndpoint);

                              operator basic_string<CharT>() const;
    const basic_string<CharT> string() const;
    void                      split(const basic_string<CharT>& aEndpoint);
}
```

We use `<concepts>` `requires` to ensure that we only get `std::string` or `std::wstring`. When possible we can update this library to support `char8_t`.
### Member Variables

Variable | Description
---------|-------------
`std::basic_string<CharT>`<br/>-_`sourceUri`_ | Stores the original string
`UriScheme`<br/>-_`scheme`_ | Defaults to `{UriScheme::WebHttp}`.<br/>Represents the [scheme](#enum-urischeme) for this Uri.
`Auth`<br/>-_`authority`_ | Template type represents the authority. Current implementation defaults to `AuthorityHttp<>` and must not be changed. We've limited the template to check via the `requires`.
`std::vector<std::basic_string<CharT>>`<br/>-_`path`_ | An array of strings representing the segments in the path.
`std::map<std::basic_string<CharT>,std::basic_string<CharT>>`<br/>-_`query`_ | A collection of key-value elements in the query section.
`std::basic_string<CharT>`<br/>-_`fragment`_ | The fragment segment of the Uri.
`std::basic_string<CharT>`<br/>-_`urlPart`_ | Shortcut to the url segment (balance post the authority section).
`std::basic_string<CharT>`<br/>-_`queryPart`_ | Shortcut to the query segment.

### Member Functions

#### Uri::Uri
```cpp
    Uri();
```
Default (empty) constructor.

#### `Uri::split`
```cpp
    static Uri<CharT, Auth> split(const std::basic_string<CharT>&) noexcept(false);
```
<small>since v1.8.0</small> 
This method performs the parsing, splitting and general decomposition of the source string.<br/>It may throw!

##### returns
A Uri<> object

#### `Uri::Uri`
```cpp
    Uri(const std::basic_string<CharT>&) noexcept;
```
Delegates to the [`split`](#split) function.

#### `Uri::operator basic_string()`
```cpp
    operator basic_string<CharT>() const;
```
Convenience delgates to `string()`.

##### returns
A `std::basic_string<CharT>` is returned.

#### `Uri::string`
```cpp
    const std::basic_string<CharT> string() const;
```
Rebuild the Uri string.

##### returns

Returns the `sourceUri` member variable or rebuilds the string via std::format.

<hr/>

## struct `siddiqsoft::AuthorityHttp`

### Signature
```cpp
    template <typename CharT>
        requires std::same_as<char, CharT> || std::same_as<wchar_t, CharT>
    struct AuthorityHttp
    {
        std::basic_string<CharT> userInfo {};
        std::basic_string<CharT> host {};
        uint16_t                 port {0};

        operator std::basic_string<CharT>() const
    };
```

#### Member Variables

Variable | Description
---------|-------------
`std::basic_string<CharT>`<br/>-_`userInfo`_ | The fragment segment of the Uri.
`std::basic_string<CharT>`<br/>-_`host`_ | Shortcut to the url segment (balance post the authority section).
`uint16_t`<br/>-_`port`_ | Shortcut to the query segment.

#### Member Functions

```cpp
    operator std::basic_string<CharT>() const
```

<hr/>

## enum `siddiqsoft::UriScheme`

### Signature

```cpp
enum class UriScheme
{
    WebHttp,  WebHttps,  Ldap,  Mailto,  News,  Tel,  Telnet,  Urn,  Unknown
};
```

<hr/>

## static `siddiqsoft::SplitUri`
```cpp
    template <typename T = char>
        requires(std::same_as<char, T> || std::same_as<wchar_t, T>)
    static Uri<T, AuthorityHttp<T>> SplitUri(const std::basic_string<T>& aEndpoint);

    template <typename T = char>
        requires(std::same_as<char, T> || std::same_as<wchar_t, T>)
    static Uri<T, AuthorityHttp<T>> SplitUri(const T* aEndpoint);
```

This convenience wrapper instantiates a [`Uri<>`](#siddiqsofturi) object via its constructor.

> NOTE:
> There is a `SplitUri(const T*)` in addition to the `SplitUri(const std::basic_string<T>&)` to account for failed implict conversion between T* into basic_string<T>.


## static `siddiqsoft::w2n`

### Signature
```cpp
    [[nodiscard]] static inline std::string w2n(const std::wstring& ws);
```
Wrapper atop `wcstombs_s` with limits on 256 characters as the destination. Used internally by the various serializer functions.
It is meant to be used internally to handle conversion of small data sets.

<hr/>

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

## Notes

There are two internal functions used by the library. They may be used as you wish but note the inline documentation and accomodate accordingly.

- `_NORW` is a macro that I gleamed from the `<string>` header. Basically it allows us to use a single narrow string in cases such as format strings and other non-Unicode-sensitive strings without the ugly `L""` prefix or having to write an explicit case for `char` and `wchar_t`.
- `w2n` is a wrapper for `wcstombs_s` with limits on 256 characters as the destination. It is meant to be used internally to handle conversion of small data.
- 