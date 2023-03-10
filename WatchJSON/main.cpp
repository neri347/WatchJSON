#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include <windows.h>
using namespace std;
using namespace rapidjson;

const size_t bufferSize = 65536;

char* SaveFile(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	char* readBuffer = new char[bufferSize];
	FileReadStream readStream(fp, readBuffer, bufferSize);
	Document d;
	d.ParseStream(readStream);
	fclose(fp);
	return readBuffer;
}

void PrintFile(char* readBuffer)
{
	for (int i = 0; i < bufferSize; ++i)
	{
		if (readBuffer[i] == NULL)
			break;
		cout << readBuffer[i];
	}
	cout << endl;
}

int main()
{
	// setting.ini ���� �б� - ���丮 �̸� ��������
	std::fstream file;
	const std::wstring filename = L"setting.ini";
	file.open(filename);
	if (file.fail())
	{
		cout << "setting.ini ������ �������� �ʽ��ϴ�!" << endl;
		return 0;
	}

	// ���丮 �̸� ��������
	std::smatch match;
	std::regex re("FILENAME=(.*?);.*");
	std::string line;
	while (!file.eof())
	{
		std::getline(file, line);
		if (std::regex_match(line, match, re))
		{
			break;
		}
	}
	string dirname = match[1];

	// �ش� ���丮�� �ִ� json ���ϸ�� ���
	vector<filesystem::path> filenameVec;
	for (auto& f : filesystem::directory_iterator(filesystem::current_path() / dirname))
	{
		if (f.path().extension() == ".json")
		{
			filenameVec.push_back(f.path().filename());
			cout << f.path().filename() << endl;
		}
	}

	// json ���� �� Ư�� ������ ����
	string filenameInput;
	cout << "����� ������ �̸��� �Է����ּ���(Ȯ���� �����ϰ� �Է�): ";
	cin >> filenameInput;
	filenameInput = "JSON\\" + filenameInput + ".json";
	const char* jsonfilename = filenameInput.c_str();

	// ���� ���� ����
	char* readBufferLegacy = new char[bufferSize];
	readBufferLegacy = SaveFile(jsonfilename);

	// ���� ������ ���			
	PrintFile(readBufferLegacy);

	// ���� ������ ������ ���, �ڵ����� �����ؼ� �˷��ֱ� ("cat.json ������ �����Ǿ����ϴ�. �����̽��� ������ �ٽ� �ε��մϴ�.")
	char* readBuffer = new char[bufferSize];
	while (true)
	{
		Sleep(3000);
		// ���� ���� ���� ����
		readBuffer = SaveFile(jsonfilename);
		// ������ ����Ǿ� �ִ� ����� ��
		// ������ �ٸ��� �˷��ֱ�
		if (strcmp(readBufferLegacy, readBuffer) != 0)
		{
			cout << "cat.json ������ �����Ǿ����ϴ�. �����̽��� ������ �ٽ� �ε��մϴ�." << endl;
			while (true)
			{
				if (GetAsyncKeyState(VK_SPACE) & 0x8000)
				{
					break;
				}
			}
			// �ֽ� �������� �ٽ� �ε�
			PrintFile(readBuffer);
		}
		readBufferLegacy = readBuffer;

		// ���� ����� �����ǰų� ����Ǹ� ��������� ȭ�鿡 ǥ��
		vector<filesystem::path> newfilenameVec;
		for (auto& f : filesystem::directory_iterator(filesystem::current_path() / dirname))
		{
			if (f.path().extension() == ".json")
			{
				newfilenameVec.push_back(f.path().filename());
			}
		}
		//��
		// ���� ��ϰ� ����� �� ����� ����� �ٸ���
		if (newfilenameVec.size() != filenameVec.size())
		{
			// ���� �̸� ��� ���
			cout << "���� ����� �����Ǿ����ϴ�.\n";
			filenameVec.clear();
			for (size_t i = 0; i < newfilenameVec.size(); ++i)
			{
				cout << newfilenameVec[i] << endl;
				filenameVec.push_back(newfilenameVec[i]);
			}
		}
		else
		{
			for (size_t i = 0; i < newfilenameVec.size(); ++i)
			{
				if (newfilenameVec[i] == filenameVec[i])
				{
					continue;
				}
				if (i == newfilenameVec.size() - 1)
				{
					// ���� �̸� ��� ���
					cout << "���� ����� �����Ǿ����ϴ�.\n";
					filenameVec.clear();
					for (size_t i = 0; i < newfilenameVec.size(); ++i)
					{
						cout << newfilenameVec[i] << endl;
						filenameVec.push_back(newfilenameVec[i]);
					}
				}
			}
		}
		// filesystem::remove(f.path());	is_modified = true;

	}

	delete[] readBufferLegacy;
	file.close();
	return 0;
}