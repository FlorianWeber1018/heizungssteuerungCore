#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H
#include <boost/property_tree/ptree_fwd.hpp>
namespace pt = boost::property_tree;

namespace EventHandler{
bool POST_ButtonClick(const boost::property_tree::ptree &passedParams);
bool POST_UpdateModuleParam(const boost::property_tree::ptree &passedParams);


}//namespace EventHandler
#endif // EVENTHANDLER_H
