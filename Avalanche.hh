#include <TVector3.h>

class Avalanche {
	public:
		TVector3 initialPosition;
		double initialEnergy;
		double initialTime;

		TVector3 finalPosition;
		double finalEnergy;
		double finalTime;

		int status;

		Avalanche(TVector3 x0, double e0, double t0);
};

Avalanche::Avalanche(TVector3 x0, double e0, double t0) : initialPosition(x0), initialEnergy(e0), initialTime(t0) {}
