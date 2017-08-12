#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#include <DC_http.h>

#include <iostream>
#include <conio.h>

void dosth()noexcept {}

int main() {
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();

	SSL_CTX * ctx = SSL_CTX_new(SSLv23_client_method());
	SSL * ssl;

	BIO *bio = BIO_new_ssl_connect(ctx);
	BIO_get_ssl(bio, &ssl);
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

	BIO_set_conn_hostname(bio, "liuziangexit.com:443");

	if (bio == NULL)
	{
		std::cout << "bio==null\n";
		_getch();
		/* Handle the failure */
	}
	if (BIO_do_connect(bio) <= 0)
	{
		std::cout << "connect failed\n";
		_getch();
		/* Handle failed connection */
	}
	std::cout << "connect ok\n";
	if (SSL_get_verify_result(ssl) != X509_V_OK)
	{
		std::cout << "verification failed\n";
	}

	//_getch();
	char buffer[8096] = { 0 };
	DC::Web::http::request req;
	req.setHTTPVersion(1.1);
	req.Method() = DC::Web::http::methods::GET;
	req.URI() = "/";
	req.Headers().add(DC::Web::http::addHeader("Host", "liuziangexit.com"));
	DC::Web::http::add_ContentLength(req);
	auto sendthis(req.toStr());
	BIO_write(bio, sendthis.c_str(), sendthis.size());
	auto look = BIO_read(bio, buffer, 8096);
	
	std::cout << "\n\n\n\n" << buffer;;
	dosth();
	_getch();
}