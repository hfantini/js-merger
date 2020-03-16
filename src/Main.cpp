/*

	+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
	|
	|  HENRIQUE FANTINI @ 2019
	|  JSMerger
	|
	|  = DESCRIÇÃO DO PROJETO =
	|
	|   Este projeto é um pequeno aplicativo que une um conjunto de arquivos em
	|   um único. Ele foi feito especialmente para trabalhar com Javascript e CSS.
	|
	+ - - - - - - - - - - - === DESCRIÇÃO DO ARQUIVO ===  - - - - - - - - - - - +
	|
	|     ARQUIVO: Main.cs
	|		 TIPO: <CLASS>
	|       AUTOR: Henrique Fantini
	| 	DATA/HORA: 2018-11-14 (AAAA-MM-DD) / 07:38 (HH:MM 24H)
	|
	+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +

*/

// == INCLUDE
// ==============================================================================

using namespace std;

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<tchar.h>
#include<filesystem>
#include<map>
#include<windows.h>

// == CODE
// ==============================================================================

const char* defaultMargin = "    ";
map<string, string> mapParam;
map<string, string>::iterator it;
vector<string> fileList = vector<string>();
vector<string> failedFileList = vector<string>();
vector<string> ignoredFileList = vector<string>();

void processParam(int argc, char *argv[])
{
	for (int count = 0; count < argc; ++count)
	{
		string opt = argv[count];

		if ( (opt.compare("-h") == 0) || (opt.compare("-m") == 0) || (opt.compare("-r") == 0) || (opt.compare("-f") == 0) )
		{
			mapParam.insert( make_pair(argv[count], "") );
		}
		else if ( (opt.compare("-o") == 0 || opt.compare("-s") == 0 || opt.compare("-t") == 0) )
		{
			mapParam.insert(make_pair(argv[count], argv[count + 1]));
		}
		else if ( (opt.compare("-i") == 0) )
		{
			string ignoreStr = argv[count + 1];
			stringstream ss(ignoreStr);
			string token;

			while ( getline(ss, token, ',') )
			{
				ignoredFileList.push_back(token);
			}
			
		}
	}

	// SET DEFAULTS

	if ( mapParam.find("-t") == mapParam.end() )
	{
		mapParam.insert( make_pair("-t", "js") );
	}
}

void displayHeader()
{
	cout << R"(
    + - - - - - - - - - - - - - - - - - - - - - - - - - +
    |                                                   |
    |           JSMerger - By Henrique Fantini          |
    |              www.henriquefantini.com              | 
    |                       0.0.1a                      |
    |                                                   |
    + - - - - - - - - - - - - - - - - - - - - - - - - - +)" << endl << endl;
}

