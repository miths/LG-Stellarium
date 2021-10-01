

#include "StelCore.hpp"
#include "StelMovementMgr.hpp"
#include "StelApp.hpp"
#include "StelLGConnect.hpp"
#include "StelModuleMgr.hpp"
#include "modules/LandscapeMgr.hpp"
#include "modules/ConstellationMgr.hpp"
#include "./scripting/StelMainScriptAPI.hpp"

#include <QSettings>

using namespace std;



void UDP_connect::LG_communicate_master(StelCore *core, StelMovementMgr *mmgr, QSettings* conf, StelMainScriptAPI *msapi, char* ip_addr){
		usleep(5 * 1000000);
		std::cout<<"DEBUG in udp function"<< endl;

		bool firstTime= true;
		//char* ip_addr= get_broadcasting_addr().toUtf8().data();
		
		int listenfd;
		int JD_changed_countdown= 0;
		socklen_t len;
		struct sockaddr_in servaddr, cliaddr;
		cliaddr.sin_addr.s_addr = inet_addr(ip_addr);
		bzero(&servaddr, sizeof(servaddr));

		// Create a UDP Socket
		listenfd = socket(AF_INET, SOCK_DGRAM, 0);		
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(PORT);
		servaddr.sin_family = AF_INET;
		std::cout<<"DEBUG in udp function 1 "<< endl;
		// bind server address to socket descriptor
		bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
		
		//receive the datagram
		
		while(1){
			char buffer[1000];
			len = sizeof(cliaddr);
			//int n = recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr,&len); //receive message from server
			if (sizeof(buffer)>= 0){
				//buffer[n] = '\0';
				//puts(buffer);
				//std::cout<<"DEBUG: msg rcv in master "<<endl;
				std::stringstream ss;
				std::cout<<"DEBUG in udp function 2 "<< endl;
				const Vec3d curr= mmgr-> getViewDirectionJ2000();
				const double fov= mmgr-> getCurrentFov();
				const double skyTime= core->getPresetSkyTime();
				const double timeRate= core->getTimeRate();
				const double Jday= core->getJD();
				const double DeltaT= core->getDeltaT();
				
				const bool atmFlag= GETSTELMODULE(LandscapeMgr)->getFlagAtmosphere();
				const bool lndFlag= GETSTELMODULE(LandscapeMgr)->getFlagLandscape();
				const bool crdFlag= GETSTELMODULE(LandscapeMgr)->getFlagCardinalsPoints();
				
				const bool cstArt= GETSTELMODULE(ConstellationMgr)->getFlagArt();
				const bool cstLin= GETSTELMODULE(ConstellationMgr)->getFlagLines();
				const bool cstLbl= GETSTELMODULE(ConstellationMgr)->getFlagLabels();
				
				/*
				const bool atmFlag= false;
				const bool lndFlag= false;
				const bool crdFlag= false;
				
				const bool cstArt= false;
				const bool cstLin= false;
				const bool cstLbl= false;
				*/
				
				const QString loc= msapi->getObserverLocation();
				const QString date= msapi->getDate();
				bool JD_changed_signal= core-> JD_changed;
				if (core->JD_changed== true) {
					core-> JD_changed= false;
					}
				if (firstTime== true){
					JD_changed_signal= true;
					firstTime= false;
				}
				if (JD_changed_signal== true) { JD_changed_countdown= 20; }
				
				if (JD_changed_countdown> 0) { 
					JD_changed_signal= true;
					JD_changed_countdown--;
				}
				
// pack data into string
				{
					ss << curr[0]<<"|"<< curr[1]<<"|"<< curr[2]<< "|"<< fov<< "|"<< date.toStdString()<< "|"<< timeRate<< "|"<< atmFlag<< "|"<< lndFlag<< "|"<< crdFlag<< "|"<< cstArt<< "|"<< cstLin<< "|"<< cstLbl<< "|"<< loc.toStdString()<<"|"<<JD_changed_signal;

				}

				std::string s= ss.str();
				std::cout<<"DEBUG in udp function 3 "<< endl;
				sendto(listenfd, s.c_str() , MAXLINE, 0,(struct sockaddr*)&cliaddr, sizeof(cliaddr));
				std::cout<<"DEBUG: msg sent in master"<<endl;
			}
		}
	}
