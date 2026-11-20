#include "EventStoreDrawer.h"

#include "DrawingAlgorithms.h"
#include "k4GaudiCED.h"
#include "k4GaudiCEDUtils.h" // warning()

#include "k4FWCore/DataWrapper.h"     // DataWrapperBase
#include "k4FWCore/FunctionalUtils.h" // frameLocation

#include "GaudiKernel/AnyDataWrapper.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartIF.h"

#include <memory> //  std::unique_ptr.

std::vector<std::pair<std::string, const podio::CollectionBase*>> k4ced::getEventStoreCollections() {
  // Get the event store
  auto eventSvc = Gaudi::svcLocator()->service<IDataProviderSvc>("EventDataSvc");

  SmartIF<IDataManagerSvc> mgr{eventSvc};
  if (!eventSvc || !mgr) {
    warning() << "Cannot access the Gaudi event store" << std::endl;
    return {};
  }

  // Get /Event at the top of the event store tree
  SmartDataPtr<DataObject> root(eventSvc, "/Event");
  if (!root || !root->registry()) {
    warning() << "Cannot find /Event in the Gaudi event store" << std::endl;
    return {};
  }

  // Get every entry directly below "/Event"
  std::vector<IRegistry*> leaves;
  if (mgr->objectLeaves(root->registry(), leaves).isFailure()) {
    warning() << "Cannot list collections in the Gaudi event store" << std::endl;
    return {};
  }

  // Keep the entries that are collections
  std::vector<std::pair<std::string, const podio::CollectionBase*>> collections;
  for (const auto* leaf : leaves) {
    if (leaf->name() == k4FWCore::frameLocation) {
      continue; // Skip the "/_Frame" entry from k4FWCore
    }

    const podio::CollectionBase* collection = nullptr;

    if (const auto* wrapper = dynamic_cast<AnyDataWrapper<std::unique_ptr<podio::CollectionBase>>*>(leaf->object())) {
      collection = wrapper->getData().get();
    } else if (auto* oldWrapper = dynamic_cast<DataWrapperBase*>(leaf->object())) {
      collection = oldWrapper->collectionBase(); // Fallback for older wrapper
    }

    if (!collection) {
      continue; // Skip collections that are not wrapped
    }

    collections.emplace_back(leaf->name().substr(1), collection); // Remove the leading "/"
  }

  return collections;
}

bool k4ced::drawCollection(const std::string& name, const podio::CollectionBase& coll) {
  const auto type = coll.getTypeName();

  // Draw SimTrackerHits
  if (type == edm4hep::SimTrackerHitCollection::typeName) {
    const SimTrackerHitsParams params{};

    k4GaudiCED::add_layer_description(name, params.layer);
    drawSimTrackerHits(static_cast<const edm4hep::SimTrackerHitCollection&>(coll), params);

    return true;
  }

  return false;
}
