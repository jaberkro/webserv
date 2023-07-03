#ifndef LOCATION_HPP
# define LOCATION_HPP
# include <string>
# include <vector>

class Location
{
    private:

        std::string                                     _locationMatch;
        std::string                                     _locationModifier;
        std::vector<std::pair<std::string,std::string> > _locationDirectives;
        std::vector<std::pair<int,std::string> >         _errorPages;
        std::vector<std::string>                        _tryFiles;

        std::string                                     _root;
        std::vector<std::string>                        _indexes;
		bool											_autoindex;
    
    public:
        // functions
        void                setModifier(std::string modifier);
        const std::string   getModifier(void) const;

        void                setMatch(std::string match);
        const std::string   getMatch(void) const;

        void	            setRoot(std::string root);
		std::string	        getRoot() const;
        
        void	                    setIndexes(std::vector<std::string> indexes);
		std::vector<std::string>	getIndexes() const;
		std::string                 getIndex(size_t i) const;

        void	setAutoindex(bool autoindex);
		bool	getAutoindex() const;
};

#endif