#include "Gaudi/Auditor.h"
#include "Gaudi/Property.h"

#include "EventStoreDrawer.h"
#include "k4GaudiCED.h"
#include "k4GaudiCEDUtils.h"

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"

#include <algorithm>
#include <set>
#include <string>
#include <vector>

// Stops the job after chosen algorithms and shows the event so far in glced
struct CEDAuditor final : Gaudi::Auditor {
  using Auditor::Auditor;

  Gaudi::Property<std::vector<std::string>> stopAfterSome{this, "StopAfterSome", {}, "Stop after these algorithms"};
  Gaudi::Property<bool> stepThroughEvery{this, "StepThroughEvery", false, "Stop after every "};

  EventContext::ContextEvt_t currentEvent = EventContext::INVALID_CONTEXT_EVT;

  std::set<std::string> toExclude;

  std::set<std::string> writers;
  bool isEventWritten = false;

  StatusCode initialize() override {
    // Only connect auditor to glced if it is set up
    if (!stopAfterSome.value().empty() || stepThroughEvery) {
      k4ced::k4GaudiCED::connect();
      k4ced::k4GaudiCED::letAuditorWait();
    }

    return StatusCode::SUCCESS;
  }

  void before(const std::string& event, const std::string&, const EventContext& ctx) override {
    if (event == IAuditor::Execute && currentEvent == EventContext::INVALID_CONTEXT_EVT) {
      for (const auto* alg : service<IAlgManager>("ApplicationMgr")->getAlgorithms()) {
        // Required for skipping all sequencers and the Reader when executing breakpoint for every algorithm
        if (alg->isSequence() || alg->type() == "Reader") {
          toExclude.insert(alg->name());
        }

        // Required for returning once the file is written in reconstruction
        if (alg->type() == "Writer") {
          writers.insert(alg->name());
        }
      }
    }

    // Reset what was drawn before the first algorithm of a new event
    if (event == IAuditor::Execute && ctx.evt() != currentEvent) {
      currentEvent = ctx.evt();
      k4ced::resetEventStore();
      k4ced::k4GaudiCED::resetEventStarted();
      isEventWritten = false;
    }
  }

  void after(const std::string& event, const std::string& caller, const EventContext&, const StatusCode&) override {
    try {
      // Only execute breakpoint after an algorithm has ran
      if (event != IAuditor::Execute) {
        return;
      }

      // In reconstruction, return when the event has been written and there is nothing left to run
      if (isEventWritten || writers.contains(caller)) {
        isEventWritten = true;
        return;
      }

      const auto& names = stopAfterSome.value();

      const bool isStopAfterSome = std::find(names.begin(), names.end(), caller) != names.end();
      const bool isStepThroughEvery = stepThroughEvery && !toExclude.contains(caller);

      // Only execute breakpoint either for predefined algorithms or every algorithm
      if (!isStopAfterSome && !isStepThroughEvery) {
        return;
      }

      // Avoid unnecessary debug lines
      k4ced::GlobalLog::instance().level() = msgLevel();
      k4ced::GlobalLog::instance().name() = name();

      // Draw current event store
      if (!k4ced::k4GaudiCED::isEventStarted()) {
        k4ced::drawEventStore();
      }

      // Send data to CED server and listen for user feedback
      k4ced::k4GaudiCED::sendEvent();
      k4ced::k4GaudiCED::processInput("Breakpoint after " + caller +
                                      ". Double click for picking. Press <ENTER> to continue.");
    } catch (...) {
      error() << "Stop after " << caller << " failed" << endmsg;
    }
  }
};

DECLARE_COMPONENT(CEDAuditor)
