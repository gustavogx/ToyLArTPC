/// \file PhotonSD.cc
/// \brief Implementation of the ToyLArTPC::PhotonSD class.

#include "PhotonSD.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace ToyLArTPC {

PhotonSD::PhotonSD(const G4String& name, const G4String& hitsCollectionName)
    : G4VSensitiveDetector(name)
{
    collectionName.insert(hitsCollectionName);
}

void PhotonSD::Initialize(G4HCofThisEvent* hce)
{
    // Create a new hits collection for this event
    fHitsCollection = new PhotonHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Register it with the event
    G4int hcID = G4SDManager::GetSDMpointer()
                     ->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, fHitsCollection);
}

G4bool PhotonSD::ProcessHits(G4Step* step, G4TouchableHistory* /*history*/)
{
    // ---- Accept only optical photons ----
    G4Track* track = step->GetTrack();
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
        return false;

    // ---- Apply detection efficiency ----
    if (fEfficiency < 1.0) {
        if (G4UniformRand() > fEfficiency)
            return false;
    }

    // ---- Record the hit ----
    auto hit = new PhotonHit();

    // Tile copy number (identifies which tile was hit)
    G4int tileID = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
    hit->SetTileID(tileID);

    hit->SetTime(step->GetPreStepPoint()->GetGlobalTime());
    hit->SetPosition(step->GetPreStepPoint()->GetPosition());

    // Wavelength from photon energy: λ = hc / E
    G4double energy = track->GetKineticEnergy();
    if (energy > 0.) {
        G4double wavelength = (1.239841939 * eV * um) / energy;  // hc in eV·µm
        hit->SetWavelength(wavelength);
    }

    fHitsCollection->insert(hit);

    // Kill the photon after detection
    track->SetTrackStatus(fStopAndKill);

    return true;
}

} // namespace ToyLArTPC

