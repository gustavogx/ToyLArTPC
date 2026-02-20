/// \file DetectorConstruction.cc
/// \brief Implementation of the ToyLArTPC::DetectorConstruction class.

#include "DetectorConstruction.hh"
#include "PhotonSD.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

namespace ToyLArTPC {

DetectorConstruction::DetectorConstruction(bool fullYield)
    : G4VUserDetectorConstruction(), fFullYield(fullYield)
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // --- Materials ---
    auto nist = G4NistManager::Instance();
    G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* lAr      = nist->FindOrBuildMaterial("G4_lAr");


    // --- Liquid Argon optical properties ---
    // LAr scintillation peaks at ~128 nm (9.69 eV).
    // We define properties over a photon energy range around that peak.
    auto larMPT = new G4MaterialPropertiesTable();

    // Photon energy sampling points (≈115 nm – 145 nm)
    std::vector<G4double> photonEnergy = { 8.55 * eV, 9.69 * eV, 10.78 * eV };

    // Refractive index of LAr (~1.38 in VUV)
    std::vector<G4double> rIndex = { 1.38, 1.38, 1.38 };
    larMPT->AddProperty("RINDEX", photonEnergy, rIndex);

    // Absorption length (~60 cm for VUV in pure LAr)
    std::vector<G4double> absLength = { 60. * cm, 60. * cm, 60. * cm };
    larMPT->AddProperty("ABSLENGTH", photonEnergy, absLength);

    // Rayleigh scattering length (~90 cm at 128 nm)
    std::vector<G4double> rayleigh = { 90. * cm, 90. * cm, 90. * cm };
    larMPT->AddProperty("RAYLEIGH", photonEnergy, rayleigh);

    // Scintillation emission spectrum (single Gaussian-like peak at 128 nm)
    std::vector<G4double> scintFast = { 0.0, 1.0, 0.0 };
    larMPT->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, scintFast);
    larMPT->AddProperty("SCINTILLATIONCOMPONENT2", photonEnergy, scintFast);

    // Scintillation yield: physical value is ~24 000 photons/MeV.
    // Use the full value for production runs, or 100× reduced for fast/visualization.
    G4double scintYield = fFullYield ? 24000. / MeV : 240. / MeV;
    larMPT->AddConstProperty("SCINTILLATIONYIELD", scintYield);

    // Resolution scale (statistical broadening; 1.0 = Poisson)
    larMPT->AddConstProperty("RESOLUTIONSCALE", 1.0);

    // Time constants: fast (singlet ~6 ns) and slow (triplet ~1.6 µs)
    larMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1",   6. * ns);
    larMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 1600. * ns);

    // Relative yield of fast vs slow component (for muons: ~25% fast)
    larMPT->AddConstProperty("SCINTILLATIONYIELD1", 0.25);
    larMPT->AddConstProperty("SCINTILLATIONYIELD2", 0.75);

    lAr->SetMaterialPropertiesTable(larMPT);

    // --- World volume ---
    G4double worldSize = 20.0 * m;
    auto solidWorld = new G4Box("World", worldSize / 2, worldSize / 2, worldSize / 2);
    auto logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");
    auto physWorld  = new G4PVPlacement(
        nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, true);

    // --- LArTPC active volume ---
    G4double tpcX = 2.0 * m;
    G4double tpcY = 10.0 * m;
    G4double tpcZ = 10.0 * m;
    auto solidTPC = new G4Box("TPC", tpcX / 2, tpcY / 2, tpcZ / 2);
    auto logicTPC = new G4LogicalVolume(solidTPC, lAr, "TPC");
    new G4PVPlacement(
        nullptr, G4ThreeVector(), logicTPC, "TPC", logicWorld, false, 0, true);

    // --- Photon detector tiles ---
    // Tile dimensions
    G4double pdThick  = 1.0 * mm;    // X – thickness
    G4double pdHeight = 10.0 * cm;   // Y – height
    G4double pdLength = 1.0 * m;     // Z – length

    auto solidPD = new G4Box("PhotonDet",
                             pdThick / 2, pdHeight / 2, pdLength / 2);
    // Use LAr as the sensor material — no optical boundary, so photons
    // enter freely and the sensitive detector handles detection.
    fPhotonDetLogical = new G4LogicalVolume(solidPD, lAr, "PhotonDet");

    // Visual attributes for the tiles
    auto pdVis = new G4VisAttributes(G4Colour(0.0, 0.8, 1.0, 0.6));
    pdVis->SetForceSolid(true);
    fPhotonDetLogical->SetVisAttributes(pdVis);

    // Grid layout: 5 rows (Y) × 5 columns (Z) = 25 tiles per wall
    const G4int nRows = 5;
    const G4int nCols = 5;

    // Equal spacing across each face
    // Y positions: divide tpcY into (nRows+1) gaps
    // Z positions: divide tpcZ into (nCols+1) gaps
    G4double ySpacing = tpcY / (nRows + 1);
    G4double zSpacing = tpcZ / (nCols + 1);

    // X position: flush against the inner TPC wall
    // Tiles are inside the TPC, centered at ±(tpcX/2 - pdThick/2)
    G4double xInner = tpcX / 2 - pdThick / 2;

    G4int copyNo = 0;
    for (G4int wall = 0; wall < 2; ++wall) {
        G4double xPos = (wall == 0) ? -xInner : +xInner;

        for (G4int row = 0; row < nRows; ++row) {
            G4double yPos = -tpcY / 2 + (row + 1) * ySpacing;

            for (G4int col = 0; col < nCols; ++col) {
                G4double zPos = -tpcZ / 2 + (col + 1) * zSpacing;

                new G4PVPlacement(
                    nullptr,
                    G4ThreeVector(xPos, yPos, zPos),
                    fPhotonDetLogical,
                    "PhotonDet",
                    logicTPC,
                    false,
                    copyNo++,
                    true);
            }
        }
    }

    return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
    auto photonSD = new PhotonSD("ToyLArTPC/PhotonSD", "PhotonHitsCollection");
    G4SDManager::GetSDMpointer()->AddNewDetector(photonSD);
    SetSensitiveDetector(fPhotonDetLogical, photonSD);
}

} // namespace ToyLArTPC

