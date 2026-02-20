/// \file main.cc
/// \brief Main program for the ToyLArTPC Geant4 simulation.
///
/// Usage:
///   ./ToyLArTPC <events.root>                                    Interactive mode (Qt)
///   ./ToyLArTPC <events.root> -n <nEvents> [-t <nThreads>]       Batch mode

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"

#include <string>
#include <iostream>

namespace {

void PrintUsage()
{
    std::cerr << "Usage:\n"
              << "  ToyLArTPC <events.root>                                       Interactive (Qt)\n"
              << "  ToyLArTPC <events.root> -n <nEvents> [-t <nThreads>] [-full-yield]  Batch\n"
              << "\n"
              << "Options:\n"
              << "  -n <nEvents>   Number of events to simulate (omit for interactive mode)\n"
              << "  -t <nThreads>  Number of worker threads (0 = auto)\n"
              << "  -full-yield    Use physical scintillation yield (24000 ph/MeV)\n"
              << "                 Default is reduced yield (240 ph/MeV) for fast runs\n"
              << "\n"
              << "  Generate the events file first with:\n"
              << "    ./GenerateMarleyEvents marley_config.js <nEvents> events.root\n";
}

} // anonymous namespace

int main(int argc, char** argv)
{
    // --- Parse command-line arguments ---
    if (argc < 2) {
        PrintUsage();
        return 1;
    }

    std::string eventFile = argv[1];
    G4int nEvents  = 0;      // 0 means interactive mode
    G4int nThreads = 0;      // 0 means let Geant4 decide
    bool  fullYield = false;  // reduced yield by default

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-n" && i + 1 < argc) {
            nEvents = std::stoi(argv[++i]);
        } else if (arg == "-t" && i + 1 < argc) {
            nThreads = std::stoi(argv[++i]);
        } else if (arg == "-full-yield") {
            fullYield = true;
        } else {
            PrintUsage();
            return 1;
        }
    }

    // Construct the run manager
    auto runManager = G4RunManagerFactory::CreateRunManager();

    if (nThreads > 0) {
        runManager->SetNumberOfThreads(nThreads);
    }

    // --- Load pre-generated events on main thread (ROOT is not thread-safe) ---
    ToyLArTPC::PrimaryGeneratorAction::LoadEvents(eventFile);

    // --- Mandatory user initialization classes ---
    runManager->SetUserInitialization(new ToyLArTPC::DetectorConstruction(fullYield));

    auto physicsList = new FTFP_BERT();
    physicsList->RegisterPhysics(new G4OpticalPhysics());
    runManager->SetUserInitialization(physicsList);

    runManager->SetUserInitialization(new ToyLArTPC::ActionInitialization());

    // Initialize the Geant4 kernel
    runManager->Initialize();

    if (nEvents > 0) {
        // ---- Batch mode ----
        runManager->BeamOn(nEvents);
    } else {
        // ---- Interactive mode ----
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);

        auto visManager = new G4VisExecutive();
        visManager->Initialize();

        G4UImanager::GetUIpointer()->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();

        delete ui;
        delete visManager;
    }

    // Clean up
    delete runManager;

    return 0;
}

