#include "HBase/HBase.h"
#include <iostream>
#include <random>
#include <future>

#include<iostream>
#include<utility>
#include<type_traits>
using namespace std;

// LXMQ://vhostname/username:password@192.168.2.21:5672,192.168.2.22:5672,192.168.2.23:5672
// "amqp://user:password@hostname:port/vhost" notation
bool ParseLXConnectionString(const char* connstr)
{
	if (!connstr)
		return false;

	std::string str;
	do 
	{
		str += (char)towlower(*connstr);
	} while (*(++connstr) != 0);

	// LXMQ://
	int pos = str.find("lxmq://");
	if (pos != 0)
		return false;

	pos += 7;	// strlen("LXMQ://");
	int pos_old = pos;

	// vhost
	pos = str.find("/", pos_old);
	if (pos == std::string::npos)
		return false;

	std::string vhost = str.substr(pos_old, pos - pos_old);
	if (vhost.empty())
		vhost = "/";

	pos_old = pos + 1;

	// username
	pos = str.find(":", pos_old);
	if (pos == std::string::npos)
		return false;

	std::string username = str.substr(pos_old, pos - pos_old);
	pos_old = pos + 1;

	// psw
	pos = str.find("@", pos_old);
	if (pos == std::string::npos)
		return false;

	std::string psw = str.substr(pos_old, pos - pos_old);
	pos_old = pos + 1;

	while (true)
	{
		// ip 
		pos = str.find(":", pos_old);
		if (pos == std::string::npos)
			return false;

		std::string ip = str.substr(pos_old, pos - pos_old);
		pos_old = pos + 1;

		// port
		pos = str.find(",", pos_old);
		if (pos == std::string::npos)
		{
			std::string port = str.substr(pos_old, str.length() - pos_old);
			pos_old = pos + 1;

			// endpoints.push_back(endpt);
			break;
		}
		else
		{
			std::string port = str.substr(pos_old, pos - pos_old);
			pos_old = pos + 1;
			//endpoints.push_back(endpt);
		}
	}

	return true;
}

int main()
{
	ParseLXConnectionString("LXMQ://vhostname/username:password@192.168.2.21:5672,192.168.2.22:5672,192.168.2.23:5672");

	getchar();
	return 0;
}