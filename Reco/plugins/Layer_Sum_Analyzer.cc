/* Need full layer, 7 cell cluster, and 19 cell cluster histograms for each layer
 * also need each for all layers summed
 * use ADC to MIP conversion of 1 MIP = 10 ADC Counts */

/**
	@Author: Ryan Quinn <ryan>
		7 July 2016
		quinn@physics.umn.edu
*/



// system include files
#include <memory>
#include <iostream>
#include "TH2Poly.h"
#include "TH1F.h"
#include "TF1.h"
#include "TProfile.h"
#include <sstream>
#include <fstream>
#include <math.h>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "HGCal/DataFormats/interface/HGCalTBRecHitCollections.h"
#include "HGCal/DataFormats/interface/HGCalTBDetId.h"
#include "HGCal/DataFormats/interface/HGCalTBRecHit.h"
#include "HGCal/Geometry/interface/HGCalTBCellVertices.h"
#include "HGCal/Geometry/interface/HGCalTBTopology.h"
#include "HGCal/Geometry/interface/HGCalTBGeometryParameters.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "HGCal/CondObjects/interface/HGCalElectronicsMap.h"
#include "HGCal/CondObjects/interface/HGCalCondObjectTextIO.h"
#include "HGCal/DataFormats/interface/HGCalTBElectronicsId.h"
#include "HGCal/DataFormats/interface/HGCalTBDataFrameContainers.h"
#include "HGCal/Geometry/interface/HGCalTBCellVertices.h"
#include "HGCal/Geometry/interface/HGCalTBCellParameters.h"


#include "HGCal/Reco/src/ShowerShape.cc"

// chooses which particle to look at. Inverts threshold filtering.
// if nothing is selected, electrons are the default
const bool ELECTRONS(1);// uses > *CELLS_THRESHOLD
const bool PIONS(0);// uses > PION_*CELLS_THRESHOLD and < *CELLS_THRESHOLD

//double Layer_Z[16]  = {1.2, 2., 3.5, 4.3, 5.8, 6.3, 8.7, 9.5, 11.4, 12.2, 13.8, 14.6, 16.6, 17.4, 20., 20.8};

//double ADCtoMIP_CERN[16] =  {17.32, 17.16, 16.45, 17.39, 17.75, 17.27, 16.55, 16.25, 17.52, 17.18, 17.10, 17.88, 15.87, 16.71, 16.92, 15.72};
//double ADCtoMIP_CERN[16] =  {17.287,16.9493,17.9461,16.5649,17.4171,17.5042,16.1005,16.4102, 1., 1., 1., 1., 1., 1., 1., 1.};
double ADCtoMIP_CERN[16] =  {17.24, 16.92, 17.51, 16.4, 17.35, 17.49, 16.29, 16.32, 1., 1., 1., 1., 1., 1., 1., 1.};
double ADCtoMIP_FNAL[16] =  {1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};

//double MIP2ParticleCalib = 1.3;  // FNAL to proton 120GeV
//500MeV muon/125GeV pion
double MIP2ParticleCalib[16] = {53.01/56.54, 53.47/56.42, 54.16/56.44, 54.17/56.19, 53.90/56.46, 53.97/56.31, 54.51/56.37, 55.43/56.09, 1., 1., 1., 1., 1., 1., 1., 1.}; // CERN to pion 125GeV
//double MIP2ParticleCalib[16] = {0.94, 0.94, 0.94, 0.94, 0.94, 0.94, 0.94, 0.94, 1., 1., 1., 1., 1., 1., 1., 1.}; // CERN to pion 125GeV

//double ADCtoMIP[16] = {16.02,16.85,15.36,14.73,10.66,15.64,16.52,14.24,10.07,14.42,16.14,17.33,16.61,16.84,15.79,16.43};// one MIP is equal to _ADCtoMIP_ ADC Counts
//double LayerWeight[16] = {0.6374029601923652, 0.7392021202456731, 0.6374273268336504, 0.7392021202456731, 0.6374273268336504, 0.8861075434658853, 0.8487578715427883, 1.0330129666860974, 0.8487578715427883, 1.0330129666860974, 0.8487578715427883, 1.5226977107534714, 1.2714189609610644, 1.5226977107534714, 1.2714189609610644, 1.5226977107534714};// X0 weights

//double LayerWeight[16] = {1.4091566745180932, 0.7020676448403224, 0.6054055986179145, 0.7020676448403224, 0.6054055986179145, 0.8415931435769973, 0.8061197656138868, 0.9811186423136724, 0.8061197656138868, 0.9811186423136724, 0.8061197656138868, 1.4462036381025891, 1.2075480996058319, 1.4462036381025891, 1.2075480996058319, 1.4462036381025891};

double LayerWeight_16L_FNAL[16] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}; 
double X0depth_16L_FNAL[16] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}; 

