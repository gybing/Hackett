#include <iostream>
#include <random>
#include <future>

#include<iostream>
#include<utility>
#include<type_traits>
using namespace std;

std::string& replace(const std::string& text, const std::string& from, const std::string& to)
{
	std::string dest(text);

	if (!dest.empty() && !from.empty() && !to.empty())
	{
		for (std::string::size_type pos = 0;
			(pos = dest.find(from, pos)) != std::string::npos;
			pos += to.size())
		{
			dest.replace(pos, from.size(), to);
		}
	}

	return dest;
}

int main()
{
	std::string s1 = "abcdefg";
	std::string s2 = "";
	std::string s3 = "%%%";
	std::string s4;

	s2 = "a";
	s4 = replace(s1, s2, s3);

	s2 = "bc";
	s4 = replace(s1, s2, s3);

	s2 = "g";
	s4 = replace(s1, s2, s3);

	s2 = "fg";
	s4 = replace(s1, s2, s3);

	s2 = "%";
	s4 = replace(s1, s2, s3);

	s2 = "\n";
	s4 = replace(s1, s2, s3);

	s2 = " ";
	s4 = replace(s1, s2, s3);

	getchar();
	return 0;
}