#pragma once

#include "BasicRefresher.h"
#include <random>
#include <cpprest/http_client.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

#define set_progress_bar(pb, val) if(src->specify_run.try_lock())\
{\
	src->specify_run.unlock();\
	\
	while(!src->GLUER(pb,Lc,k).try_lock()) {}\
	src->GLUER(pb,Valu,e) = val;\
	src->GLUER(pb, Lc,k).unlock();\
}else return;

#define chck_close(x) if(src->specify_run.try_lock()){ src->specify_run.unlock(); x; } else return;

class SpecifyRefresher : public BasicRefresher
{
private:

	const str addr;

	int get_api_int(const str& url, const str& key)
	{
		json::value data;
		int ret = 0;
		try { GET_request(protocol + addr + L":" + port, url, data).wait(); }
		catch (...) { return -1; }
		ret = data[key].as_integer();
		return ret;
	}

	double get_api_real(const str& url, const str& key)
	{
		json::value data;
		double ret = 0.0;
		try { GET_request(protocol + addr + L":" + port, url, data).wait(); }
		catch (...) { return -1; }
		ret = data[key].as_double();
		return ret;
	}

	str get_api_str(const str& url, const str& key)
	{
		json::value data;
		str ret{ L"" };
		try { GET_request(protocol + addr + L":" + port, url, data).wait(); }
		catch (...) { return L"\0"; }
		ret = data[key].as_string();
		return ret + L'\0';
	}

	str get_api_SI(const str& url, const str& key)
	{
		json::value data;
		try { GET_request(protocol + addr + L":" + port, url, data).wait(); }
		catch (...) { return L"\0"; }
		return std::to_wstring(data[key].as_double()) + L" " + data[L"SI"].as_string();
	}

	str get_api_version(const str& url, const str& key, const str& additional_wrapper = str())
	{
		json::value data;
		try { GET_request(protocol + addr + L":" + port, url, data).wait(); }
		catch (...) { return L"\0"; }
		if (additional_wrapper != str())
			data = data[additional_wrapper];
		return std::to_wstring(data[key][L"major"].as_number().to_int32()) + L"." + std::to_wstring(data[key][L"minor"].as_number().to_int32());
	}

	str get_api_array(const str& url, const str& key)
	{
		json::value data;
		try { GET_request(protocol + addr + L":" + port, url, data).wait(); }
		catch (...) { return L"\0"; }
		const auto ret = data[key].as_array();
		str ret_str{ L"" };
		for (const auto& var : ret)
			ret_str += var.as_string() + L"; ";
		return ret_str;
	}

	std::vector<str>&& get_api_array(const str& url, const str& key, bool)
	{
		json::value data;
		try { GET_request(protocol + addr + L":" + port, url, data).wait(); }
		catch (...) { return std::vector<str>(); }
		const auto ret = data[key].as_array();
		std::vector<str> ret_vec;
		for (const auto& var : ret)
			ret_vec.emplace_back(var.as_string());
		return std::move(ret_vec);
	}

public:
	
	SpecifyRefresher(MainComponent* src, const str& i_addr) : BasicRefresher{ src }, addr{ i_addr } { src = nullptr; }

	virtual void operator()() override
	{
		
		while (src->specify_run.try_lock())
		{
			src->specify_run.unlock();
			chck_close( src->specProcModel.set_text(
				get_api_str(L"/api/cpu/model", L"model").c_str())
			);
			chck_close( src->specProcCores.set_text(
				std::to_wstring(get_api_int(L"/api/cpu/cores", L"cores")).c_str()) 
			);
			chck_close(src->specRamCapacity.set_text(
				get_api_SI(L"/api/mem/size", L"space_total").c_str())
			);
			chck_close( src->specOS.set_text(
				get_api_str(L"/api/system/os_info", L"os_info").c_str())
			);
			chck_close( src->specOSVersion.set_text(
				get_api_version(L"/api/system/os_version", L"os").c_str()) 
			);
			chck_close( src->specOSSPVersion.set_text(
				get_api_version(L"/api/system/os_version", L"sp").c_str())
			);
			chck_close( src->specIP.set_text(
				get_api_array(L"/api/net", L"avaiable_ips").c_str()) 
			);

			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}

};