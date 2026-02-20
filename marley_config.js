// MARLEY configuration for supernova electron neutrinos on 40Ar
// Fermi-Dirac spectrum with typical supernova parameters
{
  seed: 123456,

  // Pure 40Ar target
  target: {
    nuclides: [ 1000180400 ],
    atom_fractions: [ 1.0 ],
  },

  // CC electron-neutrino scattering on 40Ar
  reactions: [ "ve40ArCC_Bhattacharya2009.react" ],

  // Supernova neutrino source: Fermi-Dirac spectrum
  source: {
    type: "fermi-dirac",
    neutrino: "ve",
    Emin: 0,
    Emax: 60,
    temperature: 3.5,
    eta: 0,
  },

  // Neutrinos travel along +z direction
  direction: { x: 0.0, y: 0.0, z: 1.0 },
}

