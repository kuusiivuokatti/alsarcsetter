#include <iostream>
#include <vector>
#include <filesystem>
#include <unistd.h>

#include "../include/main.h"

void PrintHelp(){
	std::cout<<"Help goes here\n";
}

int main(int argc,char *argv[]){

	if(argc==1){
		PrintHelp();
		return 1;
	}

	if(geteuid()==0){
		std::cout<<"Running as root, exiting\n";
		return 1;
	};

	std::string dirHome=getenv("HOME");
	std::string dirConf=dirHome+"/alsasetrc";
	std::string fileConfCur=dirHome+"/.asoundrc";
	
	if(DEBUG){std::cout<<"Debugging is active\n"<<"Home directory set to "<<dirHome<<"\nConfig directory set to "<<dirConf<<"\nCurrent config file set to "<<fileConfCur<<"\n";}

	std::vector<std::string>args(argv,argv+argc);
	for(auto &arg:args){
		if(arg!=argv[0]){
			if(DEBUG){std::cout<<"Current argument "<<arg<<"\n";}
			if(arg=="-h" || arg=="--help"){
				PrintHelp();
				break;
			}else if(arg=="-v" || arg=="--version"){
				std::cout<<NAME<<" version "<<VERSION<<"\n";
				break;
			}else if(arg=="-s" || arg=="--set"){
				if(std::filesystem::exists(dirConf)){
					std::vector<std::pair<int,std::string>>fileConfVec;
					int i=0,j=0;
					for(const auto &file:std::filesystem::directory_iterator(dirConf)){
						if(not(std::filesystem::is_directory(file))){
							fileConfVec.emplace_back(i,file.path());
							i+=1;
						}
					}
					if(i>0){
						std::cout<<"Found these files at "<<dirConf<<"\n";
						for(auto it:fileConfVec){
							std::cout<<"["<<it.first<<"] "<<it.second<<"\n";
						}
						std::cout<<"Select a config file number to use: ";
						j=0;
						std::cin>>j;
						if(j>=0 && j<i){
							std::string fileConfSel=fileConfVec[j].second;
							std::cout<<fileConfSel<< " selected, use this as the new config (y/N)?\n";
							char input;
							std::cin>>input;
							if(input=='y' || input=='Y'){
								try{
									if(std::filesystem::remove(fileConfCur)){
										if(DEBUG){std::cout<<"Old config file "<<fileConfCur<<" removed\n";};
									}
									symlink(fileConfSel.c_str(),fileConfCur.c_str());
									std::cout<<"New config file in use\n";
								}catch(const std::filesystem::filesystem_error &err){
									std::cout<<"Error removing old config file "<<err.what()<<"\n";
								}
							}else{
								std::cout<<"No config changes made\n";
							};
						}else{
							std::cout<<"Invalid config file number\n";
						}
					}else{
						std::cout<<"No files found in "<<dirConf<<"\n";
					}
				}else{
					std::cout<<"Config file directory "<<dirConf<<" not found\n";
				}
				break;
			}else{
				std::cout<<"Unknown argument "<<arg<<"\n";
				return 1;
			}
		}
	}

	return 0;

}
