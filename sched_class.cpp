#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <vector>
#include <algorithm>

using namespace std;

#include "sched.h"

/*
this function will return a numerical sorted value 
for a weekday string
example: monday = 1, tuesday = 2
*/
int getDayValue(const string &wkds){
	if (wkds == "Monday")
		return 1;
	else if (wkds == "Tuesday")
		return 2;
	else if (wkds == "Wednesday")
		return 3;
	else if (wkds == "Thursday")
		return 4;
	else if (wkds == "Friday")
		return 5;
	else if (wkds == "Saturday")
		return 6;
	else if (wkds == "Sunday")
		return 7;

	return 99;
}

/*
this function will return a numeric value
that can be used for time comparison
the input time string should be of the format
hh:mm<am/pm>
example: 09:15AM will return 915
07:30PM will return 1930
12:00PM will return 1200
*/
int getTimeValue(const string &ts){

	// get the first two character for hh
	// get the fourth and fifth character for mm
	// get 6th and 7th character for am/pm
	// convert hh to integer
	// if pm add 12 to integer hh
	// multiply hh by 100 and add mm

	int sttm;

	string hh, mm, ampm;
	hh = ts.substr(0,2);
	mm = ts.substr(3,2);
	ampm = ts.substr(5,2);
	sttm = stoi(hh);
	if ((ampm == "PM") && (stoi(hh) < 12))
		sttm += 12;
	sttm = sttm*100 + stoi(mm);
	return sttm;
}

/*
this function is used for custom sorting for the room report
this function will be called from the stable_sort library as the third argument
sorting is done first by the room, then day, then start time, then course code
if the sort condition is satisfied, the function returns true
else returns false
*/

bool sort_room(Sched s1, Sched s2){

	int wday1;
	int wday2;
	wday1 = getDayValue(s1.cdays);
	wday2 = getDayValue(s2.cdays);

	int sttm1;
	int sttm2;
	sttm1 = getTimeValue(s1.cstarttm);
	sttm2 = getTimeValue(s2.cstarttm);

	// compare by room first
	if (s1.croom < s2.croom)
		return true;
	// then compare by day
	if ((s1.croom == s2.croom) && (wday1 < wday2))
		return true;

	// then compare by start time
	if ((s1.croom == s2.croom) && (wday1 == wday2) && (sttm1 < sttm2)) 
		return true;

	// then compare by course code
	if ((s1.croom == s2.croom) && (wday1 == wday2) && (sttm1 == sttm2) && (s1.ccode < s2.ccode)) 
		return true;

	// if all comparisons fail return false
	return false;
}

/*
this function is used for custom sorting for the department report
this function will be called from the stable_sort library as the third argument
sorting is done first by the course code, then day, then start time
if the sort condition is satisfied, the function returns true
else returns false
*/


bool sort_dept(Sched s1, Sched s2){

	int wday1;
	int wday2;
	wday1 = getDayValue(s1.cdays);
	wday2 = getDayValue(s2.cdays);

	int sttm1;
	int sttm2;
	sttm1 = getTimeValue(s1.cstarttm);
	sttm2 = getTimeValue(s2.cstarttm);

	if (s1.ccode < s2. ccode)
		return true;

	if ((s1.ccode == s2.ccode) && (wday1 < wday2))
		return true;

	if ((s1.ccode == s2.ccode) && (wday1 == wday2) && (sttm1 < sttm2))
		return true;

	return false;
}

/*
this function is used for custom sorting for the week days
this function will be called from the stable_sort library as the third argument
sorting is done first by monday < tuesday < wednesday < thursday < friday
if the sort condition is satisfied, the function returns true
else returns false
*/


bool sort_wkday(string s1, string s2){
	if (s1 == "Monday")
		return true;
	if (s1 == "Tuesday" && s2 != "Monday" && s2 != "Tuesday")
		return true;
	if (s1 == "Wednesday" && s2 != "Monday" && s2 != "Tuesday" && s2 != "Wednesday")
		return true;
	if (s1 == "Thursday" && s2 != "Monday" && s2 != "Tuesday" && s2 != "Wednesday" && s2 != "Thursday")	
		return true;
	if (s1 == "Friday" && s2 != "Monday" && s2 != "Tuesday" && s2 != "Wednesday" && s2 != "Thursday" && s2 != "Friday")
		return true;
	return false;
}

/*
this function will return the column lengths required for reporting 
col1 = depcode length, col2 = course code length, col3 = title length, col4 = day length
if useFilter is true the lengths are calculated for the records that matches the room provided in the input
else the lengths are calculated for all the records
*/

