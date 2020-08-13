#include "SharedMemoryReader.h"

TCHAR szMsg[] = TEXT("Message from first process.");
SharedMemoryReader::SharedMemoryReader(string name, int pageCount, int pageSize)
{
	readCount = 0;
	this->FileName = name;
	this->pageCount = pageSize / PADDING;
	this->pageSize = pageSize;
	this->mutexName = this->FileName + "mutex";

	thread spawn_thread1(&SharedMemoryReader::BackgroundProcess, this);
	spawn_thread1.detach();

	GetAllocationGranulityValue();

	/*calculateAllGranulitySize();*/

	OpenSharedMEmoryFile();
	//OpenExistingNamedMutex();
	std::cout << "Shared file opend and mutex created" << std::endl;
	/*Sleep(5000);*/
	//ReadDataFromSharedMemory();
	calculateAllGranulitySize();
}

bool SharedMemoryReader::print()
{
	return true;
}

void SharedMemoryReader::GetAllocationGranulityValue()
{
	GetSystemInfo(&SysInfo);
	GranulityAllocationValue = SysInfo.dwAllocationGranularity;
}

bool SharedMemoryReader::OpenSharedMEmoryFile()
{
	std::wstring sopeFileName = std::wstring(FileName.begin(), FileName.end());
	LPCWSTR shFileName = sopeFileName.c_str();

	FileHandle = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		shFileName);
	if (FileHandle == NULL)
	{
		std::cout << "Shared file name is not found" << std::endl;
		return false;
	}
	return true;
}

void SharedMemoryReader::calculateAllGranulitySize()
{
	int readCount = 0;
	for (int i = 0; i < 10; i++)
	{
		vector<int> collectionOfvector;

		GranulityLowOffset = (readCount / GranulityAllocationValue) * GranulityAllocationValue;
		//	collectionOfvector.push_back(GranulityLowOffset);

		GranulityViewSize = (readCount % GranulityAllocationValue) + BYTESIZE;
		//	collectionOfvector.push_back(GranulityViewSize);

		GranulityVewDelta = readCount - GranulityLowOffset;
		//collectionOfvector.push_back(GranulityVewDelta);

		granularityCollection[readCount] = GranulityVewDelta/* collectionOfvector*/;

		ReadAllMemoryAddress(readCount, GranulityLowOffset, GranulityViewSize);
		ReadAllMemoryAddressAndEvent(readCount, GranulityLowOffset, GranulityViewSize, i);
		readCount += BYTESIZE;
	}
	int jamCount = 0;
	for (auto done : _dictcollectionOfViews)
	{

		auto bhai = (char*)_dictcollectionOfViews[readCount] + granularityCollection[readCount];
		jamCount += BYTESIZE;
	}
	/*for (auto result : granularityCollection)
	{

		std::cout << result.first << " ";
		for (auto vectorResult : result.second)
		{
			std::cout << vectorResult << std::endl;
		}
		cout << endl;

	}*/
	/*Sleep(5000);*/


	//GranulityLowOffset = (readCount / GranulityAllocationValue) * GranulityAllocationValue;
	////std::cout << "The file map view starts at %ld bytes into the file.\n" << GranulityLowOffset << endl;


	//GranulityViewSize = (readCount % GranulityAllocationValue) + BYTESIZE;
	////cout << "The file map view is %ld bytes large.\n" << GranulityViewSize << endl;

	//GranulityVewDelta = readCount - GranulityLowOffset;
	//cout << "The data is  bytes into the view.\n" << GranulityVewDelta << endl;
}

bool SharedMemoryReader::OpenExistingNamedMutex()
{





	std::wstring scopeNamedMutex = std::wstring(mutexName.begin(), mutexName.end());
	LPCWSTR shMutex = scopeNamedMutex.c_str();

	namedMutex = OpenMutex(
		MUTEX_ALL_ACCESS,
		FALSE,
		shMutex
	);
	if (namedMutex == NULL)
	{
		std::cout << "Named mutex is not exist" << std::endl;
		return false;
	}
	return true;
}

void SharedMemoryReader::ReadAllMemoryAddress(int readCount, DWORD lowOffset, DWORD ViewSize)
{


	_dictcollectionOfViews[readCount] = (void*)MapViewOfFile(FileHandle,
		FILE_MAP_ALL_ACCESS,
		0,
		lowOffset,
		ViewSize);
	std::cout << "Address entry for " << readCount << " lowest " << lowOffset << " ViewSize " << ViewSize << endl;

}

void SharedMemoryReader::ReadAllMemoryAddressAndEvent(int readCount, DWORD lowOffset, DWORD ViewSize, int index)
{
	std::string newFileName = FileName + "mutex" + std::to_string(index);

	std::wstring sopeFileName = std::wstring(newFileName.begin(), newFileName.end());
	LPCWSTR shFileName = sopeFileName.c_str();
	auto open_event = OpenEvent(EVENT_ALL_ACCESS, TRUE, shFileName);

	_dictcollectionOfViews[readCount] = (void*)MapViewOfFile(FileHandle,
		FILE_MAP_ALL_ACCESS,
		0,
		lowOffset,
		ViewSize);
	thread spawn_thread(&SharedMemoryReader::SpawnParralerThread, this, open_event, _dictcollectionOfViews[readCount], granularityCollection[readCount], readCount);
	spawn_thread.detach();

	//auto data1 = std::async(std::launch::async,  &SharedMemoryReader::SpawnParralerThread, this, open_event, _dictcollectionOfViews[readCount], granularityCollection[readCount], readCount);


	std::cout << "Address entry for " << readCount << " lowest " << lowOffset << " ViewSize " << ViewSize << " thread id " << endl;


}

