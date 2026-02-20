/// \file RunAction.cc
/// \brief Implementation of the ToyLArTPC::RunAction class.

#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"

namespace ToyLArTPC {

RunAction::RunAction()
{
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("root");
    analysisManager->SetVerboseLevel(1);

    // Create ntuple (id = 0)
    analysisManager->CreateNtuple("PhotonCounts", "Photon counts per sensor per event");
    for (G4int i = 0; i < kNTiles; ++i) {
        G4String colName = "sensor_" + std::to_string(i);
        analysisManager->CreateNtupleIColumn(colName);
    }
    analysisManager->FinishNtuple();
}

void RunAction::BeginOfRunAction(const G4Run* /*run*/)
{
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->OpenFile("ToyLArTPC");
}

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}

} // namespace ToyLArTPC

