#include <stdio.h>
#include <strings.h>
#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include <sstream>
#include <thread>

#include "StelCore.hpp"
#include "StelMovementMgr.hpp"
#include "StelApp.hpp"
#include "StelModuleMgr.hpp"
#include "modules/LandscapeMgr.hpp"
#include "modules/ConstellationMgr.hpp"
#include "./scripting/StelMainScriptAPI.hpp"


// for broadcast ip setting
#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
// -------------------------------

#include <QSettings>
#include <QString>

#define PORT 5000
#define MAXLINE 1000


#include <fstream>


using namespace std;

class StelMovementMgr;
class StelCore;
class LandscapeMgr;
class ConstellationMgr;
class StelMainScriptAPI;




class UDP_connect{

private:
	// to split received string
	static size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
	{
		size_t pos = txt.find( ch );
		size_t initialPos = 0;
		strs.clear();

		// Decompose statement
		while( pos != std::string::npos ) {
		    strs.push_back( txt.substr( initialPos, pos - initialPos ) );
		    initialPos = pos + 1;
		    pos = txt.find( ch, initialPos );
		}
		// Add the last one
		strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

		return strs.size();
	}

	
	StelCore *core;
	StelMovementMgr *mmgr;
	QSettings* conf;
	StelMainScriptAPI *msapi;
	bool is_master;
	bool is_slave;
	int unsigned microsecond;
	
	
	
public:
	static void LG_communicate_master(StelCore *core, StelMovementMgr *mmgr, QSettings* conf, StelMainScriptAPI *msapi, char* ip_addr);
	static void LG_communicate_slave(StelCore *core, StelMovementMgr *mmgr, QSettings* conf, StelMainScriptAPI *msapi, int unsigned microsecond, QString ip);
	UDP_connect(StelCore *_core, StelMovementMgr *_mmgr){
		core= _core;
		mmgr= _mmgr;
		//StelMainScriptAPI *msapi;
		conf=  StelApp::getInstance().getSettings();
		microsecond= 1000000;
		// get values from config file
		conf->beginGroup("LGConnect");
		bool is_master= conf->value("thisPC").toInt()==1;
		bool is_slave= conf->value("thisPC").toInt()==0;
		QString ip= conf->value("ip_addr").toString();
		conf->endGroup();
		char* ip_addr= get_broadcasting_addr().toUtf8().data();
		if (is_master== true){
			// run as new thread
			cout<<"reached here "<<endl;
			std::thread t1(LG_communicate_master, core, mmgr, conf, msapi, ip_addr);
			t1.detach();
		 }
		else if (is_slave== true){
			// run as new thread
			std::thread t1(LG_communicate_slave, core, mmgr, conf, msapi, microsecond, ip);
			t1.detach();
		}
	}
	
	static QString get_broadcasting_addr(){
	
		struct ifaddrs * ifAddrStruct=NULL;
		struct ifaddrs * ifa=NULL;
		void * tmpAddrPtr=NULL;

		getifaddrs(&ifAddrStruct);
		int count= 0;
		for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
			count++;
			if (count< 4) continue;
		     if(ifa->ifa_ifu.ifu_broadaddr->sa_family == AF_INET){
		     char addressBuffer[INET_ADDRSTRLEN];
		        tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_ifu.ifu_broadaddr)->sin_addr;
		        inet_ntop(ifa->ifa_ifu.ifu_broadaddr->sa_family,
		                     tmpAddrPtr,
		                     addressBuffer,
		                     sizeof(addressBuffer));
		                     printf(addressBuffer);
		                     QString ret= QString::fromStdString(string(addressBuffer));
		                     return ret;
		             break;
		        }
		}
		
		return NULL;
	
	}
	
	

};
