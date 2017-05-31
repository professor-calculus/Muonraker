#ifndef HISTOMANAGER_HH
#define HISTOMANAGER_HH

#include "globals.hh"

class TProfile;
class TH1D;
class TFile;
class TObjArray;
class TTree;

class HistoMessenger;

const G4int MaxHisto = 21;
typedef struct {double px, py, pz, x, y, z, en, charge; int evt, status; char name[5];} MOMENTA;
typedef struct {double S1,S2,S3,S4,S5,S6; int evt;} SCINT;
typedef struct {double t,x,y,px,py,pz,en; int plane,event,track; char name[5];} TRACK;
typedef struct {double energy; int event;} EN;
typedef struct {double x,y,z,energy; int event;} SH;

class HistoManager
{
  public:

    HistoManager();
   ~HistoManager();

    void SetFileName   (const G4String name) { fileName = name;};
    G4String GetFileName() {return savef;};
    void book();
    void save();
    void SetHisto (G4int, G4int, G4double, G4double, const G4String& unit="none");
    void FillHisto(G4int id, G4double e, G4double weight = 1.0);
    void FillCloud(G4double, G4double);
    void FillMomentum(G4double, G4double, G4double, G4double, G4double, G4double, G4int, G4double, G4double, G4String, G4int);
    void FillExits(G4double, G4double, G4double, G4double, G4double, G4double, G4int, G4double, G4double, G4String, G4int);
	void FillScintillatorData(G4double E[6], G4int event);
	void FillTrackHit(G4double,G4double, G4double, G4double, G4double, G4double, G4double, G4int, G4int, G4int, G4String);
	void FillEnergy(G4double En, G4int evt);
	void FillShower(G4double En, G4double x, G4double y, G4double z, G4int evt);
    void SetMuonOnly(G4bool m) { MuonOnly = m;};
    G4bool GetMuonOnly() {return MuonOnly;};

    G4bool    HistoExist    (G4int id)  {return exist[id];}
    G4double  GetHistoUnit  (G4int id)  {return Unit[id];}
    G4double  GetBinWidth   (G4int id)  {return Width[id];}

  private:

	G4String MakeFileName();

	static MOMENTA momentum;
	static MOMENTA exits;
	static SCINT scintillators;
	static TRACK tracking;
	static EN targetenergy;
	static SH Shower;

    G4String                 fileName;
    G4String				 savef;
    TH1D*					 histo[MaxHisto];
    TProfile*				 BetheBloch;
   // TFile*					 OutPutFile;
    //TFile*					 Tupletest;
    TObjArray*				 Datalist;
    TTree*					 tree;
    TTree*					 scint;
    TTree*					 tracker;
    TTree*					 exit;
    //TTree*					 taren;
    TTree*					 shower;
    G4bool                   exist[MaxHisto];
    G4String                 Label[MaxHisto];
    G4String                 Title[MaxHisto];
    G4int                    Nbins[MaxHisto];
    G4double                 Vmin [MaxHisto];
    G4double                 Vmax [MaxHisto];
    G4double                 Unit [MaxHisto];
    G4double                 Width[MaxHisto];
    G4bool                   ascii[MaxHisto];

    G4bool                   factoryOn;
    G4bool					 MuonOnly;
    G4bool 					 AnyMuons;
    HistoMessenger*          histoMessenger;

  private:

};

#endif // HISTOMANAGER_HH
