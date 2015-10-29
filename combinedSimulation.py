'''Main script to combine the simulations.'''

import subprocess

def main():
	photon_angle = 45. # degree
	photon_energy = 10. # eV

	drift_lut_file = 'outfiles/driftLUT.root'
	avalanche_lut_file = 'outfiles/avalancheLUT.root'

	# Run Geant4 photoconversion simulation to get primary electrons.
	subprocess.call(['photoconversion/photoconversion', photon_energy, photon_angle])

	# Use generated LUT to get secondary electrons for the simulated primary ones.
	subprocess.call(['drift/drift', drift_lut_file])

	# Do transparency cut with generated secondary electrons.
	subprocess.call(['transparencyCut/transparencyCut'])

	# Use generated LUT to calculate the avalanche for given secondary electrons.
	subprocess.call(['avalanche/avalanche', avalanche_lut_file])

if __name__  == '__main__':
	main()