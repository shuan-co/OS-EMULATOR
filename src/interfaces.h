#ifndef INTERFACES_H
#define INTERFACES_H

// Libraries

#include <iostream>
#include <string.h>
#include <windows.h>
#include <iomanip>
#include <ctime>
#include <array>
#include <sstream> // Required for std::ostringstream

using namespace std;

/*
    All Graphical User Interface Designs and Animations
*/

#include "dummyProcess.h"

// GUI Interfaces Class Definition

class Interfaces
{
private:
    static void marginWidth(const int width)
    {
        std::cout << std::setw(width) << " ";
    }

    static void displayText(const string text)
    {
        std::cout << text;
    }

    static std::string reduceProcessName(const std::string &processName, size_t maxLength = 40)
    {
        size_t lastSlash = processName.find_last_of("\\/");
        size_t secondLastSlash = processName.find_last_of("\\/", lastSlash - 1);

        std::string reducedName;

        if (secondLastSlash !=
            std::string::npos)
        {
            reducedName = "..." +
                          processName.substr(secondLastSlash + 1);
        }
        else
        {
            reducedName = processName;
        }

        if (reducedName.length() > maxLength)
        {
            return reducedName.substr(0, maxLength - 3) + "...";
        }

        return reducedName;
    }

    static void SetConsoleSize(int width, int height)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        COORD bufferSize = {static_cast<SHORT>(width),
                            static_cast<SHORT>(height)};

        SetConsoleScreenBufferSize(hConsole,
                                   bufferSize);

        SMALL_RECT windowSize = {0, 0,
                                 static_cast<SHORT>(width - 1),
                                 static_cast<SHORT>(height - 1)};

        SetConsoleWindowInfo(hConsole,
                             TRUE,
                             &windowSize);
    }

    static void SetCursorPosition(int x, int y)
    {
        HANDLE hConsole = GetStdHandle(
            STD_OUTPUT_HANDLE);
        COORD position = {static_cast<SHORT>(x),
                          static_cast<SHORT>(y)};
        SetConsoleCursorPosition(hConsole,
                                 position);
    }

    static void SetTextColor(int color)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    }

    static void DisplayCurrentDateTimeTop(int width)
    {
        std::time_t currentTime = std::time(nullptr);

        std::tm *localTime = std::localtime(&currentTime);

        char buffer[100];

        std::strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", localTime);

        SetCursorPosition(0, 0);

        std::cout << buffer;
    }

    static void DrawBorder(int width, int height, int yOffset)
    {
        SetCursorPosition(0, yOffset);
        std::cout << "+";

        for (int i = 1; i < width - 1; i++)
        {
            std::cout << "-";
        }
        std::cout << "+";

        SetCursorPosition(0, height - 1 + yOffset);
        std::cout << "+";

        for (int i = 1; i < width - 1; i++)
        {
            std::cout << "-";
        }
        std::cout << "+";

        for (int i = 1 + yOffset; i < height - 1 + yOffset; i++)
        {
            SetCursorPosition(0, i);
            std::cout << "|";
            SetCursorPosition(width - 1, i);
            std::cout << "|";
        }
    }

    static void drawDivider(const int width, const int height, const int posx, const int posy, bool isHorizontal, char dividerSymbol)
    {
        if (isHorizontal)
        {
            SetCursorPosition(posx, posy);
            std::cout << "+";

            for (int i = 1; i < width - 1; i++)
            {
                SetCursorPosition(posx + i, posy);
                std::cout << dividerSymbol;
            }

            std::cout << "+";
        }
        else
        {
            SetCursorPosition(posx, posy);
            std::cout << "+";

            for (int i = 1; i < height - 1; i++)
            {
                SetCursorPosition(posx, posy + i);
                std::cout << "|";
            }

            SetCursorPosition(posx, posy + height - 1);
            std::cout << "+";
        }
    }

    static string enterCommandMarqueeText;

