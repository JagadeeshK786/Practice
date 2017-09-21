#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <list>

#include <conio.h>

#include <DC_File.h>
#include <DC_STR.h>
#include <liuzianglib.h>

struct request
{
	request() = default;

	request(const request&) = default;

	request(request&&) = default;

	request& operator=(const request&) = default;

	request& operator=(request&&) = default;

	std::string client_address;
	std::string time;
	std::string method;
	std::string uri;
	uint16_t status_code;
	std::string user_agent;	
	uint32_t content_length;
	std::string http_referer;
};

request parse_line(const std::string& str)noexcept {
	std::string temp;
	std::istringstream sst(str);
	request returnthis;
	sst >> returnthis.client_address >> temp >> temp >> returnthis.time >> temp >> returnthis.method >> returnthis.uri >> temp
		>> returnthis.status_code >> returnthis.content_length >> returnthis.http_referer;

	returnthis.user_agent = DC::STR::getSub(str, sst.tellg(), str.size());

	if (!returnthis.time.empty())
		returnthis.time.erase(returnthis.time.begin());

	if (!returnthis.method.empty())
		returnthis.method.erase(returnthis.method.begin());

	if (returnthis.user_agent.size() > 1) {
		returnthis.user_agent.resize(returnthis.user_agent.size() - 1);
		returnthis.user_agent.erase(returnthis.user_agent.begin());
	}

	if (returnthis.http_referer.size() > 1) {
		returnthis.http_referer.resize(returnthis.http_referer.size() - 1);
		returnthis.http_referer.erase(returnthis.http_referer.begin());
	}

	return returnthis;
}

std::vector<request> read_log(const DC::File::file_ptr& log_file) {
	std::string line;
	std::vector<request> returnthis;
	char ch{ 0 };

	while (true) {
		ch = fgetc(log_file.get());

		if (ch == EOF)
			break;

		if (ch == '\n') {
			returnthis.emplace_back(parse_line(line));
			line.clear();
			continue;
		}

		line.push_back(ch);
	}
	if (!line.empty())
		returnthis.emplace_back(parse_line(line));

	return returnthis;
}

uint64_t address2int(const std::string& addr)noexcept {
	if (addr.empty()) return 0;
	auto temp(DC::STR::replace(addr, DC::STR::find(addr, "."), ""));
	return DC::STR::toType<uint64_t>(temp);
}

int main() {
	std::string filename("access.log");
	//std::cout << "输入文件名:";
	//std::cin >> filename;

	DC::File::file_ptr log_file(fopen(filename.c_str(), "r"));
	if (!log_file)
		return 0;

	std::vector<request> lines(read_log(log_file));

	auto find_pred = [](const request& req) {
		if (req.uri == "/resource/HEIF-Utility-CN.zip")
			return true;
		return false;
	};

	std::list<request> result;
	decltype(std::find_if(lines.begin(), lines.end(), find_pred)) iter = lines.begin();
	
	while (true) {
		iter = std::find_if(iter + 1, lines.end(), find_pred);
		if (iter == lines.end()) break;
		result.push_back(*iter);
	}
	std::cout << result.size() << "\n";

	result.sort([](const request& req1, const request& req2) {
		return address2int(req1.client_address) > address2int(req2.client_address);
	});
	
	if (!result.empty() && result.size() > 1) {
		auto list_iter = result.begin();
		list_iter++;
		while (true) {
			if (list_iter == result.end())
				break;

			auto temp_list_iter = list_iter;
			
			if (list_iter->client_address == (--temp_list_iter)->client_address) {				
				result.erase(temp_list_iter);
				list_iter = result.begin();
				list_iter++;
				continue;
			}
			list_iter++;
		}
	}

	_getch();
}