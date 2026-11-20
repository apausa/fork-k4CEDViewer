#pragma once

#include "podio/CollectionBase.h"

#include <string>
#include <utility>
#include <vector>

namespace k4ced {

// Returns every collection in the Gaudi event store.
std::vector<std::pair<std::string, const podio::CollectionBase*>> getEventStoreCollections();

// Draws one collection with the default parameters of its type
bool drawCollection(const std::string& name, const podio::CollectionBase& coll);

} // namespace k4ced