public:
    static void displayHeader()
    {
        cout << " ______     ______     ______     ______   ______     ______     __  __    \n";
        cout << "/\\  ___\\   /\\  ___\\   /\\  __ \\   /\\  == \\ /\\  ___\\   /\\  ___\\   /\\ \\_\\ \\   \n";
        cout << "\\ \\ \\____  \\ \\___  \\  \\ \\ \\/\\ \\  \\ \\  _-/ \\ \\  __\\   \\ \\___  \\  \\ \\____ \\  \n";
        cout << " \\ \\_____\\  \\/\\_____\\  \\ \\_____\\  \\ \\_\\    \\ \\_____\\  \\/\\_____\\  \\/\\_____\\ \n";
        cout << "  \\/_____/   \\/_____/   \\/_____/   \\/_/     \\/_____/   \\/_____/   \\/_____/ \n";
        cout << "=============================================================================\n";
    }

    static void displayMenu()
    {
        cout << "\033[38;2;21;188;12m"; // RGB(21, 188, 12)
        cout << "Hello, Welcome to CSOPESY Emulator!\n\n";

        cout << "Developers: \n";
        cout << "Co, Shuan Noel\n";
        cout << "Gregorio, John Marc\n";
        cout << "Villanueva, Miguel\n";
        cout << "Quinones, Angelo Yanto\n\n";

        cout << "Last Updated: ";
        cout << "\033[38;2;226;219;150m"; // RGB(226, 219, 150)
        cout << "10-12-2024\n";

        // Reset to default color
        cout << "\033[0m";
        cout << "=============================================================================\n\n";
    }

    static void displayGetCommand()
    {
        // Reset to default color
        cout << "\033[0m";
        cout << "root:\\> ";
    }

    static void displaySMIHeader()
    {
        SetConsoleSize(200, 200);
        int consoleWidth = 92;
        int consoleHeight = 12;

        SetConsoleTitle(L"OPSeYOS-SMI");

        DisplayCurrentDateTimeTop(consoleWidth);

        DrawBorder(consoleWidth, consoleHeight, 1);

        SetCursorPosition(2, 2);
        SetTextColor(10);

        const int margin = 12;

        displayText("OPSeYOS-SMI 100.00v");
        marginWidth(margin);

        displayText("Driver Version: 100.00v");
        marginWidth(margin);

        displayText("CUDA Version: 10.00v");

        SetTextColor(7);

        drawDivider(consoleWidth, 3, 0, 3, true, '-');

        drawDivider(consoleWidth, 1, 0, 7, true, '=');

        SetCursorPosition(2, 4);
        displayText("GPU");
        marginWidth(2);
        displayText("Name");
        marginWidth(19);
        displayText("Driver-Model");

        SetCursorPosition(2, 5);
        displayText("Fan");

        marginWidth(2);
        displayText("Temp");

        marginWidth(3);
        displayText("Perf");

        marginWidth(11);
        displayText("Pwr:Usage/Cap");

        drawDivider(1, 5, 43, 3, false, '-');

        SetCursorPosition(44, 4);
        marginWidth(1);
        displayText("Bus-Id");

        marginWidth(10);
        displayText("Disp.A");

        SetCursorPosition(55, 5);
        displayText("Memory-Usage");

        drawDivider(1, 5, 68, 3, false, '-');

        SetCursorPosition(70, 4);
        displayText("Volatile Uncorr. ECC");

        SetCursorPosition(70, 5);
        displayText("GPU-Util");

        marginWidth(2);
        displayText("Compute M.");

        SetCursorPosition(84, 6);
        displayText("MIG M.");

        SetCursorPosition(5, 8);
        displayText("0");

        marginWidth(2);
        displayText("NVIDIA GeForce RTX 2060 ...");

        marginWidth(2);
        displayText("WDDM");

        SetCursorPosition(3, 8);
        displayText("45%");

        marginWidth(3);
        displayText("45C");

        marginWidth(4);
        displayText("P0");

        marginWidth(13);
        displayText("36W /  184W");

        drawDivider(1, 5, 43, 7, false, '-');

        SetCursorPosition(47, 8);
        displayText("00000000:01:00.0");

        marginWidth(2);
        displayText("On");

        SetCursorPosition(48, 9);
        displayText("1670MiB /   8192MiB");

        drawDivider(1, 5, 68, 7, false, '-');

        SetCursorPosition(87, 8);
        displayText("N/A");

        SetCursorPosition(75, 9);
        displayText("0%");

        marginWidth(6);
        displayText("Default");

        SetCursorPosition(87, 10);
        displayText("N/A");
    }

    static void displayProcessInfo(const int numProcesses, DummyProcess *processes)
    {
        // Updated User Interface
        displayText("----------------------------------\n");
        displayText("Running Processes:\n");

        int processNameWidth = 15; 
        int dateWidth = 25;        
        int coreWidth = 12;        
        int giCiWidth = 12;        

        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].gi < processes[i].ci)
            {
                std::ostringstream oss;

                // Process name
                oss << std::left << std::setw(processNameWidth) << processes[i].processName;

                // Date
                oss << std::left << std::setw(dateWidth) << processes[i].type;

                // Core field (aligned correctly)
                if (processes[i].core == -1)
                {
                    oss << std::setw(coreWidth) << "Core: N/A";
                }
                else
                {
                    oss << std::setw(coreWidth) << ("Core: " + std::to_string(processes[i].core));
                }

                // Gi/Ci field
                oss << std::setw(giCiWidth) << (std::to_string(processes[i].gi) + " / " + std::to_string(processes[i].ci));

                displayText(oss.str() + "\n");
            }
        }

        displayText("\nFinished Processes:\n");

        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i].gi >= processes[i].ci)
            {
                std::ostringstream oss;

                // Process name
                oss << std::left << std::setw(processNameWidth) << processes[i].processName;

                // Date
                oss << std::left << std::setw(dateWidth) << processes[i].type;

                // Core or Finished status
                oss << std::setw(coreWidth) << "Finished";

                // Gi/Ci field
                oss << std::setw(giCiWidth) << (std::to_string(processes[i].gi) + " / " + std::to_string(processes[i].ci));

                displayText(oss.str() + "\n");
            }
        }

        displayText("\n----------------------------------\n\n");

        // int consoleWidth = 92;
        // int consoleHeight = 6 + numProcesses;
        // int yOffset = 14;
        // DrawBorder(consoleWidth, consoleHeight, yOffset);

        // SetCursorPosition(2, yOffset + 1);
        // SetTextColor(10);
        // displayText("Processes: ");

        // SetCursorPosition(2, yOffset + 2);
        // displayText("#");

        // marginWidth(5);
        // displayText("CurrentLine");

        // marginWidth(5);
        // displayText("TotalLines");

        // marginWidth(3);
        // displayText("PID");

        // marginWidth(10);
        // displayText("Creation");

        // marginWidth(10);
        // displayText("Process name");

        // SetTextColor(7);
        // drawDivider(consoleWidth, 1, 0, yOffset + 4, true, '=');

        // for (int i = 0; i < numProcesses; i++)
        // {
        //     SetCursorPosition(4, yOffset + 5 + i);
        //     displayText(to_string(processes[i].gpu));

        //     SetCursorPosition(13, yOffset + 5 + i);
        //     displayText(to_string(processes[i].gi));

        //     SetCursorPosition(28, yOffset + 5 + i);
        //     displayText(to_string(processes[i].ci));

        //     SetCursorPosition(38, yOffset + 5 + i);
        //     displayText(to_string(processes[i].pid));

        //     SetCursorPosition(45, yOffset + 5 + i);
        //     displayText(processes[i].type);

        //     SetCursorPosition(70, yOffset + 5 + i);
        //     displayText(reduceProcessName(processes[i].processName));

        // }
        // SetCursorPosition(0, yOffset + 5 + numProcesses + 1);
        // displayText("Press any key to return...");
    }

    static void displayMarqueeHeader()
    {
        cout << "*********************************" << endl;
        cout << "* Displaying a marquee console! *" << endl;
        cout << "*********************************" << endl;
    }

    static void marqueeConsoleAnimation(const int x,
                                        const int y,
                                        const string marqueeText =
                                            "Hello world in marquee!")
    {
        system("cls");

        // Get Console Size:
        int currentWidth, currentHeight;
        GetConsoleSize(currentWidth, currentHeight);
        SetConsoleSize(currentWidth, currentHeight);

        SetCursorPosition(0, 0);
        displayMarqueeHeader();

        SetCursorPosition(x + 1, y + 3);
        displayText(marqueeText);
    }

    static void displayUserGetCommand(const string userCommand, const int yOffset = 0)
    {
        SetCursorPosition(0, yOffset);
        cout << "Enter a command for MARQUEE_CONSOLE: " << userCommand;
    }

    static void displayCommandHistory(const array<string, 3> &commands, int historyIndex, const int yOffset = 0)
    {
        SetCursorPosition(0, yOffset);
        int startIdx = (historyIndex - 1 + 3) % 3;
        cout << "\n";

        for (int i = 0; i < 3; ++i)
        {
            int idx = (startIdx - i + 3) % 3;
            if (!commands.at(idx).empty())
            {
                std::cout << "Command processed in MARQUEE_CONSOLE: " << commands.at(idx) << "\n";
            }
        }
    }

    static void resetCursorPositionMarquee(const string userCommand, const int yOffset = 0)
    {
        SetCursorPosition(enterCommandMarqueeText.length() + userCommand.length(), yOffset);
    }

    static void GetConsoleSize(int &width, int &height)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;

        if (GetConsoleScreenBufferInfo(hConsole, &csbi))
        {
            width = csbi.dwSize.X;
            height = csbi.dwSize.Y;
        }
        else
        {
            std::cerr << "Error retrieving console size." << std::endl;
        }
    }
};

string Interfaces::enterCommandMarqueeText = "Enter a command for MARQUEE_CONSOLE: ";

#endif