double LayerWeight_8L_conf1[16] = {33.074, 13.184, 14.17, 9.788, 9.766, 9.766, 16.339, 14.129, 0., 0., 0., 0., 0., 0., 0., 0.};
double X0depth_8L_conf1[16] = {6.268, 1.131, 1.131, 1.362, 0.574, 1.301, 0.574, 2.42, 0., 0., 0., 0., 0., 0., 0., 0.};
double LayerWeight_8L_conf2[16] = {35.866, 30.864, 28.803, 23.095, 20.657, 19.804, 36.322, 27.451, 0., 0., 0., 0., 0., 0., 0., 0.};
double X0depth_8L_conf2[16] = {5.048, 3.412, 3.412, 2.866, 2.512, 1.625, 2.368, 6.021, 0., 0., 0., 0., 0., 0., 0., 0.};
double weights2GeV = 1.e-03;
double MIP2GeV_sim = 52.81e-06;
//double weights2MIP = 52.8/63.6;   // rescale weights from mean to MPV
double weights2MIP = 1.;   // rescale weights from mean to MPV

//double LayerWeight[16] = {0.4847555727337982, 1.0214605968539232, 0.4847555727337982, 1.0214605968539232, 0.4847555727337982, 1.1420105918768606, 0.6423912113800805, 1.2625605868997982, 0.6423912113800805, 1.2625605868997982, 0.6423912113800805, 1.6643939036429232, 0.9576624886726451, 1.6643939036429232, 0.9576624886726451, 1.6643939036429232};// dE/dx weights

//double LayerSumWeight = 1.;

//const int CMTHRESHOLD = 30;// anything less than this value is added to the commonmode sum
const int CMTHRESHOLD = 2;// anything less than this value is added to the commonmode sum

// applied to all layers sum after commonmode subtraction and the ADC to MIP conversion
const double ALLCELLS_THRESHOLD = 50.;
const double SEVENCELLS_THRESHOLD = 50.;
const double NINETEENCELLS_THRESHOLD = 50.;
const double PION_ALLCELLS_THRESHOLD = 15.;
const double PION_7CELLS_THRESHOLD = -100.;
const double PION_19CELLS_THRESHOLD = -100.;


using namespace std;



class Layer_Sum_Analyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>
{

public:
	explicit Layer_Sum_Analyzer(const edm::ParameterSet&);
	~Layer_Sum_Analyzer();
	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
	virtual void beginJob() override;
	void analyze(const edm::Event& , const edm::EventSetup&) override;
	virtual void endJob() override;

	// ----------member data ---------------------------
	edm::EDGetToken HGCalTBRecHitCollection_;
        int layers_config_;

	struct {
		HGCalElectronicsMap emap_;
	} essource_;
        string mapfile_;
	int sensorsize = 128;
	std::vector<std::pair<double, double>> CellXY;
	std::pair<double, double> CellCentreXY;
        HGCalTBTopology IsCellValid;
	HGCalTBCellVertices TheCell;
        double maxdist = (0.5 + sqrt(3) / 2.) * HGCAL_TB_CELL::FULL_CELL_SIDE;  // <<< FIXME maxdist > HGCAL_TB_CELL::FULL_CELL_SIDE !!
        double maxdist_secNB = (0.5 + sqrt(3)) * HGCAL_TB_CELL::FULL_CELL_SIDE;
        double maxdist_thirdNB = (0.5 + sqrt(3) * 2.) * HGCAL_TB_CELL::FULL_CELL_SIDE;

        double Weights_L[MAXLAYERS];
        double X0_L[MAXLAYERS];
        double ADCtoMIP[MAXLAYERS];
        double ADCtoMIPup[MAXLAYERS];
        double ADCtoMIPdw[MAXLAYERS];


        TH1F *h_CM_layer[MAXLAYERS];
	TH1F *h_eAll_L[MAXLAYERS], *h_e7_L[MAXLAYERS], *h_e19_L[MAXLAYERS], *h_eMax_L[MAXLAYERS];
        //systematic => MIP +/- 5%
	TH1F *h_eAll_L_up[MAXLAYERS], *h_e7_L_up[MAXLAYERS], *h_e19_L_up[MAXLAYERS], *h_eMax_L_up[MAXLAYERS];
	TH1F *h_eAll_L_dw[MAXLAYERS], *h_e7_L_dw[MAXLAYERS], *h_e19_L_dw[MAXLAYERS], *h_eMax_L_dw[MAXLAYERS];
        //systematic => MIP +/- 5%
	TH1F *h_eAll_L_AbsW_Mip[MAXLAYERS], *h_e7_L_AbsW_Mip[MAXLAYERS], *h_e19_L_AbsW_Mip[MAXLAYERS], *h_eMax_L_AbsW_Mip[MAXLAYERS];
	TH1F *h_eAll_L_AbsW_GeV[MAXLAYERS], *h_e7_L_AbsW_GeV[MAXLAYERS], *h_e19_L_AbsW_GeV[MAXLAYERS], *h_eMax_L_AbsW_GeV[MAXLAYERS];
      	TH1F *h_x_L[MAXLAYERS], *h_y_L[MAXLAYERS];
	TH2F *h_x_y_L[MAXLAYERS];