void UDP_connect::LG_communicate_slave(StelCore *core, StelMovementMgr *mmgr, QSettings* conf, StelMainScriptAPI *msapi, int unsigned microsecond, QString ip){
	usleep(5 * microsecond);//sleeps for 5 second

    //std::cout<<"DEBUG in udp function"<<endl;
	//char* ip_addr= ip.toUtf8().data();
    //char buffer[1000];
    char *message = "Hello Server";
    int sockfd, n;
    
    //struct sockaddr_in servaddr;
    // clear servaddr
    
    
	//socklen_t len;
	int listenfd;
	struct sockaddr_in servaddr, cliaddr;
	//cliaddr.sin_addr.s_addr = inet_addr(ip_addr);
	bzero(&servaddr, sizeof(servaddr));

	// Create a UDP Socket
	listenfd = socket(AF_INET, SOCK_DGRAM, 0);		
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	servaddr.sin_family = AF_INET;
	std::cout<<"DEBUG in udp function 1 "<< endl;
	// bind server address to socket descriptor
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));







    //bzero(&servaddr, sizeof(servaddr));
	//std::cout<<"DEBUG: ipaddr "<< ip_addr<<endl;
	/*
    servaddr.sin_addr.s_addr = inet_addr(ip_addr);   //  192.168.43.12 | 10.0.2.18
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;
    */
    // create datagram socket
    //sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // connect to server
    /*
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }
*/

    while(1){
	char buffer[1000];
        // request to send datagram
        // no need to specify server address in sendto
        // connect stores the peers IP and port
        
        //sendto(sockfd, message, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr));
		
		//std::cout<<"DEBUG: msg sent in slave"<<endl;
        // waiting for response
        std::cout<<"DEBUG waiting for response"<< endl;
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
		puts(buffer);
		std::cout<<"DEBUG: msg rcv in slave"<<endl;
        
        char str[(sizeof(buffer)) + 1];
            memcpy(str, buffer, sizeof(buffer));
        str[sizeof(buffer)] = 0; // Null termination.
        puts(buffer);

        Vec3d curr= mmgr->getViewDirectionJ2000();

        std::vector<std::string> v;
        split(str, v, '|');
        
        if (v.size()!= 14) continue;
        
        Vec3d pos;
// retrieve data
        pos[0]= std::stod(v.at(0));
        pos[1]= std::stod(v.at(1));
        pos[2]= std::stod(v.at(2));
        double fov= std::stod(v.at(3));
        QString date= QString::fromStdString(v.at(4));
        double timeRate= std::stod(v.at(5));
        bool atmFlag= v.at(6)== "1";
        bool lndFlag= v.at(7)== "1";
        bool crdFlag= v.at(8)== "1";
        bool cstArt= v.at(9)== "1";
        bool cstLin= v.at(10)== "1";
        bool cstLbl= v.at(11)== "1";
        QString loc= QString::fromStdString(v.at(12));
	bool JD_changed= v.at(13)== "1";

        if (pos[0]!= curr[0] or pos[1]!= curr[1] or pos[2]!= curr[2] ){ mmgr->setViewDirectionJ2000(pos);}
        if (fov!= mmgr->getCurrentFov()){mmgr->zoomTo(fov, 0);}

	if (JD_changed== true) {msapi-> setDate(date); }
        if (timeRate!= core->getTimeRate()){core->setTimeRate(timeRate);}
        if (atmFlag!= GETSTELMODULE(LandscapeMgr)->getFlagAtmosphere()){
            LandscapeMgr *lmr= (LandscapeMgr*)GETSTELMODULE(LandscapeMgr);
            if (lmr) {lmr->setFlagAtmosphere(atmFlag);}
        }
        if (lndFlag!= GETSTELMODULE(LandscapeMgr)->getFlagLandscape()){
            LandscapeMgr *lmr= (LandscapeMgr*)GETSTELMODULE(LandscapeMgr);
            if (lmr) {lmr->setFlagLandscape(lndFlag);}
        }
        if (crdFlag!= GETSTELMODULE(LandscapeMgr)->getFlagCardinalsPoints()){
            LandscapeMgr *lmr= (LandscapeMgr*)GETSTELMODULE(LandscapeMgr);
            if (lmr) {lmr->setFlagCardinalsPoints(crdFlag);}
        }
        if (cstArt!= GETSTELMODULE(ConstellationMgr)->getFlagArt()){
            ConstellationMgr *cmr= (ConstellationMgr*)GETSTELMODULE(ConstellationMgr);
            if (cmr) {cmr->setFlagArt(cstArt);}
        }
        if (cstLin!= GETSTELMODULE(ConstellationMgr)->getFlagLines()){
            ConstellationMgr *cmr= (ConstellationMgr*)GETSTELMODULE(ConstellationMgr);
            if (cmr) {cmr->setFlagLines(cstLin);}
        }
        if (cstLbl!= GETSTELMODULE(ConstellationMgr)->getFlagLabels()){
            ConstellationMgr *cmr= (ConstellationMgr*)GETSTELMODULE(ConstellationMgr);
            if (cmr) {cmr->setFlagLabels(cstLbl);}
        }
        
        if (loc!= msapi->getObserverLocation()){
            {msapi-> setObserverLocation(loc, 0);}
        }


    }
    close(sockfd);

}


