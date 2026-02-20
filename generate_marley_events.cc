/// \file generate_marley_events.cc
/// \brief Standalone program to pre-generate MARLEY neutrino events
///        and save them to a ROOT file for later use in the Geant4 sim.
///
/// Usage:
///   ./GenerateMarleyEvents <config.js> <nEvents> [output.root]

#include "TFile.h"
#include "TTree.h"

#include "marley/Event.hh"
#include "marley/JSONConfig.hh"
#include "marley/Particle.hh"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Usage: GenerateMarleyEvents <config.js> <nEvents> "
                     "[output.root]\n";
        return 1;
    }

    // --- Set MARLEY env if not already set ---
    if (!std::getenv("MARLEY")) {
        setenv("MARLEY", "/home/gvaldivi/Scientific/marley", 0);
    }

    const std::string configFile = argv[1];
    const int nEvents = std::stoi(argv[2]);
    const std::string outFile = (argc > 3) ? argv[3] : "marley_events.root";

    // --- Initialise MARLEY generator ---
    marley::JSONConfig config(configFile);
    marley::Generator gen = config.create_generator();

    // --- Set up ROOT output ---
    TFile file(outFile.c_str(), "RECREATE");
    TTree tree("MarleyEvents", "Pre-generated MARLEY neutrino events");

    // Truth-level neutrino info
    double nu_energy = 0.;
    tree.Branch("nu_energy", &nu_energy, "nu_energy/D");

    // Final-state particles (variable-length arrays)
    int nParticles = 0;
    std::vector<int> pdg;
    std::vector<double> px, py, pz, energy, mass;

    tree.Branch("nParticles", &nParticles, "nParticles/I");
    tree.Branch("pdg",    &pdg);
    tree.Branch("px",     &px);
    tree.Branch("py",     &py);
    tree.Branch("pz",     &pz);
    tree.Branch("energy", &energy);
    tree.Branch("mass",   &mass);

    // --- Event loop ---
    std::cout << "Generating " << nEvents << " MARLEY events..." << std::endl;

    for (int i = 0; i < nEvents; ++i) {
        marley::Event ev = gen.create_event();

        // Neutrino truth: projectile is the first initial-state particle
        const auto& initParticles = ev.get_initial_particles();
        nu_energy = (!initParticles.empty())
                        ? initParticles.front()->total_energy()
                        : 0.;

        // Final-state particles
        const auto& finals = ev.get_final_particles();
        nParticles = static_cast<int>(finals.size());

        pdg.clear();    px.clear();   py.clear();
        pz.clear();     energy.clear(); mass.clear();

        for (const auto* fp : finals) {
            pdg.push_back(fp->pdg_code());
            px.push_back(fp->px());
            py.push_back(fp->py());
            pz.push_back(fp->pz());
            energy.push_back(fp->total_energy());
            mass.push_back(fp->mass());
        }

        tree.Fill();

        if ((i + 1) % 1000 == 0 || i + 1 == nEvents) {
            std::cout << "  " << (i + 1) << " / " << nEvents << "\r"
                      << std::flush;
        }
    }

    std::cout << std::endl;

    tree.Write();
    file.Close();

    std::cout << "Wrote " << nEvents << " events to " << outFile << std::endl;
    return 0;
}

