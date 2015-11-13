#include "Run.hpp"

#include "G4VProcess.hh"
#include <iomanip>

Run::Run() : G4Run() {} 

Run::~Run() {} 

void Run::CountProcesses(const G4String part, const G4VProcess* proc)  {
	G4String procName = proc->GetProcessName();

	std::map<G4String,std::map<G4String,G4int> >::iterator part_it = fProcCounter.find(part);
	if (part_it == fProcCounter.end()) fProcCounter[part] = std::map<G4String,G4int>();

	std::map<G4String,G4int>::iterator it = fProcCounter[part].find(procName);
	if (it == fProcCounter[part].end()) fProcCounter[part][procName] = 1;
	else fProcCounter[part][procName]++;
}

void Run::EndOfRun() {
	G4cout << "\nProcess calls frequency :" << G4endl;

	std::map<G4String, std::map<G4String,G4int> >::iterator part_it;
	for (part_it = fProcCounter.begin(); part_it != fProcCounter.end(); part_it++) {
		G4String part = part_it->first;
		G4cout << " " << part << ":" << G4endl;
		std::map<G4String,G4int>::iterator it;         
		for (it = fProcCounter[part].begin(); it != fProcCounter[part].end(); it++) {
			G4cout << " " << std::setw(20) << it->first << " = "<< std::setw(7) << it->second << G4endl;
		}
		G4cout << G4endl;
	}

	fProcCounter.clear();
}

void Run::Merge(const G4Run* run) {
	/*
	const Run* localRun = static_cast<const Run*>(run);

	std::map<G4String, std::map<G4String,G4int> >::const_iterator part_it;
	for (part_it = localRun->fProcCounter.begin(); part_it != localRun->fProcCounter.end(); part_it++) {
		const G4String part = part_it->first;

		std::map<G4String,G4int>::const_iterator it;
		for (it = localRun->fProcCounter[part].begin(); it !=localRun->fProcCounter[part].end(); it++) {
			G4String procName = it->first;
			G4int localCount  = it->second;
			if (fProcCounter[part].find(procName) == fProcCounter[part].end()) {
				fProcCounter[part][procName] = localCount;
			} else {
				fProcCounter[part][procName] += localCount;
			}
		}
	}
	*/

	G4Run::Merge(run);
}
