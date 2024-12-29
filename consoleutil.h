#pragma once

#include <windows.h>
#include <stdio.h>
#include <QFontDatabase>
#include <QString>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

namespace ConsoleUtils {
namespace {
HWND consoleWindow = nullptr;
bool initialized = false;

void SetConsoleUTF8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

QString GetFontPath() {
    QString exeDir = QCoreApplication::applicationDirPath();
    QString fontPath = QDir(exeDir).filePath("assets/sarasa-term-sc-regular.ttf");
    return QDir::cleanPath(fontPath);
}

bool LoadFont() {
    QString fontPath = GetFontPath();
    if (!QFile::exists(fontPath)) {
        qDebug() << "Font file not found:" << fontPath;
        return false;
    }

    // Add font to system
    const wchar_t* fontPathW = reinterpret_cast<const wchar_t*>(fontPath.utf16());
    if (!AddFontResourceW(fontPathW)) {
        qDebug() << "Failed to add font resource. Error code:" << GetLastError();
        return false;
    }

    // Add to Qt font database
    int fontId = QFontDatabase::addApplicationFont(fontPath);
    if (fontId == -1) {
        qDebug() << "Failed to add font to Qt font database";
        return false;
    }

    // Notify the system about the font change
    SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    return true;
}

void RemoveFont() {
    QString fontPath = GetFontPath();
    if (QFile::exists(fontPath)) {
        const wchar_t* fontPathW = reinterpret_cast<const wchar_t*>(fontPath.utf16());
        RemoveFontResourceW(fontPathW);
        SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    }
}

void SetConsoleFont(const wchar_t* fontName = L"Sarasa Term SC", SHORT fontSize = 16) {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = fontSize;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, fontName);
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}
}

inline void InitConsole() {
    if (initialized) {
        return;
    }

    FreeConsole();
    AllocConsole();
    consoleWindow = GetConsoleWindow();

    SetConsoleUTF8();

    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONOUT$", "w", stderr);
    freopen_s(&dummy, "CONIN$", "r", stdin);

    // _setmode(_fileno(stdout), _O_U8TEXT);
    // _setmode(_fileno(stderr), _O_U8TEXT);
    // _setmode(_fileno(stdin), _O_U8TEXT);

    if (LoadFont()) {
        SetConsoleFont(L"Sarasa Term SC", 16);
    } else {
        SetConsoleFont(L"Microsoft YaHei Mono", 16);
    }

    COORD bufferSize = {120, 3000};
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);

    SMALL_RECT windowSize = {0, 0, 119, 30};
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);

    ShowWindow(consoleWindow, SW_HIDE);
    initialized = true;
}

inline void ShowConsole() {
    if (!initialized) InitConsole();
    ShowWindow(consoleWindow, SW_SHOW);
}

inline void HideConsole() {
    if (!initialized) InitConsole();
    ShowWindow(consoleWindow, SW_HIDE);
}

inline bool IsConsoleVisible() {
    if (!initialized) return false;
    return consoleWindow && IsWindowVisible(consoleWindow);
}

inline void ToggleConsole() {
    if (IsConsoleVisible()) {
        HideConsole();
    } else {
        ShowConsole();
    }
}

inline void CleanupConsole() {
    if (consoleWindow) {
        RemoveFont();
        FreeConsole();
        consoleWindow = nullptr;
        initialized = false;
    }
}
}
