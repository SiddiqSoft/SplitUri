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

#pragma once

#ifndef SPLITURI_HPP
#define SPLITURI_HPP


#include <string>
#include <type_traits>
#include <format>
#include <concepts>
#include <ranges>
#include <map>
#include <vector>
#include <algorithm>
#include <optional>
#include <array>
#include <cstdint>

#if defined(UNIX) || !defined(_WIN32)
#include <codecvt>
#include <locale>
#endif

namespace siddiqsoft
{
#if !defined(_NORW)
    /// @brief In support of the macro NORW which allows us to declare/use narrow/wide strings as needed. Plucked from the MS stl
    /// implementation
    template <class _CharT>
        requires std::same_as<_CharT, char> || std::same_as<_CharT, wchar_t>
    [[nodiscard]] constexpr const _CharT* NorW(const char* const _Str, const wchar_t* const _WStr) noexcept
    {
        if constexpr (std::is_same_v<_CharT, char>) {
            return _Str;
        }
        else {
            return _WStr;
        }
    }
#define _NORW(_CharT, _Literal) (NorW<_CharT>(_Literal, L##_Literal))
#endif

    /// @brief Helper function takes wstring and converts to string
    /// WARNING! This is *not* a general method and is fixed to operate on data that is known to be well within the limit of 256
    /// chars: email addresses and host names.
    /// @param ws wstring
    /// @return string
    [[nodiscard]] static inline std::string w2n(const std::wstring& ws)
    {
        if (!ws.empty()) {
#ifdef _WIN32
            size_t convertedCount {};
            char   ns[256] {};
            return 0 == wcstombs_s(&convertedCount, ns, 255, ws.c_str(), 255) ? ns : std::string {};
#else
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            return converter.to_bytes(ws);
#endif
        }
        return {};
    }

    /*
        WHATWG URL Standard - https://url.spec.whatwg.org/

        A URL's components per the WHATWG URL Standard:

          scheme://username:password@host:port/path?query#fragment

        Special schemes and their default ports (WHATWG Section 4.2):
          "ftp"    -> 21
          "file"   -> null
          "http"   -> 80
          "https"  -> 443
          "ws"     -> 80
          "wss"    -> 443

        Key conformance points:
        - Schemes are ASCII lowercased during parsing
        - Port is null when it equals the scheme's default port
        - Username and password are separate fields
        - Leading/trailing C0 control and space characters are stripped
        - ASCII tab and newline characters are removed from input
        - Backslash (\) is treated as forward slash (/) in special scheme URLs
        - IPv6 addresses are supported in bracket notation [::1]
        - Path segments "." and ".." are resolved
        - Percent-encoding is applied per WHATWG percent-encode sets
    */

    /// @brief AuthorityHttp type
    /// @tparam T std::string or std::wstring
    template <typename CharT>
        requires std::same_as<char, CharT> || std::same_as<wchar_t, CharT>
    struct AuthorityHttp
    {
        std::basic_string<CharT> userInfo {};
        std::basic_string<CharT> password {};
        std::basic_string<CharT> host {};
        uint16_t                 port {0}; // maximum of 65535

        operator std::basic_string<CharT>() const
        {
            std::basic_string<CharT> ret;
            // WHATWG: username:password@host:port
            if (!userInfo.empty()) {
                ret += userInfo;
                if (!password.empty()) {
                    ret += _NORW(CharT, ":");
                    ret += password;
                }
                ret += _NORW(CharT, "@");
            }
            ret += host;
            if (port > 0) {
                ret += _NORW(CharT, ":");
                if constexpr (std::is_same_v<CharT, char>) ret += std::to_string(port);
                if constexpr (std::is_same_v<CharT, wchar_t>) ret += std::to_wstring(port);
            }
            return ret;
        }
    };

#ifdef NLOHMANN_JSON_VERSION_MAJOR
    static void to_json(nlohmann::json& dest, const AuthorityHttp<char>& s)
    {
        dest["userInfo"] = s.userInfo;
        dest["password"] = s.password;
        dest["host"]     = s.host;
        dest["port"]     = s.port;
    }