        TH1F* h_Radius[MAXLAYERS];      
        TH1F *h_E1oE7_L[MAXLAYERS], *h_E1oE19_L[MAXLAYERS], *h_E7oE19_L[MAXLAYERS];
        TH1F *h_E1oE7_L_up[MAXLAYERS], *h_E1oE19_L_up[MAXLAYERS], *h_E7oE19_L_up[MAXLAYERS];
        TH1F *h_E1oE7_L_dw[MAXLAYERS], *h_E1oE19_L_dw[MAXLAYERS], *h_E7oE19_L_dw[MAXLAYERS];

        TH1F *h_eAll_all, *h_e7_all, *h_e19_all;
        TH1F *h_eAll_all_up, *h_e7_all_up, *h_e19_all_up;
        TH1F *h_eAll_all_dw, *h_e7_all_dw, *h_e19_all_dw;

        TH1F *h_eAll_all_AbsW_Mip, *h_e7_all_AbsW_Mip, *h_e19_all_AbsW_Mip;
        TH1F *h_eAll_all_AbsW_GeV, *h_e7_all_AbsW_GeV, *h_e19_all_AbsW_GeV;

        TProfile *tp_E1_vs_L, *tp_E7_vs_L, *tp_E19_vs_L;
        TProfile *tp_E1oSumL_vs_L, *tp_E7oSumL_vs_L, *tp_E19oSumL_vs_L;
        TProfile *tp_E1oE7_vs_L, *tp_E1oE19_vs_L, *tp_E7oE19_vs_L;

	TH2F *HighGain_LowGain_2D;
	TH2F *Energy_LowGain_2D;

        int EVENT = 0;

	map<int, double> Time_Stamp;
	map<int, double> Delta_Time_Stamp;
	double Time_Temp = 0.;
};



