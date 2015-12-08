# Submitting the job

Split the input file:

`./scripts/splitFile.py theta0_200keV_100k/photoconversion.root -j 64 -t coatingTree -b`

## Drift 
about 2min per event

`bsub -q atlasnodeshort -n 64 -W 30 -R "span[ptile=64]" -app Reserve500M -J MM_100k_simulation_drift ./job_drift.sh theta0_200keV_100k`

## Avalanche

`bsub -q atlasnodeshort -n 64 -W 100 -R "span[ptile=64]" -app Reserve500M -J MM_100k_simulation_avalanche ./job_avalanche.sh theta0_200keV_100k`

# Monitoring the job

`watch "bqueues atlasnodeshort && bjobs"`

`tail -n 1 theta0_200keV_100k/photoconversion_*_drift.txt`

`tail -n 1 theta0_200keV_100k/photoconversion_*_aval.txt`
