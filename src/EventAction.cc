/// \file EventAction.cc
/// \brief Implementation of the ToyLArTPC::EventAction class.

#include "EventAction.hh"
#include "PhotonHit.hh"
#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"

#include <vector>

namespace ToyLArTPC {

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
    // Nothing needed at the start of the event
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    // Retrieve the hits collection ID on the first event
    if (fHCID < 0) {
        fHCID = G4SDManager::GetSDMpointer()
                    ->GetCollectionID("PhotonHitsCollection");
    }

    // Get the hits collection for this event
    auto hce = event->GetHCofThisEvent();
    if (!hce) return;

    auto hitsCollection =
        static_cast<PhotonHitsCollection*>(hce->GetHC(fHCID));
    if (!hitsCollection) return;

    // Count photons per tile
    const G4int nTiles = RunAction::kNTiles;
    std::vector<G4int> counts(nTiles, 0);

    G4int nHits = hitsCollection->entries();
    for (G4int i = 0; i < nHits; ++i) {
        G4int tileID = (*hitsCollection)[i]->GetTileID();
        if (tileID >= 0 && tileID < nTiles) {
            counts[tileID]++;
        }
    }

    // Fill the ntuple (ntuple id = 0)
    auto analysisManager = G4AnalysisManager::Instance();
    for (G4int col = 0; col < nTiles; ++col) {
        analysisManager->FillNtupleIColumn(col, counts[col]);
    }
    analysisManager->AddNtupleRow();
}

} // namespace ToyLArTPC