Layer_Sum_Analyzer::Layer_Sum_Analyzer(const edm::ParameterSet& iConfig)
{
  std::cout << " welcome costruttore MAXSKIROCS = " << MAXSKIROCS << std::endl;
  // initialization
  usesResource("TFileService");
  edm::Service<TFileService> fs;
  HGCalTBRecHitCollection_ = consumes<HGCalTBRecHitCollection>(iConfig.getParameter<edm::InputTag>("HGCALTBRECHITS"));
  layers_config_ = iConfig.getParameter<int>("layers_config");
  
  //booking the histos
  for(int layer = 0; layer < MAXLAYERS; layer++) {
    h_CM_layer[layer] = fs->make<TH1F>(Form("h_CM_layer%d",layer+1), "", 5000, -500, 500);
    
    h_eAll_L[layer] = fs->make<TH1F>(Form("h_eAll_L%d", layer+1), "", 40010, -10, 40000);
    h_eMax_L[layer] = fs->make<TH1F>(Form("h_eMax_L%d",layer+1), "", 40010, -10, 40000);
    h_e7_L[layer] = fs->make<TH1F>(Form("h_e7_L%d", layer+1),"",  40010, -10, 40000);
    h_e19_L[layer] = fs->make<TH1F>(Form("h_e19_L%d", layer+1), "", 40010, -10, 40000);
    //syst 
    h_eAll_L_up[layer] = fs->make<TH1F>(Form("h_eAll_L%d_up", layer+1), "", 40010, -10, 40000);
    h_eMax_L_up[layer] = fs->make<TH1F>(Form("h_eMax_L%d_up",layer+1), "", 40010, -10, 40000);
    h_e7_L_up[layer] = fs->make<TH1F>(Form("h_e7_L%d_up", layer+1),"",  40010, -10, 40000);
    h_e19_L_up[layer] = fs->make<TH1F>(Form("h_e19_L%d_up", layer+1), "", 40010, -10, 40000);
    h_eAll_L_dw[layer] = fs->make<TH1F>(Form("h_eAll_L%d_dw", layer+1), "", 40010, -10, 40000);
    h_eMax_L_dw[layer] = fs->make<TH1F>(Form("h_eMax_L%d_dw",layer+1), "", 40010, -10, 40000);
    h_e7_L_dw[layer] = fs->make<TH1F>(Form("h_e7_L%d_dw", layer+1),"",  40010, -10, 40000);
    h_e19_L_dw[layer] = fs->make<TH1F>(Form("h_e19_L%d_dw", layer+1), "", 40010, -10, 40000);
    //
    h_eAll_L_AbsW_Mip[layer] = fs->make<TH1F>(Form("h_eAll_L%d_AbsW_Mip", layer+1), "", 40010, -10, 4.e6);
    h_eMax_L_AbsW_Mip[layer] = fs->make<TH1F>(Form("h_eMax_L%d_AbsW_Mip",layer+1), "", 40010, -10, 4.e6);
    h_e7_L_AbsW_Mip[layer] = fs->make<TH1F>(Form("h_e7_L%d_AbsW_Mip", layer+1),"",  40010, -10, 4.e6);
    h_e19_L_AbsW_Mip[layer] = fs->make<TH1F>(Form("h_e19_L%d_AbsW_Mip", layer+1), "", 40010, -10, 4.e6);
    
    h_eAll_L_AbsW_GeV[layer] = fs->make<TH1F>(Form("h_eAll_L%d_AbsW_GeV", layer+1), "", 5100, -10, 500);
    h_eMax_L_AbsW_GeV[layer] = fs->make<TH1F>(Form("h_eMax_L%d_AbsW_GeV",layer+1), "", 5100, -10, 500);
    h_e7_L_AbsW_GeV[layer] = fs->make<TH1F>(Form("h_e7_L%d_AbsW_GeV", layer+1),"",  5100, -10, 500);
    h_e19_L_AbsW_GeV[layer] = fs->make<TH1F>(Form("h_e19_L%d_AbsW_GeV", layer+1), "", 5100, -10, 500);
    
    h_x_L[layer] = fs->make<TH1F>(Form("X_L%d", layer+1), "", 2000, -10., 10. );
    h_y_L[layer] = fs->make<TH1F>(Form("Y_L%d", layer+1), "", 2000, -10., 10. );
    h_x_y_L[layer] = fs->make<TH2F>(Form("YvsX_L%d", layer+1), "", 2000, -10., 10., 2000, -10., 10. );

    h_E1oE7_L[layer] = fs->make<TH1F>(Form("h_E1oE7_L%d",layer+1), "", 5000, -5, 5);
    h_E1oE19_L[layer] = fs->make<TH1F>(Form("h_E1oE19_L%d",layer+1), "", 5000, -5, 5);
    h_E7oE19_L[layer] = fs->make<TH1F>(Form("h_E7oE19_L%d",layer+1), "", 5000, -5, 5);
    h_Radius[layer] = fs->make<TH1F>(Form("h_Radius_L%d",layer+1), "", 5000, 0., 20.);
    ///syst
    h_E1oE7_L_up[layer] = fs->make<TH1F>(Form("h_E1oE7_L%d",layer+1), "", 5000, -5, 5);
    h_E1oE19_L_up[layer] = fs->make<TH1F>(Form("h_E1oE19_L%d",layer+1), "", 5000, -5, 5);
    h_E7oE19_L_up[layer] = fs->make<TH1F>(Form("h_E7oE19_L%d",layer+1), "", 5000, -5, 5);
    h_E1oE7_L_dw[layer] = fs->make<TH1F>(Form("h_E1oE7_L%d",layer+1), "", 5000, -5, 5);
    h_E1oE19_L_dw[layer] = fs->make<TH1F>(Form("h_E1oE19_L%d",layer+1), "", 5000, -5, 5);
    h_E7oE19_L_dw[layer] = fs->make<TH1F>(Form("h_E7oE19_L%d",layer+1), "", 5000, -5, 5);
    //
  }
  

  h_eAll_all = fs->make<TH1F>("h_eAll_all", "", 40010, -10, 40000);
  h_e7_all = fs->make<TH1F>("h_e7_all", "", 40010, -10, 40000);
  h_e19_all = fs->make<TH1F>("h_e19_all", "", 40010, -10, 40000);
  h_eAll_all->Sumw2();
  h_e7_all->Sumw2();
  h_e19_all->Sumw2();
  //////////
  h_eAll_all_up = fs->make<TH1F>("h_eAll_all_up", "", 40010, -10, 40000);
  h_e7_all_up = fs->make<TH1F>("h_e7_all_up", "", 40010, -10, 40000);
  h_e19_all_up = fs->make<TH1F>("h_e19_all_up", "", 40010, -10, 40000);
  h_eAll_all_up->Sumw2();
  h_e7_all_up->Sumw2();
  h_e19_all_up->Sumw2();
  h_eAll_all_dw = fs->make<TH1F>("h_eAll_all_dw", "", 40010, -10, 40000);
  h_e7_all_dw = fs->make<TH1F>("h_e7_all_dw", "", 40010, -10, 40000);
  h_e19_all_dw = fs->make<TH1F>("h_e19_all_dw", "", 40010, -10, 40000);
  h_eAll_all_dw->Sumw2();
  h_e7_all_dw->Sumw2();
  h_e19_all_dw->Sumw2();

 
  h_eAll_all_AbsW_Mip = fs->make<TH1F>("h_eAll_all_AbsW_Mip", "", 40010, -10, 4.e6);
  h_e7_all_AbsW_Mip = fs->make<TH1F>("h_e7_all_AbsW_Mip", "", 40010, -10, 4.e6);
  h_e19_all_AbsW_Mip = fs->make<TH1F>("h_e19_all_AbsW_Mip", "", 40010, -10, 4.e6);
  h_eAll_all_AbsW_Mip->Sumw2();
  h_e7_all_AbsW_Mip->Sumw2();
  h_e19_all_AbsW_Mip->Sumw2();
  
  h_eAll_all_AbsW_GeV = fs->make<TH1F>("h_eAll_all_AbsW_GeV", "", 5100, -10, 500);
  h_e7_all_AbsW_GeV = fs->make<TH1F>("h_e7_all_AbsW_GeV", "", 5100, -10, 500);
  h_e19_all_AbsW_GeV = fs->make<TH1F>("h_e19_all_AbsW_GeV", "", 5100, -10, 500);
  h_eAll_all_AbsW_GeV->Sumw2();
  h_e7_all_AbsW_GeV->Sumw2();
  h_e19_all_AbsW_GeV->Sumw2();
  
  HighGain_LowGain_2D = fs->make<TH2F>("h2_HGvsLG", "", 4000, 0, 4000, 4000, 0, 4000);
  Energy_LowGain_2D = fs->make<TH2F>("h2_EvsLG", "", 4000, 0, 4000, 4000, 0, 4000);
  
  tp_E1_vs_L = fs->make<TProfile>("tp_E1_vs_L", "", MAXLAYERS+1, 0, MAXLAYERS+1);
  tp_E7_vs_L = fs->make<TProfile>("tp_E7_vs_L", "", MAXLAYERS+1, 0, MAXLAYERS+1);
  tp_E19_vs_L = fs->make<TProfile>("tp_E19_vs_L", "", MAXLAYERS+1, 0, MAXLAYERS+1);
  tp_E1oSumL_vs_L = fs->make<TProfile>("tp_E1oSumL_vs_L", "", MAXLAYERS+1, 0, MAXLAYERS+1);
  tp_E7oSumL_vs_L = fs->make<TProfile>("tp_E7oSumL_vs_L", "", MAXLAYERS+1, 0, MAXLAYERS+1);
  tp_E19oSumL_vs_L = fs->make<TProfile>("tp_E19oSumL_vs_L", "", MAXLAYERS+1, 0, MAXLAYERS+1);
  tp_E1oE7_vs_L = fs->make<TProfile>("tp_E1oE7_vs_L", "", MAXLAYERS+1, 0, MAXLAYERS+1);
  tp_E1oE19_vs_L = fs->make<TProfile>("tp_E1oE19_vs_L", "", MAXLAYERS+1, 0, MAXLAYERS+1);
  tp_E7oE19_vs_L = fs->make<TProfile>("tp_E7oE19_vs_L", "", MAXLAYERS+1, 0, MAXLAYERS+1);
  
  
  //loading the proper weights
  if(MAXLAYERS != 8) {
    std::cout << " update weights " << std::endl;
    return;
  }

  for(int iL=0; iL<MAXLAYERS; ++iL){
    if(layers_config_ == 0){
      Weights_L[iL] = LayerWeight_16L_FNAL[iL];
      X0_L[iL] = X0depth_16L_FNAL[iL];
      ADCtoMIP[iL] = ADCtoMIP_FNAL[iL];
      ADCtoMIPup[iL] = ADCtoMIP_FNAL[iL] * 1.05;
      ADCtoMIPdw[iL] = ADCtoMIP_FNAL[iL] * 0.95;
      mapfile_ = iConfig.getParameter<std::string>("mapFile_FNAL");
    }
    else{
      ADCtoMIP[iL] = ADCtoMIP_CERN[iL];
      ADCtoMIPup[iL] = ADCtoMIP_CERN[iL] * 1.05;
      ADCtoMIPdw[iL] = ADCtoMIP_CERN[iL] * 0.95;
      mapfile_ = iConfig.getParameter<std::string>("mapFile_CERN");
      if(layers_config_ == 1){
	Weights_L[iL] = LayerWeight_8L_conf1[iL];
	X0_L[iL] = X0depth_8L_conf1[iL];
      }
      if(layers_config_ == 2){
	Weights_L[iL] = LayerWeight_8L_conf2[iL];
	X0_L[iL] = X0depth_8L_conf2[iL]; 
      }
      if(layers_config_ == -1){
	Weights_L[iL] = 1.;
	X0_L[iL] = 0.;
      }
    }
  }
  
  
  
  
}//constructor ends here


