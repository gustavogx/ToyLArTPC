/// \file EventAction.hh
/// \brief Definition of the ToyLArTPC::EventAction class.

#ifndef TOYLARTPC_EVENTACTION_HH
#define TOYLARTPC_EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "globals.hh"

namespace ToyLArTPC {

/// At the end of each event, counts photon hits per tile and fills the ntuple.
class EventAction : public G4UserEventAction
{
public:
    EventAction()  = default;
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event)   override;

private:
    G4int fHCID = -1;   ///< Hits collection ID (cached)
};

} // namespace ToyLArTPC

#endif // TOYLARTPC_EVENTACTION_HH

