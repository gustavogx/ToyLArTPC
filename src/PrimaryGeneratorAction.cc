/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the ToyLArTPC::PrimaryGeneratorAction class.
///
/// Reads pre-generated MARLEY events from a ROOT file and injects
/// them into Geant4 as primary vertices.  Fully thread-safe.

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <stdexcept>

namespace ToyLArTPC {

// --- Static members ---
std::vector<MarleyEventData> PrimaryGeneratorAction::fgEvents;
std::atomic<int>             PrimaryGeneratorAction::fgNextEvent{0};

void PrimaryGeneratorAction::LoadEvents(const std::string& eventFile)
{
    TFile file(eventFile.c_str(), "READ");
    if (file.IsZombie() || !file.IsOpen()) {
        throw std::runtime_error(
            "PrimaryGeneratorAction: cannot open " + eventFile);
    }

    auto* tree = dynamic_cast<TTree*>(file.Get("MarleyEvents"));
    if (!tree) {
        throw std::runtime_error(
            "PrimaryGeneratorAction: no TTree 'MarleyEvents' in " + eventFile);
    }

    double nu_energy = 0.;
    int nParticles = 0;
    std::vector<int>*    pdg    = nullptr;
    std::vector<double>* px     = nullptr;
    std::vector<double>* py     = nullptr;
    std::vector<double>* pz     = nullptr;
    std::vector<double>* energy = nullptr;
    std::vector<double>* mass   = nullptr;

    tree->SetBranchAddress("nu_energy",  &nu_energy);
    tree->SetBranchAddress("nParticles", &nParticles);
    tree->SetBranchAddress("pdg",    &pdg);
    tree->SetBranchAddress("px",     &px);
    tree->SetBranchAddress("py",     &py);
    tree->SetBranchAddress("pz",     &pz);
    tree->SetBranchAddress("energy", &energy);
    tree->SetBranchAddress("mass",   &mass);

    const Long64_t nEntries = tree->GetEntries();
    fgEvents.resize(static_cast<size_t>(nEntries));

    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        auto& ev = fgEvents[static_cast<size_t>(i)];
        ev.nuEnergy   = nu_energy;
        ev.nParticles = nParticles;
        ev.pdg    = *pdg;
        ev.px     = *px;
        ev.py     = *py;
        ev.pz     = *pz;
        ev.energy = *energy;
        ev.mass   = *mass;
    }

    file.Close();

    std::cout << "PrimaryGeneratorAction: loaded "
              << fgEvents.size() << " events from " << eventFile
              << std::endl;
}

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction()
{
    if (fgEvents.empty()) {
        throw std::runtime_error(
            "PrimaryGeneratorAction: no events loaded. "
            "Call PrimaryGeneratorAction::LoadEvents() first.");
    }
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // Pick the next event (wraps around if more Geant4 events than entries).
    const int idx = fgNextEvent.fetch_add(1)
                    % static_cast<int>(fgEvents.size());
    const auto& ev = fgEvents[static_cast<size_t>(idx)];

    // Randomise the interaction vertex uniformly within the TPC (2×10×10 m).
    // MARLEY momenta are already in MeV, matching Geant4 internal units.
    G4double halfX  = 1.0 * m;
    G4double halfYZ = 5.0 * m;
    G4double vx = (2.0 * G4UniformRand() - 1.0) * halfX;
    G4double vy = (2.0 * G4UniformRand() - 1.0) * halfYZ;
    G4double vz = (2.0 * G4UniformRand() - 1.0) * halfYZ;
    auto* vertex = new G4PrimaryVertex(vx, vy, vz, 0.);

    for (int j = 0; j < ev.nParticles; ++j) {
        auto* particle = new G4PrimaryParticle(ev.pdg[j]);
        particle->SetMomentum(ev.px[j] * MeV, ev.py[j] * MeV, ev.pz[j] * MeV);
        vertex->SetPrimary(particle);
    }

    anEvent->AddPrimaryVertex(vertex);
}

} // namespace ToyLArTPC

