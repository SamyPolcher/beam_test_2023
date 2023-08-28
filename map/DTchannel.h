#ifndef DTCHANNEL_H
#define DTCHANNEL_H

#include "DetectorTable.h"

// channel refers to real dream channel

class DTchannel : protected DetectorTable
{
public:
	DTchannel() = default;
	~DTchannel() = default;
	DTchannel(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3);

	void setInversion(bool iC0, bool iC1, bool iC2, bool iC3);
	int toGB(int ch);  // convert real channel to gerber channnel

	bool isConnected(int ch);
	bool isNeighbour(int ch1, int ch2);
	bool isEdge(int ch);
	int stripNb(int ch);
	char axis(int ch);

private:
	std::vector<int> dreamConnect;
	std::vector<bool> inv = {false, false, false, false}; 
};

#endif