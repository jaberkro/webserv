#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <vector>
# include <map>

class Location
{
	public:
		Location();
		Location(const Location &src);
		Location& operator=(const Location &src);
		~Location();

		void                				setModifier(std::string modifier);
		const std::string					getModifier(void) const;

		void								setMatch(std::string match);
		const std::string   				getMatch(void) const;

		void	            				setRoot(std::string root);
		std::string	        				getRoot() const;
		
		void	                    		setIndexes(std::vector<std::string> indexes);
		std::vector<std::string>			getIndexes() const;
		std::string                 		getIndex(size_t i) const;

		void								setAutoindex(bool autoindex);
		bool								getAutoindex() const;
		
		void								setMaxBodySize(unsigned long long maxBodySize);
		unsigned long long					getMaxBodySize() const;
		
		void								setErrorPages(std::map<int, std::string> errorPages);
		std::map<int, std::string> const &	getErrorPages() const;

		void	                    		setAllowed(std::vector<std::string> allowed);
		std::vector<std::string>			getAllowed() const;
		std::string                 		getAllow(size_t i) const;

		void	                    		setDenied(std::vector<std::string> denied);
		std::vector<std::string>			getDenied() const;
		std::string                 		getDeny(size_t i) const;

		void                				setReturn(int code, std::string text);
		int									getReturnCode(void) const;
		const std::string			   		getReturnLink(void) const;
		const std::string			   		getReturnMessage(void) const;

		void								setUploadDir(std::string uploadDir);
		std::string							getUploadDir() const;

		void								setCgiScriptName(std::string cgiScriptName);
		std::string							getCgiScriptName() const;

	private:
		std::string											_locationMatch;
		std::string											_locationModifier;
		std::vector<std::pair<std::string, std::string> >	_locationDirectives;
		std::map<int, std::string>							_errorPages;

		std::string											_root;
		std::vector<std::string>							_indexes;
		bool												_autoindex;
		unsigned long long									_maxBodySize;
		std::vector<std::string>							_allowed;
		std::vector<std::string>							_denied;
		int													_returnCode;
		std::string											_returnLink;
		std::string											_returnMessage;
		std::string											_uploadDir;
		std::string											_cgiScriptName;
};

#endif