#include "Gaudi/Auditor.h"
#include "Gaudi/Property.h"

#include "k4GaudiCED.h"

#include <string>
#include <vector>

// Stops the job after chosen algorithms and shows the event so far in glced
struct CEDAuditor final : Gaudi::Auditor {
  using Auditor::Auditor;

  Gaudi::Property<std::vector<std::string>> stopAfterSome{this, "StopAfterSome", {}, "Stop after these algorithms"};
  Gaudi::Property<bool> stepThroughEvery{this, "StepThroughEvery", false, "Stop after every "};

  StatusCode initialize() override {
    // Only connect auditor to glced if it is set up
    if (!stopAfterSome.value().empty() || stepThroughEvery) {
      k4ced::k4GaudiCED::connect();
      k4ced::k4GaudiCED::letAuditorWait();
    }

    return StatusCode::SUCCESS;
  }
};

DECLARE_COMPONENT(CEDAuditor)