    static void to_json(nlohmann::json& dest, const AuthorityHttp<wchar_t>& s)
    {
        dest["userInfo"] = w2n(s.userInfo);
        dest["password"] = w2n(s.password);
        dest["host"]     = w2n(s.host);
        dest["port"]     = s.port;
    }
#endif

    /// @brief AuthorityLdap type
    /// @tparam T std::string or std::wstring
    template <typename CharT>
        requires std::same_as<CharT, char> || std::same_as<CharT, wchar_t>
    struct AuthorityLdap
    {
        std::basic_string<CharT> host {};

        operator std::basic_string<CharT>() const { return std::format(_NORW(CharT, "[{}]"), host); }
    };


    /// @brief AuthorityNone type
    /// @tparam T std::string or std::wstring
    template <typename CharT>
        requires std::same_as<char, CharT> || std::same_as<wchar_t, CharT>
    struct AuthorityNone
    {
        std::basic_string<CharT> none {};

        operator std::basic_string<CharT>() const { return none; }

#ifdef NLOHMANN_JSON_VERSION_MAJOR
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AuthorityNone<CharT>, none);
#endif // NLOHMANN_JSON_VERSION_MAJOR
    };

    /// @brief URI scheme enum
    /// Per WHATWG URL Standard Section 4.2, the special schemes are: ftp, file, http, https, ws, wss
    enum class UriScheme
    {
        WebHttp,
        WebHttps,
        WebSocket,
        WebSocketSecure,
        Ftp,
        File,
        Ldap,
        Mailto,
        News,
        Tel,
        Telnet,
        Urn,
        Unknown
    };

    template <typename CharT>
    const std::basic_string<CharT> to_string(const UriScheme& s)
    {
        static const std::map<UriScheme, std::tuple<std::string, std::wstring>> UriSchemeMap {
                {UriScheme::WebHttp, {"http", L"http"}},
                {UriScheme::WebHttps, {"https", L"https"}},
                {UriScheme::WebSocket, {"ws", L"ws"}},
                {UriScheme::WebSocketSecure, {"wss", L"wss"}},
                {UriScheme::Ftp, {"ftp", L"ftp"}},
                {UriScheme::File, {"file", L"file"}},
                {UriScheme::Ldap, {"ldap", L"ldap"}},
                {UriScheme::Mailto, {"mailto", L"mailto"}},
                {UriScheme::News, {"news", L"news"}},
                {UriScheme::Tel, {"tel", L"tel"}},
                {UriScheme::Telnet, {"telnet", L"telnet"}},
                {UriScheme::Urn, {"urn", L"urn"}},
                {UriScheme::Unknown, {"Unknown", L"Unknown"}}};

        return std::get<std::basic_string<CharT>>(UriSchemeMap.at(s));
    }

#ifdef NLOHMANN_JSON_VERSION_MAJOR
    /// @brief Serializer for JSON for UriScheme
    NLOHMANN_JSON_SERIALIZE_ENUM(UriScheme,
                                 {{UriScheme::WebHttp, "http"},
                                  {UriScheme::WebHttps, "https"},
                                  {UriScheme::WebSocket, "ws"},
                                  {UriScheme::WebSocketSecure, "wss"},
                                  {UriScheme::Ftp, "ftp"},
                                  {UriScheme::File, "file"},
                                  {UriScheme::Ldap, "ldap"},
                                  {UriScheme::Mailto, "mailto"},
                                  {UriScheme::News, "news"},
                                  {UriScheme::Tel, "tel"},
                                  {UriScheme::Telnet, "telnet"},
                                  {UriScheme::Urn, "urn"},
                                  {UriScheme::Unknown, nullptr}});
