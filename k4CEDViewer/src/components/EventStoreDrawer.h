#pragma once

#include <string>
#include <vector>

namespace k4ced {

// Returns the name of every collection in the event store.
std::vector<std::string> getEventStoreCollections();

} // namespace k4ced
