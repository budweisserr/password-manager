#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <algorithm>
#include <vector>

#pragma once

class PasswordManager {
private:
    std::string mainPass;
    std::string filePath;
    std::string fileCategories;
    std::string fileTimestamps;
    std::vector<std::string> categories;
    std::vector<std::string> fileStr;
    const std::string capSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string lowSet = "abcdefghijklmnopqrstuvwxyz";
    const std::string numSet = "0123456789";
    const std::string symSet = "!@#$%^&_";
public:
    bool compare(std::string& a, std::string& b) {
        a.erase(a.find(':'));
        b.erase(b.find(':'));
        return a < b;
    }
    void setFilePath(const std::string& p_path) {
        filePath = p_path;
    }
    void setMainPass(const std::string& pass) {
        mainPass = pass;
    }
    void setFile() {
        std::fstream file;
        fileStr.push_back(fileCategories);
        fileStr.insert(fileStr.begin(), 1, fileTimestamps);
        file.open(filePath, std::ios::out);
        for (int i = 0; i != fileStr.size(); i++) {
            file << fileStr[i];
            file << "\n";
        }
        file.close();
    }
    void readFile() {
        std::fstream file;
        fileStr.clear();
        file.open(filePath, std::ios::in);
        file >> fileTimestamps;
        while (!file.eof()) {
            std::string current;
            file >> current;
            if (current != "\n") fileStr.push_back(current);
            current.clear();
        }
        file.close();
        if (fileTimestamps == "") return;
        fileStr.pop_back();
        if (fileStr.size() == 0) return;
        fileCategories = fileStr.back();
        fileStr.pop_back();
    }
    void addTimestamp() {
        readFile();
        time_t now = time(0);
        struct tm* newtime = localtime(&now);;
        std::string strTime;
        std::string day = std::to_string(newtime->tm_mday);
        if (day.length() == 1) {
            day.insert(day.begin(), '0');
        }
        std::string month = std::to_string(1 + newtime->tm_mon);
        if (month.length() == 1) {
            month.insert(month.begin(), '0');
        }
        std::string hour = std::to_string(newtime->tm_hour);
        if (hour.length() == 1) {
            hour.insert(hour.begin(), '0');
        }
        std::string minutes = std::to_string(newtime->tm_min);
        if (minutes.length() == 1) {
            minutes.insert(minutes.begin(), '0');
        }
        strTime = day + month + hour + minutes;
        if (fileTimestamps == "") {
            fileTimestamps += strTime;
        }
        else {
            fileTimestamps = fileTimestamps + ":" + strTime;
        }
        setFile();
    }
    void printTimestamps() {
        readFile();
        std::string print;
        int counter = 0;
        for (int i = 0; i < fileTimestamps.length(); i++) {
            if (fileTimestamps[i] == ':') {
                std::cout << print.substr(0, 2) << '.' << print.substr(2, 2) << ' ' << print.substr(4, 2) << ":" << print.substr(6, 2) << std::endl;
                print.clear();
                continue;
            }
            print += fileTimestamps[i];
        }
    }
    void decryptFileFunc() {
        for (int i = 0; i != fileStr.size(); i++) {
            for (int j = 0; j != fileStr[i].length(); j++) {
                if (fileStr[i][j] == ':') continue;
                fileStr[i][j] = decrypt(fileStr[i][j]);
            }
        }
    }
    char encrypt(char p_letter) {
        int convert = ((int)mainPass[0] + (int)mainPass[mainPass.length() - 1]) / mainPass.length();
        return p_letter + convert;
    }
    char decrypt(char p_letter) {
        int convert = ((int)mainPass[0] + (int)mainPass[mainPass.length() - 1]) / mainPass.length();
        return p_letter - convert;
    }
    void addPass(const std::string& p_Name, const std::string& p_Pass, const std::string& p_Categ) {
        readFile();
        std::string add, compare, compareCateg;
        compare = p_Categ;
        for (int i = 0; i != compare.length(); i++) {
            compare[i] = encrypt(compare[i]);
        }
        if (fileCategories.find(compare) == std::string::npos) {
            std::cout << "This category does not exist. Try again." << std::endl;
            return;
        }
        else {
            for (int i = 0; i != p_Name.length(); i++) {
                add += encrypt(p_Name[i]);
            }
            add += ':';
            for (int i = 0; i != p_Pass.length(); i++) {
                add += encrypt(p_Pass[i]);
            }
            add += ':';
            for (int i = 0; i != p_Categ.length(); i++) {
                add += encrypt(p_Categ[i]);
            }
            fileStr.push_back(add);
            setFile();
            std::cout << "Password successfully added!" << std::endl;
        }
    }
    std::vector<std::string> searchPass(const std::string& p_Str, const int& param) {
        readFile();
        decryptFileFunc();
        std::vector<std::string> result;
        if (param == 1) {
            std::string name;
            for (int i = 0; i != fileStr.size(); i++) {
                name = fileStr[i].substr(0, p_Str.size());
                std::string currentPass;
                if (name == p_Str) {
                    for (int j = name.size() + 1; j != fileStr[i].length(); j++) {
                        if (fileStr[i][j] == ':') break;
                        currentPass += fileStr[i][j];
                    }
                }
                if (currentPass != "") {
                    result.push_back(currentPass);
                }
            }
        }
        else if (param == 2) {
            for (int i = 0; i != fileStr.size(); i++) {
                int j = fileStr[i].length() - 1;
                std::string category, temp, currentPass;
                for (; j >= 0; j--) {
                    if (fileStr[i][j] == ':') break;
                    temp += fileStr[i][j];
                }
                if (temp.length() == p_Str.length()) {
                    for (int k = temp.length() - 1; k >= 0; k--) {
                        category += temp[k];
                    }
                }
                if (category == p_Str) {
                    temp.clear();
                    j--;
                    for (; j >= 0; j--) {
                        if (fileStr[i][j] == ':') break;
                        temp += fileStr[i][j];
                    }
                    for (int k = temp.length() - 1; k >= 0; k--) {
                        currentPass += temp[k];
                    }
                }
                if (currentPass != "") {
                    result.push_back(currentPass);
                }
            }
        }
        return result;
    }

