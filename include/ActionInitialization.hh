/// \file ActionInitialization.hh
/// \brief Definition of the ToyLArTPC::ActionInitialization class.

#ifndef TOYLARTPC_ACTIONINITIALIZATION_HH
#define TOYLARTPC_ACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

namespace ToyLArTPC {

/// Sets up the user action classes (PrimaryGenerator, etc.).
class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization() = default;
    ~ActionInitialization() override = default;

    void Build() const override;
};

} // namespace ToyLArTPC

#endif // TOYLARTPC_ACTIONINITIALIZATION_HH

