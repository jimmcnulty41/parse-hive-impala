//  parse_hive_impala.cpp
//	Simple script to analyze output files from speed tests
//  Takes command line arguments in the following order:
//		<input-file-name>
//		<query engine (hive, impala, shark, etc.)>
//		<query_id>
//		<table_size>
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
#include <cstring>

using namespace std;

string get_file_contents(const char* file_name);
string getLastLine(ifstream& in);
string get_speed_hive(const string & file_contents);
string get_speed_impala(const string & file_contents);
string get_size(const string & file_contents);
void output_speed_to_file(const string& output_file_name, const string& speed,
	const string &query_engine, const string &query_id, const string & size);
	
const int num_args = 4;

int main(int argc, char* argv[]){
	if (argc == 2){
		if (strcmp("-h",argv[1])){
			cout << "Takes command line arguments in the following order:"
				<< "\t<input-file-name>"
				<< "\t<query engine (hive, impala, shark, etc.)>" 
				<< "\t<query_id>"
				<< "\t<table_size>";
		}	
		
	}
	if (argc < num_args + 1){
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
	
	//	Get the size from the file specified
	last_line = get_file_contents(argv[4]);
	string table_size = get_size(last_line);

	//	Output to file
	output_speed_to_file("benchmarks_results.txt", results, argv[2], argv[3], table_size);
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

string get_size(const string &file_contents){
	istringstream input(file_contents);
	string results1;
	input >> results1;
	string results2;
	input >> results2;
	return results1 + results2;
}

void output_speed_to_file(const string& file_name, const string& speed, 
			const string &query_engine, const string &query_id,
			const string & table_size){
	ofstream out_file;
	out_file.open(file_name.c_str(), ios::app);
	if (!out_file){
		cerr << "Error with out_file\n";
		exit(2);
	}
	string result_size, benchmark_size;
	cin >> benchmark_size;
	cin >> result_size;
	out_file << table_size << speed << "," << query_engine << "," << benchmark_size << ","
		<< query_id << "," << result_size << endl;
	out_file.close();
}