    void addCategory(std::string& p_Categ) {
        readFile();
        for (int i = 0; i != p_Categ.length(); i++) {
            p_Categ[i] = encrypt(p_Categ[i]);
        }
        if (fileCategories == "") {
            fileCategories += p_Categ;
        }
        else {
            fileCategories = fileCategories + ':' + p_Categ;
        }
        setFile();
    }

    void removeCategory(std::string& p_Categ) {
        readFile();
        for (int i = 0; i != p_Categ.length(); i++) {
            p_Categ[i] = encrypt(p_Categ[i]);
        }
        for (int i = 0; i < fileStr.size(); i++) {
            if (fileStr[i].find(p_Categ) != std::string::npos) {
                fileStr.erase(fileStr.begin() + i);
            }
        }
        if (fileCategories.find(p_Categ) == 0) {
            fileCategories.erase(fileCategories.find(p_Categ), p_Categ.length() + 1);
            std::cout << "Category successfully deleted with matching passwords!" << std::endl;
        }
        else if (fileCategories.find(p_Categ) == std::string::npos) {
            std::cout << "No such category to delete" << std::endl;
        }
        else {
            fileCategories.erase(fileCategories.find(p_Categ) - 1, p_Categ.length() + 1);
            std::cout << "Category successfully deleted with matching passwords!" << std::endl;
        }
        setFile();

    }

    void removePass(std::string& p_Name, std::string& p_Pass, std::string& p_Categ) {
        readFile();
        bool deleted = false;
        std::cout << "Are you sure you want to delete password? [y|n]" << std::endl;
        std::string choise;
        std::cin >> choise;
        if (choise == "y") {
            std::string remove;
            for (int i = 0; i != p_Name.length(); i++) {
                remove += encrypt(p_Name[i]);
            }
            remove += ':';
            for (int i = 0; i != p_Pass.length(); i++) {
                remove += encrypt(p_Pass[i]);
            }
            remove += ':';
            for (int i = 0; i != p_Categ.length(); i++) {
                remove += encrypt(p_Categ[i]);
            }
            for (int i = 0; i != fileStr.size(); i++) {
                if (fileStr[i] == remove) {
                    fileStr.erase(fileStr.begin() + i);
                    std::cout << "Password successfully deleted!" << std::endl;
                    deleted = true;
                    break;
                }
            }
        }
        else {
            std::cout << "Returning to menu..." << std::endl;
            return;
        }
        if (!deleted) std::cout << "No matching!";
        setFile();
    }

