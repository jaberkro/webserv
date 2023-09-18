#include "Response.hpp"
#include <iostream>

locIterator	findExactLocationMatch(std::string target, \
std::vector<Location> const & locations)
{
	locIterator	it;

	for (it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getModifier() != "=")
			continue;
		if (target == it->getMatch())
			return (it);
	}
	return (it);
}

locIterator	findWildcardLocationMatch(std::string target, \
std::vector<Location> const & locations)
{
	locIterator	it;
	std::vector<std::string>				targetSplit;

	// std::cerr << "Finding wildcard loc match" << std::endl;
	for (it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getModifier() != "=" && it->getMatch().find('*') == 0)
		{
			std::string	needle = it->getMatch().substr(1);
			splitUri(target, targetSplit);
			for (size_t i = 0; i < targetSplit.size(); i++)
			{
				size_t	idx = targetSplit[i].find(needle);
				if (idx < std::string::npos && \
					idx + needle.length() == targetSplit[i].length())
					return (it);
			}
		}
	}
	return (it);
}

static size_t compareTargetSplitMatchSplit(std::vector<std::string> targetSplit, \
std::vector<std::string> matchSplit)
{
	size_t	len = matchSplit.size();
	size_t	idx;

	for (idx = 0; idx < len; idx++)
	{
		// std::cerr << "comparing " << targetSplit[idx] << " and ";
		// std::cerr << matchSplit[idx] << std::endl;
		if (idx == targetSplit.size() || targetSplit[idx] != matchSplit[idx])
		{
			idx = 0;
			break;
		}
	}
	return (idx);
}

locIterator	findClosestLocationMatch(std::string target, \
std::vector<Location> const & locations)
{
	size_t						overlap = 0;
	std::vector<std::string>	targetSplit;
	locIterator	longest = 		locations.end();
	size_t						idx;

	// std::cerr << "Finding closest loc match" << std::endl;
	splitUri(target, targetSplit);
	for (auto it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getModifier() == "=")
			continue;
		std::vector<std::string>	matchSplit;
		splitUri(it->getMatch(), matchSplit);
		// std::cerr << "Location " << it->getMatch() << std::endl;
		idx = compareTargetSplitMatchSplit(targetSplit, matchSplit);
		if (idx > overlap)
		{
			overlap = idx;
			longest = it;
		}
		matchSplit.clear();
	}
	std::cerr << "Location: " << longest->getMatch() << std::endl;
	return (longest);
}

void	splitUri(std::string const & uri, std::vector<std::string> & chunks)
{
	size_t	begin = 0;
	size_t	end;

	while (begin < uri.length())
	{
		end = uri[begin];
		if (uri[begin] == '/') 
			end = uri.find_first_of('/', begin) + 1;
		else
			end = uri.find_first_of('/', begin);
		chunks.push_back(uri.substr(begin, end - begin));
		begin = end;
	}
}
