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
	// setting.ini 파일 읽기 - 디렉토리 이름 가져오기
	std::fstream file;
	const std::wstring filename = L"setting.ini";
	file.open(filename);
	if (file.fail())
	{
		cout << "setting.ini 파일이 존재하지 않습니다!" << endl;
		return 0;
	}

	// 디렉토리 이름 가져오기
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

	// 해당 디렉토리에 있는 json 파일명들 출력
	vector<filesystem::path> filenameVec;
	for (auto& f : filesystem::directory_iterator(filesystem::current_path() / dirname))
	{
		if (f.path().extension() == ".json")
		{
			filenameVec.push_back(f.path().filename());
			cout << f.path().filename() << endl;
		}
	}

	// json 파일 중 특정 파일을 선택
	string filenameInput;
	cout << "출력할 파일의 이름을 입력해주세요(확장자 제외하고 입력): ";
	cin >> filenameInput;
	filenameInput = "JSON\\" + filenameInput + ".json";
	const char* jsonfilename = filenameInput.c_str();

	// 파일 내용 저장
	char* readBufferLegacy = new char[bufferSize];
	readBufferLegacy = SaveFile(jsonfilename);

	// 파일 내용을 출력			
	PrintFile(readBufferLegacy);

	// 파일 내용이 수정된 경우, 자동으로 감지해서 알려주기 ("cat.json 파일이 수정되었습니다. 스페이스를 누르면 다시 로드합니다.")
	char* readBuffer = new char[bufferSize];
	while (true)
	{
		Sleep(3000);
		// 파일 내용 새로 저장
		readBuffer = SaveFile(jsonfilename);
		// 기존에 저장되어 있던 내용과 비교
		// 내용이 다르면 알려주기
		if (strcmp(readBufferLegacy, readBuffer) != 0)
		{
			cout << "cat.json 파일이 수정되었습니다. 스페이스를 누르면 다시 로드합니다." << endl;
			while (true)
			{
				if (GetAsyncKeyState(VK_SPACE) & 0x8000)
				{
					break;
				}
			}
			// 최신 버전으로 다시 로드
			PrintFile(readBuffer);
		}
		readBufferLegacy = readBuffer;

		// 파일 목록이 수정되거나 변경되면 변경사항을 화면에 표시
		vector<filesystem::path> newfilenameVec;
		for (auto& f : filesystem::directory_iterator(filesystem::current_path() / dirname))
		{
			if (f.path().extension() == ".json")
			{
				newfilenameVec.push_back(f.path().filename());
			}
		}
		//비교
		// 원래 목록과 변경된 후 목록의 사이즈가 다르면
		if (newfilenameVec.size() != filenameVec.size())
		{
			// 파일 이름 목록 출력
			cout << "파일 목록이 수정되었습니다.\n";
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
					// 파일 이름 목록 출력
					cout << "파일 목록이 수정되었습니다.\n";
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