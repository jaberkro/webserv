#include <string>
#include <iostream>

std::string protectedSubstr(std::string s, size_t start)
{
	std::string	newString;

	if (s == "")
		return ("");
	try
	{
		newString = s.substr(start);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		std::cerr << s << std::endl;
		exit(EXIT_FAILURE);
	}
	return (newString);
}

std::string protectedSubstr(std::string s, size_t start, size_t size)
{
	std::string	newString;

	if (s == "")
		return ("");
	try
	{
		newString = s.substr(start, size);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		std::cerr << s << std::endl;
		exit(EXIT_FAILURE);
	}
	return (newString);
}

std::string	ltrim(std::string s)
{
	size_t i = 0;

	if (s == "")
		return ("");
	while (i < s.size() && (s.at(i) == '\t' || s.at(i) == ' ' || \
	s.at(i) == '\v' || s.at(i) == '\b' || s.at(i) == '\r'))
	{
		i++;
	}
	if (i == s.size())
		return ("");
	else if (i == 0)
		return (s);
	return (protectedSubstr(s, i));
}

std::string	rtrim(std::string s)
{
	size_t i = s.size() - 1;

	if (s == "")
		return ("");
	while (i >= 0 && (s.at(i) == '\t' || s.at(i) == ' ' || s.at(i) == '\v' || \
	s.at(i) == '\b' || s.at(i) == '\r'))
	{
		i--;
	}
	if (i < 0)
		return ("");
	else if (i == s.size() - 1)
		return (s);
	return (protectedSubstr(s, 0, i + 1));
}
