/// \file RunAction.hh
/// \brief Definition of the ToyLArTPC::RunAction class.

#ifndef TOYLARTPC_RUNACTION_HH
#define TOYLARTPC_RUNACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"

namespace ToyLArTPC {

/// Opens/closes the ROOT output file and creates the photon-count ntuple.
class RunAction : public G4UserRunAction
{
public:
    RunAction();
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run)   override;

    /// Total number of photon detector tiles (2 walls × 25 tiles).
    static constexpr G4int kNTiles = 50;
};

} // namespace ToyLArTPC

#endif // TOYLARTPC_RUNACTION_HH

