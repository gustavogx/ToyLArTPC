/// \file PhotonSD.hh
/// \brief Definition of the ToyLArTPC::PhotonSD class.

#ifndef TOYLARTPC_PHOTONSD_HH
#define TOYLARTPC_PHOTONSD_HH

#include "G4VSensitiveDetector.hh"
#include "PhotonHit.hh"

namespace ToyLArTPC {

/// Sensitive detector attached to each photon detector tile.
/// Records only optical photons; charged particles are ignored.
/// A configurable detection efficiency is applied per photon.
class PhotonSD : public G4VSensitiveDetector
{
public:
    PhotonSD(const G4String& name, const G4String& hitsCollectionName);
    ~PhotonSD() override = default;

    void   Initialize(G4HCofThisEvent* hce) override;
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

    /// Set the photon detection efficiency (0.0 – 1.0).
    void     SetEfficiency(G4double eff) { fEfficiency = eff; }
    G4double GetEfficiency() const       { return fEfficiency; }

private:
    PhotonHitsCollection* fHitsCollection = nullptr;
    G4double              fEfficiency     = 1.0;   // default: 100 %
};

} // namespace ToyLArTPC

#endif // TOYLARTPC_PHOTONSD_HH

