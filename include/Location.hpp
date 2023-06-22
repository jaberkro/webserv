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
		Location(Location &);
		Location &	operator=(Location &);
		void	setLocationMatch(std::string str);
		void	setLocationModifier(std::string str);
		void	addDirective(std::string key, std::string value);
		void	addErrorPage(int key, std::string value);
};

#endif