    void editPass(std::string& p_Name, std::string& p_Pass, std::string& p_Categ) {
        readFile();
        std::string str, newPass, oldPass;
        bool edited = false;
        for (int i = 0; i != p_Name.length(); i++) {
            str += encrypt(p_Name[i]);
        }
        str += ':';
        for (int i = 0; i != p_Pass.length(); i++) {
            str += encrypt(p_Pass[i]);
            oldPass += encrypt(p_Pass[i]);
        }
        str += ':';
        for (int i = 0; i != p_Categ.length(); i++) {
            str += encrypt(p_Categ[i]);
        }
        for (int i = 0; i != fileStr.size(); i++) {
            if (fileStr[i] == str) {
                std::cout << "Enter new password: ";
                std::cin >> newPass;
                for (int j = 0; j != newPass.length(); j++) {
                    newPass[j] = encrypt(newPass[j]);
                }
                str.erase(str.find(oldPass), oldPass.size());
                str.insert(str.find(':') + 1, newPass);
                fileStr[i] = str;
                std::cout << "Password successfully edited!" << std::endl;
                edited = true;
                break;
            }
        }
        if (!edited) std::cout << "No matching!" << std::endl;
        setFile();
    }
    static bool nameComparator(std::string& a, std::string& b) {
        return a.substr(0, a.find(":")) < b.substr(0, b.find(":"));
    }
    static bool categoryComparator(std::string& a, std::string& b) {
        return a.substr(a.rfind(":")) < b.substr(b.rfind(":"));
    }

    void sortPass(const int& param) {
        readFile();
        decryptFileFunc();
        bool sorted = false;
        std::vector<std::string> toSort = fileStr;
        if (param == 1) {
            std::cout << "Sorted passwords by name: " << std::endl;
            std::sort(toSort.begin(), toSort.end(), nameComparator);
            for (int i = 0; i != toSort.size(); i++) {
                size_t stop = toSort[i].find(":");
                size_t rstop = toSort[i].rfind(":");
                std::cout << "Name: " << toSort[i].substr(0, stop) << std::endl;
                std::cout << "Password: " << toSort[i].substr(stop + 1, rstop - stop - 1) << std::endl;
                std::cout << "Category: " << toSort[i].substr(rstop + 1) << std::endl;
                std::cout << std::endl;
                sorted = true;
            }
        }
        else if (param == 2) {
            std::cout << "Sorted passwords by category: " << std::endl;
            std::sort(toSort.begin(), toSort.end(), categoryComparator);
            for (int i = 0; i != toSort.size(); i++) {
                size_t stop = toSort[i].find(":");
                size_t rstop = toSort[i].rfind(":");
                std::cout << "Name: " << toSort[i].substr(0, stop) << std::endl;
                std::cout << "Password: " << toSort[i].substr(stop + 1, rstop - stop - 1) << std::endl;
                std::cout << "Category: " << toSort[i].substr(rstop + 1) << std::endl;
                std::cout << std::endl;
                sorted = true;
            }
        }
        if (!sorted) std::cout << "File does not have elements to sort!" << std::endl;
    }

    std::string generatePass(bool caps, bool nums, bool symb, int length) {
        std::string password = "";
        std::string combinedSet = lowSet;
        combinedSet += caps ? capSet : "";
        combinedSet += nums ? numSet : "";
        combinedSet += symb ? symSet : "";
        char randChar;
        srand(time(NULL));
        for (int i = 0; i < length; i++) {
            randChar = combinedSet[rand() % combinedSet.length() + 1];
            while (randChar == '\0')
                randChar = combinedSet[rand() % combinedSet.length() + 1];
            password += randChar;
        }
        return password;
    }
};