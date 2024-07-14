#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

void printFileInfo(const string &path, const string &name, int indent, bool useAccessTime, bool htmlMode, bool showTypeInfo);
void listDirectory(const string &path, int indent, int maxDepth, int currentDepth, int spaces, bool useAccessTime, bool htmlMode, bool sortAlphabetically, bool showTypeInfo);
string visualizeSize(off_t size, bool htmlMode);
string visualizeAge(time_t t, bool useAccessTime, bool htmlMode);
string generateHTMLColor(int ageLevel);
void printHTMLHeader(const vector<string> &args);
void printHTMLFooter();
string getFileType(const string &filePath);
string mapFileTypeToColor(const string &fileType);
void printHTMLColorKey();

map<string, string> fileTypeColors = {
    {"ASCII text", "#0000FF"},
    {"C++ source", "#00FF00"},
    {"executable", "#FF0000"},
    {"directory", "#FFFF00"},
    {"symbolic link", "#FF00FF"},
    {"default", "#000000"}  // Default color for unspecified types
};

int main(int argc, char *argv[]) {
    int indentSpaces = 4;
    int maxDepth = 2;
    bool useAccessTime = false;
    bool htmlMode = false;
    bool sortAlphabetically = false;
    bool showTypeInfo = false;
    vector<string> files;
    vector<string> args;

    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "-i=", 3) == 0) {
            indentSpaces = atoi(argv[i] + 3);
            if (indentSpaces < 1 || indentSpaces > 8) {
                cerr << "Indent spaces should be in the range of 1 to 8." << endl;
                return 1;
            }
        } else if (strncmp(argv[i], "-d=", 3) == 0) {
            maxDepth = atoi(argv[i] + 3);
            if (maxDepth < 0 || maxDepth > 8) {
                cerr << "Recursion depth should be in the range of 0 to 8." << endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            useAccessTime = true;
        } else if (strcmp(argv[i], "-h") == 0) {
            htmlMode = true;
        } else if (strcmp(argv[i], "-s") == 0) {
            sortAlphabetically = true;
        } else if (strcmp(argv[i], "-t") == 0) {
            showTypeInfo = true;
        } else {
            files.push_back(argv[i]);
        }
    }

    if (htmlMode) {
        printHTMLHeader(args);
    }

    if (files.empty()) {
        listDirectory(".", 0, maxDepth, 0, indentSpaces, useAccessTime, htmlMode, sortAlphabetically, showTypeInfo);
    } else {
        for (const auto &file : files) {
            struct stat fileInfo;
            if (lstat(file.c_str(), &fileInfo) < 0) {
                perror("lstat");
                continue;
            }
            if (S_ISDIR(fileInfo.st_mode)) {
                if (htmlMode) {
                    cout << "<p style='margin-left:" << 0 << "px;'><b>" << file << "/</b></p>" << endl;
                } else {
                    cout << file << "/" << endl;
                }
                listDirectory(file, 1, maxDepth, 1, indentSpaces, useAccessTime, htmlMode, sortAlphabetically, showTypeInfo);
            } else {
                printFileInfo("", file, 0, useAccessTime, htmlMode, showTypeInfo);
            }
        }
    }

    if (htmlMode) {
        printHTMLFooter();
        if (showTypeInfo) {
            printHTMLColorKey();
        }
    }

    return 0;
}

void printFileInfo(const string &path, const string &name, int indent, bool useAccessTime, bool htmlMode, bool showTypeInfo) {
    struct stat fileInfo;
    string fullPath = path + "/" + name;

    if (lstat(fullPath.c_str(), &fileInfo) < 0) {
        perror("lstat");
        return;
    }

    for (int i = 0; i < indent; ++i) {
        if (!htmlMode) {
            cout << " ";
        }
    }

    string fileName = name;
    if (S_ISDIR(fileInfo.st_mode)) {
        fileName += "/";
    } else if (fileInfo.st_mode & S_IXUSR) {
        fileName += "*";
    } else if (S_ISLNK(fileInfo.st_mode)) {
        fileName += "@";
    }

    string size = visualizeSize(fileInfo.st_size, htmlMode);
    string age = visualizeAge(useAccessTime ? fileInfo.st_atime : fileInfo.st_mtime, useAccessTime, htmlMode);

    if (htmlMode) {
        cout << "<p style='margin-left:" << indent * 2 << "px;'>" << size << " " << age << " " << fileName;
    } else {
        cout << fileName << " " << size << " " << age;
    }

    if (showTypeInfo && !S_ISDIR(fileInfo.st_mode) && !(fileInfo.st_mode & S_IXUSR) && !S_ISLNK(fileInfo.st_mode)) {
        string fileType = getFileType(fullPath);
        if (htmlMode) {
            string color = mapFileTypeToColor(fileType);
            cout << " <font color='" << color << "'>" << fileType << "</font>";
        } else {
            cout << " " << fileType;
        }
    }

    if (htmlMode) {
        cout << "</p>" << endl;
    } else {
        cout << endl;
    }
}

