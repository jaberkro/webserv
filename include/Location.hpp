#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <vector>
# include <string>

class Location
{
	private:

		std::string								_match;
		std::string								_modifier;
		std::string								_root;
		std::string								_index;
		std::vector<std::pair<int,std::string>>	_errorPages;
		std::vector<std::string>				_indexes;
		bool									_autoindex;
		unsigned int							_maxBodySize;

	public:

		Location();
		~Location();
		Location(Location const &);
		Location &			operator=(Location const &);
		
		void				setMatch(std::string match);
		std::string const &	getMatch(void) const;

		void				setModifier(std::string modifier);
		std::string const &	getModifier(void) const;

		void				setRoot(std::string root);
		std::string const &	getRoot(void) const;

		void						setIndexes(std::vector<std::string> indexes);
		std::vector<std::string>	getIndexes() const;
		// std::string				getIndex(size_t i) const;
		void				setIndex(std::string index);
		std::string const &	getIndex(void) const;

		void				setAutoindex(bool autoindex);
		bool				getAutoindex() const;
		
		void				setMaxBodySize(unsigned int maxBodySize);
		unsigned int		getMaxBodySize() const;

		void			addErrorPage(int key, std::string value);
		std::vector<std::pair<int,std::string>> const &	getErrorPages(void) const;
};

#endif