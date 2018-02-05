#include <map>
#include <ASCIIMenus/menu-system.hpp>


// Static init
std::map<std::string, Container *> MenuRegistry::registry_ = std::map<std::string, Container *>();