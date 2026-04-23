SplitUri : Parse Uri Utility
-------------------------------------------
<!-- badges -->
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.SplitUri?branchName=main)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=14&branchName=main)
[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.SplitUri?branchName=develop)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=14&branchName=develop)
![](https://img.shields.io/nuget/v/SiddiqSoft.SplitUri)
![](https://img.shields.io/github/v/tag/SiddiqSoft/SplitUri)
![](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/14)
<!--![](https://img.shields.io/azure-devops/coverage/siddiqsoft/siddiqsoft/14)-->
<!-- end badges -->

# Objective

Parse the Uri for HTTP resources with support for `std::format` and `nlohmann.json` serializers.

Conforms to the [WHATWG URL Standard](https://url.spec.whatwg.org/) specification.

While there are many, many libraries available, the goal of this implementation is to make *usage* easy.

## Features
- **WHATWG URL Standard conformance** including:
  - All special schemes: `ftp` (port 21), `file`, `http` (port 80), `https` (port 443), `ws` (port 80), `wss` (port 443)
  - Case-insensitive scheme parsing (e.g., `HTTPS://` is recognized as `https`)
  - Host lowercasing for special schemes per the domain-to-ASCII algorithm
  - Separate `username` and `password` credential fields
  - Leading/trailing C0 control and space character stripping
  - ASCII tab and newline removal from input
  - Backslash (`\`) normalization to forward slash (`/`) for special schemes
  - IPv6 address support in bracket notation `[::1]`
  - Path dot-segment resolution (`.` and `..` segments)
  - Port validation (must be 0-65535, out-of-range results in validation error)
  - `defaultPortForScheme()` and `isSpecialScheme()` utility functions
- Literals support so you can use `auto u= "https://www.siddiqsoft.com/"_Uri;`
- Formatters for `std::format` as well as serializers for `nlohmann.json` library.
- Support for `std::string` and `std::wstring`
- Cross-platform header-only with googletest framework for tests. _Build and tests are for Visual Studio 2019 under x64._

Please refer to the [documentation](https://siddiqsoft.github.io/SplitUri/).

## WHATWG URL Standard

This library conforms to the [WHATWG URL Standard](https://url.spec.whatwg.org/) with the following key behaviors:

### Special Schemes (§4.2)
| Scheme | Default Port |
|--------|-------------|
| `ftp`  | 21          |
| `file` | _(none)_    |
| `http` | 80          |
| `https`| 443         |
| `ws`   | 80          |
| `wss`  | 443         |

### URL Components
```
scheme://username:password@host:port/path?query#fragment
```

### Key Conformance Points
- **Scheme**: Parsed case-insensitively and stored as the canonical lowercase form
- **Host**: Lowercased for special schemes (domain-to-ASCII)
- **Port**: Validated as 16-bit unsigned integer (0-65535); out-of-range ports result in port being set to 0
- **Credentials**: `username` and `password` are parsed as separate fields from `user:pass@host`
- **Input sanitization**: Leading/trailing C0 control characters and spaces are stripped; ASCII tabs and newlines are removed
- **Path resolution**: Single-dot (`.`) and double-dot (`..`) path segments are resolved
- **Backslash normalization**: `\` is treated as `/` in path parsing for special schemes

## Usage

> Breaking change with `v1.9.0` the literals namespace has been changed to `siddiqsoft::splituri_literals`.

> Breaking change with WHATWG conformance: `AuthorityHttp` now has a separate `password` field. The `userInfo` field contains only the username. Host names are lowercased for special schemes.

### New Scheme Types
```cpp
// WebSocket support
auto ws = siddiqsoft::SplitUri("ws://example.com:9090/chat");
// ws.scheme == UriScheme::WebSocket
// ws.authority.port == 9090

// WebSocket Secure support
auto wss = siddiqsoft::SplitUri("wss://example.com/chat");
// wss.scheme == UriScheme::WebSocketSecure
// wss.authority.port == 443

// FTP support
auto ftp = siddiqsoft::SplitUri("ftp://files.example.com/pub/readme.txt");
// ftp.scheme == UriScheme::Ftp
// ftp.authority.port == 21
```

### Password Parsing
```cpp
auto uri = siddiqsoft::SplitUri("https://user:secret@example.com/");
// uri.authority.userInfo == "user"
// uri.authority.password == "secret"
// uri.authority.host == "example.com"
```

### Utility Functions
```cpp
// Check if a scheme is special per WHATWG
bool special = siddiqsoft::isSpecialScheme(siddiqsoft::UriScheme::WebHttps); // true

// Get default port for a scheme
uint16_t port = siddiqsoft::defaultPortForScheme(siddiqsoft::UriScheme::WebHttps); // 443
```

<p align="right">
&copy; 2021 Siddiq Software LLC. All rights reserved.
</p>