#endif

    /// @brief WHATWG URL Standard Section 4.2 - Special scheme default ports
    /// Returns the default port for a special scheme, or 0 if not a special scheme with a port.
    /// Per WHATWG: ftp=21, http=80, https=443, ws=80, wss=443, file=null(0)
    static constexpr uint16_t defaultPortForScheme(UriScheme s) noexcept
    {
        switch (s) {
            case UriScheme::Ftp: return 21;
            case UriScheme::WebHttp: return 80;
            case UriScheme::WebHttps: return 443;
            case UriScheme::WebSocket: return 80;
            case UriScheme::WebSocketSecure: return 443;
            default: return 0;
        }
    }

    /// @brief WHATWG URL Standard Section 4.2 - Check if a scheme is "special"
    static constexpr bool isSpecialScheme(UriScheme s) noexcept
    {
        switch (s) {
            case UriScheme::Ftp:
            case UriScheme::File:
            case UriScheme::WebHttp:
            case UriScheme::WebHttps:
            case UriScheme::WebSocket:
            case UriScheme::WebSocketSecure: return true;
            default: return false;
        }
    }

    /// @brief Uri class
    /// @tparam T Maybe std::string or std::wstring
    /// @tparam Auth Current implementation only supports AuthorityHttp<T>
    template <typename CharT = char, class Auth = AuthorityHttp<CharT>>
        requires(std::same_as<char, CharT> || std::same_as<wchar_t, CharT>) && std::same_as<Auth, AuthorityHttp<CharT>>
    struct Uri
    {
    private:
        std::basic_string<CharT> sourceUri {};

        /// @brief WHATWG Section 4.4 - Strip leading/trailing C0 control or space characters
        static std::basic_string<CharT> stripLeadingTrailingC0(const std::basic_string<CharT>& input)
        {
            if (input.empty()) return input;

            size_t start = 0;
            size_t end   = input.size();

            // C0 control or space: U+0000 to U+001F and U+0020
            while (start < end && (static_cast<uint32_t>(input[start]) <= 0x20)) {
                ++start;
            }
            while (end > start && (static_cast<uint32_t>(input[end - 1]) <= 0x20)) {
                --end;
            }
            return input.substr(start, end - start);
        }

        /// @brief WHATWG Section 4.4 - Remove all ASCII tab (0x09) and newline (0x0A, 0x0D) from input
        static std::basic_string<CharT> removeTabAndNewline(const std::basic_string<CharT>& input)
        {
            std::basic_string<CharT> result;
            result.reserve(input.size());
            for (auto ch : input) {
                if (ch != static_cast<CharT>(0x09) && ch != static_cast<CharT>(0x0A) && ch != static_cast<CharT>(0x0D)) {
                    result += ch;
                }
            }
            return result;
        }

        /// @brief WHATWG Section 4.1 - Check if a path segment is a single-dot segment ("." or "%2e")
        static bool isSingleDotSegment(const std::basic_string<CharT>& seg)
        {
            if (seg.size() == 1 && seg[0] == static_cast<CharT>('.')) return true;
            if (seg.size() == 3) {
                return seg[0] == static_cast<CharT>('%') && (seg[1] == static_cast<CharT>('2')) &&
                       (seg[2] == static_cast<CharT>('e') || seg[2] == static_cast<CharT>('E'));
            }
            return false;
        }

        /// @brief WHATWG Section 4.1 - Check if a path segment is a double-dot segment
        /// Matches: "..", ".%2e", "%2e.", "%2e%2e" (case-insensitive)
        static bool isDoubleDotSegment(const std::basic_string<CharT>& seg)
        {
            std::basic_string<CharT> lower = seg;
            for (auto& c : lower) {
                if (c >= static_cast<CharT>('A') && c <= static_cast<CharT>('Z'))
                    c = c + (static_cast<CharT>('a') - static_cast<CharT>('A'));
            }

            if (lower == _NORW(CharT, "..")) return true;
            if (lower == _NORW(CharT, ".%2e")) return true;
            if (lower == _NORW(CharT, "%2e.")) return true;
            if (lower == _NORW(CharT, "%2e%2e")) return true;
            return false;
        }

        /// @brief WHATWG Section 4.4 path state - Resolve "." and ".." segments in path
        static void resolveDotSegments(std::vector<std::basic_string<CharT>>& pathSegments, UriScheme scheme)
        {
            std::vector<std::basic_string<CharT>> resolved;
            for (auto& seg : pathSegments) {
                if (isDoubleDotSegment(seg)) {
                    if (!resolved.empty()) {
                        // Don't pop Windows drive letters for file scheme
                        if (scheme == UriScheme::File && resolved.size() == 1) {
                            auto& s = resolved[0];
                            if (s.size() == 2 &&
                                ((s[0] >= static_cast<CharT>('A') && s[0] <= static_cast<CharT>('Z')) ||
                                 (s[0] >= static_cast<CharT>('a') && s[0] <= static_cast<CharT>('z'))) &&
                                s[1] == static_cast<CharT>(':')) {
                                continue;
                            }
                        }
                        resolved.pop_back();
                    }
                }
                else if (!isSingleDotSegment(seg)) {
                    resolved.push_back(std::move(seg));
                }
                // Single-dot segments are simply skipped
            }
            pathSegments = std::move(resolved);
        }

        /// @brief Case-insensitive scheme prefix check helper
        /// Lowercases the first N characters of input and checks against the given prefix
        static bool startsWithScheme(const std::basic_string<CharT>& input, const CharT* prefix, size_t prefixLen)
        {
            if (input.size() < prefixLen) return false;
            for (size_t i = 0; i < prefixLen; ++i) {
                CharT ch = input[i];
                // ASCII lowercase
                if (ch >= static_cast<CharT>('A') && ch <= static_cast<CharT>('Z'))
                    ch = ch + (static_cast<CharT>('a') - static_cast<CharT>('A'));
                if (ch != prefix[i]) return false;
            }
            return true;
        }

    public:
        UriScheme                                                    scheme {UriScheme::WebHttp};
        Auth                                                         authority {};
        std::vector<std::basic_string<CharT>>                        path {};
        std::map<std::basic_string<CharT>, std::basic_string<CharT>> query {};
        std::basic_string<CharT>                                     fragment {};
        std::basic_string<CharT>                                     pathPart {};  // contains the "path" part as a full string
        std::basic_string<CharT>                                     urlPart {};   // contains the "balance" post Authority section
        std::basic_string<CharT>                                     queryPart {}; // contains the "query" part


        /// @brief Default constructor
        Uri() { };

        /// @brief Given an endpoint Uri, decompose it into the Uri
        /// Conforms to the WHATWG URL Standard (https://url.spec.whatwg.org/)
        /// @param aEndpoint The endpoint uri
        void split(const std::basic_string<CharT>& aEndpoint)
        {
            size_t posProtocol {std::string::npos};

            /// @brief Parse the path elements into a vector of path elements
            auto parsePathElements = [](const std::basic_string<CharT>& str, std::vector<std::basic_string<CharT>>& tokens) {
                auto lastPos = str.find_first_not_of(_NORW(CharT, "/"), 0);
                auto pos = str.find_first_of(_NORW(CharT, "/"), lastPos);

                while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
                    tokens.push_back(str.substr(lastPos, pos - lastPos));
                    lastPos = str.find_first_not_of(_NORW(CharT, "/"), pos);
                    pos = str.find_first_of(_NORW(CharT, "/"), lastPos);
                };
                return str;
            };

            /// @brief Parse query string elements into a map
            auto parseQueryElements = [](const std::basic_string<CharT>&                               s,
                                         std::map<std::basic_string<CharT>, std::basic_string<CharT>>& ret) -> void {
                size_t       i    = 0;
                const size_t slen = s.length();
                while ((i < slen) && i != std::string::npos) {
                    size_t endOfSection = s.find_first_of(_NORW(CharT, "&"), i);
                    size_t sectionLen = endOfSection == std::string::npos ? slen - i : endOfSection - i;
                    size_t splitPos   = s.find_first_of(_NORW(CharT, "="), i);
                    size_t keyLen     = 0;
                    size_t valLen     = 0;
                    if (splitPos >= endOfSection) {
                        splitPos = endOfSection;
                        valLen   = 0;
                        keyLen   = sectionLen;
                    }
                    else {
                        keyLen = splitPos - i;
                        valLen = endOfSection == std::string::npos ? slen - ++splitPos : endOfSection - ++splitPos;
                    }

                    auto key = s.substr(i, keyLen);

                    if ((valLen > 0) && (keyLen > 0)) {
                        auto val = s.substr(splitPos, valLen);
                        ret[key] = val;
                    }
                    else if (keyLen > 0) {
                        ret[key] = std::basic_string<CharT> {};
                    }

                    if ((endOfSection == std::string::npos) || (keyLen == 0)) {
                        break;
                    }
                    i = ++endOfSection;
                }
            };

            // WHATWG Section 4.4 Step 1: Strip leading/trailing C0 control or space
            auto cleanedInput = stripLeadingTrailingC0(aEndpoint);

            // WHATWG Section 4.4 Step 3: Remove all ASCII tab and newline
            cleanedInput = removeTabAndNewline(cleanedInput);

            // WHATWG: Scheme detection - case-insensitive per spec
            // Per WHATWG Section 4.2, special schemes: ftp(21), file(null), http(80), https(443), ws(80), wss(443)
            if (startsWithScheme(cleanedInput, _NORW(CharT, "https://"), 8)) {
                posProtocol    = 8;
                scheme         = UriScheme::WebHttps;
                authority.port = 443;
            }
            else if (startsWithScheme(cleanedInput, _NORW(CharT, "http://"), 7)) {
                posProtocol    = 7;
                scheme         = UriScheme::WebHttp;
                authority.port = 80;
            }
            else if (startsWithScheme(cleanedInput, _NORW(CharT, "wss://"), 6)) {
                posProtocol    = 6;
                scheme         = UriScheme::WebSocketSecure;
                authority.port = 443;
            }
            else if (startsWithScheme(cleanedInput, _NORW(CharT, "ws://"), 5)) {
                posProtocol    = 5;
                scheme         = UriScheme::WebSocket;
                authority.port = 80;
            }
            else if (startsWithScheme(cleanedInput, _NORW(CharT, "ftp://"), 6)) {
                posProtocol    = 6;
                scheme         = UriScheme::Ftp;
                authority.port = 21;
            }

            if (posProtocol != std::string::npos) {
                // WHATWG Section 4.4 authority state: Parse credentials (username:password@)
                if (auto posAt = cleanedInput.find(_NORW(CharT, "@"), posProtocol); posAt != std::string::npos) {
                    auto credentialsPart = cleanedInput.substr(posProtocol, (posAt - posProtocol));

                    // WHATWG: Split credentials into username and password at first ':'
                    auto posColon = credentialsPart.find(_NORW(CharT, ":"));
                    if (posColon != std::string::npos) {
                        authority.userInfo = credentialsPart.substr(0, posColon);
                        authority.password = credentialsPart.substr(posColon + 1);
                    }
                    else {
                        authority.userInfo = credentialsPart;
                    }
                    posProtocol = posAt + 1;
                }

                // Search for the next `/` which would allow us to extract the server:port portion.
                // WHATWG: For special schemes, also treat `\` as a path separator
                auto pos2 = cleanedInput.find(_NORW(CharT, "/"), posProtocol);
                if (isSpecialScheme(scheme)) {
                    auto posBackslash = cleanedInput.find(_NORW(CharT, "\\"), posProtocol);
                    if (posBackslash != std::string::npos && (pos2 == std::string::npos || posBackslash < pos2)) {
                        pos2 = posBackslash;
                    }
                }

                // Extract the host:port portion
                authority.host =
                        cleanedInput.substr(posProtocol, pos2 != std::string::npos ? (pos2 - posProtocol) : std::string::npos);

                // WHATWG Section 3.5 - Handle IPv6 addresses in brackets
                if (!authority.host.empty() && authority.host[0] == static_cast<CharT>('[')) {
                    auto closeBracket = authority.host.find(static_cast<CharT>(']'));
                    if (closeBracket != std::string::npos) {
                        if (closeBracket + 1 < authority.host.size() &&
                            authority.host[closeBracket + 1] == static_cast<CharT>(':')) {
                            auto portStr = authority.host.substr(closeBracket + 2);
                            if (!portStr.empty()) {
                                try {
                                    int portVal = std::stoi(std::basic_string<CharT>(portStr));
                                    // WHATWG Section 4.4 port state: port must be 0-65535
                                    if (portVal >= 0 && portVal <= 65535) {
                                        authority.port = static_cast<uint16_t>(portVal);
                                    }
                                    else {
                                        // WHATWG: port-out-of-range validation error
                                        authority.port = 0;
                                    }
                                }
                                catch (...) {
                                }
                            }
                            authority.host = authority.host.substr(0, closeBracket + 1);
                        }
                    }
                }
                else if (auto pos3 = authority.host.find(_NORW(CharT, ":")); pos3 != std::string::npos) {
                    auto portStr = authority.host.substr(pos3 + 1);
                    if (!portStr.empty()) {
                        try {
                            int portVal = std::stoi(std::basic_string<CharT>(portStr));
                            // WHATWG Section 4.4 port state: port must be a 16-bit unsigned integer (0-65535)
                            if (portVal >= 0 && portVal <= 65535) {
                                authority.port = static_cast<uint16_t>(portVal);
                            }
                            else {
                                // WHATWG: port-out-of-range validation error
                                authority.port = 0;
                            }
                        }
                        catch (...) {
                        }
                    }
                    authority.host = authority.host.substr(0, pos3);
                }

                // WHATWG Section 3.5 - Lowercase the host for special schemes (domain to ASCII)
                if (isSpecialScheme(scheme)) {
                    for (auto& ch : authority.host) {
                        if (ch >= static_cast<CharT>('A') && ch <= static_cast<CharT>('Z')) {
                            ch = ch + (static_cast<CharT>('a') - static_cast<CharT>('A'));
                        }
                    }
                }

                if (pos2 != std::string::npos) {
                    // The rest of the endpoint is the url portion.
                    urlPart = cleanedInput.substr(pos2);

                    // WHATWG: For special schemes, normalize backslashes in the URL part
                    if (isSpecialScheme(scheme)) {
                        // We normalize backslashes only in the path portion for display
                        // but keep the original urlPart for backward compatibility
                    }

                    // Continue to split the path, query and fragment
                    // Pull out the fragment
                    auto posFragment = cleanedInput.rfind(_NORW(CharT, "#"));
                    if (posFragment != std::string::npos) {
                        fragment = cleanedInput.substr(posFragment + 1);
                    }

                    auto posQueryPart = cleanedInput.find(_NORW(CharT, "?"), pos2);

                    // Parse the "path" part into vector and store in the pathPart string
                    size_t pathEnd = std::string::npos;
                    if (posQueryPart != std::string::npos)
                        pathEnd = posQueryPart - pos2;
                    else if (posFragment != std::string::npos)
                        pathEnd = posFragment - pos2;

                    if (pathPart = cleanedInput.substr(pos2, pathEnd); !pathPart.empty()) {
                        // For path parsing, normalize backslashes for special schemes
                        std::basic_string<CharT> normalizedPathPart = pathPart;
                        if (isSpecialScheme(scheme)) {
                            for (auto& ch : normalizedPathPart) {
                                if (ch == static_cast<CharT>('\\')) {
                                    ch = static_cast<CharT>('/');
                                }
                            }
                        }
                        parsePathElements(normalizedPathPart, path);

                        // WHATWG Section 4.4 path state: Resolve "." and ".." segments
                        resolveDotSegments(path, scheme);
                    }

                    // Parse the query part if present
                    if (posQueryPart != std::string::npos) {
                        queryPart = cleanedInput.substr(posQueryPart + 1,
                                                     posFragment != std::string::npos ? (posFragment) - (posQueryPart + 1)
                                                                                      : std::string::npos);
                        if (!queryPart.empty()) parseQueryElements(queryPart, query);
                    }

                    // The decode was successful. Save the original string
                    sourceUri = aEndpoint;
                }
            }
        }


        /// @brief Constructor parses out the uri given the argument
        /// @param endpoint std::string or std::wstring
        Uri(const std::basic_string<CharT>& aEndpoint) noexcept
        {
            try {
                split(aEndpoint);
            }
            catch (...) {
            }
        }


        /// @brief Operator rebuilds the Uri string
        operator std::basic_string<CharT>() const { return string(); }

        /// @brief Rebuild the Uri string
        /// @return Uri string of desired type (std::string or std::wstring)
        const std::basic_string<CharT> string() const
        {
            return sourceUri.empty()
                         ? std::format(_NORW(CharT, "{}://{}{}{}{}"),
                                       to_string<CharT>(scheme),
                                       !authority.userInfo.empty()
                                               ? (!authority.password.empty()
                                                          ? std::format(_NORW(CharT, "{}:{}@"), authority.userInfo,
                                                                        authority.password)
                                                          : std::format(_NORW(CharT, "{}@"), authority.userInfo))
                                               : std::basic_string<CharT> {},
                                       authority.host,
                                       (authority.port > 0) ? std::format(_NORW(CharT, ":{}"), authority.port) : _NORW(CharT, ""),
                                       urlPart)
                         : sourceUri;
        }
    };


    /// @brief Helper function to create a `Uri<>` object from the given source string
    /// @tparam T Defaults to `char` but it is restricted to `char` or `wchar_t`.
    /// @param aEndpoint The source enpoint
    /// @return The `Uri<>` object
    template <typename T = char>
        requires(std::same_as<char, T> || std::same_as<wchar_t, T>)
    static Uri<T, AuthorityHttp<T>> SplitUri(const std::basic_string<T>& aEndpoint)
    {
        return Uri<T, AuthorityHttp<T>> {aEndpoint};
    }


    /// @brief Helper function to create a `Uri<>` object from the given source string
    /// @tparam T Defaults to `char` but it is restricted to `char` or `wchar_t`.
    /// @param aEndpoint The source enpoint
    /// @return The `Uri<>` object
    template <typename T = char>
        requires(std::same_as<char, T> || std::same_as<wchar_t, T>)
    static Uri<T, AuthorityHttp<T>> SplitUri(const T* aEndpoint)
    {
        return Uri<T, AuthorityHttp<T>> {std::basic_string<T> {aEndpoint}};
    }


