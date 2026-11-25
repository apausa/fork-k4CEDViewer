#include "Gaudi/Auditor.h"
#include "Gaudi/Property.h"

#include "k4GaudiCED.h"

#include <algorithm>
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

  void after(const std::string& event, const std::string& caller, const EventContext&, const StatusCode&) override {
    try {
      // Only execute breakpoint after an algorithm has ran
      if (event != IAuditor::Execute) {
        return;
      }

      const auto& names = stopAfterSome.value();

      // Only execute breakpoint for predefined algorithms
      if (std::find(names.begin(), names.end(), caller) == names.end()) {
        return;
      }

      // Take over draw() responsibilities
      k4ced::k4GaudiCED::sendEvent();
      k4ced::k4GaudiCED::processInput();
    } catch (...) {
      error() << "Stop after " << caller << " failed" << endmsg;
    }
  }
};

DECLARE_COMPONENT(CEDAuditor)
