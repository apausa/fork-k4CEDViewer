#pragma once

#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"

namespace k4ced {

// Parameters for drawing SimTrackerHits.
struct SimTrackerHitsParams {
  int layer = 1;
  int size = 2;
  int marker = 0;
  unsigned colorScheme = 12;
};

// Parameters for drawing SimCalorimeterHits.
struct SimCalorimeterHitsParams {
  int layer = 2;
  int size = 2;
  int marker = 0;
  unsigned colorScheme = 12;
};

// Draws one SimTrackerHit collection.
void drawSimTrackerHits(const edm4hep::SimTrackerHitCollection& col, const SimTrackerHitsParams& p);

// Draws one SimCalorimeterHit collection.
void drawSimCalorimeterHits(const edm4hep::SimCalorimeterHitCollection& col, const SimCalorimeterHitsParams& p);

} // namespace k4ced
