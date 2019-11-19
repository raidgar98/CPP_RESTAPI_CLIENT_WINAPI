#pragma once

#include "MainComponent.h"
#include <random>
#include <cpprest/http_client.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class BasicRefresher
{
protected:

	static str protocol;
	static str port;

	MainComponent* src{ nullptr };
	std::vector<std::wstring> address_pool;
	std::map<std::wstring, uint16_t> reconnections;
	
	BasicRefresher(MainComponent* i_src) : src{ i_src }
	{
		while (!src->mtx_addresses.try_lock()) { std::this_thread::yield(); }
		address_pool = src->addresses; 
		src->mtx_addresses.unlock();
		for (const auto& var : address_pool)
			reconnections[var] = 0;
	}

	pplx::task<void> POST_request(const std::wstring& host, const std::wstring& uri, json::value& in_out) const
	{
		return http_client(host).request(methods::POST, uri, in_out.serialize(), L"application/json").then([&](http_response resp)
			{
				if (resp.status_code() != status_codes::OK) in_out = json::value::null();
				in_out = json::value::parse(resp.extract_string().get());
			});
	}

	pplx::task<void> GET_request(const std::wstring& host, const std::wstring& uri, json::value& out) const
	{
		return http_client(host).request(methods::GET, uri).then([&](http_response resp)
			{
				if (resp.status_code() != status_codes::OK) out = json::value::null();
				out = json::value::parse(resp.extract_string().get());
			});
	}

public:

	static bool online(const std::wstring& host)
	{
		try {
			return http_client(host).request(methods::GET, L"/api").get().status_code() == status_codes::OK;
		}
		catch (const std::exception & e)
		{
			throw;
		}
	}


	virtual void operator()() = 0;

	~BasicRefresher() { src = nullptr; }
};