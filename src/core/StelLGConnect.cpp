

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



void UDP_connect::LG_communicate_master(StelCore *core, StelMovementMgr *mmgr, QSettings* conf, StelMainScriptAPI *msapi){
		std::cout<<"in udp function"<< endl;

		bool firstTime= true;
		char buffer[1000];
		//char *message = "Hello Client";
		int listenfd;
		socklen_t len;
		struct sockaddr_in servaddr, cliaddr;
		bzero(&servaddr, sizeof(servaddr));

		// Create a UDP Socket
		listenfd = socket(AF_INET, SOCK_DGRAM, 0);		
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(PORT);
		servaddr.sin_family = AF_INET;

		// bind server address to socket descriptor
		bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
		
		//receive the datagram
		
		while(1){
			//std::cout<<"in while loop"<< endl;
			len = sizeof(cliaddr);
			int n = recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr,&len); //receive message from server
			////puts(buffer);
			if (sizeof(buffer)> 0){
				buffer[n] = '\0';
				//puts(buffer);

				//std::string filename(boost::archive::tmpdir());
					//char filename = "/text.txt";

				std::stringstream ss;
				//std::ofstream ofs("/text.txt");

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
				const QString loc= msapi->getObserverLocation();
				const QString date= msapi->getDate();
				bool JD_changed_signal= core-> JD_changed;
				if (core->JD_changed== true) {
					core-> JD_changed= false;
					//std::cout<<"JD_changed is ture"<< endl;
					}
				if (firstTime== true){
					JD_changed_signal= true;
					firstTime= false;
				}
				//std::cout<<loc.toStdString()<< "loc here......"<<endl<<endl<<endl;
				//cout<<curr[0]<<" "<<curr[1]<<" "<<curr[2]<< endl;
				//const Data g(mmgr-> getViewDirectionJ2000());
				{
					//boost::archive::text_oarchive oa(ss);
					// write class instance to archive
					ss << curr[0]<<"|"<< curr[1]<<"|"<< curr[2]<< "|"<< fov<< "|"<< date.toStdString()<< "|"<< timeRate<< "|"<< atmFlag<< "|"<< lndFlag<< "|"<< crdFlag<< "|"<< cstArt<< "|"<< cstLin<< "|"<< cstLbl<< "|"<< loc.toStdString()<<"|"<<JD_changed_signal;
				//cout<<Jday<<" JD here "<< DeltaT<< endl;
				//cout<<Jday+JTime<<" skytime here "<< endl;

				}

				std::string s= ss.str();
				//std::string str =  ia;
				sendto(listenfd, s.c_str() , MAXLINE, 0,(struct sockaddr*)&cliaddr, sizeof(cliaddr));
				//std::cout<<"sent new pos"<<endl;
			}
		}
	}
void UDP_connect::LG_communicate_slave(StelCore *core, StelMovementMgr *mmgr, QSettings* conf, StelMainScriptAPI *msapi, int unsigned microsecond, QString ip){
	usleep(5 * microsecond);//sleeps for 3 second

    std::cout<<"in udp function"<<endl;
	char* ip_addr= ip.toUtf8().data();
    char buffer[100];
    char *message = "Hello Server";
    int sockfd, n;
    struct sockaddr_in servaddr;
    std::cout<<"in udp function 1"<<endl;
    // clear servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = inet_addr(ip_addr);   //  192.168.43.12 | 10.0.2.18
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;
    std::cout<<"in udp function 2"<<endl;
    // create datagram socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // connect to server
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }
    

    //std::cout<<core->getMouseJ2000Pos()<<"mouse pos"<<endl;
    std::cout<<"in udp function 3"<<endl;


    while(1){

        // request to send datagram
        // no need to specify server address in sendto
        // connect stores the peers IP and port
        sendto(sockfd, message, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr));
        //std::cout<<"in while loop2"<< endl;
        // waiting for response
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
        //std::cout<<sizeof(buffer)<< endl;
        //if (sizeof(buffer)> 5){
        //puts(buffer);
        char str[(sizeof(buffer)) + 1];
            memcpy(str, buffer, sizeof(buffer));
        str[sizeof(buffer)] = 0; // Null termination.
        //std::stringstream ss;
        //ss.str (str);
        

        Vec3d curr= mmgr->getViewDirectionJ2000();

        std::vector<std::string> v;
        split(str, v, '|');
        
        if (v.size()!= 14) continue;
        
        Vec3d pos;
        pos[0]= std::stod(v.at(0));
        pos[1]= std::stod(v.at(1));
        pos[2]= std::stod(v.at(2));
        double fov= std::stod(v.at(3));
        //double skyTime= std::stod(v.at(4));
        QString date= QString::fromStdString(v.at(4));
        double timeRate= std::stod(v.at(5));
        //double Jday= std::stod(v.at(6));
        bool atmFlag= v.at(6)== "1";
        bool lndFlag= v.at(7)== "1";
        bool crdFlag= v.at(8)== "1";
        bool cstArt= v.at(9)== "1";
        bool cstLin= v.at(10)== "1";
        bool cstLbl= v.at(11)== "1";
        QString loc= QString::fromStdString(v.at(12));
	bool JD_changed= v.at(13)== "1";
		//double deltaT= std::stod(v.at(15));

        
        //std::cout<<pos[0]<<pos[1]<<pos[2]<<"new pos arrived"<<endl;
        if (pos[0]!= curr[0] or pos[1]!= curr[1] or pos[2]!= curr[2] ){ mmgr->setViewDirectionJ2000(pos);}
        if (fov!= mmgr->getCurrentFov()){mmgr->zoomTo(fov, 0);}
/*
        if (Jday!= core->getMJDay()){core->setMJDay(Jday);}
*/
/*	if (JD_changed== true){
		//core->setJD(Jday);
		msapi-> setDate(date);
		//core->update(deltaT);
		//core->JD_changed= false;
		cout<<"JD changed"<< endl;
	}*/
		if (JD_changed== true){
			//core->setJD(Jday);
			msapi-> setDate(date);
			//core->update(deltaT);
			//core->JD_changed= false;
			//cout<<"JD changed"<< endl;
		}
        //if (skyTime!= core-> getPresetSkyTime()){core->setPresetSkyTime(skyTime);}
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
            //StelMainScriptAPI *msa= (StelMainScriptAPI*)GETSTELMODULE(StelMainScriptAPI);
            {msapi-> setObserverLocation(loc, 0);}
            //cout<<"loc changed................"<<endl<<endl<<endl;
        }

        //}

    }
    //std::cout<< newg.pos>> newg.aup>>endl;
    close(sockfd);

}