void displayHelp()
{
	cout << R"(
    JSMerger is a tiny software developed to merge multiple files in only one 
    output. Planned for .JS (Javascript) and .CSS files.
	)" << endl;

	cout << R"(
    USAGE: jsmerger [opts]
	)" << endl;

	printf("%s+ %-10s + %-20s + %-40s +\n", defaultMargin, "----------", "--------------------", "----------------------------------------");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "  OPTION", "       PARAM", "              DESCRIPTION");
	printf("%s+ %-10s + %-20s + %-40s +\n", defaultMargin, "----------", "--------------------", "----------------------------------------");

	// -h

	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "   -h", " -", "DISPLAYS INFORMATION ABOUT THE USAGE.");
	printf("%s+ %-10s + %-20s + %-40s +\n", defaultMargin, "----------", "--------------------", "----------------------------------------");

	// -t

	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "   -t", " 'js' 'css'", "TYPE FILE OF THE MERGE PROCESS.");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "DEFAULT: js");
	printf("%s+ %-10s + %-20s + %-40s +\n", defaultMargin, "----------", "--------------------", "----------------------------------------");

	// -s

	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "   -s", " [FOLDER]", "THE PATH TO THE FOLDER THAT CONTAINS ALL");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "FILES THAT YOU WANT TO MERGE.");
	printf("%s+ %-10s + %-20s + %-40s +\n", defaultMargin, "----------", "--------------------", "----------------------------------------");

	// -o

	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "   -o", " [FILE]", "THE OUTPUT FILE PATH. IF THE FILE");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "DOESN'T EXIST IT WILL BE CREATED. IF");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "DOES, USER WILL BE PROMPTED FOR ");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "OVERWRITING.");
	printf("%s+ %-10s + %-20s + %-40s +\n", defaultMargin, "----------", "--------------------", "----------------------------------------");

	// -r

	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "   -r", " -", "TELLS TO THE PROCESS THAT IT MUST SCAN");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "IN SEARCH OF FILES IN A RECURSIVE WAY");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "EXPLORING THE SUBFOLDERS.");
	printf("%s+ %-10s + %-20s + %-40s +\n", defaultMargin, "----------", "--------------------", "----------------------------------------");

	// -m

	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "   -m", " -", "MINIFY THE OUTPUT FILE.");
	printf("%s+ %-10s + %-20s + %-40s +\n", defaultMargin, "----------", "--------------------", "----------------------------------------");

	// -i

	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "   -i", " [STRING] filename", "DEFINES FILES THAT MUST BE IGNORED BY");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "THE PROCESS. SEPARE THE FILES BY COMMA");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "(WITHOUT SPACES). EXAMPLE: file1.js,");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "file2.js, file3.js...");
	printf("%s+ %-10s + %-20s + %-40s +\n", defaultMargin, "----------", "--------------------", "----------------------------------------");

	// -f

	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "   -f", " -", "FORCES THE FILE OVERWRITING. USER WON'T");
	printf("%s| %-10s | %-20s | %-40s |\n", defaultMargin, "", "", "BE ANYMORE ASKED FOR IT.");
	printf("%s+ %-10s + %-20s + %-40s +\n", defaultMargin, "----------", "--------------------", "----------------------------------------");
}

void displayError(string err)
{
	cout << endl << defaultMargin << "ERROR: " << err << endl;
}

bool checkParam()
{
	it = mapParam.find("-o");

	if (it == mapParam.end())
	{
		displayError("OUTPUT FILE NOT SPECIFIED.");
		return false;
	}

	it = mapParam.find("-s");

	if (it == mapParam.end())
	{
		displayError("SOURCE FOLDER PATH NOT SPECIFIED.");
		return false;
	}

	return true;
}

bool compareFileExtension(string fileName, string value)
{
	bool retValue = false;

	if (fileName.substr(fileName.find_last_of(".") + 1) == value)
	{
		retValue = true;
	}

	return retValue;
}

