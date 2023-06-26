#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <string>

class Location
{
	private:

		std::string										_match;
		std::string										_modifier;
		std::string										_root;
		std::string										_index;
		std::vector<std::pair<int,std::string>>			_errorPages;
	
	public:

		Location();
		~Location();
		Location(Location const &);
		Location &			operator=(Location const &);
		std::string const &	getMatch(void) const;
		std::string const &	getModifier(void) const;
		std::string const &	getRoot(void) const;
		std::string const &	getIndex(void) const;
		std::vector<std::pair<int,std::string>> const &	getErrorPages(void) const;
		
		void			setMatch(std::string match);
		void			setModifier(std::string modifier);
		void			setRoot(std::string root);
		void			setIndex(std::string index);
		void			addErrorPage(int key, std::string value);
};

#endif