Layer_Sum_Analyzer::~Layer_Sum_Analyzer()
{

	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)

}


// ------------ method called for each event  ------------
void
Layer_Sum_Analyzer::analyze(const edm::Event& event, const edm::EventSetup& setup)
{

  //  std::cout << " >>> Layer_Sum_Analyzer::analyze " << std::endl;
  EVENT = (event.id()).event();

  if((event.id()).event() == 1) {
    Time_Temp = event.time().value();
    Time_Stamp[EVENT] = Time_Temp;
    Delta_Time_Stamp[EVENT] = 0.;
  } else {
    Time_Stamp[EVENT] = event.time().value();
    Delta_Time_Stamp[EVENT] = event.time().value() - Time_Temp;
    Time_Temp = event.time().value();
  }
  
  //opening Rechits
  edm::Handle<HGCalTBRecHitCollection> Rechits;
  event.getByToken(HGCalTBRecHitCollection_, Rechits);
  
  // looping over each rechit to fill histogram
  double commonmode[MAXLAYERS];
  double commonmode_up[MAXLAYERS];
  double commonmode_dw[MAXLAYERS];
  int cm_num[MAXLAYERS];   int cm_num_up[MAXLAYERS];   int cm_num_dw[MAXLAYERS];
  double max[MAXLAYERS], max_x[MAXLAYERS], max_y[MAXLAYERS];
  for(int iL=0; iL<MAXLAYERS; ++iL){
    max[iL] = max_x[iL] = max_y[iL] = 0.;
    commonmode[iL] = 0.;
    commonmode_up[iL] = 0.;
    commonmode_dw[iL] = 0.;
    cm_num[iL] = 0;
    cm_num_up[iL] = 0;
    cm_num_dw[iL] = 0;
  }

  
  //CM subtraction => REPLACE WITH EXTERNAL CLASS => FIX
  for(auto Rechit : *Rechits){
    
    if(!IsCellValid.iu_iv_valid((Rechit.id()).layer(), (Rechit.id()).sensorIU(), (Rechit.id()).sensorIV(), (Rechit.id()).iu(), (Rechit.id()).iv(), sensorsize))  continue;
   
    //getting electronics ID
    uint32_t EID = essource_.emap_.detId2eid(Rechit.id());
    HGCalTBElectronicsId eid(EID);
    
    int n_layer = (Rechit.id()).layer() - 1;
    int n_cell_type = (Rechit.id()).cellType();
    //    int n_skiroc = eid.iskiroc() - 1;


    //getting X and Y coordinates
    CellCentreXY = TheCell.GetCellCentreCoordinatesForPlots((Rechit.id()).layer(), (Rechit.id()).sensorIU(), (Rechit.id()).sensorIV(), (Rechit.id()).iu(), (Rechit.id()).iv(), sensorsize);

    HighGain_LowGain_2D->Fill(Rechit.energyLow(), Rechit.energyHigh());
    Energy_LowGain_2D->Fill(Rechit.energy(), Rechit.energyHigh());
    
    // needed? FIXME
    if(n_cell_type != 0 && n_cell_type != 4) continue;
    
    if(Rechit.energy() > max[n_layer]) {      
      max[n_layer] = Rechit.energy();
      max_x[n_layer] = CellCentreXY.first;
      max_y[n_layer] = CellCentreXY.second;
    }

    if((Rechit.energy()) / ADCtoMIP[n_layer] <= CMTHRESHOLD) {
      commonmode[n_layer] += Rechit.energy();
      cm_num[n_layer]++;
    }

    if((Rechit.energy()) / ADCtoMIPup[n_layer] <= CMTHRESHOLD) {
      commonmode_up[n_layer] += Rechit.energy();
      cm_num_up[n_layer]++;
    }

    if((Rechit.energy()) / ADCtoMIPdw[n_layer] <= CMTHRESHOLD) {
      commonmode_dw[n_layer] += Rechit.energy();
      cm_num_dw[n_layer]++;
    }

  }//Rechit loop ends here
  //	std::cout << " >>> found commonmode = " << commonmode << std::endl;


  for(int iL=0; iL<MAXLAYERS; ++iL){
    commonmode[iL] = commonmode[iL]/cm_num[iL];
    commonmode_up[iL] = commonmode_up[iL]/cm_num_up[iL];
    commonmode_dw[iL] = commonmode_dw[iL]/cm_num_dw[iL];
    
    h_CM_layer[iL]->Fill(commonmode[iL]);
  }

  ShowerShape shosha(Rechits, ADCtoMIP, commonmode, CMTHRESHOLD, max, max_x, max_y);
  ShowerShape shosha_up(Rechits, ADCtoMIPup, commonmode_up, CMTHRESHOLD, max, max_x, max_y);
  ShowerShape shosha_dw(Rechits, ADCtoMIPdw, commonmode_dw, CMTHRESHOLD, max, max_x, max_y);
 
  float E1SumL_R = 0.;
  float E7SumL_R = 0.;
  float E19SumL_R = 0.;
  float EAllSumL_R = 0.;
  float E1SumL_Rup = 0.;
  float E7SumL_Rup = 0.;
  float E19SumL_Rup = 0.;
  float EAllSumL_Rup = 0.;
  float E1SumL_Rdw = 0.;
  float E7SumL_Rdw = 0.;
  float E19SumL_Rdw = 0.;
  float EAllSumL_Rdw = 0.;

  float E1SumL_AbsW_Mip = 0.;
  float E7SumL_AbsW_Mip = 0.;
  float E19SumL_AbsW_Mip = 0.;
  float EAllSumL_AbsW_Mip = 0.;

  float E1SumL_AbsW_GeV = 0.;
  float E7SumL_AbsW_GeV = 0.;
  float E19SumL_AbsW_GeV = 0.;
  float EAllSumL_AbsW_GeV = 0.;

  for(int iL=0; iL<MAXLAYERS; ++iL){
    float e1, e7, e19, eAll;
    shosha.getAllEnergy(iL, e1, e7, e19, eAll);

    h_eAll_L[iL]->Fill(eAll);
    h_eMax_L[iL]->Fill(e1);
    h_e7_L[iL]->Fill(e7);
    h_e19_L[iL]->Fill(e19);

    float e1_up, e7_up, e19_up, eAll_up;
    shosha.getAllEnergy(iL, e1_up, e7_up, e19_up, eAll_up);

    h_eAll_L_up[iL]->Fill(eAll_up);
    h_eMax_L_up[iL]->Fill(e1_up);
    h_e7_L_up[iL]->Fill(e7_up);
    h_e19_L_up[iL]->Fill(e19_up);

    float e1_dw, e7_dw, e19_dw, eAll_dw;
    shosha.getAllEnergy(iL, e1_dw, e7_dw, e19_dw, eAll_dw);

    h_eAll_L_dw[iL]->Fill(eAll_dw);
    h_eMax_L_dw[iL]->Fill(e1_dw);
    h_e7_L_dw[iL]->Fill(e7_dw);
    h_e19_L_dw[iL]->Fill(e19_dw);


    float layerE1_Mip = e1 * (weights2GeV * weights2MIP * Weights_L[iL] / MIP2GeV_sim + 1.);
    float layerE7_Mip = e7 * (weights2GeV * weights2MIP * Weights_L[iL] / MIP2GeV_sim + 1.);
    float layerE19_Mip = e19 * (weights2GeV * weights2MIP * Weights_L[iL] / MIP2GeV_sim + 1.);
    float layerEAll_Mip = eAll * (weights2GeV * weights2MIP * Weights_L[iL] / MIP2GeV_sim + 1.);

    float layerE1_GeV = e1 * (weights2GeV * weights2MIP * Weights_L[iL] + 1. * MIP2GeV_sim);
    float layerE7_GeV = e7 * (weights2GeV * weights2MIP * Weights_L[iL] + 1. * MIP2GeV_sim);
    float layerE19_GeV = e19 * (weights2GeV * weights2MIP * Weights_L[iL] + 1. * MIP2GeV_sim);
    float layerEAll_GeV = eAll * (weights2GeV * weights2MIP * Weights_L[iL] + 1. * MIP2GeV_sim);

    h_eAll_L_AbsW_Mip[iL]->Fill(layerEAll_Mip);
    h_eMax_L_AbsW_Mip[iL]->Fill(layerE1_Mip);
    h_e7_L_AbsW_Mip[iL]->Fill(layerE7_Mip);
    h_e19_L_AbsW_Mip[iL]->Fill(layerE19_Mip);

    h_eAll_L_AbsW_GeV[iL]->Fill(layerEAll_GeV);
    h_eMax_L_AbsW_GeV[iL]->Fill(layerE1_GeV);
    h_e7_L_AbsW_GeV[iL]->Fill(layerE7_GeV);
    h_e19_L_AbsW_GeV[iL]->Fill(layerE19_GeV);
    
    tp_E1_vs_L->Fill(iL+1, e1);
    tp_E7_vs_L->Fill(iL+1, e7);
    tp_E19_vs_L->Fill(iL+1, e19);

    if(e7 != 0.){
      h_E1oE7_L[iL]->Fill(e1/e7);
      tp_E1oE7_vs_L->Fill(iL+1, e1/e7);
    }
    if(e7_up != 0.) h_E1oE7_L_up[iL]->Fill(e1_up/e7_up);
    if(e7_dw != 0.) h_E1oE7_L_dw[iL]->Fill(e1_dw/e7_dw);


    if(e19 != 0){
      h_E1oE19_L[iL]->Fill(e1/e19);
      h_E7oE19_L[iL]->Fill(e7/e19);
      tp_E1oE19_vs_L->Fill(iL, e1/e19);
      tp_E7oE19_vs_L->Fill(iL, e7/e19);
    }
    if(e19_up != 0){
      h_E1oE19_L_up[iL]->Fill(e1_up/e19_up);
      h_E7oE19_L_up[iL]->Fill(e7_up/e19_up);
    }
    if(e19_dw != 0){
      h_E1oE19_L_dw[iL]->Fill(e1_dw/e19_dw);
      h_E7oE19_L_dw[iL]->Fill(e7_dw/e19_dw);
    }

    float xTmp, yTmp;
    shosha.getPos(iL, xTmp, yTmp);
    h_x_L[iL]->Fill(xTmp / e19);
    h_y_L[iL]->Fill(yTmp / e19);
    h_x_y_L[iL]->Fill(xTmp / e19,  yTmp / e19);

    E1SumL_R += e1;
    E7SumL_R += e7;
    E19SumL_R += e19;
    EAllSumL_R += eAll;
    E1SumL_Rup += e1_up;
    E7SumL_Rup += e7_up;
    E19SumL_Rup += e19_up;
    EAllSumL_Rup += eAll_up;
    E1SumL_Rdw += e1_dw;
    E7SumL_Rdw += e7_dw;
    E19SumL_Rdw += e19_dw;
    EAllSumL_Rdw += eAll_dw;

    E1SumL_AbsW_Mip += layerE1_Mip;
    E7SumL_AbsW_Mip += layerE7_Mip;
    E19SumL_AbsW_Mip += layerE19_Mip;
    EAllSumL_AbsW_Mip += layerEAll_Mip;

    E1SumL_AbsW_GeV += layerE1_GeV;
    E7SumL_AbsW_GeV += layerE7_GeV;
    E19SumL_AbsW_GeV += layerE19_GeV;
    EAllSumL_AbsW_GeV += layerEAll_GeV;
  }

  /*
  for(int iL=0; iL<MAXLAYERS; ++iL){
    if(E1SumL_R != 0.){
      tp_E1oSumL_vs_layer->Fill(iL, seedEnergy[iL]/E1SumL_R);
      tp_E7oSumL_vs_layer->Fill(iL, sevencells_sum[iL]/E7SumL_R);
      tp_E19oSumL_vs_layer->Fill(iL, nineteencells_sum[iL]/E19SumL_R);
    }
  }
  */

  h_eAll_all->Fill(EAllSumL_R);
  h_e7_all->Fill(E7SumL_R);
  h_e19_all->Fill(E19SumL_R);
  h_eAll_all_up->Fill(EAllSumL_Rup);
  h_e7_all_up->Fill(E7SumL_Rup);
  h_e19_all_up->Fill(E19SumL_Rup);
  h_eAll_all_dw->Fill(EAllSumL_Rdw);
  h_e7_all_dw->Fill(E7SumL_Rdw);
  h_e19_all_dw->Fill(E19SumL_Rdw);
  
  h_eAll_all_AbsW_Mip->Fill(EAllSumL_AbsW_Mip);
  h_e7_all_AbsW_Mip->Fill(E7SumL_AbsW_Mip);
  h_e19_all_AbsW_Mip->Fill(E19SumL_AbsW_Mip);
  
  h_eAll_all_AbsW_GeV->Fill(EAllSumL_AbsW_GeV);
  h_e7_all_AbsW_GeV->Fill(E7SumL_AbsW_GeV);
  h_e19_all_AbsW_GeV->Fill(E19SumL_AbsW_GeV);
  
  
}// analyze ends here


// ------------ method called once each job just before starting event loop  ------------
void
Layer_Sum_Analyzer::beginJob()
{
  std::cout << " Layer_Sum_Analyzer::beginJob " << std::endl;
	HGCalCondObjectTextIO io(0);
	edm::FileInPath fip(mapfile_);
	if (!io.load(fip.fullPath(), essource_.emap_)) {
		throw cms::Exception("Unable to load electronics map");
	}

	for(int iii = 0; iii < MAXLAYERS; iii++)
		ADCtoMIP[iii] = ADCtoMIP[iii] * MIP2ParticleCalib[iii]; // Converting response to 120 GeV protons to MIPs
	/*
	        for(int iii= 0; iii<16;iii++){
	            LayerWeight[iii] += 0.8;
	            LayerSumWeight += LayerWeight[iii];
	           }
	*/


}

// ------------ method called once each job just after ending the event loop  ------------
void
Layer_Sum_Analyzer::endJob()
{
  std::cout << " Layer_Sum_Analyzer::endJob " << std::endl;

}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
Layer_Sum_Analyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Layer_Sum_Analyzer);