void SharedMemoryReader::SpawnParralerThread(HANDLE openEvent, void* address, DWORD vieDelta, int readCount)
{

	while (1)
	{
		WaitForSingleObject(
			openEvent,
			INFINITE);
		auto startReading = std::chrono::high_resolution_clock::now();
		auto output = (char*)address + vieDelta;
		auto iData = (New_Request*)output;
		auto stopReading = iData->stopWatchTime = std::chrono::high_resolution_clock::now();



		_eventQueue.push(*iData);
		//	_BackgroundQueue.Push(*iData);
		auto stopQueueing = std::chrono::high_resolution_clock::now();
		//auto queueFinish = std::chrono::high_resolution_clock::now();
		auto elapsedQueueing = (stopQueueing.time_since_epoch().count() - stopReading.time_since_epoch().count()) * 0.001;
		auto ReadingElapsed = (stopReading.time_since_epoch().count() - iData->ClOrderID) * 0.001;

		std::cout << " Queue " << elapsedQueueing << std::endl;

		/*recordCount += 1;
		if (ReadingElapsed < 30)
			localCount += 1;
		if (recordCount == 1000)
		{
			std::cout << localCount << " " << ReadingElapsed << std::endl;

			break;
		}*/


		///*iData->print();*/

		////auto elapsed = startReading.time_since_epoch().count() - iData->SecurityId;
		//cout << readCount << " queuing elapsed " << ReadingElapsed * 0.001 <<std::endl;
		////cout << iData->ClOrderID << " " << iData->RequestType << endl;
	}

}


void SharedMemoryReader::ReadDataFromSharedMemory()
{
	auto start_calculation = chrono::high_resolution_clock::now();
	calculateAllGranulitySize();
	auto stop_Calculation = chrono::high_resolution_clock::now();
	// std::cout << to_string((stop_Calculation.time_since_epoch().count() - start_calculation.time_since_epoch().count())) << endl;

	int dataCount = 0;



	//readSharedAddress = (void*)MapViewOfFile(FileHandle,
	//	FILE_MAP_ALL_ACCESS,
	//	0,
	//	GranulityLowOffset,
	//	GranulityViewSize);
	for (;;)
	{
		WaitForSingleObject(
			namedMutex,    // handle to mutex
			INFINITE);



		//	if (readSharedAddress != NULL)
		{
			auto start_reading = chrono::high_resolution_clock::now();
			auto output = (char*)_dictcollectionOfViews[dataCount] + granularityCollection[dataCount]/*GranulityVewDelta*/;

			/*auto output_new = _collectionOfViews[]*/
			/*auto iData = (Test*)output;
			std::cout << "Old " << iData->id << std::endl;*/
			//	auto time = *(long long int*)output;

			auto iData = (Test*)output;


			/*CopyMemory((PVOID)_dictcollectionOfViews[dataCount], szMsg,  (28 * sizeof(TCHAR)));*/


			//auto stop_reading = chrono::high_resolution_clock::now();
			//auto elasped = ((stop_reading.time_since_epoch().count() - time) - 8) * 0.001;
			//std::cout << "Old " << time << " elpased " << elasped << std::endl;

			/*std::cout << "Calculation : " << elasped << " rcv time " << time << " reading time " << (stop_reading.time_since_epoch().count() - start_reading.time_since_epoch().count()) * 0.001 << " " << (((stop_reading.time_since_epoch().count() - time) * 0.001)) << endl;*/

			//readCount = readCount + BYTESIZE;
			//if (output[0] != '0' && time != 0)
			{
				Test test;
				test.id = 0;


				//const char check[10] = "0";
				/*	auto ReplaceStart = std::chrono::high_resolution_clock::now();*/
					// CopyMemory((PVOID)output, &test, (sizeof(Test)));
					/*auto ReplaceStop = std::chrono::high_resolution_clock::now();*/
				//CopyMemory((PVOID)output, &check, 10 * (sizeof(char)));

				auto time1 = *(long long int*)output;

				auto iData1 = (Test*)output;

				//std::cout << " new " << output << " " << time << endl;
				std::cout << "Old " << iData->id << " New " << iData1->id << endl;
				//const char* data = "N";
				//CopyMemory((PVOID)_dictcollectionOfViews[dataCount], data, (2*sizeof(char)));
				////memcpy((PVOID)_dictcollectionOfViews[dataCount], data, (sizeof(char)));
				//std::cout << output <<" "<<time <<" count "<<dataCount<<" again "<< *(long long int*)output << std::endl;
				dataCount = dataCount + BYTESIZE;

			}
			if (dataCount == 1000)
			{
				dataCount = 0;
				std::cout << "Start from Zero" << std::endl;
			}
			Sleep(1000);

			/*CreateEventW*/
			ReleaseMutex(namedMutex);
		}
	}
}

void SharedMemoryReader::BackgroundProcess()
{

	for (;;)
	{
		New_Request request;
		if (_eventQueue.size() > 0 && _eventQueue.pop(request))
		{
			request.print();
			auto ReadingElapsed = (request.stopWatchTime.time_since_epoch().count() - request.ClOrderID) * 0.001;
			cout << readCount << " reading elapsed " << ReadingElapsed << std::endl;


		}
		else
			_eventQueue.wait(INFINITE);

	}

}
