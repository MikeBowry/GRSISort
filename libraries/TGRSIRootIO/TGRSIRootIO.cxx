
#include "TGRSIRootIO.h"
#include "TFragmentQueue.h"

#include "TGRSIOptions.h"
#include "TGRSIRunInfo.h"
#include "TGRSIStats.h"
#include "TGRSIint.h"

ClassImp(TGRSIRootIO)



TGRSIRootIO *TGRSIRootIO::fTGRSIRootIO = 0;

TGRSIRootIO *TGRSIRootIO::Get()  {
   if(!fTGRSIRootIO)
      fTGRSIRootIO = new TGRSIRootIO;
   return fTGRSIRootIO;
}

TGRSIRootIO::TGRSIRootIO() { 
   //printf("TGRSIRootIO has been created.\n");

  fFragmentTree = 0;
  fBadFragmentTree =0;
  fEpicsTree    = 0;
  fPPG          = 0;

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

void TGRSIRootIO::LoadRootFile(TFile *file) {
   if(file)
      finfiles.push_back(file);
   return;
}

void TGRSIRootIO::SetUpFragmentTree() {

   if(foutfile)
      foutfile->cd();
   fTimesFillCalled = 0;
   fFragmentTree = new TTree("FragmentTree","FragmentTree");
   fBufferFrag = 0;
   fFragmentTree->Bronch("TFragment","TFragment",&fBufferFrag,128000,99);
	printf("FragmentTree set up.\n");

}


void TGRSIRootIO::SetUpBadFragmentTree() {
   if(!TGRSIOptions::WriteBadFrags())
      return;
   if(foutfile)
      foutfile->cd();
   fTimesBadFillCalled = 0;
   fBadFragmentTree = new TTree("BadFragmentTree","BadFragmentTree");
   fBadBufferFrag = 0;
   fBadFragmentTree->Bronch("TFragment","TFragment",&fBadBufferFrag,128000,99);
	printf("BadFragmentTree set up.\n");

}

void TGRSIRootIO::SetUpPPG() {
   if(foutfile)
      foutfile->cd();
   fTimesPPGCalled = 0;
   if(TGRSIRunInfo::SubRunNumber() <= 0) {
      fPPG = new TPPG;
   } else {
      TFile* prev_sub_run = new TFile(Form("fragment%05d_%03d.root",TGRSIRunInfo::RunNumber(),TGRSIRunInfo::SubRunNumber()));
      if(prev_sub_run->IsOpen()) {
         if(prev_sub_run->Get("TPPG") != NULL) {
            fPPG = (TPPG*) (prev_sub_run->Get("TPPG")->Clone());
         } else {
            printf("Error, could not find PPG in file fragment%05d_%03d.root, not adding previous PPG data\n",TGRSIRunInfo::RunNumber(),TGRSIRunInfo::SubRunNumber());
            fPPG = new TPPG;
         }
         prev_sub_run->Close();
      } else {
         printf("Error, could not find file fragment%05d_%03d.root, not adding previous PPG data\n",TGRSIRunInfo::RunNumber(),TGRSIRunInfo::SubRunNumber());
         fPPG = new TPPG;
      }
   }
}


void TGRSIRootIO::SetUpEpicsTree() {
   if(TGRSIOptions::IgnoreEpics()) 
     return;
   if(foutfile)
      foutfile->cd();
   fEPICSTimesFillCalled = 0;
   fEpicsTree = new TTree("EpicsTree","EpicsTree");
   fEXBufferFrag = 0;
   fEpicsTree->Bronch("TEpicsFrag","TEpicsFrag",&fEXBufferFrag,128000,99);
	printf("EPICS-Tree set up.\n");

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
   *fBufferFrag = *frag;
   int bytes =  fFragmentTree->Fill();
   if(bytes < 1)
      printf("\n fill failed with bytes = %i\n",bytes);
   fTimesFillCalled++;
}


void TGRSIRootIO::FillBadFragmentTree(TFragment *frag) {
   if(!TGRSIOptions::WriteBadFrags())
      return;
   *fBadBufferFrag = *frag;
   int bytes =  fBadFragmentTree->Fill();
   if(bytes < 1)
      printf("\n (BadTree) fill failed with bytes = %i\n",bytes);
   fTimesBadFillCalled++;
}

void TGRSIRootIO::FillPPG(TPPGData* data){
   //Set PPG Stuff here
   fPPG->AddData(data);
   ++fTimesPPGCalled;
}

void TGRSIRootIO::FillEpicsTree(TEpicsFrag *EXfrag) {
  if(TGRSIOptions::IgnoreEpics()) 
    return;
   *fEXBufferFrag = *EXfrag;
   int bytes =  fEpicsTree->Fill();
   if(bytes < 1)
      printf("\n fill failed with bytes = %i\n",bytes);
   fEPICSTimesFillCalled++;
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
   TList *list = fFragmentTree->GetUserInfo();
   std::map < unsigned int, TChannel * >::iterator iter;
   foutfile->cd();
	TChannel *chan = TChannel::GetDefaultChannel();//new TChannel(iter->second);
	if(chan != NULL) {
      chan->SetNameTitle(Form("TChannels[%i]",TChannel::GetNumberOfChannels()),
                         Form("%i TChannels.",TChannel::GetNumberOfChannels()));
                           // using the write command on any tchannel will now write all 
      chan->WriteToRoot(); // the tchannels to a root file.  additionally reading a tchannel
                           // from a rootfile will read all the channels saved to it.  tchannels
                           // are now saved as a text buffer to the root file.  pcb.
	                        // update. (3/9/2015) the WriteToRoot function should now 
                           // corretcly save the tchannels even if the came from the odb(i.e. internal 
                           // data buffer not set.)  pcb.
   } else {
		printf("Failed to get default channel, not going to write TChannel information!\n");
	}
   
   foutfile->cd();
   fFragmentTree->AutoSave(); //Write();
	return;
}


void TGRSIRootIO::FinalizeBadFragmentTree() {
   if(!fFragmentTree || !foutfile)
      return;
   foutfile->cd();
   fBadFragmentTree->AutoSave(); //Write();
	return;
}

void TGRSIRootIO::FinalizePPG(){
   if(!fPPG || !foutfile)
      return;
   foutfile->cd();
   if(fPPG->PPGSize()){
      printf("Writing PPG\n");
      fPPG->Write("TPPG",TObject::kSingleKey);
   }
   return;
}

void TGRSIRootIO::FinalizeEpicsTree() {
  if(TGRSIOptions::IgnoreEpics()) 
    return;
  if(!fEpicsTree || !foutfile)
      return;
   foutfile->cd();
   fEpicsTree->AutoSave(); //Write();
	return;
}

bool TGRSIRootIO::SetUpRootOutFile(int runnumber, int subrunnumber) {
  
   char filename[64];
   if(subrunnumber>-1)
      sprintf(filename,"fragment%05i_%03i.root",runnumber,subrunnumber); 
   else
      sprintf(filename,"fragment%05i.root",runnumber);
	printf("Creating root outfile: %s\n",filename);
   //Add the filename to the possible root files so that it can be auto sorted. 
   //If there are no -s or -a flags these extra names do not matter
   std::string tempname(filename);
   TGRSIOptions::AddInputRootFile(tempname);
   foutfile = new TFile(filename,"recreate");

   if(!foutfile->IsOpen()) {
      return false;
   }
   
   SetUpFragmentTree();
   SetUpBadFragmentTree();
   SetUpPPG();
   SetUpEpicsTree();

   return true;
}

void TGRSIRootIO::CloseRootOutFile()   {


   if(!foutfile)
      return;
   foutfile->cd();
   printf(DMAGENTA "\n Fill tree called " DYELLOW "%i " DMAGENTA "times.\n" RESET_COLOR, fTimesFillCalled);
   if(TGRSIOptions::WriteBadFrags())
     printf(DRED "   Fill bad tree called " DYELLOW "%i " DRED "times.\n" RESET_COLOR, fTimesBadFillCalled);
   
   FinalizeFragmentTree();
   FinalizePPG();
   FinalizeEpicsTree();

   if(TGRSIRunInfo::GetNumberOfSystems()>0) {
      printf(DMAGENTA " Writing RunInfo with " DYELLOW "%i " DMAGENTA " systems to file." RESET_COLOR "\n",TGRSIRunInfo::GetNumberOfSystems());
      //get run start and stop (in seconds) from the fragment tree
      TGRSIRunInfo::Get()->SetRunStart(fFragmentTree->GetMinimum("MidasTimeStamp"));
      TGRSIRunInfo::Get()->SetRunStop( fFragmentTree->GetMaximum("MidasTimeStamp"));
      TGRSIRunInfo::Get()->SetRunLength(fFragmentTree->GetMaximum("MidasTimeStamp") - fFragmentTree->GetMinimum("MidasTimeStamp"));
      printf("set run start to %.0f, and stop to %.0f (run length %.0f)\n",TGRSIRunInfo::Get()->RunStart(),TGRSIRunInfo::Get()->RunStop(),TGRSIRunInfo::Get()->RunLength());
      TGRSIRunInfo::Get()->Write();
   }

   if(TGRSIStats::GetSize() > 0) {
       TGRSIRootIO::Get()->WriteRunStats();
   }



   foutfile->Close();
   //if(fFragmentTree) {
   //   delete fFragmentTree;
   //   fFragmentTree = 0;
   //}
   //if(fEpicsTree) {
   //   delete fEpicsTree;
   //   fEpicsTree = 0;
   //}

	delete foutfile;	
	foutfile = 0;
	return;

};


void TGRSIRootIO::MakeUserHistsFromFragmentTree() {

   //printf("here1\n");

   int runnumber =0; //This is where hists run number problem is happening
   int subrunnumber=0; //Might have to make these vectors?
 
   TChain *chain = new TChain("FragmentTree");

   for(int x=0;x<TGRSIOptions::GetInputRoot().size();x++) {
      TFile f(TGRSIOptions::GetInputRoot().at(x).c_str(),"read");
      //printf("%s  f.FindObject(\"FragmentTree\") =0x%08x\n",f.GetName(),     f.FindObject("FragmentTree"));
      //if(f.FindObject("FragmentTree")) {
	//printf("here 4 \n");
	chain->Add(TGRSIOptions::GetInputRoot().at(x).c_str());
      //}
      f.Close();
   }

   const char *firstfilename = TGRSIOptions::GetInputRoot().at(0).c_str();
   runnumber    = GetRunNumber(firstfilename);
   subrunnumber = GetSubRunNumber(firstfilename);

   //printf("here2\n");
   if(chain->GetNtrees()==0)
	return;
   
   //printf("here3\n");

   TProofLite *proof = (TProofLite*)TProofLite::Open("");
   proof->ClearCache();
   proof->Exec("gSystem->Load(\"libTGRSIFormat.so\")");
   proof->SetProgressDialog(TGRSIOptions::ProgressDialog());
   //Going to get run number from file name. This will allow us to chain->chop off the subrun numbers

 
   chain->SetProof();
   TFragmentSelector *fragSelc = new TFragmentSelector(runnumber,subrunnumber);
   chain->Process(fragSelc); 

   chain->Delete();

   return;
}


int TGRSIRootIO::GetRunNumber(std::string filename) {
   if(filename.length()==0) {
      return 0;
   }
/*   std::size_t foundslash = filename.rfind('/');
   std::cout << "foundslash is: " << foundslash << std::endl;
   if(foundslash != std::string::npos){
      filename = filename.substr(foundslash,std::string::npos);
   }*/
   std::size_t found = filename.rfind(".root");
   if(found == std::string::npos) {
      return 0;
   }
   std::size_t found2 = filename.rfind('-');
   //printf("found 2 = %i\n",found2);

   if(found2 == std::string::npos)
      found2 = filename.rfind('_');
   std::string temp;
   if(found2 == std::string::npos || filename.compare(found2+4,5,".root") !=0 ) {
      temp = filename.substr(found-5,5);
   }
   else {
      temp = filename.substr(found-9,5);
   }
   //printf(" %s \t %i \n",temp.c_str(),atoi(temp.c_str()));
   return atoi(temp.c_str());
};


int TGRSIRootIO::GetSubRunNumber(std::string filename)	{
   if(filename.length()==0)
      return -1;

   std::size_t found = filename.rfind("-");
   if(found != std::string::npos) {
      std::string temp = filename.substr(found+1,3);
      //printf("%i \n",atoi(temp.c_str()));
      return atoi(temp.c_str());
   }
   found = filename.rfind("_");
   if(found != std::string::npos) {
      std::string temp = filename.substr(found+1,3);
      //printf("%i \n",atoi(temp.c_str()));
      return atoi(temp.c_str());
   }
   return -1;
};

void TGRSIRootIO::WriteRunStats(){
  printf("entering writing\n");
  if(!foutfile)
    return;
  printf("actually writing\n");
   TGRSIStats *stats = 0;
   TTree *fStatsTree = new TTree("StatsTree","StatsTree");
   fStatsTree->Bronch("TGRSIStats","TGRSIStats",&stats);

  std::map<int,TGRSIStats*>::iterator iter;
  std::ofstream statsout;
  statsout.open(Form("stats%05i_%03i.log",TGRSIRunInfo::RunNumber(),TGRSIRunInfo::SubRunNumber()));
  statsout << "\nRun time to the nearest second = " << TGRSIStats::GetRunTime()  << std::endl << std::endl;
  for(iter = TGRSIStats::GetMap()->begin();iter!=TGRSIStats::GetMap()->end();iter++) {
		int tmp_add = iter->second->GetAddress();
		TChannel *chan = TChannel::GetChannel(tmp_add);
		if(!chan)
			continue;
		stats = iter->second;
                fStatsTree->Fill();
		statsout << "0x"<< std::hex <<  stats->GetAddress() << std::dec  << "\t" <<  chan->GetChannelName() << "\tDeadtime: " << ((float)(stats->GetDeadTime()))*(1E-9) << " seconds." << std::endl;
  }
  TGRSIStats::GetMap()->clear();
  statsout <<  std::endl;
  statsout.close();
  fStatsTree->Write();
  return;
}







//void TGRSIRootIO::StartMakeAnalysisTree() { 
	//std::vector<char**> args;
	//for(int x=0;x<TGRSIOptions::Get()->GetInputRoot().size();x++) {
	//	args.push_back(TGRSIOptions::Get()->GetInputRoot().at(x).c_str());
	//}
	//MakeAnalysisTree(args.size(),args.data());
//	MakeAnalysisTree(TGRSIOptions::Get()->GetInputRoot());
//}


//void TGRSIRootIO::MakeAnalysisTree(std::vector<std::string>){}