void listDirectory(const string &path, int indent, int maxDepth, int currentDepth, int spaces, bool useAccessTime, bool htmlMode, bool sortAlphabetically, bool showTypeInfo) {
    DIR *dir;
    struct dirent *entry;
    vector<string> entries;

    if (!(dir = opendir(path.c_str()))) return;

    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        entries.push_back(entry->d_name);
    }

    if (sortAlphabetically) {
        sort(entries.begin(), entries.end());
    }

    for (const auto &entryName : entries) {
        printFileInfo(path, entryName, indent * spaces, useAccessTime, htmlMode, showTypeInfo);

        string fullPath = path + "/" + entryName;
        struct stat fileInfo;
        if (lstat(fullPath.c_str(), &fileInfo) < 0) {
            perror("lstat");
            continue;
        }

        if (S_ISDIR(fileInfo.st_mode) && currentDepth < maxDepth) {
            listDirectory(fullPath, indent + 1, maxDepth, currentDepth + 1, spaces, useAccessTime, htmlMode, sortAlphabetically, showTypeInfo);
        }
    }

    closedir(dir);
}

string visualizeSize(off_t size, bool htmlMode) {
    if (!htmlMode) {
        int hashes = 0;
        string result;

        if (size >= 10000000)
            hashes = 7;
        else if (size >= 1000000)
            hashes = 6;
        else if (size >= 100000)
            hashes = 5;
        else if (size >= 10000)
            hashes = 4;
        else if (size >= 1000)
            hashes = 3;
        else if (size >= 100)
            hashes = 2;
        else if (size >= 10)
            hashes = 1;

        for (int i = 0; i < hashes; ++i) result += "#";
        return result;
    } else {
        int fontSize = 1;

        if (size >= 10000000)
            fontSize = 7;
        else if (size >= 1000000)
            fontSize = 6;
        else if (size >= 100000)
            fontSize = 5;
        else if (size >= 10000)
            fontSize = 4;
        else if (size >= 1000)
            fontSize = 3;
        else if (size >= 100)
            fontSize = 2;

        return "<font size='" + to_string(fontSize) + "'>";
    }
}

string visualizeAge(time_t fileTime, bool useAccessTime, bool htmlMode) {
    time_t now = time(nullptr);
    double diff = difftime(now, fileTime);
    string result;
    int dots = 0;

    if (diff < 60)
        dots = 1;
    else if (diff < 3600)
        dots = 2;
    else if (diff < 86400)
        dots = 3;
    else if (diff < 604800)
        dots = 4;
    else if (diff < 2592000)
        dots = 5;
    else if (diff < 31536000)
        dots = 6;
    else
        dots = 7;

    if (!htmlMode) {
        for (int i = 0; i < dots; ++i) result += ".";
        return result;
    } else {
        return generateHTMLColor(dots);
    }
}

string generateHTMLColor(int ageLevel) {
    static const vector<string> colors = {
        "#000000", "#333333", "#666666", "#999999", "#CCCCCC", "#EEEEEE", "#FFFFFF"};
    return "<font color='" + colors[ageLevel - 1] + "'>";
}

void printHTMLHeader(const vector<string> &args) {
    cout << "<html>" << endl;
    cout << "<head><title>";
    for (const auto &arg : args) {
        cout << arg << " ";
    }
    cout << "</title></head>" << endl;
    cout << "<body>" << endl;
}

void printHTMLFooter() {
    cout << "</body>" << endl;
    cout << "</html>" << endl;
}

string getFileType(const string &filePath) {
    string quotedPath = "'" + filePath + "'";
    string command = "file -b " + quotedPath;
    char buffer[128];
    string result = "";

    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        perror("popen");
        return "";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);
    result.erase(result.find_last_not_of(" \n\r\t") + 1);  // Trim trailing whitespace
    return result;
}

string mapFileTypeToColor(const string &fileType) {
    for (const auto &pair : fileTypeColors) {
        if (fileType.find(pair.first) != string::npos) {
            return pair.second;
        }
    }

    return fileTypeColors["default"];  // Default color
}

void printHTMLColorKey() {
    cout << "<p><b>File Type Color Key:</b></p>" << endl;
    cout << "<ul>" << endl;
    for (const auto &pair : fileTypeColors) {
        cout << "<li><font color='" << pair.second << "'>" << pair.first << "</font></li>" << endl;
    }
    cout << "</ul>" << endl;
}
