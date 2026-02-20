/// \file PhotonHit.hh
/// \brief Definition of the ToyLArTPC::PhotonHit class.

#ifndef TOYLARTPC_PHOTONHIT_HH
#define TOYLARTPC_PHOTONHIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

namespace ToyLArTPC {

/// Hit recorded when an optical photon is detected by a photon detector tile.
class PhotonHit : public G4VHit
{
public:
    PhotonHit() = default;
    ~PhotonHit() override = default;

    // Memory allocation operators for hit pool
    inline void* operator new(size_t);
    inline void  operator delete(void* aHit);

    // Setters
    void SetTileID(G4int id)                    { fTileID = id; }
    void SetTime(G4double t)                    { fTime = t; }
    void SetPosition(const G4ThreeVector& pos)  { fPosition = pos; }
    void SetWavelength(G4double wl)             { fWavelength = wl; }

    // Getters
    G4int          GetTileID()     const { return fTileID; }
    G4double       GetTime()       const { return fTime; }
    G4ThreeVector  GetPosition()   const { return fPosition; }
    G4double       GetWavelength() const { return fWavelength; }

private:
    G4int         fTileID     = -1;
    G4double      fTime       = 0.;
    G4ThreeVector fPosition;
    G4double      fWavelength = 0.;
};

// Hits collection type
using PhotonHitsCollection = G4THitsCollection<PhotonHit>;

// Thread-local allocator
extern G4ThreadLocal G4Allocator<PhotonHit>* PhotonHitAllocator;

inline void* PhotonHit::operator new(size_t)
{
    if (!PhotonHitAllocator)
        PhotonHitAllocator = new G4Allocator<PhotonHit>;
    return (void*) PhotonHitAllocator->MallocSingle();
}

inline void PhotonHit::operator delete(void* aHit)
{
    PhotonHitAllocator->FreeSingle((PhotonHit*) aHit);
}

} // namespace ToyLArTPC

#endif // TOYLARTPC_PHOTONHIT_HH