void getMaxLengths(const vector <Sched> &data, const string &rname, bool useFilter, unsigned int &col1, unsigned int &col2, unsigned int &col3, unsigned int &col4){
	for (unsigned int j = 0; j < data.size(); j++){

		if ((data[j].depcode.length() > col1) && ((data[j].croom == rname && useFilter) || (!useFilter)))
			col1 = data[j].depcode.length();

		if ((data[j].ccode.length() > col2) && ((data[j].croom == rname && useFilter) || (!useFilter)))
			col2 = data[j].ccode.length();

		if ((data[j].ctitle.length() > col3) && ((data[j].croom == rname && useFilter) || (!useFilter)))
			col3 = data[j].ctitle.length();

		if ((data[j].cdays.length() > col4) && ((data[j].croom == rname && useFilter) || (!useFilter)))
			col4 = data[j].cdays.length();
	}
}

/*
this function will take a vector <Sched> &data and output the room report to a file
*/

void rwrite_out(vector <Sched> &data, string &filename){
	//if no data found, write error message and close file
	ofstream f;
	if (data.size() == 0){
		f.open(filename.c_str());
		f << "No data available.";
		f.close();
		return;
	}
	vector <string> room_name;
	vector <int> room_sum;
	string temp_name;

	//calculate the name and record count for each room
	for (unsigned int i=0; i < data.size(); i++){
		if (i == 0){
			room_name.push_back(data[i].croom);
			room_sum.push_back(1);
			temp_name = data[i].croom;
		}
		else {
			if (temp_name == data[i].croom)
				room_sum.back()++;
			else{
				room_name.push_back(data[i].croom);
				temp_name = data[i].croom;
				room_sum.push_back(1);
			}
		}

	}
	
	//open the file - for each room: write the name of the room 
	//followed by report column headers
	//the values of department, course number, class title, day, start time, end time
	//finally write the number of records for that room
	f.open(filename.c_str());
	for (unsigned int i = 0; i < room_name.size(); i++){

		f << "Room " << room_name[i] << endl;

		unsigned int col1 = 4, col2 = 9, col3 = 11, col4 = 3, col5 = 10, col6 = 8;
		bool useRoomFilter = true;
		getMaxLengths(data, room_name[i], useRoomFilter, col1, col2, col3, col4);

		ostringstream RptCols;
		RptCols << left << setw(col1) << "Dept"         << "  ";
        RptCols << left << setw(col2) << "Coursenum"     << "  ";
        RptCols << left << setw(col3) << "Class Title" << "  ";
        RptCols << left << setw(col4) << "Day"         << "  ";
        RptCols << left << setw(col5) << "Start Time"    << "  ";
        RptCols << left << setw(col6) << "End Time";
        RptCols << "\n";

        RptCols << left << setw(col1) << string(col1, '-') << "  ";
        RptCols << left << setw(col2) << string(col2, '-') << "  ";
        RptCols << left << setw(col3) << string(col3, '-') << "  ";
        RptCols << left << setw(col4) << string(col4, '-') << "  ";
        RptCols << left << setw(col5) << string(col5, '-') << "  ";
        RptCols << left << setw(col6) << string(col6, '-');
        RptCols << "\n";

        f << RptCols.str();

        for (unsigned int j = 0; j < data.size(); j++){
           	if (data[j].croom == room_name[i]){
           		f << left << setw(col1) << data[j].depcode << "  ";
           		f << left << setw(col2) << data[j].ccode << "  ";
           		f << left << setw(col3) << data[j].ctitle << "  ";
           		f << left << setw(col4) << data[j].cdays << "  ";
           		f << left << setw(col5) << data[j].cstarttm << "  ";
           		f << left << setw(col6) << data[j].cendtm << endl;
           	}
        }
        f << room_sum[i] << " entries" << endl << endl;
	}
}


