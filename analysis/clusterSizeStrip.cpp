#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"

#include "../reco/definitions.h"
#include "../map/StripTable.h"
#include "clusterSize.h"


int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

  TChain* chain = new TChain("events");
  std::string detName = "test";
  for( int i = 1; i < argc; i++) {

    TString input = argv[i];

    if( input.Contains( "root" ) ){
      chain->Add( input );
    }
    else{
      std::cout<<"Detector Name: "<<argv[i]<<std::endl;
      detName = argv[i];
    }
  }
  StripTable det(basedir+"../map/strip_map.txt");
  clusterSizeRegion(chain, detName, det);
  clusterSizeLims(chain, detName, det, {55, 62}, {61, 70});

  return 0;
}

