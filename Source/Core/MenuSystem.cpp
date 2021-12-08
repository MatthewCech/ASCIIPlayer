#include "MenuSystem.hpp"

// Static init
std::map<std::string, Container*> MenuRegistry::registry_ = std::map<std::string, Container*>();

