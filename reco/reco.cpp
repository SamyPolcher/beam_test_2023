#include "TFile.h"
#include "TFile.h"
#include "TTree.h"
#include "TInterpreter.h"
#include "definitions.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>
using namespace std;

    bool compareHits( hit &a, hit &b ) { return a.channel < b.channel; }

void reco( string name) {

  TFile *infile = TFile::Open(name.data());
  if( !infile ){
    return;
  }

  TTree *nt = (TTree*)infile->Get("nt");

  uint64_t eventId;
  uint64_t timestamp;
  uint64_t delta_timestamp;
  uint16_t ftst;
  vector<unsigned short> *ampl   = new vector<unsigned short>();
  vector<unsigned short> *sample = new vector<unsigned short>();
  vector<unsigned short> *channel = new vector<unsigned short>();

  nt->SetBranchAddress( "amplitude", &ampl );
  nt->SetBranchAddress( "sample",    &sample );
  nt->SetBranchAddress( "channel",   &channel );
  nt->SetBranchAddress( "eventId", &eventId );
  nt->SetBranchAddress( "timestamp", &timestamp );
  nt->SetBranchAddress( "delta_timestamp", &delta_timestamp);
  nt->SetBranchAddress( "ftst", &ftst );

  //
  name = name.substr( name.rfind('/')+1  ).insert( 0, "rec_");
  TFile *fout  = TFile::Open( name.data(), "recreate");
  TTree outnt( "events","test_beam_2023");
  outnt.SetDirectory( fout );

  hit ahit;
  vector<hit> *hits = new vector<hit>();
  vector<cluster> *cls = new vector<cluster>();
  outnt.Branch( "hits", &hits );
  outnt.Branch( "clusters", &cls );
  outnt.Branch( "eventId", &eventId );
  outnt.Branch( "timestamp", &timestamp );
  outnt.Branch( "delta_timestamp", &delta_timestamp);
  outnt.Branch( "ftst", &ftst );


  // loop over the events
  // --------------------
  for ( int iev=0; iev<nt->GetEntries() ; iev++){
    nt->GetEntry(iev);

    map<uint16_t,uint16_t> maxamp;
    map<uint16_t,uint16_t> sampmax;
    map<uint16_t,float> flex;
    map<uint16_t,vector<uint16_t>> amplitudes;

    // make hits
    // ---------

    // loop over the fired channels and organize them as hits
    for( uint64_t j=0; j < ampl->size() ; j++ ){
      int jch = channel->at(j);
         
      amplitudes[jch].push_back( ampl->at(j));

      /// find maximum amplitude and its associated sample
      if( maxamp.find( jch ) == maxamp.end() ){
        maxamp[jch] = 0;
      }
      if( maxamp[jch]  <  ampl->at(j) ){
         maxamp[jch]   =  ampl->at(j);
         sampmax[jch]  = sample->at(j);
      }
    }

    // find the inflection point
    for( auto &a : amplitudes ){
      uint16_t max = 0;
      uint16_t imax = 0;
      //for( int i=0;i<a.second.size()-1; i++){
      for( int i=0;i<a.second.size()-1; i++){
        if( i > sampmax[a.first] ) break;
        uint16_t diff = a.second.at(i+1) - a.second.at(i);
        if( diff > max ){
          max = diff;
          imax = i;
        }
      }
      flex[a.first] = (float)(2*imax+1)/2.;
    }


    // fill a vector of hits
    hits->clear();
    for( auto &m : maxamp ){
      ahit.channel   = m.first;
      ahit.maxamp    = m.second;
      ahit.samplemax = sampmax[m.first];
      ahit.inflex    = flex[m.first];
      hits->push_back(ahit);
    }


    // make clusters
    // -------------
    // a cluster is a sequence of contiguous hits
    // TODO: 
    //   1.  check for hits begin in the same part of the detector
    //   2.  check for missing/dead strip

    // sort hits, probably not needed, but just in case
    sort( hits->begin(), hits->end(), compareHits );

    //for( auto it = hits->begin(); it < hits->end(); it++ ) cout << it->channel << ":" << it->maxamp << " ";
    //cout << endl;

    cls->clear();
    int oldch = -1;
    for( auto it = hits->begin() ; it < hits->end(); ){
      
      if( oldch < 0 ){
        oldch = it->channel;
        int num = 0;
        int den = 0;
        int size = 0;
        while( oldch >= 0 ){

          num += it->channel * it->maxamp;
          den += it->maxamp;

          size++;
          //cout << "it " << it->channel << endl;
          it++;
          if( it == hits->end() || (it->channel - oldch) > 1 ){
            oldch = -1;
          }
          else {
            oldch = it->channel;
          }

        }

        // make a cluster
        cluster cl;
        cl.size     = size;
        cl.centroid = (float) num / den;
        cls->push_back( cl );

      }

    }

    //for( auto &c : *cls ) cout << c.centroid << ":" << c.size << " ";
    //cout << endl;
    


    outnt.Fill();

    //if( eventId > 10  )break;

  }
  fout->Write();
  fout->Close();
  infile->Close();

}

int main( int argc, char **argv ){

  if( argc < 2 ) {
    cerr << " no file name specified \n";
    return 1;
  }

  for( int i=1; i< argc; i++){
    string name = argv[i];
    if( name.find( ".root" ) > name.size() ){
      cerr << " warning : " << name << " is not a root file; we'll ignore it" << endl;
      continue;
    }
    reco( argv[i] );
  }
  return 0;
}

