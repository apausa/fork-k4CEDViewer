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

// Draws every collection from the Gaudi event store
void drawEventStore();

// Resets the log of drawn collections so the next event is drawn
void resetEventStore();

} // namespace k4ced