/*
this function will take a vector <Sched> &data and output the department report to a file
*/
void dwrite_out(vector <Sched> &data, string &filename){
	//if no data found, write error message and close file
	ofstream f;
	if (data.size() == 0){
		f.open(filename.c_str());
		f << "No data available.";
		f.close();
		return;
	}

	string dep_name;
	vector <int> room_sum;
	string temp_name;

	//calculate the number of records for the given department
	for (unsigned int i=0; i < data.size(); i++){
		if (i == 0){
			room_sum.push_back(1);
			dep_name = data[i].depcode;
		}
		else
			room_sum.back()++;
	}

	//open the file - write the department name
	//followed by report column headers
	//the values of course number, class title, day, start time, end time
	//finally write the number of records for the department
	f.open(filename.c_str());
	for (unsigned int i = 0; i < room_sum.size(); i++){

		f << "Dept " << dep_name << endl;

		unsigned int col1 = 4, col2 = 9, col3 = 11, col4 = 3, col5 = 10, col6 = 8;
		bool useRoomFilter = false;
		getMaxLengths(data, dep_name, useRoomFilter, col1, col2, col3, col4);

		ostringstream RptCols;
        RptCols << left << setw(col2) << "Coursenum"     << "  ";
        RptCols << left << setw(col3) << "Class Title" << "  ";
        RptCols << left << setw(col4) << "Day"         << "  ";
        RptCols << left << setw(col5) << "Start Time"    << "  ";
        RptCols << left << setw(col6) << "End Time";
        RptCols << "\n";

        RptCols << left << setw(col2) << string(col2, '-') << "  ";
        RptCols << left << setw(col3) << string(col3, '-') << "  ";
        RptCols << left << setw(col4) << string(col4, '-') << "  ";
        RptCols << left << setw(col5) << string(col5, '-') << "  ";
        RptCols << left << setw(col6) << string(col6, '-');
        RptCols << "\n";

        f << RptCols.str();

        for (unsigned int j = 0; j < data.size(); j++){
           	f << left << setw(col2) << data[j].ccode << "  ";
           	f << left << setw(col3) << data[j].ctitle << "  ";
          	f << left << setw(col4) << data[j].cdays << "  ";
        	f << left << setw(col5) << data[j].cstarttm << "  ";
           	f << left << setw(col6) << data[j].cendtm << endl;
           	}

        f << room_sum[i] << " entries" << endl << endl;
       	}
}

/*
this function will take a vector <Sched> &data and output the custom report
*/
void custom_rep (vector <Sched> &data, string &filename){
	vector <string> wkdays;

	//get the list of unique week days and write to vector wkdays
	for (unsigned int i=0; i < data.size(); i++){
		if (i == 0)
			wkdays.push_back(data[i].cdays);
		else{
			bool lfound = false;
			for (unsigned int j = 0; j < wkdays.size(); j++){
				if (data[i].cdays == wkdays[j]){
					lfound = true;
					break;
				}
			}
			if (!lfound)
				wkdays.push_back(data[i].cdays);
		}
	}
	stable_sort(wkdays.begin(), wkdays.end(), sort_wkday);	//sort to week days

	vector <string> depts;

	//get a list of unique departments and store it in vector depts
	for (unsigned int i=0; i < data.size(); i++){
		if (i == 0)
			depts.push_back(data[i].depcode);
		else{
			bool lfound = false;
			for (unsigned int j = 0; j < depts.size(); j++){
				if (data[i].depcode == depts[j]){
					lfound = true;
					break;
				}
			}
			if (!lfound)
				depts.push_back(data[i].depcode);
		}
	}
	stable_sort(depts.begin(), depts.end());	//sort departments

	vector <int> v_dwsum;
	vector <vector <int> > v_v_dwsum;

	//for each department and week day calculate the record count and store it in a vector of vectors <int>
	for (unsigned int i=0; i < depts.size(); i++){
		for (unsigned int j=0; j < wkdays.size(); j++){
			int dwsum = 0;
			for (unsigned int k=0; k < data.size(); k++){
				if (data[k].depcode == depts[i] && data[k].cdays == wkdays[j])
					dwsum++;
			}
			v_dwsum.push_back(dwsum);
		}
		v_v_dwsum.push_back(v_dwsum);
		v_dwsum.clear();
	}

	//write custom report
	//if no data found, write error message and close file
	ofstream f;
	if (data.size() == 0){
		f.open(filename.c_str());
		f << "No data available.";
		f.close();
		return;
	}

	//open file and write out report title followed by 
	//the report headers of department and weekdays
	f.open(filename.c_str());
	f << left << setw(90) << "Custom Report: Count of Class Sections by Department & Week day. kasinv, Vishaal Kasinath" << endl << endl;
	f << left << string(92, '-') << endl;
	f << left << setw(20) << "Department" << "  ";
	for (unsigned int i = 0; i < wkdays.size(); i++)
		f << right << setw(10) << wkdays[i] << "  ";
	f << right << setw(10) << "Total" << "  " << endl;
	f << left << string(92, '-') << endl;

	//for each department write the counts for each weekday and the total for the entire week
	for (unsigned int i = 0; i < depts.size(); i++){

		f << left << setw(20) << depts[i] << "  ";

		int row_total = 0;

		for (unsigned int j = 0; j < wkdays.size(); j++){
			f << right << setw(10) << v_v_dwsum[i][j] << "  ";
			row_total += v_v_dwsum[i][j];
		}
		f << right << setw(10) << row_total << "  " << endl;
    }
    f << left << string(92, '-') << endl;
    f << left << setw(20) << "Grand Total" << "  ";

    //for all the departments, write out the total for each weekday and a final grand total
    int gtotal = 0;
    for (unsigned int j = 0; j < wkdays.size(); j++){
		int col_total = 0;
		for (unsigned int i = 0; i < depts.size(); i++){
			gtotal += v_v_dwsum[i][j];
			col_total += v_v_dwsum[i][j];
		}
		f << right << setw(10) << col_total << "  ";
	}
	f << right << setw(10) << gtotal << "  " << endl;

	f << left << string(92, '-') << endl;
}



