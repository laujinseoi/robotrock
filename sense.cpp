#include <csignal>
#include <iostream>

#include "mraa.hpp"

#define echoPin 7 
#define trigPin 8 
#define LEDPin 13 

int running = 1;
void sig_handler(int signo)
{
	if (signo == SIGINT) {
		printf("closing spi nicely\n");
		running = 0;
	}
}

int main() 
{
	signal(SIGINT, sig_handler);
	std::cout<<"Fine till ehre!"<<std::endl;
	mraa::Gpio echo = mraa::Gpio(echoPin);
	mraa::Gpio trig = mraa::Gpio(trigPin);
	echo.dir(mraa::DIR_IN);
	trig.dir(mraa::DIR_OUT);
	int ctr=0;
	int val = echo.read();
	std::cout << "Read: " << val << " "<<(ctr++)<<std::endl;
	sleep(0.01);
	trig.write(0);	
	sleep(0.002);
	trig.write(1);	
	sleep(0.010);
	trig.write(0);	
	sleep(0.002);
	while (running) {
		int val = echo.read();
		std::cout << "Read: " << val << " "<<(ctr++)<<std::endl;
		if(ctr>100) running=0;
		sleep(0.01);
	}
}
