﻿#pragma once
#ifndef SPLITURI_HPP
#define SPLITURI_HPP

#include <tuple>
#include <string>
#include <type_traits>
#include <format>
#include <concepts>
#include <variant>

#include "nlohmann/json.hpp"

#include "siddiqsoft/string2map.hpp"
#include "siddiqsoft/string2vector.hpp"


namespace siddiqsoft
{
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

	template <class T> requires std::same_as<std::string, T> || std::same_as<std::wstring, T> struct AuthorityHttp
	{
		T        userInfo {};
		T        seperatorAt {"@"};
		T        host {};
		T        separatorColon {":"};
		uint16_t port {0};

		operator T() const
		{
			return std::format("{}{}{}{}{}",
			                   userInfo,
			                   !userInfo.empty() ? seperatorAt : T {},
			                   host,
			                   port > 0 ? separatorColon : T {},
			                   port > 0 ? std::to_string(port) : T {});
		}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(AuthorityHttp<T>, userInfo, host, port);
	};

	template <class T> requires std::same_as<std::string, T> || std::same_as<std::wstring, T> struct AuthorityLdap
	{
		T host {};

		operator T() const { return std::format("[{}]", host); }

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(AuthorityLdap<T>, host);
	};


	template <class T> requires std::same_as<std::string, T> || std::same_as<std::wstring, T> struct AuthorityNone
	{
		T none {};

		operator T() const { return none; }

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(AuthorityNone<T>, none);
	};

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


	template <class T = std::string, class Auth = AuthorityHttp<T>>
			requires(std::same_as<std::string, T> || std::same_as<std::wstring, T>) &&
			std::same_as<AuthorityHttp<T>, Auth> struct Uri
	{
		UriScheme      scheme;
		T              separatorColonSlashSlash {"://"};
		Auth           authority;
		T              separatorSlash {"/"};
		std::vector<T> path {};
		T              separatorQuestion {"?"};
		std::map<T, T> query {};
		T              fragment {};

		T url {}; // contains the "balance" post Authority section

		operator T()
		{
			// Build the url back

			// Compose the final uri
			return std::format(
					"{}{}{}{}{}", scheme, separatorColonSlashSlash, authority, url.starts_with("/") ? "" : separatorSlash, url);
		}


		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Uri<T>, scheme, authority, url, path, query, fragment);
	};


	/// @brief Given an endpoint Uri, decompose it into the Uri object
	/// @tparam T Specify std::string or std::wstring (defaults to std::string)
	/// @tparam Auth The authority type (only AuthorityHttp is supported)
	/// @param aEndpoint The endpoint uri
	/// @return Object of Uri with base std::string or std::wstring
	template <class T = std::string, class Auth = AuthorityHttp<T>>
			Uri<T, Auth> static SplitUri(const T& aEndpoint) requires std::same_as<std::string, T> || std::same_as<std::wstring, T>
	{
		Uri<T> uri {};

#pragma region Comparision Helpers
		constexpr auto matchHttps = []() -> auto
		{
			if constexpr (std::is_same_v<T, std::wstring>) return L"https://";
			return "https://";
		}
		();
		constexpr auto matchHttp = []() -> auto
		{
			if constexpr (std::is_same_v<T, std::wstring>) return L"http://";
			return "http://";
		}
		();
		constexpr auto matchSlash = []() -> auto
		{
			if constexpr (std::is_same_v<T, std::wstring>) return L"/";
			return "/";
		}
		();
		constexpr auto matchColon = []() -> auto
		{
			if constexpr (std::is_same_v<T, std::wstring>) return L":";
			return ":";
		}
		();
		constexpr auto matchHash = []() -> auto
		{
			if constexpr (std::is_same_v<T, std::wstring>) return L"#";
			return "#";
		}
		();
		constexpr auto matchAt = []() -> auto
		{
			if constexpr (std::is_same_v<T, std::wstring>) return L"@";
			return "@";
		}
		();
		constexpr auto matchQuestion = []() -> auto
		{
			if constexpr (std::is_same_v<T, std::wstring>) return L"?";
			return "?";
		}
		();
		constexpr auto matchEq = []() -> auto
		{
			if constexpr (std::is_same_v<T, std::wstring>) return L"=";
			return "=";
		}
		();
		constexpr auto matchAmp = []() -> auto
		{
			if constexpr (std::is_same_v<T, std::wstring>) return L"&";
			return "&";
		}
		();


#pragma endregion

		size_t posProtocol = std::string::npos;

		if (aEndpoint.starts_with(matchHttps))
		{
			posProtocol = 8;
			uri.scheme  = UriScheme::WebHttps;
			uri.authority.port = 443; // replace later if present
		}
		else if (aEndpoint.starts_with(matchHttp))
		{
			posProtocol = 7;
			uri.scheme  = UriScheme::WebHttp;
			uri.authority.port = 80; // replace later if present
		}

		if (posProtocol != std::string::npos)
		{
			if (auto posAt = aEndpoint.find(matchAt, posProtocol); posAt != std::string::npos)
			{
				// We have a user part
				uri.authority.userInfo = aEndpoint.substr(posProtocol, (posAt - posProtocol));
				// Advance to the end of the user "@" which begins the host part
				posProtocol = ++posAt;
			}

			// Search for the next `/` which would allow us to extract the server:port portion.
			// It is possible that the aEndpoint does not contain the trailing `/` in which case
			// the url portion is "empty" and the only thing left is to attempt to extract the server
			// portion.
			auto pos2 = aEndpoint.find(matchSlash, posProtocol);

			// Extract the server:port portion.. make sure we don't calculate based on missing `/`
			uri.authority.host =
					aEndpoint.substr(posProtocol, pos2 != std::string::npos ? (pos2 - posProtocol) : std::string::npos);
			if (auto pos3 = uri.authority.host.find(matchColon); pos3 != std::string::npos)
			{
				// First Extract the port
				uri.authority.port = std::stoi(uri.authority.host.substr(++pos3));
				// Reset the server
				uri.authority.host = uri.authority.host.substr(0, (--pos3 - 0));
			}

			if (pos2 != std::string::npos)
			{
				// Finally, the rest of the endpoint is the url portion.
				uri.url = aEndpoint.substr(pos2);

				// Continue to split the path, query and fragment
				// Pull out the fragment
				auto posFragment = aEndpoint.rfind(matchHash);
				if (posFragment != std::string::npos)
				{
					// We have a fragment
					uri.fragment = aEndpoint.substr(posFragment + 1);
				}

				auto posQueryPart = aEndpoint.find(matchQuestion, pos2);

				auto pathSegment =
						aEndpoint.substr(pos2, posQueryPart != std::string::npos ? posQueryPart - (pos2) : std::string::npos);
				if (!pathSegment.empty()) uri.path = siddiqsoft::string2vector::parse<T>(pathSegment, matchSlash);

				auto querySegment =
						aEndpoint.substr(posQueryPart + 1,
				                         posFragment != std::string::npos ? (posFragment) - (posQueryPart + 1) : std::string::npos);
				if (!querySegment.empty()) uri.query = siddiqsoft::string2map::parse<T>(querySegment, matchEq, matchAmp);
			}
		}

		return std::move(uri);
	}
} // namespace siddiqsoft


