#ifndef SERVER_HPP
# define SERVER_HPP
# include <vector>
# include <string>

class Server
{
	public:
		Server();
		Server(std::vector<unsigned short> ports, std::vector<std::string> hosts, std::vector<std::string> serverNames);
		Server(const Server &src);
		Server& operator=(const Server &src);
		~Server();

		void								addPort(unsigned short port);

		unsigned short						getPort(int i) const;
		const std::vector<unsigned short>	getPorts(void) const;

		const std::string					getHost(int i) const;
		const std::vector<std::string>		getHosts() const;

		const std::string					getServerName(int i) const;
		const std::vector<std::string>		getServerNames() const;

	private:
		std::vector<unsigned short>		_ports;
		std::vector<std::string>		_hosts;
		std::vector<std::string>		_serverNames;
};

#endif