#ifdef NLOHMANN_JSON_VERSION_MAJOR
    /// @brief Json serializer for wstring vector
    static void to_json(nlohmann::json& dest, const std::vector<std::wstring>& s)
    {
        for (auto& i : s) {
            dest.push_back(w2n(i));
        }
    }

    /// @brief Json serializer for wstring map
    static void to_json(nlohmann::json& dest, const std::map<std::wstring, std::wstring>& s)
    {
        for (auto& [k, v] : s) {
            dest[w2n(k)] = w2n(v);
        }
    }

    /// @brief Json serializer for the Uri object
    template <typename CharT>
        requires std::same_as<CharT, char> || std::same_as<CharT, wchar_t>
    static void to_json(nlohmann::json& dest, const Uri<CharT, AuthorityHttp<CharT>>& s)
    {
        dest["scheme"] = s.scheme;

        if constexpr (std::is_same_v<CharT, char>) {
            dest["authority"] = s.authority;
            dest["path"]      = s.path;
            dest["query"]     = s.query;
            dest["fragment"]  = s.fragment;
            dest["urlPart"]   = s.urlPart;
            dest["pathPart"]  = s.pathPart;
            dest["queryPart"] = s.queryPart;
        }

        if constexpr (std::is_same_v<CharT, wchar_t>) {
            dest["authority"] = w2n(s.authority);
            dest["fragment"]  = w2n(s.fragment);
            dest["urlPart"]   = w2n(s.urlPart);
            dest["pathPart"]  = w2n(s.pathPart);
            dest["queryPart"] = w2n(s.queryPart);

            for (auto& [k, v] : s.query) {
                dest["query"][w2n(k)] = w2n(v);
            }

            for (auto& i : s.path) {
                dest["path"].push_back(w2n(i));
            }
        }
    }