bool scanDirectory(string folderPath)
{
	string currentLookingForExtension = mapParam.at("-t");
	HANDLE hFind;
	WIN32_FIND_DATA findData;
	
	wstring folderPathFile(folderPath.length(), L' ');
	std:copy(folderPath.begin(), folderPath.end(), folderPathFile.begin());

	string folderPathFunc = folderPath + "/*";

	hFind = FindFirstFile(folderPathFunc.c_str(), &findData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		displayError( "DIRECTORY INEXISTENT OR INVALID." );
		return false;
	}

	do
	{
		if ( (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
			// IS DIRECTORY 

			if ( strcmp( findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0 )
			{
				it = mapParam.find("-r");

				if (it != mapParam.end())
				{
					stringstream strStream;
					const string folderPathFileConv(folderPathFile.begin(), folderPathFile.end());
					strStream << folderPathFileConv << "/" << findData.cFileName;

					scanDirectory(strStream.str());
				}
			}
		}
		else
		{
			// IS FILE

			if ( compareFileExtension(findData.cFileName, currentLookingForExtension) )
			{
				stringstream strStream;
				const string folderPathFileConv(folderPathFile.begin(), folderPathFile.end());
				strStream << folderPathFileConv << "/" << findData.cFileName;

				fileList.push_back(strStream.str());
				wcout << defaultMargin << "FILE FOUND: " << defaultMargin << folderPathFile << "/" << findData.cFileName << endl;
			}
		}
	} 
	while ( FindNextFile(hFind, &findData) );

	return true;
}

bool mergeFiles(fstream& stream, vector<string> list)
{
	fstream readStream;

	if (stream.is_open())
	{
		for (string &s : list)
		{
			string content;
			readStream.open(s);

			if (readStream.good())
			{
				string fileName = s.substr( s.find_last_of('/') + 1 );

				if ( find( ignoredFileList.begin(), ignoredFileList.end(), fileName) == ignoredFileList.end() )
				{
					cout << defaultMargin << "PROCESSING FILE: " << s << endl;

					while (!readStream.eof())
					{
						it = mapParam.find("-m");

						if (it == mapParam.end())
						{
							getline(readStream, content);
							content = content + "\r\n";

							stream.write(content.c_str(), content.length());
						}
						else
						{
							getline(readStream, content);
							stream.write(content.c_str(), content.length());
						}

						stream.flush();
					}
				}

				readStream.close();
			}
			else
			{
				failedFileList.push_back(s);
			}
		}
	}

	return true;
}

void start()
{
	it = mapParam.find("-h");

	if (it != mapParam.end())
	{
		displayHelp();
	}
	else
	{
		// == CHECKING FOR PARAMETERS

		cout << defaultMargin << "CHECKING PARAMETERS... " << endl;

		if (checkParam() == false)
		{
			return;
		}

		// == ACCESSING THE OUTPUT FILE

		string filePath = mapParam.at("-o");
		cout << defaultMargin << "CREATING OUTPUT FILE ('" << filePath << "')... " << endl;

		try
		{
			fstream file;
			file.open(filePath, std::ifstream::in);

			if ( !file )
			{
				file.open(filePath, std::ifstream::out | std::ifstream::trunc);
			}
			else
			{
				// PROMPT USER IF HE WANTS TO OVERWRITE THE EXISTENT FILE

				string opt;

				if (mapParam.find("-f") == mapParam.end())
				{
					do
					{
						cout << endl << defaultMargin << "THE FILE: '" << filePath << "' ALREADY EXISTS. DO YOU WANT TO OVERWRITE THE FILE? (Y/N): ";
						cin >> opt;

					} while (opt.compare("y") != 0 && opt.compare("n") != 0);
				}
				else
				{
					opt = "y";
				}

				if (opt.compare("y") == 0)
				{
					cout << endl;
					file.close();
					file.open(filePath, std::ifstream::out | std::ifstream::trunc);
				}
				else
				{
					return;
				}
			}

			// == ACCESSING THE INPUT FOLDER

			string folderPath = mapParam.at("-s");
			cout << defaultMargin << "SCANNING OUTPUT FOLDER ('" << folderPath << "')... " << endl << endl;

			if (scanDirectory( folderPath ) == false)
			{
				return;
			}

			if (fileList.size() == 0)
			{
				cout << endl << defaultMargin << "NO FILES FOUND IN ['" << folderPath << "']. ABORTING OPERATION..." << endl;
				return;
			}
			else
			{
				cout << endl << defaultMargin << fileList.size() << " FILE(S) FOUND." << endl;
			}

			if( ignoredFileList.size() > 0 )
			{
				cout << endl << defaultMargin << "IGNORING THE FOLLOWING FILES: " << endl << endl;
				for (auto &s : ignoredFileList)
				{
					cout << defaultMargin << "-" << s;
				}
			}

			cout << endl << endl << defaultMargin << "MERGING FILES... " << endl;
			mergeFiles(file, fileList);

			file.close();
		}
		catch (int e)
		{
			char msg[200];
			strerror_s( msg, sizeof(msg), errno);
			string strMsg = msg;

			displayError("AN EXCEPTION WAS THROWN - " + strMsg);
			return;
		}
		catch (ios_base::failure& e)
		{
			string msg = e.what();
			displayError("AN EXCEPTION WAS THROWN - " + msg);
		}
	}
}

void main(int argc, char *argv[])
{
	displayHeader();

	processParam(argc, argv);
	start();
}