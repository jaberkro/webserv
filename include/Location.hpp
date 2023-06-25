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
    
    public:

        // functions
        void                setModifier(std::string modifier);
        const std::string   getModifier(void) const;

        void                setMatch(std::string match);
        const std::string   getMatch(void) const;

};

#endif