template <class T> struct std::formatter<siddiqsoft::AuthorityHttp<T>> : std::formatter<T>
{
	auto format(const siddiqsoft::AuthorityHttp<T>& sv, std::format_context& ctx) { return std::formatter<T>::format(T(sv), ctx); }
};


template <class T> struct std::formatter<siddiqsoft::AuthorityLdap<T>> : std::formatter<T>
{
	auto format(const siddiqsoft::AuthorityLdap<T>& sv, std::format_context& ctx) { return std::formatter<T>::format(T(sv), ctx); }
};


template <class T> struct std::formatter<siddiqsoft::AuthorityNone<T>> : std::formatter<T>
{
	auto format(const siddiqsoft::AuthorityNone<T>& sv, std::format_context& ctx) { return std::formatter<T>::format(T(sv), ctx); }
};


template <class T>
struct std::formatter<std::variant<siddiqsoft::AuthorityHttp<T>, siddiqsoft::AuthorityLdap<T>, siddiqsoft::AuthorityNone<T>>>
	: std::formatter<T>
{
	auto format(const std::variant<siddiqsoft::AuthorityHttp<T>, siddiqsoft::AuthorityLdap<T>, siddiqsoft::AuthorityNone<T>>& sv,
	            std::format_context&                                                                                          ctx)
	{
		if (std::holds_alternative<siddiqsoft::AuthorityHttp<T>>(sv))
			return std::formatter<T>::format(sv.get<siddiqsoft::AuthorityHttp<T>>(), ctx);
		else if (std::holds_alternative<siddiqsoft::AuthorityLdap<T>>(sv))
			return std::formatter<T>::format(sv.get<siddiqsoft::AuthorityLdap<T>>(), ctx);

		return std::formatter<T>::format(sv.get<siddiqsoft::AuthorityNone<T>>(), ctx);
	}
};


template <> struct std::formatter<siddiqsoft::UriScheme> : std::formatter<std::string>
{
	auto format(const siddiqsoft::UriScheme& sv, std::format_context& ctx)
	{
		return std::formatter<std::string>::format(nlohmann::json(sv).get<std::string>(), ctx);
	}
};


template <class T> struct std::formatter<siddiqsoft::Uri<T>> : std::formatter<T>
{
	auto format(const siddiqsoft::Uri<T>& sv, std::format_context& ctx) { return std::formatter<T>::format(T(sv), ctx); }
};


#endif // !SPLITURI_HPP
