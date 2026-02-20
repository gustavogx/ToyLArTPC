/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the ToyLArTPC::PrimaryGeneratorAction class.

#ifndef TOYLARTPC_PRIMARYGENERATORACTION_HH
#define TOYLARTPC_PRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"

#include <atomic>
#include <string>
#include <vector>

class G4Event;

namespace ToyLArTPC {

/// Holds one pre-generated MARLEY event (all final-state particles).
struct MarleyEventData {
    double nuEnergy = 0.;
    int nParticles = 0;
    std::vector<int>    pdg;
    std::vector<double> px, py, pz, energy, mass;
};

/// Reads pre-generated MARLEY events from a ROOT file and injects
/// them into Geant4 as primary vertices.  All events must be loaded
/// on the main thread via LoadEvents() before any worker threads start.
/// An atomic counter cycles through events for each Geant4 event.
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override = default;

    void GeneratePrimaries(G4Event* event) override;

    /// Load all events from ROOT file — call on main thread only.
    static void LoadEvents(const std::string& eventFile);

private:
    /// Shared event cache (loaded once on main thread, then read-only).
    static std::vector<MarleyEventData> fgEvents;
    static std::atomic<int> fgNextEvent;
};

} // namespace ToyLArTPC

#endif // TOYLARTPC_PRIMARYGENERATORACTION_HH

