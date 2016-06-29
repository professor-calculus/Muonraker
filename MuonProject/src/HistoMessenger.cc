#include "HistoMessenger.hh"

#include <sstream>
#include "HistoManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"

HistoMessenger::HistoMessenger(HistoManager* manager)
:histoManager(manager)
{
  histoDir = new G4UIdirectory("/MuonProject/histo/");
  histoDir->SetGuidance("histograms control");

  histoCmd = new G4UIcommand("/MuonProject/histo/setHisto",this);
  histoCmd->SetGuidance("Set bining of the histo number ih :");
  histoCmd->SetGuidance("  nbBins; valMin; valMax; unit (of vmin and vmax)");
  
  G4UIparameter* ih = new G4UIparameter("ih",'i',false);
  ih->SetGuidance("histo number : from 1 to MaxHisto");
  ih->SetParameterRange("ih>0");
  histoCmd->SetParameter(ih);
  
  G4UIparameter* nbBins = new G4UIparameter("nbBins",'i',false);
  nbBins->SetGuidance("number of bins");
  nbBins->SetParameterRange("nbBins>0");
  histoCmd->SetParameter(nbBins);
  
  G4UIparameter* valMin = new G4UIparameter("valMin",'d',false);
  valMin->SetGuidance("valMin, expressed in unit");
  histoCmd->SetParameter(valMin);
  
  G4UIparameter* valMax = new G4UIparameter("valMax",'d',false);
  valMax->SetGuidance("valMax, expressed in unit");
  histoCmd->SetParameter(valMax);
  
  G4UIparameter* unit = new G4UIparameter("unit",'s',true);
  unit->SetGuidance("if omitted, vmin and vmax are assumed dimensionless");
  unit->SetDefaultValue("none");
  histoCmd->SetParameter(unit);
}

HistoMessenger::~HistoMessenger()
{
  delete histoCmd;
  delete histoDir;
}

void HistoMessenger::SetNewValue(G4UIcommand* command, G4String newValues)
{
  	if (command == histoCmd){ 
  		G4int ih,nbBins; G4double vmin,vmax; char unts[30];
     	const char* t = newValues;
     	std::istringstream is(t);
     	is >> ih >> nbBins >> vmin >> vmax >> unts;
     	G4String unit = unts;
     	G4double vUnit = 1. ;
    	if (unit != "none") vUnit = G4UIcommand::ValueOf(unit);
     	histoManager->SetHisto (ih,nbBins,vmin*vUnit,vmax*vUnit,unit);
   }
}
