﻿/*
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

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <tuple>
#include <string>
#include <type_traits>
#include <format>
#include <concepts>
#include <variant>
#include <ranges>
#include <codecvt>

#include "nlohmann/json.hpp"


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

    /*
        https://en.wikipedia.org/wiki/Uniform_Resource_Identifier#Syntax

              userinfo       host      port
              ┌──┴───┐ ┌──────┴──────┐ ┌┴┐
      https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top
      └─┬─┘   └───────────┬──────────────┘└───────┬───────┘ └───────────┬─────────────┘ └┬┘
      scheme          authority                  path                 query           fragment

      ldap://[2001:db8::7]/c=GB?objectClass?one
      └┬─┘   └─────┬─────┘└─┬─┘ └──────┬──────┘
      scheme   authority   path      query

      mailto:John.Doe@example.com
      └─┬──┘ └────┬─────────────┘
      scheme     path

      news:comp.infosystems.www.servers.unix
      └┬─┘ └─────────────┬─────────────────┘
      scheme            path

      tel:+1-816-555-1212
      └┬┘ └──────┬──────┘
      scheme    path

      telnet://192.0.2.16:80/
      └─┬──┘   └─────┬─────┘│
      scheme     authority  path

      urn:oasis:names:specification:docbook:dtd:xml:4.1.2
      └┬┘ └──────────────────────┬──────────────────────┘
      scheme                    path
    */

    /// @brief AuthorityHttp type
    /// @tparam T std::string or std::wstring
    template <typename CharT>
        requires std::same_as<char, CharT> || std::same_as<wchar_t, CharT>
    struct AuthorityHttp
    {
        std::basic_string<CharT> userInfo {};
        std::basic_string<CharT> host {};
        uint16_t                 port {0}; // maximum of 65535

        operator std::basic_string<CharT>() const
        {
            std::basic_string<CharT> ret;
            ret = userInfo;
            if (!userInfo.empty()) ret += _NORW(CharT, "@");
            ret += host;
            if (port > 0) {
                ret += _NORW(CharT, ":");
                if constexpr (std::is_same_v<CharT, char>) ret += std::to_string(port);
                if constexpr (std::is_same_v<CharT, wchar_t>) ret += std::to_wstring(port);
            }
            return ret;
        }
    };


    static void to_json(nlohmann::json& dest, const AuthorityHttp<char>& s)
    {
        dest["userInfo"] = s.userInfo;
        dest["host"]     = s.host;
        dest["port"]     = s.port;
    }

    static void to_json(nlohmann::json& dest, const AuthorityHttp<wchar_t>& s)
    {
        thread_local std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        dest["userInfo"] = converter.to_bytes(s.userInfo);
        dest["host"]     = converter.to_bytes(s.host);
        dest["port"]     = s.port;
    }


    /// @brief AuthorityLdap type
    /// @tparam T std::string or std::wstring
    template <typename CharT>
        requires std::same_as<CharT, char> || std::same_as<CharT, wchar_t>
    struct AuthorityLdap
    {
        std::basic_string<CharT> host {};

        operator std::basic_string<CharT>() const
        {
            return std::format(_NORW(CharT, "[{}]"), host);
        }
    };


    /// @brief AuthorityNone type
    /// @tparam T std::string or std::wstring
    template <typename CharT>
        requires std::same_as<char, CharT> || std::same_as<wchar_t, CharT>
    struct AuthorityNone
    {
        std::basic_string<CharT> none {};

        operator std::basic_string<CharT>() const
        {
            return none;
        }

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(AuthorityNone<CharT>, none);
    };

    /// @brief URI scheme enum
    enum class UriScheme
    {
        WebHttp,
        WebHttps,
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
                {UriScheme::Ldap, {"ldap", L"ldap"}},
                {UriScheme::Mailto, {"mailto", L"mailto"}},
                {UriScheme::News, {"news", L"news"}},
                {UriScheme::Tel, {"tel", L"tel"}},
                {UriScheme::Telnet, {"telnet", L"telnet"}},
                {UriScheme::Urn, {"urn", L"urn"}},
                {UriScheme::Unknown, {"Unknown", L"Unknown"}}};

        return std::get<std::basic_string<CharT>>(UriSchemeMap.at(s));
    }

    /// @brief Serializer for JSON for UriScheme
    NLOHMANN_JSON_SERIALIZE_ENUM(UriScheme,
                                 {{UriScheme::WebHttp, "http"},
                                  {UriScheme::WebHttps, "https"},
                                  {UriScheme::Ldap, "ldap"},
                                  {UriScheme::Mailto, "mailto"},
                                  {UriScheme::News, "news"},
                                  {UriScheme::Tel, "tel"},
                                  {UriScheme::Telnet, "telnet"},
                                  {UriScheme::Urn, "urn"},
                                  {UriScheme::Unknown, nullptr}});


    /// @brief Uri structure
    /// @tparam T Maybe std::string or std::wstring
    /// @tparam Auth Current implementation only supports AuthorityHttp<T>
    template <typename CharT = char, class Auth = AuthorityHttp<CharT>>
        requires(std::same_as<char, CharT> || std::same_as<wchar_t, CharT>)
    &&std::same_as<Auth, AuthorityHttp<CharT>> struct Uri
    {
        UriScheme                                                    scheme {UriScheme::WebHttp};
        Auth                                                         authority {};
        std::vector<std::basic_string<CharT>>                        path {};
        std::map<std::basic_string<CharT>, std::basic_string<CharT>> query {};
        std::basic_string<CharT>                                     fragment {};
        std::basic_string<CharT>                                     urlPart {};   // contains the "balance" post Authority section
        std::basic_string<CharT>                                     queryPart {}; // contains the "query" part


        /// @brief Default constructor
        Uri() {};


        /// @brief Constructor parses out the uri given the argument
        /// @param endpoint std::string or std::wstring
        Uri(const std::basic_string<CharT>& aEndpoint)
        {
            size_t posProtocol = std::string::npos;

            /// @brief Parse the path elements into a vector of path elements
            /// @param aEndpoint
            auto parsePathElements = [](const std::basic_string<CharT>& str, std::vector<std::basic_string<CharT>>& tokens) {
                // Skip delimiters at beginning.
                auto lastPos = str.find_first_not_of(_NORW(CharT, "/"), 0);
                // Find first "non-delimiter".
                auto pos = str.find_first_of(_NORW(CharT, "/"), lastPos);

                while ((std::string::npos != pos) || (std::string::npos != lastPos)) {
                    // Found a token, add it to the std::vector.
                    tokens.push_back(str.substr(lastPos, pos - lastPos));
                    // Skip delimiters.  Note the "not_of"
                    lastPos = str.find_first_not_of(_NORW(CharT, "/"), pos);
                    // Find next "non-delimiter"
                    pos = str.find_first_of(_NORW(CharT, "/"), lastPos);
                };
            };

            /// @brief Given a sequence of characters of type CharT, parse the elements into a map with each element delimited
            /// by & and split by optional =
            /// @tparam CharT Type of char
            /// @param s The sequence must be the query part of the URI
            /// @return map of strings
            auto parseQueryElements = [](const std::basic_string<CharT>&                               s,
                                         std::map<std::basic_string<CharT>, std::basic_string<CharT>>& ret) -> void {
                size_t       i    = 0;
                const size_t slen = s.length();
                while ((i < slen) && i != std::string::npos) {
                    // Find the section delimited by & or end of string
                    size_t endOfSection = s.find_first_of(_NORW(CharT, "&"), i);
                    // If we reach the end then the sectionLen is the length of source string - i
                    size_t sectionLen = endOfSection == std::string::npos ? slen - i : endOfSection - i;
                    size_t splitPos   = s.find_first_of(_NORW(CharT, "="), i);
                    size_t keyLen     = 0;
                    size_t valLen     = 0;
                    // Guard against cases where we do not have splitter
                    if (splitPos >= endOfSection) {
                        splitPos = endOfSection;
                        valLen   = 0;
                        keyLen   = splitPos - i;
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
                        ret[key] = std::basic_string<CharT> {}; // empty string must be inserted
                    }

                    if ((endOfSection == std::string::npos) || (keyLen == 0)) {
                        break;
                    }
                    i = ++endOfSection;
                }
            };

            // Scan for the start of the URI..
            if (aEndpoint.starts_with(_NORW(CharT, "https://"))) {
                posProtocol    = 8; // length of https://
                scheme         = UriScheme::WebHttps;
                authority.port = 443; // replace later if present
            }
            else if (aEndpoint.starts_with(_NORW(CharT, "http://"))) {
                posProtocol    = 7; // length of http://
                scheme         = UriScheme::WebHttp;
                authority.port = 80; // replace later if present
            }

            if (posProtocol != std::string::npos) {
                if (auto posAt = aEndpoint.find(_NORW(CharT, "@"), posProtocol); posAt != std::string::npos) {
                    // We have a user part
                    authority.userInfo = aEndpoint.substr(posProtocol, (posAt - posProtocol));
                    // Advance to the end of the user "@" which begins the host part
                    posProtocol = ++posAt;
                }

                // Search for the next `/` which would allow us to extract the server:port portion.
                // It is possible that the aEndpoint does not contain the trailing `/` in which case
                // the url portion is "empty" and the only thing left is to attempt to extract the server
                // portion.
                auto pos2 = aEndpoint.find(_NORW(CharT, "/"), posProtocol);

                // Extract the server:port portion.. make sure we don't calculate based on missing `/`
                authority.host =
                        aEndpoint.substr(posProtocol, pos2 != std::string::npos ? (pos2 - posProtocol) : std::string::npos);
                if (auto pos3 = authority.host.find(_NORW(CharT, ":")); pos3 != std::string::npos) {
                    // First Extract the port
                    authority.port = std::stoi(authority.host.substr(++pos3));
                    // Reset the server
                    authority.host = authority.host.substr(0, (--pos3 - 0));
                }

                if (pos2 != std::string::npos) {
                    // Finally, the rest of the endpoint is the url portion.
                    urlPart = aEndpoint.substr(pos2);

                    // Continue to split the path, query and fragment
                    // Pull out the fragment
                    auto posFragment = aEndpoint.rfind(_NORW(CharT, "#"));
                    if (posFragment != std::string::npos) {
                        // We have a fragment
                        fragment = aEndpoint.substr(posFragment + 1);
                    }

                    auto posQueryPart = aEndpoint.find(_NORW(CharT, "?"), pos2);

                    auto pathSegment =
                            aEndpoint.substr(pos2, posQueryPart != std::string::npos ? posQueryPart - (pos2) : std::string::npos);

                    // Parse the path into convenience vector of path elements
                    if (!pathSegment.empty()) parsePathElements(pathSegment, path);

                    // Parse the query part if present
                    if (posQueryPart != std::string::npos) {
                        queryPart = aEndpoint.substr(posQueryPart + 1,
                                                     posFragment != std::string::npos ? (posFragment) - (posQueryPart + 1)
                                                                                      : std::string::npos);
                        // Parse the query elements into a map
                        if (!queryPart.empty()) parseQueryElements(queryPart, query);
                    }
                }
            }
        }


        /// @brief Operator rebuilds the Uri string
        operator std::basic_string<CharT>() const
        {
            return std::format(_NORW(CharT, "{}://{}{}{}"),
                               to_string<CharT>(scheme),
                               authority.host,
                               (authority.port > 0) ? std::format(_NORW(CharT, ":{}"), authority.port) : _NORW(CharT, ""),
                               urlPart);
        }


        /// @brief Helper to encode the given string in context of the HTTP requirement
        /// @param s Source string
        /// @return Encoded string
        static std::basic_string<CharT> urlEncode(const std::basic_string<CharT>& s)
        {
            std::basic_string<CharT> retOutput {};
            std::ranges::for_each(s, [&retOutput](auto ch) {
                switch (ch) {
                    case _NORW(CharT, '%'): retOutput += _NORW(CharT, "%25"); break;
                    case _NORW(CharT, ' '): retOutput += _NORW(CharT, "%20"); break;
                    case _NORW(CharT, '&'): retOutput += _NORW(CharT, "%26"); break;
                    case _NORW(CharT, '<'): retOutput += _NORW(CharT, "%3c"); break;
                    case _NORW(CharT, '>'): retOutput += _NORW(CharT, "%3e"); break;
                    case _NORW(CharT, '{'): retOutput += _NORW(CharT, "%7b"); break;
                    case _NORW(CharT, '}'): retOutput += _NORW(CharT, "%7d"); break;
                    case _NORW(CharT, '\''): retOutput += _NORW(CharT, "%27"); break;
                    case _NORW(CharT, '\"'): retOutput += _NORW(CharT, "%22"); break;
                    case _NORW(CharT, '/'): retOutput += _NORW(CharT, "%2f"); break;
                    case _NORW(CharT, '\\'): retOutput += _NORW(CharT, "%5c"); break;
                    case _NORW(CharT, '@'): retOutput += _NORW(CharT, "%40"); break;
                    case _NORW(CharT, '~'): retOutput += _NORW(CharT, "%7e"); break;
                    case _NORW(CharT, '|'): retOutput += _NORW(CharT, "%7c"); break;
                    case _NORW(CharT, ','): retOutput += _NORW(CharT, "%2c"); break;
                    case _NORW(CharT, '+'): retOutput += _NORW(CharT, "%2b"); break;
                    case _NORW(CharT, ':'): retOutput += _NORW(CharT, "%3a"); break;
                    case _NORW(CharT, '`'): retOutput += _NORW(CharT, "%60"); break;
                    case _NORW(CharT, '['): retOutput += _NORW(CharT, "%5b"); break;
                    case _NORW(CharT, ']'): retOutput += _NORW(CharT, "%5d"); break;
                    case _NORW(CharT, '?'): retOutput += _NORW(CharT, "%3f"); break;
                    case _NORW(CharT, '='): retOutput += _NORW(CharT, "%3d"); break;
                    case _NORW(CharT, '$'): retOutput += _NORW(CharT, "%24"); break;
                    case _NORW(CharT, '#'): retOutput += _NORW(CharT, "%23"); break;
                    default: retOutput += ch;
                };
            });
            return retOutput;
        }
    };


    /// @brief Json serializer for wstring vector
    /// @param dest
    /// @param s stl vector of wstring
    static void to_json(nlohmann::json& dest, const std::vector<std::wstring>& s)
    {
        thread_local std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        for (auto& i : s) {
            dest.push_back(converter.to_bytes(i));
        }
    }

    /// @brief Json serializer for wstring map
    /// @param dest
    /// @param s stl map of wstring
    static void to_json(nlohmann::json& dest, const std::map<std::wstring, std::wstring>& s)
    {
        thread_local std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        for (auto& [k, v] : s) {
            dest[converter.to_bytes(k)] = converter.to_bytes(v);
        }
    }

    /// @brief Json serializer for the Uri object
    /// @tparam CharT Must be char or wchar_t
    /// @param dest
    /// @param s The Uri object
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
            dest["queryPart"] = s.queryPart;
        }

        if constexpr (std::is_same_v<CharT, wchar_t>) {
            thread_local std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

            dest["authority"] = converter.to_bytes(s.authority);
            dest["fragment"]  = converter.to_bytes(s.fragment);
            dest["urlPart"]   = converter.to_bytes(s.urlPart);
            dest["queryPart"] = converter.to_bytes(s.queryPart);

            // We cannot delegate to to_json as it mis-interprets the map<std::wstring,std::wstring>
            for (auto& [k, v] : s.query) {
                dest["query"][converter.to_bytes(k)] = converter.to_bytes(v);
            }

            // We cannot delegate to to_json as it mis-interprets the vector<std::wstring> as std::array
            for (auto& i : s.path) {
                dest["path"].push_back(converter.to_bytes(i));
            }
        }
    }


    /// @brief Given an endpoint Uri, decompose it into the Uri object via the Uri object
    /// @tparam T Specify std::string or std::wstring (defaults to std::string)
    /// @tparam Auth The authority type (only AuthorityHttp is supported)
    /// @param aEndpoint The endpoint uri
    /// @return Object of Uri with base std::string or std::wstring
    template <typename CharT = char, class Auth = AuthorityHttp<CharT>>
        requires std::same_as<char, CharT> || std::same_as<wchar_t, CharT>
    static auto SplitUri(const std::basic_string<CharT>& aEndpoint) -> Uri<CharT, Auth>
    {
        return Uri<CharT, Auth>(aEndpoint);
    }


    namespace literals
    {
        /// @brief Literal operator `_Uri` for std::string
        /// @param src Given a sequence of characters
        /// @param sz Length of given character sequence
        /// @return Uri<string,AuthorityHttp> object
        static siddiqsoft::Uri<char, siddiqsoft::AuthorityHttp<char>> operator"" _Uri(const char* src, size_t sz)
        {
            return siddiqsoft::Uri<char, siddiqsoft::AuthorityHttp<char>>(std::string(src, sz));
        }

        /// @brief Literal operator `_Uri` for std::wstring
        /// @param src Given a sequence of characters
        /// @param sz Length of given character sequence
        /// @return Uri<wstring,AuthorityHttp> object
        static siddiqsoft::Uri<wchar_t, siddiqsoft::AuthorityHttp<wchar_t>> operator"" _Uri(const wchar_t* src, size_t sz)
        {
            return siddiqsoft::Uri<wchar_t, siddiqsoft::AuthorityHttp<wchar_t>>(std::wstring(src, sz));
        }
    } // namespace literals
} // namespace siddiqsoft


