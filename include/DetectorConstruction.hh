/// \file DetectorConstruction.hh
/// \brief Definition of the ToyLArTPC::DetectorConstruction class.

#ifndef TOYLARTPC_DETECTORCONSTRUCTION_HH
#define TOYLARTPC_DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"

class G4LogicalVolume;

namespace ToyLArTPC {

/// Constructs the simplified LArTPC detector geometry,
/// including photon detector tiles on the ±x walls.
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    /// @param fullYield If true, use the physical scintillation yield (24 000 /MeV).
    ///                  If false (default), use a reduced yield (240 /MeV) for fast runs.
    explicit DetectorConstruction(bool fullYield = false);
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

private:
    bool fFullYield = false;
    G4LogicalVolume* fPhotonDetLogical = nullptr;
};

} // namespace ToyLArTPC

#endif // TOYLARTPC_DETECTORCONSTRUCTION_HH

