# Creating a job

## Local steps

- Apply your simulation settings in simulation.conf.

- Rebuild the simulation:

`../simulation/build.sh`

- Generate input file:

`./job_photoconversion.sh theta0_200keV_100k`

- Split the input file:

`./job_split.sh theta0_200keV_100k`

## Drift job submission
max about 4-5min per event

`bsub -q atlasnodeshort -n 64 -W 120 -R "span[ptile=64]" -app Reserve500M -J MM_100k_simulation_drift ./job_drift.sh theta0_200keV_100k`

## Avalanche job submission
max about 2-3min per event

`bsub -q atlasnodeshort -n 64 -W 120 -R "span[ptile=64]" -app Reserve500M -J MM_100k_simulation_avalanche ./job_avalanche.sh theta0_200keV_100k`

## Monitoring the job

`watch "bqueues atlasnodeshort && bjobs"`

`tail -n 1 theta0_200keV_100k/photoconversion_*_drift.log | grep done`

`tail -n 2 theta0_200keV_100k/photoconversion_*_aval.log | grep all`

## Rejoin output files

- Gather and rejoin generated files:

`mkdir results/theta0_200keV_100k`

`cp theta0_200keV_100k/photoconversion.root results/theta0_200keV_100k`

`./job_join.sh results/theta0_200keV_100k/drift.root theta0_200keV_100k/photoconversion_*_drift.root`

`./job_join.sh results/theta0_200keV_100k/avalanche.root theta0_200keV_100k/photoconversion_*_aval.root`
