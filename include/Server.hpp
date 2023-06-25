#ifndef SERVER_HPP
# define SERVER_HPP
# include "Location.hpp"
# include <vector>
# include <string>
# include <utility>
# include <iterator>

class Server
{
	public:
		Server();
		// Server(std::vector<std::pair<std::string, unsigned short> > listens, std::vector<std::string> serverNames);
		Server(const Server &src);
		Server& operator=(const Server &src);
		~Server();

		// std::vector<std::pair<std::string, unsigned short> >::iterator pairIterator;
		// std::vector<std::string>::iterator 							stringIterator;

		// pairIterator			getListens(void);
		// stringIterator			getServerNames(void);


		void								addListen(std::pair<std::string, unsigned short> listen);
		void								addServerName(std::string name);
		void								addLocation(Location location);

		const std::vector<std::pair<std::string, unsigned short> >	getListens() const;
		const std::string					getHost(int i) const;
		unsigned short						getPort(int i) const;

		const std::vector<std::string>		getServerNames() const;
		const std::string					getServerName(int i) const;

		const std::vector<Location>			getLocations() const;
		const Location						getLocation(int i) const;

	private:
		std::vector<std::pair<std::string, unsigned short> >	_listens;
		std::vector<std::string>								_serverNames;
		std::vector<Location>									_locations;
};

Location parseLocation(std::fstream &file, std::string line);

#endif
