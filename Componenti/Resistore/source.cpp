#include "BaseComp.h"
#include <string>
#include <fstream>

using namespace std;

Resistor::Resistor(PointInt pi, float ohm):Component("./Componenti/Resistore/imgs/resistor.png", pi){
	this->Value=ohm;
	ifstream file=ifstream("./Componenti/Resistore/Resistore.txt");
	
	int w=this->GetWidth();
	int h=this->GetHeight();
	
	while(!file.eof()){
		string tmp;
		getline(file, tmp);
		if(tmp.empty())
			break;
		
		size_t delL=tmp.find(" ");
		size_t delR=tmp.rfind(" ");
		int direzione;
		string x=tmp.substr(0, delL);
		string y=tmp.substr(delL+1, delR);
		string dir=tmp.substr(delR, tmp.size());
		float xf=stof(x);
		float yf=stof(y);
		
		if(dir.find("L") != string::npos){
			direzione=DIR_LEFT;
		}else if(dir.find("R") != string::npos){
			direzione=DIR_RIGHT;
		}else if(dir.find("U") != string::npos){
			direzione=DIR_UP;
		}else if(dir.find("D") != string::npos){
			direzione=DIR_DOWN;
		}
		Hotpoint* hotp=new Hotpoint( (float)pi.x+((float)w*xf), (float)pi.y+((float)h*yf ), HOTPOINT_EDGE, direzione);
		this->hotpoints.push_back( hotp );
		cairo_region_union(this->regionBound, hotp->GetRegion() );
	}
	file.close();
}

