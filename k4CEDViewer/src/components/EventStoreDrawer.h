#pragma once

#include "podio/CollectionBase.h"

#include <string>
#include <utility>
#include <vector>

namespace k4ced {

// Returns every collection in the Gaudi event store.
std::vector<std::pair<std::string, const podio::CollectionBase*>> getEventStoreCollections();

} // namespace k4ced
