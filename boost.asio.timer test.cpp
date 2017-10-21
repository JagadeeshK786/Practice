#include <boost\asio\steady_timer.hpp>
#include <thread>
#include <iostream>
#include <conio.h>
#include <memory>
#include "DC_Test.h"

class connection :public std::enable_shared_from_this<connection> {
public:
	connection(boost::asio::io_service& io_service) :timer(io_service) {}

	connection(const connection&) = delete;

	connection& operator=(const connection&) = delete;

public:
	void set_timeout(const std::chrono::seconds& timeout) {
		if (timeout.count() == 0)
			return;

		auto self = this->shared_from_this();
		timer.expires_from_now(timeout);
		timer.async_wait([self](const boost::system::error_code& ec) {
			if (!ec)
				self->close();
			else
				std::cout << "this async_wait has been stopped\n";
		});
	}

	void close() {
		std::cout << "close\n";
	}

private:
	boost::asio::steady_timer timer;
	DC::Test test;
};

int main() {
	{
		boost::asio::io_service service;
		auto p1 = std::make_shared<connection>(service);
		p1->set_timeout(std::chrono::seconds(5));
		p1->set_timeout(std::chrono::seconds(5));
		p1.reset();
		std::thread t([&service] {service.run(); });

		t.join();
	}
	_getch();
}