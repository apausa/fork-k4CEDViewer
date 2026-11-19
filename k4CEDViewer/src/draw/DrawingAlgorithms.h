#pragma once

#include "edm4hep/CalorimeterHitCollection.h"
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/TrackerHit3DCollection.h"
#include "edm4hep/TrackerHitPlaneCollection.h"
#include "edm4hep/VertexCollection.h"

#include <string>

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

// Parameters for drawing Vertices.
struct VerticesParams {
  int layer = 6;
  int size = 2;
  int marker = 3;
  unsigned colorScheme = 12;
};

// Parameters for drawing CalorimeterHits.
struct CalorimeterHitsParams {
  int layer = 12;
  int size = 2;
  int marker = 0;
  int color = 0xee0000; // #F00 (red)
};

// Parameters for drawing TrackerHits.
struct TrackerHitsParams {
  int layer = 11;
  int size = 2;
  int marker = 0;
  int color = 0xee0000; // #F00 (red)
};

// Parameters for drawing Jets.
struct JetsParams {
  int layer = 4;
  std::string ecalBarrelName = "EcalBarrel";
  std::string ecalEndcapName = "EcalEndcap";
};

// Parameters for drawing MCParticles.
struct MCParticlesParams {
  int layer = 0;
  int size = 3;
  double mcpECut = 0.01; // GeV
  bool usingParticleGun = false;
  bool drawMCParticlesCreatedInSimulation = false;
  std::string ecalBarrelName = "EcalBarrel";
  std::string ecalEndcapName = "EcalEndcap";
  std::string hcalBarrelName = "HcalBarrel";
  std::string hcalEndcapName = "HcalEndcap";
};

// Parameters for drawing Tracks.
struct TracksParams {
  int layer = 5;
  int size = 3;
  int marker = 0;
  int drawHelixForTracks = 1; // -1: none, 0: default, 1: atIP, 2: atFirstHit, 3: atLastHit, 4: atCalorimeter
  float lineThickness = 1.;
  bool useColorForHelixTracks = false; // 0: disabled (lightgrey), 1: enabled
  unsigned colorScheme = 0;
};

// Draws one SimTrackerHit collection.
void drawSimTrackerHits(const edm4hep::SimTrackerHitCollection& col, const SimTrackerHitsParams& p);

// Draws one SimCalorimeterHit collection.
void drawSimCalorimeterHits(const edm4hep::SimCalorimeterHitCollection& col, const SimCalorimeterHitsParams& p);

// Draws one Vertex collection.
void drawVertices(const edm4hep::VertexCollection& col, const VerticesParams& p, unsigned colorCode);

// Draws one CalorimeterHit collection.
void drawCalorimeterHits(const edm4hep::CalorimeterHitCollection& col, const CalorimeterHitsParams& p);

// Draws one TrackerHit3D or TrackerHitPlane collection.
template <class Collection>
void drawTrackerHits(const Collection& col, const TrackerHitsParams& p);

// Draws one Jet collection, with each jet as a cone.
void drawJets(const edm4hep::ReconstructedParticleCollection& col, const JetsParams& p);

// Draws one MCParticle collection. layerDescription is added to the layer if any particle is drawn.
void drawMCParticles(const edm4hep::MCParticleCollection& col, const std::string& layerDescription,
                     const MCParticlesParams& p);

// Draws one Track collection. layerDescription is added to the layer if any track is drawn.
void drawTracks(const edm4hep::TrackCollection& col, const std::string& layerDescription, const TracksParams& p);

} // namespace k4ced
