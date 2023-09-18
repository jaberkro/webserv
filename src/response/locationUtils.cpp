#include "Response.hpp"
#include <iostream>


/**
 * @brief iterates over locations and identifies the location with the exact
 * match for the target URI, if available
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance 
 * that is the exact match for the target. If no exact match was found, an 
 * iterator pointing to the end of the locations vector is returned.
 */
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

/**
 * @brief iterates over locations and identifies the location with the greatest
 * overlap with the target URI. To enable the comparison, the target and match 
 * URI are split into elements by calling the function splitURI().
 * 
 * @param target the target URI
 * @param locations a reference to a vector of Locations
 * @return std::vector<Location>::iterator pointing to a Location instance 
 * that is the closest match for the target. If no location is found, iterator 
 * to the end of the locations vector is returned.
 */
locIterator	findClosestLocationMatch(std::string target, \
	std::vector<Location> const & locations)
{
	size_t						overlap = 0;
	std::vector<std::string>	targetSplit;
	locIterator	longest = 		locations.end();
	size_t						idx;
	size_t						len = 0;

	// std::cerr << "Finding closest loc match" << std::endl;
	splitUri(target, targetSplit);
	// for (std::vector<std::string>::iterator i = targetSplit.begin(); \
	// 	i != targetSplit.end(); i++) // JMA: what does this do? Can it go?
	for (auto it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getModifier() == "=")
			continue;
		std::vector<std::string>	matchSplit;
		splitUri(it->getMatch(), matchSplit);
		len = matchSplit.size();
		// std::cerr << "Location " << it->getMatch() << std::endl;
		
		for (idx = 0; idx < len; idx++)
		{
			// std::cerr << "comparing " << targetSplit[idx] << " and " << matchSplit[idx] << std::endl;
			if (idx == targetSplit.size() || \
				targetSplit[idx] != matchSplit[idx])
			{
				idx = 0;
				break;
			}
		}
		if (idx > overlap)
		{
			overlap = idx;
			longest = it;
		}
		matchSplit.clear();
	}
	// std::cerr << "returning " << longest->getMatch() << std::endl;

	return (longest);
}

/**
 * @brief splits an URI path into chunks (a chunk is either a '/' character or 
 * a string between two '/' characters). Ignores any potential query part 
 * (delimited by a '?' character)
 * 
 * @param uri the uri string to be split
 * @param chunks reference to a vector, in which the chunks are to be placed
 */
void	splitUri(std::string const & uri, \
	std::vector<std::string> & chunks)
{
	size_t	begin = 0;
	size_t	end;

	while (begin < uri.length()/*  && begin < uri.find_first_of('?') */)
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
