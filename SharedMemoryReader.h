#pragma once
#ifndef SHAREMEMORY_H
#define SHAREMEMORY_H

#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>
#include <Windows.h>
#include <thread>
#include <future>
#include "BlockingQueue.h"
#include "Event_Based_Queue.h"
using namespace std;
#define BYTESIZE 120;
#define PADDING 10;

struct Test
{
public:
	long long int id;
	char name[10];
	void clear()
	{
		id = 0;
	}
};

#pragma pack(1)
struct New_Request
{
public:
	char RequestType;
	int64_t ClOrderID;
	int64_t OriginatorClOrderID;
	int64_t OrderQuantity;
	int64_t Price;
	int8_t OrderType;
	int8_t OrderSide;
	int8_t TimeInForce;
	int8_t SHFEComboOffsetFlag;
	int8_t SecurityType;
	char Symbol[10];
	char SecurityId[25];
	char ExchangeOrderId[40];
	std::chrono::steady_clock::time_point stopWatchTime;
	void print()
	{
		std::cout << RequestType << " " << ClOrderID << " " << OriginatorClOrderID << " " << Price << " " << std::to_string(OrderType) << " " << OrderSide << " " << TimeInForce << " " << SHFEComboOffsetFlag << " " << SecurityType << " "
			<< " " << Symbol << " " << SecurityId << " " << ExchangeOrderId << endl;
	}

};


class SharedMemoryReader
{
private:
	int pageCount;
	int pageSize;
	string FileName;
	string mutexName;
	DWORD GranulityViewSize;
	DWORD GranulityLowOffset;
	DWORD GranulityVewDelta;
	DWORD  GranulityAllocationValue;
	DWORD readCount;
	LPVOID readSharedAddress;
	bool print();
	std::unordered_map<int, void*> _dictcollectionOfViews;
	HANDLE namedMutex;
	std::unordered_map<int, DWORD> granularityCollection;
	HANDLE FileHandle;
	SYSTEM_INFO SysInfo;
	void GetAllocationGranulityValue();
	bool OpenSharedMEmoryFile();
	void calculateAllGranulitySize();
	bool OpenExistingNamedMutex();
	void ReadAllMemoryAddress(int, DWORD, DWORD);
	void ReadAllMemoryAddressAndEvent(int, DWORD, DWORD, int);

	void SpawnParralerThread(HANDLE, void*, DWORD, int);

	FIX::Queue<New_Request> _eventQueue;

	BlockingQueue<New_Request> _BackgroundQueue;
	
	

	int localCount = 0;
	int recordCount = 0;

public:
	SharedMemoryReader(string, int, int);
	void ReadDataFromSharedMemory();
	void BackgroundProcess();

};
#endif