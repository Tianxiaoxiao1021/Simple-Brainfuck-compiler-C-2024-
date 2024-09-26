#define IMGUI_DEFINE_MATH_OPERATORS
#include <windows.h>
#include <commdlg.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "SIMPLE-LOG.h"
using namespace std;
// 释放 LPCWSTR
void freeLPCWSTR(LPCWSTR& str) {
    delete[] str;
}
//获取程序根目录
std::wstring GetProgramDirectory()
{
    wchar_t path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    std::wstring directory(path);
    return directory.substr(0, directory.find_last_of(L"\\") + 1);
}
//将std::wstring 转为 std::string
std::string WStringToString(const std::wstring& wstr) {
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(bufferSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, NULL, NULL);
    return str;
}
// 将 std::string 转换为 LPCWSTR
LPCWSTR stringToLPCWSTR(const std::string& str) {
    // 获取所需的宽字符数组长度
    int wideSize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    if (wideSize == 0) {
        throw std::runtime_error("Failed to get the size of the wide string.");
    }

    // 创建宽字符数组
    std::vector<wchar_t> buffer(wideSize);

    // 进行实际的转换
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer.data(), wideSize);

    // 动态分配内存存储宽字符数据
    wchar_t* wideStr = new wchar_t[wideSize];
    std::copy(buffer.begin(), buffer.end(), wideStr);

    return wideStr; // 返回动态分配的内存指针
}
std::string wstring_to_utf8(const std::wstring& wstr) {
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::vector<char> utf8_str(len);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, utf8_str.data(), len, nullptr, nullptr);
    return std::string(utf8_str.data());
}
//获取编辑框内容并转换为 std::string
std::string GetEditBoxText(HWND hEdit)
{
    // 获取编辑框的文本长度
    int length = GetWindowTextLengthW(hEdit);
    if (length == 0)
    {
        // 如果编辑框为空，返回空字符串
        return "";
    }

    // 为文本分配内存
    std::wstring wtext(length, L'\0');

    // 获取编辑框的文本内容
    GetWindowTextW(hEdit, &wtext[0], length + 1); // length + 1 包括 null 终止符

    // 将 std::wstring 转换为 std::string
    std::string text(wtext.begin(), wtext.end());

    return text;
}
std::string gccpath;
using namespace std;
string To_Cpp_Source(const string& code) {
    stringstream ss;
    string str2;

    // 写入文件头
    ss << "#include <iostream>\n";
    ss << "using namespace std;\n";
    ss << "short ptr2 = 0, ptr3 = 0;\n";
    ss << "short* ptr = &ptr2;\n";  // 初始化 ptr 指向 ptr2
    ss << "int memory[30000] = {0};\n";  // 初始化 memory 数组
    ss << "#define func\n";  // 函数前向声明
    ss << "int main() {\n";

    // 处理代码
    bool usePtr2 = true;  // 默认使用 ptr2
    bool is_inside_func = false;
    int j = 0;
    for (int i = 0; i < code.size();i++) {
        switch (code[i]) {
        case '>':
            if (!is_inside_func) ss << "    (*ptr)++;\n";
            else ss << "(*ptr)++;";
            break;
        case '<':
            if (!is_inside_func) ss << "    (*ptr)--;\n";
            else ss << "(*ptr)--;";
            break;
        case '+':
            if (!is_inside_func) ss << "    memory[*ptr]++;\n";
            else ss << "memory[*ptr]++;";
            break;
        case '-':
            if (!is_inside_func) ss << "    memory[*ptr]--;\n";
            else ss << "memory[*ptr]--; ";
            break;
        case '.':
            if (!is_inside_func) ss << "    cout << memory[*ptr];\n";
            else ss << "cout<< memory[*ptr];";
            break;
        case ',':
            if (!is_inside_func) ss << "    cin >> memory[*ptr];\n";
            else ss << "cin >> memory[*ptr]; ";
            break;
        case '[':
            if (!is_inside_func) ss << "    while (memory[*ptr]) {\n";
            else ss << "while(memory[*ptr]){";
            break;
        case ']':
            if (!is_inside_func) ss << "    }\n";
            else ss << "}";
            break;
        case '{':
            ss << "#undef func\n";
            ss << "#define func ";
            is_inside_func = true;
            break;
        case'}':
            ss << "\n";
            is_inside_func = false;
            break;
        case '@':
            if (!is_inside_func)
            {
                usePtr2 = !usePtr2;
                ss << "    ptr = " << (usePtr2 ? "&ptr2" : "&ptr3") << ";\n";
            }
            else
            {
                usePtr2 = !usePtr2;
                ss << "ptr = " << (usePtr2 ? " & ptr2" : " & ptr3") << ";";
            }
            break;
        case '&':
            ss << "    func;\n";
            break;
        case '(':
            if (code[i + 1] != '|')
            {
                j = stoi(code.substr(code.find("(") + 1, code.find(")") - 1));
                ss << "    for(int i = 0; i < " << to_string(j) << ";i++){\n";
            }
            else {
                ss << "    for(int i = 0; i < memory[*ptr]; i++){\n";
            }
            break;
        case '^':
            ss << "    }\n";
            break;
        }
    }

    // 结束 main 函数
    ss << "    system(\"pause\");\n";
    ss << "    return 0;\n";
    ss << "}\n";

    return ss.str();
}
// 全局变量
HINSTANCE hInst;
HWND hEdit;
HWND hOutputBox;
HMENU hMenu, hFileMenu, hWorkspaceMenu, hFile_outMenu, hConfigureMenu, hOutSpaceMenu;
// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // 创建菜单
        hMenu = CreateMenu();
        hFileMenu = CreateMenu();
        hWorkspaceMenu = CreateMenu();
        hFile_outMenu = CreateMenu();
        hConfigureMenu = CreateMenu();
        hOutSpaceMenu = CreateMenu();
        AppendMenu(hFileMenu, MF_STRING, 1, L"Import");
        AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hFile_outMenu, L"Export workspace content to...");
        AppendMenu(hFile_outMenu, MF_STRING, 3, L".exe（Windows executable）and.cpp（C++ source file）");
        AppendMenu(hFile_outMenu, MF_STRING, 5, L".bf（BrainFuck source file）"); 
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
        AppendMenu(hWorkspaceMenu, MF_STRING, 2, L"Clean WorkSpace");
        AppendMenu(hWorkspaceMenu, MF_POPUP, (UINT_PTR)hOutSpaceMenu, L"OutBox...");
        AppendMenu(hOutSpaceMenu, MF_STRING, 6, L"Clean OutBox");
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hWorkspaceMenu, L"WorkSpace...");
        AppendMenu(hConfigureMenu, MF_STRING, 4, L"G++ Compiler Path");
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hConfigureMenu, L"Configuration...");
        AppendMenu(hWorkspaceMenu, MF_STRING, 7, L"Count WorkSpace Data");

        SetMenu(hWnd, hMenu);

        // 创建编辑框
        hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE, L"EDIT", nullptr,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 800, 600, hWnd, nullptr, hInst, nullptr
        );
        // 创建输出框
        hOutputBox = CreateWindowEx(
            0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
            0, 610, 800, 90, hWnd, nullptr, hInst, nullptr
        );
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) // 处理“导入”菜单项
        {
            OPENFILENAME ofn = { sizeof(OPENFILENAME) };
            WCHAR szFile[260] = { 0 };
            
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);
            ofn.lpstrFilter = L"BrainFuck Source File\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hFile != INVALID_HANDLE_VALUE)
                {
                    DWORD fileSize = GetFileSize(hFile, NULL);
                    if (fileSize != INVALID_FILE_SIZE)
                    {
                        std::vector<char> buffer(fileSize);
                        DWORD bytesRead;
                        if (ReadFile(hFile, buffer.data(), fileSize, &bytesRead, NULL))
                        {
                            int wideCharCount = MultiByteToWideChar(CP_UTF8, 0, buffer.data(), fileSize, NULL, 0);
                            if (wideCharCount > 0)
                            {
                                std::wstring fileContent(wideCharCount, L'\0');
                                MultiByteToWideChar(CP_UTF8, 0, buffer.data(), fileSize, &fileContent[0], wideCharCount);
                                SetWindowText(hEdit, fileContent.c_str());
                            }
                        }
                    }
                    CloseHandle(hFile);
                }
            }
        }
        //清空工作区选项
        else if (LOWORD(wParam) == 2)
        {
            SetWindowText(hEdit, L"");
        }
        //配置G++编译器路径选项
        else if (LOWORD(wParam) == 4)
        {
            wchar_t szPath[MAX_PATH] = { 0 };
            OPENFILENAME ofn = { 0 };

            // 初始化 OPENFILENAME 结构体
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szPath;
            ofn.nMaxFile = sizeof(szPath) / sizeof(szPath[0]);
            ofn.lpstrFilter = L"Executable Files\0*.exe\0All Files\0*.*\0";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            if (GetOpenFileName(&ofn))
            {
                // 打开文件进行写入
                std::wstring programDirectory = GetProgramDirectory();
                std::wstring filePath = programDirectory + L"gccpath.path";
                std::ofstream Gcc_path(filePath.c_str());
                if (Gcc_path.is_open())
                {
                    Gcc_path << wstring_to_utf8(szPath);
                    Gcc_path.close();
                }
                else
                {
                    std::wstring errorMsg = L"Failed to open file for writing. Error code: " + std::to_wstring(GetLastError());
                    MessageBox(hWnd, errorMsg.c_str(), L"Error", MB_OK | MB_ICONERROR);
                }
            }
        }
        //导出为可执行文件选项
        else if (LOWORD(wParam) == 3)
        {
            wchar_t szPath[MAX_PATH] = { 0 };
            OPENFILENAME ofn = { 0 };

            // 初始化 OPENFILENAME 结构体
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szPath;
            ofn.nMaxFile = sizeof(szPath) / sizeof(szPath[0]);
            ofn.lpstrFilter = L"\0C++ Source Code\0*.cpp\0All Files\0*.*\0";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
            // 显示文件对话框
            if (GetSaveFileName(&ofn))
            {
                std::wstring userPath = ofn.lpstrFile;
                std::string cppFilePath = wstring_to_utf8(userPath + L".cpp");
                std::string outstr_cpp = To_Cpp_Source(GetEditBoxText(hEdit));
                std::ofstream outfile(cppFilePath);

                if (outfile.is_open())
                {
                    int length = GetWindowTextLength(hOutputBox);
                    // 输出操作信息
                    std::wstring operation = L"BrainFuck code being converted to C++ code...\n";
                    SetWindowText(hOutputBox, operation.c_str());
                    outfile << outstr_cpp;
                    outfile.close();

                    // 输出操作信息
                    operation = L"Compiling C++ code...\n";
                    std::wstring currentText(length, L'\0');
                    GetWindowText(hOutputBox, &currentText[0], length + 1);
                    SetWindowText(hOutputBox, (currentText + operation).c_str());
                    // 记录编译开始时间
                    DWORD startTime = GetTickCount64();
                    // 生成输出文件名（与输入文件名相同，但后缀改为 .exe）
                    std::string outputFilePath = cppFilePath.substr(0, cppFilePath.find_last_of('.')) + ".exe";
                    // 使用 system() 编译 C++ 源文件
                    std::string compileCommand = "g++ -o \"" + outputFilePath + "\" \"" + cppFilePath + "\"";
                    int result = system(compileCommand.c_str());

                    // 记录编译结束时间并计算用时
                    DWORD endTime = GetTickCount64();
                    DWORD duration = endTime - startTime;
                    GetWindowText(hOutputBox, &currentText[0], length + 1);
                    if (result != 0)
                    {
                        std::wstring errorMsg = L"compile failed, code" + std::to_wstring(result) + L"\n";
                        SetWindowText(hOutputBox, (currentText+errorMsg).c_str());
                    }
                    else
                    {
                        std::wstring successMsg = L"compile" + userPath + L"successful,time use:" + std::to_wstring(duration) + L"ms\n";
                        SetWindowText(hOutputBox, (currentText+successMsg).c_str());
                    }
                }
                else {
                    MessageBox(hWnd, L"Undefined file path", L"Wrong", MB_OK);
                    break;
                }
            }
        }
        //清空输出框选项
        if (LOWORD(wParam) == 6)
        {
            SetWindowText(hOutputBox, L"");
        }
        //保存至.bf文件选项
        if (LOWORD(wParam) == 5)
        {
            wchar_t szPath[MAX_PATH] = { 0 };
            OPENFILENAME ofn = { 0 };

            // 初始化 OPENFILENAME 结构体
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szPath;
            ofn.nMaxFile = sizeof(szPath) / sizeof(szPath[0]);
            ofn.lpstrFilter = L"\0C++ source code\0*.cpp\0All Files\0*.*\0";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
            string str = GetEditBoxText(hEdit);
            // 显示文件对话框
            if (GetSaveFileName(&ofn))
            {
                std::wstring userPath = ofn.lpstrFile; 
                std::string cppFilePath = wstring_to_utf8(userPath + L".bf");
                std::ofstream outfile(cppFilePath);
                if (outfile.is_open())
                {
                    // 输出操作信息
                    std::wstring operation = L"saving code to .bf file...\n";
                    SetWindowText(hOutputBox, operation.c_str());
                    DWORD startTime = GetTickCount64();
                    outfile << str;
                    outfile.close();
                    // 计算用时
                    DWORD endTime = GetTickCount64();
                    DWORD duration = endTime - startTime;
                    std::wstring success_msg = L"save" + userPath + L"complete,time use" + std::to_wstring(duration) + L"ms\n";
                    SetWindowText(hOutputBox, success_msg.c_str());
                }
                else {
                    MessageBox(hWnd, L"Undefined file path", L"Wrong", MB_OK);
                    break;
                }
            }
        }
        //统计工作区选项
        if (LOWORD(wParam) == 7)
        {
            int words = 0;
            for (auto i : GetEditBoxText(hEdit))
            {
                words++;
            }
            
            MessageBox(hWnd, stringToLPCWSTR("words:" + to_string(words) + '\n'+ "G++ Path:" + ((gccpath.empty()) ? "Unknown" : gccpath)), L"Count Result:", MB_OK | MB_ICONINFORMATION);
        }
        break;
    case WM_SIZE:

        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        // 调整编辑框大小以适应窗口
        if (hEdit) {
            SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom - 90, SWP_NOZORDER);
        }

        // 调整输出框大小以适应窗口
        if (hOutputBox) {
            SetWindowPos(hOutputBox, NULL, 0, rcClient.bottom - 90, rcClient.right, 90, SWP_NOZORDER);
        }
        break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    WCHAR* dot;
    HDROP hDrop;
    WCHAR szFileName[MAX_PATH];
    HANDLE hFile;
    case WM_DROPFILES:
    {
        hDrop = (HDROP)wParam;
        DragQueryFile(hDrop, 0, szFileName, ARRAYSIZE(szFileName));

        hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = GetFileSize(hFile, NULL);
            std::vector<char> buffer(fileSize);
            DWORD bytesRead;
            if (ReadFile(hFile, buffer.data(), fileSize, &bytesRead, NULL)) {
                // 将 UTF-8 转换为宽字符
                int wideCharCount = MultiByteToWideChar(CP_UTF8, 0, buffer.data(), bytesRead, NULL, 0);
                if (wideCharCount > 0) {
                    std::vector<WCHAR> wideBuffer(wideCharCount);
                    MultiByteToWideChar(CP_UTF8, 0, buffer.data(), bytesRead, wideBuffer.data(), wideCharCount);
                    // 将文件内容设置到编辑框
                    SetWindowText(hEdit, wideBuffer.data());
                }
            }
            CloseHandle(hFile);
        }
        DragFinish(hDrop);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
// 入口点
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 保存实例句柄
    hInst = hInstance;
    // 定义窗口类
    const wchar_t CLASS_NAME[] = L"Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // 创建窗口
    HWND hWnd = CreateWindowEx(
        0, CLASS_NAME, L"BrainFuck Compiler v1.11.4", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, hInstance, nullptr
    );
    // 创建子窗口
    if (hWnd == nullptr) return 0;
    // 启用拖放
    DragAcceptFiles(hWnd, TRUE);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    ShowWindow(hOutputBox, nCmdShow);
    ifstream Gcc_path(L"gccpath.path");
    if (Gcc_path.is_open())
    {
        std::getline(Gcc_path, gccpath);
        Gcc_path.close();
    }
    else
    {
        MessageBox(hWnd, L"Failed:cannot read gccpath.path file to get G++ compiler path.\n Please check file name or configuration compiler path", L"BrainFuck Compiler", MB_OK | MB_ICONERROR);
    }
    // 进入消息循环
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}