#endif

    namespace splituri_literals
    {
        /// @brief Literal operator `_Uri` for std::string
        static siddiqsoft::Uri<char, siddiqsoft::AuthorityHttp<char>> operator"" _Uri(const char* src, size_t sz)
        {
            return siddiqsoft::Uri<char, siddiqsoft::AuthorityHttp<char>>(std::string(src, sz));
        }

        /// @brief Literal operator `_Uri` for std::wstring
        static siddiqsoft::Uri<wchar_t, siddiqsoft::AuthorityHttp<wchar_t>> operator"" _Uri(const wchar_t * src, size_t sz)
        {
            return siddiqsoft::Uri<wchar_t, siddiqsoft::AuthorityHttp<wchar_t>>(std::wstring(src, sz));
        }
    } // namespace splituri_literals
} // namespace siddiqsoft


template <typename CharT>
struct std::formatter<siddiqsoft::AuthorityHttp<CharT>, CharT> : std::formatter<std::basic_string<CharT>, CharT>
{
    template <class FC>
    auto format(const siddiqsoft::AuthorityHttp<CharT>& sv, FC& ctx) const
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(std::basic_string<CharT>(sv), ctx);
    }
};


template <typename CharT>
struct std::formatter<siddiqsoft::AuthorityLdap<CharT>> : std::formatter<std::basic_string<CharT>, CharT>
{
    template <class FC>
    auto format(const siddiqsoft::AuthorityLdap<CharT>& sv, FC& ctx) const
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(std::basic_string<CharT>(sv), ctx);
    }
};


template <typename CharT>
struct std::formatter<siddiqsoft::AuthorityNone<CharT>> : std::formatter<std::basic_string<CharT>, CharT>
{
    template <class FC>
    auto format(const siddiqsoft::AuthorityNone<CharT>& sv, FC& ctx) const
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(std::basic_string<CharT>(sv), ctx);
    }
};


/// @brief Explicit implementation for UriScheme which is an enum class to std::string
template <typename CharT>
struct std::formatter<siddiqsoft::UriScheme, CharT> : std::formatter<std::basic_string<CharT>, CharT>
{
    template <class FC>
    auto format(const siddiqsoft::UriScheme& s, FC& fc) const
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(siddiqsoft::to_string<CharT>(s), fc);
    }
};

template <typename CharT>
struct std::formatter<siddiqsoft::Uri<CharT, siddiqsoft::AuthorityHttp<CharT>>, CharT>
    : std::formatter<std::basic_string<CharT>, CharT>
{
    template <typename FC>
    auto format(const siddiqsoft::Uri<CharT, siddiqsoft::AuthorityHttp<CharT>>& u, FC& ctx) const
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(u, ctx);
    }
};
#endif // !SPLITURI_HPP
