#ifndef __MYCLASS_H
#define __MYCLASS_H

class Sched {
public:
	string crn;
	string depcode;
	string ccode;
	string ctitle;
	string cdays;
	string cstarttm;
	string cendtm;
	string croom;

void set_values(string, string, string, string, string, string, string, string);
};

void Sched::set_values(string v1, string v2, string v3, string v4, string v5, string v6, string v7, string v8){
	crn = v1;
	depcode = v2;
	ccode = v3;
	ctitle = v4;
	cdays = v5;
	cstarttm = v6;
	cendtm = v7;
	croom = v8;
}

#endif