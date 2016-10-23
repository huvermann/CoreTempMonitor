#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex>
#include <vector>
using namespace std;

struct CpuStatus {
	string date;
	double temperature;
	double frequency;
	double* cpuLoad;
};

double toDouble(std::string s) {
	std::replace(s.begin(), s.end(), ',', '.');
	return std::atof(s.c_str());
}

string do_console_command_get_result(char* command)
{
	FILE* pipe = popen(command, "r");
	if (!pipe)
		return "ERROR";

	char buffer[128];
	string result = "";
	while (!feof(pipe))
	{
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return(result);
}

double getTemperature(int zone)
{
	char buffer[100];
	sprintf(buffer, "cat /sys/class/thermal/thermal_zone%d/temp", zone);
	string result = do_console_command_get_result(buffer);
	if (result.size() > 0)  result.resize(result.size() - 1);
	return toDouble(result);
}

double getCpuFrequency(int cpu)
{
	char buffer[100];
	sprintf(buffer, "cat /sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_cur_freq", cpu);
	string result = do_console_command_get_result(buffer);

	if (result.size() > 0)  result.resize(result.size() - 1);
	return toDouble(result);
}

string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%X", &tstruct);

	return buf;
}

vector<string> tokenize(const string& str, char delim)
{
	vector<std::string> tokens;
	std::stringstream mySstream(str);
	string temp;

	while (getline(mySstream, temp, delim))
		tokens.push_back(temp);

	return tokens;
}



double *getMpStat() {
	
	double* result = new double[4];
	std::regex regex(R"(\d{2}:\d{2}:\d{2}\s+(\d)\s+\d+[.,]\d{2}\s+\d+[.,]\d{2}\s+\d+[.,]\d{2}\s+\d+[.,]\d{2}\s+\d+[.,]\d{2}\s+\d+[.,]\d{2}\s+\d+[.,]\d{2}\s+\d+[.,]\d{2}\s+\d+[.,]\d{2}\s+(\d+[.,]\d{2}))");

	string s = do_console_command_get_result((char*)"mpstat -P ALL");
	vector<string> lines = tokenize(s, '\n');
	for (size_t i = 0; i < lines.size(); ++i)
	{
		smatch matches;
		if (regex_search(lines[i], matches, regex))
		{
			int cpunum = stoi(matches[1].str());
			if (cpunum >= 0 && cpunum < 4) {
				result[cpunum] = 100 - toDouble(matches[2].str());
			}
		}
	}
	return result;
}

void printHelp() {
	printf("\n\nUsage:\n coretempmon [options]\n\n");
	printf("-h            This help\n");
	printf("-s            Silent output, just data");
	printf("-d <milliseconds>  Monitor delay in seconds (default=5)\n");
}

bool isRoot() {
	bool result = (getuid() == 0);
	return result;
}

void monitoring(unsigned int delay, int loops, bool silent) {
	if (!silent) {
		printf("Core Temperature Monitor 1.0\n");
		printf("Scan delay is %d seconds\n", delay);
		printf("Stop monitoring using [ctrl]-[c]\n");
		printf("Time Temperature Freq_CPU1 CPULoad1  \%CPULoad2 \%CPULoad3 \%CPULoad3\n");
	}

	bool infinite = (loops == -1);
	int counter = loops;
	CpuStatus data;

	while (infinite || counter-- >0)
	{
		usleep(delay * 1000);
		data.date = currentDateTime();
		data.frequency = getCpuFrequency(0);
		data.temperature = getTemperature(0);
		data.cpuLoad = getMpStat();
		printf("%s %0.2f°C %0.0f MHz %0.2f %0.2f %0.2f %0.2f\n", data.date.c_str(), data.temperature, data.frequency, data.cpuLoad[0], data.cpuLoad[1], data.cpuLoad[2], data.cpuLoad[3]);
		delete[] data.cpuLoad;

	}

}


int main(int argc, char *argv[])
{
    
	int c;
	unsigned int delay = 5000;
	int loops = -1;
	bool silent = false;

	while ((c = getopt(argc, argv, "shd:n:")) != -1) {
		switch (c) {

		case 'd': {
			delay = atoi(optarg);
			if (delay < 1) { delay = 5000; }
			break;
		}

		case 'h':
			printHelp();
			delay = -1;
			break;
		case 'n':
			loops = atoi(optarg);
			break;
		case 's':
			silent = true;
			break;


		default:
			abort();
		}
	}

	if (!isRoot())
	{
		printf("You must be root!\n");
	}
	else {
		if (delay > 0) {


			monitoring(delay, loops, silent);
		}
	}

    return 0;
}