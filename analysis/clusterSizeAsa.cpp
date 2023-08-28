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

// struct hit {
//   uint16_t channel;
//   uint16_t maxamp;
//   uint16_t samplemax;
//   float    inflex;
//   uint16_t clusterId;

// };

// struct cluster {
//   float    centroid;
//   uint16_t size;
//   uint16_t id;
// };

void clusterSizeRegion(TChain* chain, std::string detname) {

  std::string graphMap = detname+"_Map.png";
  std::string graphStrip = detname+"_strips.png";
  std::string graphClSize = detname+"_ClSize.png";

  TTreeReader reader(chain);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  TH1F *hcentroidX[4];
  TH1F *hcentroidY[4];
  TH1F *hclSizeX[4];
  TH1F *hclSizeY[4];

  for(int i=0; i<4; i++){
    std::string labelCentroid = "hcentroid"+std::to_string(i);
    hcentroidX[i] = new TH1F((labelCentroid+"X").c_str(), "Centroid strips in x direction", 128,0,128);
    hcentroidY[i] = new TH1F((labelCentroid+"Y").c_str(), "Centroid strips in y direction", 128,0,128);
    hcentroidX[i]->SetXTitle("strip centroid"); hcentroidY[i]->SetXTitle("strip centroid");

    std::string labelclSize = "hclSize"+std::to_string(i);
    hclSizeX[i] = new TH1F((labelclSize+"X").c_str(), "cluster size in x direction", 10,-0.5,10.5);
    hclSizeY[i] = new TH1F((labelclSize+"Y").c_str(), "cluster size in y direction", 10,-0.5,10.5);
    hclSizeX[i]->SetXTitle("cluster size"); hclSizeY[i]->SetXTitle("cluster size");
  }

  TH2F *h2c = new TH2F("h2c", "cluster map", 128,0,128,128,0,128);
  h2c->SetXTitle("centroid on y direction strips");
  h2c->SetYTitle("centroid on x direction strips");

  std::vector<cluster> clX, clY;
  std::vector<float> pitchX = {-1, -1, -1, -1};
  std::vector<float> pitchY = {-1, -1, -1, -1};

  std::vector<float> interX = {-1, -1, -1, -1};
  std::vector<float> interY = {-1, -1, -1, -1};

  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
    clX.clear();
    clY.clear();

    for( auto c : *cls ){
      // std::cout<<c.pitch<<std::endl;
      // if(c.size<4) continue;
      int connector = int(c.centroid)/64 - 4;
      if( c.axis == 'x' ){
        if(c.pitch != pitchX[connector]){ 
          std::cout<<"WARNING Xpitch for connector "<<connector<<" changed from "<<pitchX[connector]<<" to "<<c.pitch<<std::endl;
          pitchX[connector] = c.pitch;
        }
        if(c.inter != interX[connector]){
          std::cout<<"WARNING Xinter for connector "<<connector<<" changed from "<<interX[connector]<<" to "<<c.inter<<std::endl;
          interX[connector] = c.inter;
        }
        clX.push_back(c);
        hcentroidX[connector]->Fill(c.stripCentroid);
        hclSizeX[connector]->Fill(c.size);
      }else if( c.axis == 'y' ){
        if(c.pitch != pitchY[connector]){
          std::cout<<"WARNING Ypitch for connector "<<connector<<" changed from "<<pitchY[connector]<<" to "<<c.pitch<<std::endl;
          pitchY[connector] = c.pitch;
        }
        if(c.inter != interY[connector]){
          std::cout<<"WARNING Yinter for connector "<<connector<<" changed from "<<interY[connector]<<" to "<<c.inter<<std::endl;
          interY[connector] = c.inter;
        }
        clY.push_back(c);
        hcentroidY[connector]->Fill(c.stripCentroid);
        hclSizeY[connector]->Fill(c.size);
      }
    }

    for( auto x = clX.begin(); x < clX.end(); x++){
      for(auto y = clY.begin(); y < clY.end(); y++){
        h2c->Fill(y->stripCentroid, x->stripCentroid);
      }
    }
  }

  // gStyle->SetOptStat(0);

  TCanvas *cclSize = new TCanvas("cclSize", "cclSize", 1600,1000);
  cclSize->Divide(2, 2);
  for(int i=0; i<4; i++){
    std::string titleX = "C"+std::to_string(i)+": pitch "+std::to_string(pitchX[i])+" inter "+std::to_string(interX[i]);
    cclSize->cd(i+1);
    gPad->SetLogy();
    hclSizeX[i]->SetTitle(titleX.c_str());
    hclSizeX[i]->Draw();

    hclSizeY[i]->SetLineColor(kRed);
    hclSizeY[i]->Draw("same");
  }
  cclSize->cd(0);
  TLegend *leg = new TLegend(0.1,0.7,0.48,0.9);
  leg->AddEntry(hclSizeX[0],"cluster size in X","l");
  leg->AddEntry(hclSizeY[0],"cluster size in Y","l");
  leg->Draw();
  cclSize->Print(graphClSize.c_str(), "png");


  TCanvas *cstrips = new TCanvas("cstrips", "cstrips", 1600,1000);
  cstrips->Divide(2, 2);
  for(int i=0; i<4; i++){
    std::string titleX = "C"+std::to_string(i)+": pitch "+std::to_string(pitchX[i])+" inter "+std::to_string(interX[i]);
    cstrips->cd(i+1);
    hcentroidX[i]->SetTitle(titleX.c_str());
    hcentroidX[i]->Draw();

    hcentroidY[i]->SetLineColor(kRed);
    hcentroidY[i]->Draw();
  }
  cstrips->cd(0);
  TLegend *leg = new TLegend(0.1,0.7,0.48,0.9);
  leg->AddEntry(hclSizeX[0],"cluster size in X","l");
  leg->AddEntry(hclSizeY[0],"cluster size in Y","l");
  leg->Draw();
  cstrips->Print(graphStrip.c_str(), "png");

  gStyle->SetOptStat(0);
  TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
  h2c->Draw("colz");
  gPad->SetLogz();
  c3->Print(graphMap.c_str(), "png");

}

int main(int argc, char const *argv[])
{
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

  clusterSizeRegion(chain, detName);

  return 0;
}

