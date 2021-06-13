// server program for udp connection
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
#include <boost/serialization/vector.hpp>
#include <thread>

#include "StelCore.hpp"
#include "StelMovementMgr.hpp"
#include "StelApp.hpp"
//#include "StelLGConnect.hpp"
#include "StelModuleMgr.hpp"
#include "modules/LandscapeMgr.hpp"
#include "modules/ConstellationMgr.hpp"
#include "./scripting/StelMainScriptAPI.hpp"

#include <QSettings>
#include <QString>

#define PORT 5000
#define MAXLINE 1000


#include <fstream>

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using namespace std;

class StelMovementMgr;
class StelCore;
class LandscapeMgr;
class ConstellationMgr;
class StelMainScriptAPI;

//void UDP_connect(StelCore *_core, StelMovementMgr *_mmgr);


class UDP_connect{

private:

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
	static void LG_communicate_master(StelCore *core, StelMovementMgr *mmgr, QSettings* conf, StelMainScriptAPI *msapi);
	static void LG_communicate_slave(StelCore *core, StelMovementMgr *mmgr, QSettings* conf, StelMainScriptAPI *msapi, int unsigned microsecond, QString ip);
	UDP_connect(StelCore *_core, StelMovementMgr *_mmgr){
		core= _core;
		mmgr= _mmgr;
		//StelMainScriptAPI *msapi;
		conf=  StelApp::getInstance().getSettings();
		microsecond= 1000000;
		conf->beginGroup("LGConnect");
		bool is_master= conf->value("thisPC").toInt()==1;
		bool is_slave= conf->value("thisPC").toInt()==0;
		QString ip= conf->value("ip_addr").toString();
		//const char* ip_addr= conf->value("ip_addr").toString().toUtf8().data();
		conf->endGroup();
		
		if (is_master== true){
			//LG_communicate_master();
			std::thread t1(LG_communicate_master, core, mmgr, conf, msapi);
			t1.detach();
		 	//LG_communicate_master();
		 }
		else if (is_slave== true){
			std::thread t1(LG_communicate_slave, core, mmgr, conf, msapi, microsecond, ip);
			t1.detach();
		}
	}
	

};
