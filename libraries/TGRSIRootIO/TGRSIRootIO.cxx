
#include "TGRSIRootIO.h"
#include "TFragmentQueue.h"

#include "TGRSIOptions.h"

ClassImp(TGRSIRootIO)



TGRSIRootIO *TGRSIRootIO::fTGRSIRootIO = 0;

TGRSIRootIO *TGRSIRootIO::Get()  {
   if(!fTGRSIRootIO)
      fTGRSIRootIO = new TGRSIRootIO;
   return fTGRSIRootIO;
}

TGRSIRootIO::TGRSIRootIO() { 
   printf("TGRSIRootIO has been created.\n");


  foutfile = 0; //new TFile("test_out.root","recreate");

  // SetUpChannelTree();
  // SetUpFragmentTree();
   
}

TGRSIRootIO::~TGRSIRootIO() {  }

//void TGRSIRootIO::SetUpChannelTree() {
//   fTChannelTree = new TTree("ChannelTree","ChannelTree");
//   fBufferChannel = 0;
//   fTChannelTree->Branch("TChannel","TChannel",&fBufferChannel,1000,99);
//}


void TGRSIRootIO::SetUpFragmentTree() {

   if(foutfile)
      foutfile->cd();
   fTimesFillCalled = 0;
   fFragmentTree = new TTree("FragmentTree","FragmentTree");
   fBufferFrag = 0;
   fFragmentTree->Branch("TFragment","TFragment",&fBufferFrag,100000,99);

}

//void TGRSIRootIO::FillChannelTree(TChannel *chan) {
//   if(!fTChannelTree)
//      return;
//   fBufferChannel = chan;
//   fTChannelTree->Fill();
//}

void TGRSIRootIO::FillFragmentTree(TFragment *frag) {
  // if(!fFragmentTree)
  //    return;
   fBufferFrag = frag;

   int bytes =  fFragmentTree->Fill();
   if(bytes < 1)
      printf("\n fill failed with bytes = %i\n",bytes);


   fTimesFillCalled++;
}


//void TGRSIRootIO::FinalizeChannelTree() {
//   if(!fTChannelTree)
//      return;
//   fTChannelTree->Write();
//   return;
//}



void TGRSIRootIO::FinalizeFragmentTree() {
   if(!fFragmentTree || !foutfile)
      return;
   TIter *iter = TChannel::GetChannelIter();   
   TList *list = fFragmentTree->GetUserInfo();
   while(TChannel *chan = (TChannel*)iter->Next()) {
      list->Add(chan);
   }
   foutfile->cd();
   fFragmentTree->Write();
   return;
}

void TGRSIRootIO::SetUpRootOutFile(int runnumber, int subrunnumber) {
  
   char filename[64];
   if(subrunnumber>-1)
      sprintf(filename,"fragment%05i_%03i.root",runnumber,subrunnumber); 
   else
      sprintf(filename,"fragment%05i.root",runnumber);
   foutfile = new TFile(filename,"recreate");
   
   SetUpFragmentTree();

   return;
}

void TGRSIRootIO::CloseRootOutFile()   {


   if(!foutfile)
      return;
   foutfile->cd();
   printf(DMAGENTA "\n Fill tree called " DYELLOW "%i " DMAGENTA "times.\n" RESET_COLOR, fTimesFillCalled);
   

   FinalizeFragmentTree();   


   foutfile->Close();


};


void TGRSIRootIO::MakeUserHistsFromFragmentTree() {

   //printf("here1\n");

   TChain *chain = new TChain("FragmentTree");

   for(int x=0;x<TGRSIOptions::Get()->GetInputRoot().size();x++) {
      TFile f(TGRSIOptions::Get()->GetInputRoot().at(x).c_str(),"read");
      //printf("%s  f.FindObject(\"FragmentTree\") =0x%08x\n",f.GetName(),     f.FindObject("FragmentTree"));
      //if(f.FindObject("FragmentTree")) {
	//printf("here 4 \n");
	chain->Add(TGRSIOptions::Get()->GetInputRoot().at(x).c_str());
      //}
      f.Close();
   }

   //printf("here2\n");
   if(chain->GetNtrees()==0)
	return;
   
   //printf("here3\n");

   TProofLite *proof = (TProofLite*)TProofLite::Open("");
   proof->ClearCache();
   proof->Exec("gSystem->Load(\"$(GRSISYS)/libraries/libGRSIFormat.so\")");

   int runnumber =0;
   int subrunnumber=0;
 
 
   chain->SetProof();
   TFragmentSelector *fragSelc = new TFragmentSelector(runnumber,subrunnumber);
   chain->Process(fragSelc); 

   chain->Delete();

   return;
}