template <typename CharT>
struct std::formatter<siddiqsoft::AuthorityHttp<CharT>, CharT> : std::formatter<std::basic_string<CharT>, CharT>
{
    template <class FC>
    auto format(const siddiqsoft::AuthorityHttp<CharT>& sv, FC& ctx)
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(std::basic_string<CharT>(sv), ctx);
    }
};


template <typename CharT>
struct std::formatter<siddiqsoft::AuthorityLdap<CharT>> : std::formatter<std::basic_string<CharT>, CharT>
{
    template <class FC>
    auto format(const siddiqsoft::AuthorityLdap<CharT>& sv, FC& ctx)
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(std::basic_string<CharT>(sv), ctx);
    }
};


template <typename CharT>
struct std::formatter<siddiqsoft::AuthorityNone<CharT>> : std::formatter<std::basic_string<CharT>, CharT>
{
    template <class FC>
    auto format(const siddiqsoft::AuthorityNone<CharT>& sv, FC& ctx)
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(std::basic_string<CharT>(sv), ctx);
    }
};


template <typename CharT>
struct std::formatter<
        std::variant<siddiqsoft::AuthorityHttp<CharT>, siddiqsoft::AuthorityLdap<CharT>, siddiqsoft::AuthorityNone<CharT>>>
    : std::formatter<std::basic_string<CharT>, CharT>
{
    template <class FC>
    auto
    format(const std::variant<siddiqsoft::AuthorityHttp<CharT>, siddiqsoft::AuthorityLdap<CharT>, siddiqsoft::AuthorityNone<CharT>>&
                   sv,
           FC&     ctx)
    {
        if (std::holds_alternative<siddiqsoft::AuthorityHttp<CharT>>(sv))
            return std::formatter<CharT>::format(sv.get<siddiqsoft::AuthorityHttp<CharT>>(), ctx);
        else if (std::holds_alternative<siddiqsoft::AuthorityLdap<CharT>>(sv))
            return std::formatter<CharT>::format(sv.get<siddiqsoft::AuthorityLdap<CharT>>(), ctx);

        return std::formatter<CharT>::format(sv.get<siddiqsoft::AuthorityNone<CharT>>(), ctx);
    }
};

/// @brief Explicit implementation for UriScheme which is an enum class to std::string
template <typename CharT>
struct std::formatter<siddiqsoft::UriScheme, CharT> : std::formatter<std::basic_string<CharT>, CharT>
{
    template <class FC>
    auto format(const siddiqsoft::UriScheme& s, FC& fc)
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(siddiqsoft::to_string<CharT>(s), fc);
    }
};

template <typename CharT>
struct std::formatter<siddiqsoft::Uri<CharT, siddiqsoft::AuthorityHttp<CharT>>, CharT>
    : std::formatter<std::basic_string<CharT>, CharT>
{
    template <typename FC>
    auto format(const siddiqsoft::Uri<CharT, siddiqsoft::AuthorityHttp<CharT>>& u, FC& ctx)
    {
        return std::formatter<std::basic_string<CharT>, CharT>::format(u, ctx);
    }
};
#endif // !SPLITURI_HPP