/* Main Function */
int main( int argc, char* argv[]) {

	//First check for valid number of arguments
	//We should have a minmum of 4 (sourcefilename + 3 others... depending on operation)
	//and a maximum of 5 (sourcefilename + 4 others... depending on operation)

	if (argc < 4) {
		cerr << "not enough input arguments";
		return 1;
	}

	else if (argc > 5) {
		cerr << "too many input arguments";
		return 2;
	}


	// At this point, we should have either 3 or 4 arguments, other than the source filename
	// The first argument is the input file name
	// check if the file exists. if file does not exist, throw an error and stop
	string InFileName;
	InFileName = argv[1];
	ifstream f(InFileName.c_str());
	if (!f.good()){
		cerr << "error opening input file " + InFileName;
		return 3;
	}

	// the second argument is the output file. we have to create the file
	// at this time, not really much error checking to do for the second argument
	string OutFileName = argv[2];


	// the third argument should be one of the following
	// room, department, or custom
	// if the third argument is not one of these 3, throw an error
	const string C_ROOM = "room";
	const string C_DEPT = "dept";
	const string C_CUSTOM = "custom";

	string operation;
	operation = argv[3];

	if ( (operation != C_ROOM) && 
		 (operation != C_DEPT) && 
		 (operation != C_CUSTOM)){
		cerr << "invalid file operation: " + operation;

		return 4;
	}

	// if here, we have a valid operation to perform on the input file
	// for department need the department name (argument 4)
	// if we do not have the 4th argument, error out.
	string filt_dept;
	string filt_room;

	if (operation == C_DEPT){
		if (argc < 5){
			cerr << "need department name for operation " + operation;
			return 5;
			}
		else
			filt_dept = argv[4];

	if (operation == C_ROOM && argc >= 5)
		filt_room = argv[4];

	}

	// if we are here, we have a valid file, operation and the arguments required
	// Read the file and push_back each line into out vector string variable = data
	//sort transformed data for department output, write department output
	//sort transformed data for room output, write room output
	//use transformed data for custom output
	vector <Sched> raw_data;
	Sched t, t1;

	//open file and read one line at a time, parse values into temporary class variable
	while (f >> t.crn >> t.depcode >> t.ccode >> t.ctitle >> t.cdays >> t.cstarttm >> t.cendtm >> t.croom)
		raw_data.push_back(t);

	string day_desc, day_abbr;
	vector <Sched> trans_data;

	//deconstruct the abbreviated days of the week into individual records 
	//for example a record with days = MWF 
	//will now become three records one with day Monday, another record with day Wednesday, and another record with day Friday
	for (unsigned int i=0; i < raw_data.size(); i++){
		t = raw_data[i];
		day_abbr = t.cdays;
		for (unsigned int j=0; j < day_abbr.length(); j++){
			if (day_abbr[j] == 'M')
				day_desc = "Monday";
			else if (day_abbr[j] == 'T')
				day_desc = "Tuesday";
			else if (day_abbr[j] == 'W')
				day_desc = "Wednesday";
			else if (day_abbr[j] == 'R')
				day_desc = "Thursday";
			else if (day_abbr[j] == 'F')
				day_desc = "Friday";
			else if (day_abbr[j] == 'S')
				day_desc = "Saturday";
			else if (day_abbr[j] == 'U')
				day_desc = "Sunday";
			t.cdays = day_desc;
			trans_data.push_back(t);
		}

	}


	vector <Sched> filt_data;
	if (operation == C_DEPT){	//if department report, filter for the given department and store final data in filt_data
		for (unsigned int i=0; i < trans_data.size(); i++){
			if (trans_data[i].depcode == filt_dept)
				filt_data.push_back(trans_data[i]);
		}
		stable_sort(filt_data.begin(), filt_data.end(), sort_dept);	//filter based on the criteria provided
			dwrite_out(filt_data, OutFileName);	//call function to write department report
	}

	else if (operation == C_ROOM){	//if room report
		if (filt_room.length() > 0){	//if we have a room filter, filter the data into filt_data, else move all the data into filt_data
			for (unsigned int i=0; i < trans_data.size(); i++){
				if (trans_data[i].croom == filt_room)
					filt_data.push_back(trans_data[i]);
			}
		}
		else
			filt_data = trans_data;
		
		stable_sort(filt_data.begin(), filt_data.end(), sort_room);	//filter based on the criteria provided
		rwrite_out(filt_data, OutFileName);	//call function to write room report
	}
	else if (operation == C_CUSTOM){	//if custom report, move all data into filt_data
		filt_data = trans_data;
		custom_rep(filt_data, OutFileName);
	}
	return 0;
}