#include "EventStoreDrawer.h"

#include "DrawingAlgorithms.h"
#include "k4GaudiCED.h"
#include "k4GaudiCEDUtils.h" // warning()

#include "k4FWCore/DataWrapper.h"     // DataWrapperBase
#include "k4FWCore/FunctionalUtils.h" // frameLocation

#include "DD4hep/Detector.h"

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
#include <set>

// Logo of collection names already drawn in the event
static std::set<std::string> drawnCollections;

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

  // Draw SimCalorimeterHits
  else if (type == edm4hep::SimCalorimeterHitCollection::typeName) {
    const SimCalorimeterHitsParams params{};

    k4GaudiCED::add_layer_description(name, params.layer);
    drawSimCalorimeterHits(static_cast<const edm4hep::SimCalorimeterHitCollection&>(coll), params);

    return true;
  }

  // Draw Vertices
  else if (type == edm4hep::VertexCollection::typeName) {
    const VerticesParams params{};

    k4GaudiCED::add_layer_description(name, params.layer);
    drawVertices(static_cast<const edm4hep::VertexCollection&>(coll), params, 0);

    return true;
  }

  // Draw CalorimeterHits
  else if (type == edm4hep::CalorimeterHitCollection::typeName) {
    const CalorimeterHitsParams params{};

    k4GaudiCED::add_layer_description(name, params.layer);
    drawCalorimeterHits(static_cast<const edm4hep::CalorimeterHitCollection&>(coll), params);

    return true;
  }

  // Draw TrackerHit3Ds
  else if (type == edm4hep::TrackerHit3DCollection::typeName) {
    const TrackerHitsParams params{};

    k4GaudiCED::add_layer_description(name, params.layer);
    drawTrackerHits(static_cast<const edm4hep::TrackerHit3DCollection&>(coll), params);

    return true;
  }

  // Draw TrackerHitPlanes
  else if (type == edm4hep::TrackerHitPlaneCollection::typeName) {
    const TrackerHitsParams params{};

    k4GaudiCED::add_layer_description(name, params.layer);
    drawTrackerHits(static_cast<const edm4hep::TrackerHitPlaneCollection&>(coll), params);

    return true;
  }

  // Draw MCParticles
  else if (type == edm4hep::MCParticleCollection::typeName) {
    const MCParticlesParams params{};

    drawMCParticles(static_cast<const edm4hep::MCParticleCollection&>(coll), name, params);

    return true;
  }

  // Draw Tracks
  else if (type == edm4hep::TrackCollection::typeName) {
    TracksParams params{};
    params.drawHelixForTracks = 0; // @check first track state

    drawTracks(static_cast<const edm4hep::TrackCollection&>(coll), name, params);

    return true;
  }

  // Draw Clusters
  else if (type == edm4hep::ClusterCollection::typeName) {
    const ClustersParams params{};

    k4GaudiCED::add_layer_description(name, params.layer);
    drawClusters(static_cast<const edm4hep::ClusterCollection&>(coll), params);

    return true;
  }

  // Draw Jets or ReconstructedParticles
  else if (type == edm4hep::ReconstructedParticleCollection::typeName) {
    const auto& particles = static_cast<const edm4hep::ReconstructedParticleCollection&>(coll);

    if (name.find("Jet") != std::string::npos) {
      const JetsParams params{};

      k4GaudiCED::add_layer_description(name, params.layer);
      drawJets(particles, params);
    } else {
      const ReconstructedParticlesParams params{};

      k4GaudiCED::add_layer_description(name, params.layer);
      drawReconstructedParticles(particles, params);
    }

    return true;
  }

  return false;
}

void k4ced::drawEventStore() {
  if (drawnCollections.empty()) {
    k4GaudiCED::clearEvent();
    k4GaudiCED::drawDD4hepDetector(dd4hep::Detector::getInstance(), false, {});
  }

  for (const auto& [name, coll] : getEventStoreCollections()) {
    if (drawnCollections.contains(name)) {
      continue; // Already drawn at an earlier stop
    }

    if (drawCollection(name, *coll)) {
      drawnCollections.insert(name);
    }
  }
}

void k4ced::resetEventStore() { drawnCollections.clear(); }
