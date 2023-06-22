#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <string>

class Location
{
	private:

		std::string										_locationMatch;
		std::string										_locationModifier;
		std::vector<std::pair<std::string,std::string>>	_locationDirectives;
		std::vector<std::pair<int,std::string>>			_errorPages;
		std::vector<std::string>						_tryFiles;
	
	public:

		// functions
		Location();
		~Location();
		Location(Location const &);
		Location &		operator=(Location const &);
		std::string const &	getLocationMatch(void) const;
		std::string const &	getLocationModifier(void) const;
		std::vector<std::pair<std::string,std::string>> const &	getLocationDirectives(void) const;
		std::vector<std::pair<int,std::string>> const &	getErrorPages(void) const;
		
		void			setLocationMatch(std::string str);
		void			setLocationModifier(std::string str);
		void			addDirective(std::string key, std::string value);
		void			addErrorPage(int key, std::string value);
};

#endif