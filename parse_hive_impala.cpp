//  parse_hive_impala.cpp
//	Simple script to analyze output files from speed tests
//  Takes command line arguments in the following order:
//		<input-file-name>
//		<query engine (hive, impala, shark, etc.)>
//		<query_id>
//
// Input from file with following format must be piped in as input
//		<table size>
//		<result size> <filler>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

string get_file_contents(const char* file_name);
string getLastLine(ifstream& in);
string get_speed_hive(const string & file_contents);
string get_speed_impala(const string & file_contents);
void output_speed_to_file(const string& output_file_name, const string& speed,
	const string &query_engine, const string &query_id);
	
const int num_args = 4;

int main(int argc, char* argv[]){
	if (argc < num_args){
		cerr << "Not enough arguments\n Only supplied " << argc -1
			<< " arguments.\nIn reverse order:\n";
		for (int i = argc - 1; argc > 0; --i)
			cerr << "\t" << argv[i] << "\n";
		exit(1);
	}

	//	Get the input file contents
	string last_line = get_file_contents(argv[1]);

	//	Pass to appropriate parser
	string results;
	string query_type(argv[2]);
	if (query_type == "hive"){
		results = get_speed_hive(last_line);
	}
	else if (query_type == "impala"){
		results = get_speed_impala(last_line);
	}
	else {
		cerr << "Invalid query engine\n";
		exit(1);
	}

	//	Output to file
	output_speed_to_file("benchmarks_results.txt", results, argv[2], argv[3]);
}

string get_file_contents(const char* file_name){
	ifstream in_file;
	in_file.open(file_name);
	if (!in_file.good()){
		cerr << "Input file cannot be read\n";
		exit(2);
	}
	return getLastLine(in_file);
	
}

string getLastLine(ifstream& in)
{
	string line;
	string holder;
	while (getline(in, holder)) 
	{
		line = holder;
	}
	return line;
}

string get_speed_hive(const string & file_contents){
	//	Hive output has form: "Time taken: <time> seconds"
	istringstream input(file_contents);
	string results;
	input >> results >> results >> results;
	return results;
}

string get_speed_impala(const string &file_contents){
	//	Impala output has form: Returned <#> row(s) in <time>s
	istringstream input(file_contents);
	string results;
	input >> results >> results >> results >> results >> results;
	results = results.substr(0, results.size() - 1);
	return results;
}

void output_speed_to_file(const string& file_name, const string& speed, 
			const string &query_engine, const string &query_id){
	ofstream out_file;
	out_file.open(file_name.c_str(), ios::app);
	if (!out_file){
		cerr << "Error with out_file\n";
		exit(2);
	}
	string result_size;
	cin >> benchmark_size;
	cin >> result_size;
	out_file << speed << "," << query_engine << "," << benchmark_size << ","
		<< query_id << "," << result_size << endl;
	out_file.close();
}
