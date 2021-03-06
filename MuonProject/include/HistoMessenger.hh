#ifndef HISTOMESSENGER_HH
#define HISTOMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

class HistoManager;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

class HistoMessenger: public G4UImessenger
{
  public:

   HistoMessenger(HistoManager* );
  ~HistoMessenger();

   void SetNewValue(G4UIcommand* ,G4String );

  private:

   HistoManager*           histoManager;

   G4UIdirectory*          histoDir;
   G4UIcommand*            histoCmd;

};

#endif // HISTOMESSENGER_HH
