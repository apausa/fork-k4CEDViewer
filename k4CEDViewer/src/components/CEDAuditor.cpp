#include "Gaudi/Auditor.h"
#include "Gaudi/Property.h"

#include <string>
#include <vector>

// Stops the job after chosen algorithms and shows the event so far in glced
struct CEDAuditor final : Gaudi::Auditor {
  using Auditor::Auditor;

  Gaudi::Property<std::vector<std::string>> stopAfterSome{this, "StopAfterSome", {}, "Stop after these algorithms"};
  Gaudi::Property<bool> stepThroughEvery{this, "StepThroughEvery", false, "Stop after every "};
};

DECLARE_COMPONENT(CEDAuditor)
