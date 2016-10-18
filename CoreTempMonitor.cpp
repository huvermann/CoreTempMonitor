#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

string do_console_command_get_result (char* command)
{
	FILE* pipe = popen(command, "r");		//Send the command, popen exits immediately
	if (!pipe)
		return "ERROR";
	
	char buffer[128];
	string result = "";
	while(!feof(pipe))						//Wait for the output resulting from the command
	{
		if(fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return(result);
}

string getTemperature(int zone)
{
  char buffer[100];
  sprintf(buffer, "cat /sys/class/thermal/thermal_zone%d/temp", zone);
  string result = do_console_command_get_result(buffer);
  // Remove last char
  if (result.size () > 0)  result.resize (result.size () - 1);
  return (result);
}

string getCpuFrequency(int cpu)
{
	char buffer[100];
	sprintf(buffer, "cat /sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_cur_freq", cpu);
	string result = do_console_command_get_result(buffer);

	if (result.size() > 0)  result.resize(result.size() - 1);
	return (result);
}

string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%X", &tstruct);

	return buf;
}

void printHelp() {
	printf("\n\nUsage:\n coretempmon [options]\n\n");
	printf("-h            This help\n");
	printf("-s            Silent output, just data");
	printf("-d <seconds>  Monitor delay in seconds (default=5)\n");
}

bool isRoot() {
	bool result = (getuid() == 0);
	return result;
}

void monitoring(unsigned int delay, int loops, bool silent){
	if (!silent) {
		printf("Core Temperature Monitor 1.0\n");
		printf("Scan delay is %d seconds\n", delay);
		printf("Stop monitoring using [ctrl]-[c]\n");
		printf("Time Temperature Freq_CPU1 Freq_CPU1 Freq_CPU1 Freq_CPU1\n");
	}
	
	bool infinite = (loops == -1);
	int counter = loops;

	while (infinite || counter-- >0)
	{
		//sleep(delay);
		usleep(delay*1000);
		string temp0 = getTemperature(0);
		string temp1 = getTemperature(1);
		string cpufreq0 = getCpuFrequency(0);
		string cpufreq1 = getCpuFrequency(1);
		string cpufreq2 = getCpuFrequency(2);
		string cpufreq3 = getCpuFrequency(3);
		string curTime = currentDateTime();

		printf("%s %s°C %s°C %s MHz %s MHz %s MHz %s MHz\n", curTime.c_str(), temp0.c_str(), temp1.c_str(), cpufreq0.c_str(), cpufreq1.c_str(), cpufreq2.c_str(), cpufreq3.c_str());
	}

}


main(int argc, char *argv[]) {
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

	/*if (getuid())*/
	if (!isRoot())
	{
		printf("You must be root!\n");
	}
	else {
		if (delay > 0) {


			monitoring(delay, loops, silent);
		}
	}
}

