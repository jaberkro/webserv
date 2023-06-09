#ifndef CONFIG_HPP
# define CONFIG_HPP
# include "Server.hpp"
# include <vector>

class Config 
{
	public:
		Config();
		Config(const Config &src);
		Config& operator=(const Config &src);
		~Config();

		void						addServer(Server &Server);
		const Server& 				getServer(int i) const;
		const std::vector<Server>& 	getServers() const;

	private:
		std::vector<Server>	_servers;
};

#endif
