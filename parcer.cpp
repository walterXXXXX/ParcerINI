#include <fstream>
#include <sstream>
#include <exception>
#include <string>
#include <typeinfo>
#include "parcer.h"

IniParcer::IniParcer(std::string fileName) {
	std::ifstream fin(fileName);
	if (!fin.is_open()) throw std::runtime_error("���� " + fileName + " �� ������\n");
	_fileName = fileName;

	int countStr = 0;			// ������� �����
	std::string str = "";		// ����������� ������
	std::string secName = "";	// ��� ������� ������
	bool isEmpty = true;		// ���� ���������� ��������� ������ � �������

	while (std::getline(fin, str)) {
		countStr++; // ����� ������� ������

		// ������� �� ������ ����������� - ���, ������� � ������� ';'
		str.erase(find(str.cbegin(), str.cend(), ';'), str.end()); 
		trimStr(str); // � ������� � ������ � � ����� ������ 
//		std::cout << str << std::endl;
		if (str == "") continue; // ���� � ����� ����� �������� ������, ��������� �� ���������
		
		auto it = str.cbegin();
		if (*it != '[' && isEmpty)
			throw std::logic_error("������ � ������ " + std::to_string(countStr) +
				". �� ������� ���������� �� ����� ������\n");

		if (*it == '[') {
			// ��������� ��� ������
			auto it1 = find(it, str.cend(), ']');
			if (it1 == str.cend())
				throw std::logic_error("������ � ������ " + std::to_string(countStr) +
					". �� ������ ������ ']' � ���������� ����� ������\n");
			if (it1 != str.cend()-1)
				throw std::logic_error("������ � ������ " + std::to_string(countStr) +
					". ������������ ������� ����� ����� ������ []..\n"); // ��������� ����������� � ������� ��� �������
			
			secName = std::string(it + 1, it1);
			if (secName == "")
				throw std::logic_error("������ � ������ " + std::to_string(countStr) +
					". ����������� ��� ������\n");
			addSection(secName);
			isEmpty = false;
		}
		else {
			// ��������� ��� ����������
			auto it2 = find(it, str.cend(), '=');
			if (it2 == str.cend())
				throw std::logic_error("������ � ������ " + std::to_string(countStr) +
					". ����������� ���� ��������� '=' � ���������� ����������\n");
			if (it2 == str.cbegin())
				throw std::logic_error("������ � ������ " + std::to_string(countStr) +
					". ����������� ��� ���������� ����� �� ����� '='\n");

			// �������� ��� � �������� ���������� ����� � ������ �� =
			std::string varName(str.cbegin(), (it2));
			std::string varValue((it2 + 1), str.cend());
			// ������� ������� � ��������� � ������ � ����� ���������� ��������
			trimStr(varName);
			trimStr(varValue);
			// ��������� ������������ ����� ����������
			if (find(varName.cbegin(), varName.cend(), ' ') != varName.cend() || 
				find(varName.cbegin(), varName.cend(), '\t') != varName.cend())
					throw std::logic_error("������ � ������ " + std::to_string(countStr) +
						". ������������ ������ � ����� ����������\n");

			// ���������� ��� ���������� � ��������� �� �������� 
			enum class Vartype {
				INT,
				DOUBLE,
				STRING
			};
			Vartype varType = Vartype::INT;
			if (varValue == "")
				varType = Vartype::STRING;
			for (auto it = varValue.cbegin(); it < varValue.cend(); it++) {
				if ((*it < '0' || *it > '9') && *it != '.' && *it != '-') {
					varType = Vartype::STRING;
					break;
				}
				if (*it == '-' && it != varValue.cbegin()) {
					varType = Vartype::STRING;
					break;
				}
				if (*it == '.' && (varType == Vartype::DOUBLE || it == varValue.cbegin() || it == varValue.cend() - 1)) {
					varType = Vartype::STRING;
					break;
				}
				if (*it == '.' && varType == Vartype::INT) {
					varType = Vartype::DOUBLE;
				}
			}

			switch (varType)
			{
			case Vartype::INT:
				addVariable(secName, varName, std::stoi(varValue));
				break;
			case Vartype::DOUBLE:
				addVariable(secName, varName, std::stod(varValue));
				break;
			case Vartype::STRING:
				addVariable(secName, varName, varValue);
				break;
			default:
				break;
			}

		}
	}	
	if (isEmpty)
		throw std::logic_error("���� " + fileName + " ����\n");
	fin.close();
}

void IniParcer::addSection(std::string secName) {
	// ��������� ������ secName, ���� ��� ��� �� ����������, ����� �����
	sections.insert(std::make_pair(secName, std::map<std::string, std::any>()));
}

void IniParcer::addVariable(std::string secName, std::string varName, std::any variable) {
	if (sections.find(secName) == sections.cend())
		throw std::invalid_argument("���������� �������� ����������, �.�. ������ � ������ " + secName + " �� �������\n");
	// ��������� ���������� varName � ��������� ������ � ����������� ��������, ���� ��� ��� ����������
	sections[secName].insert_or_assign(varName, variable);
}	

void IniParcer::trimStr(std::string& str) { 
	// ��������������� �������, ������� ������� � ��������� � ������ � ����� ������
	auto it1 = str.cbegin();
	auto it2 = str.cend();
	while ((it1 != it2) && (*it1 == ' ' || *it1 == '\t'))
		it1++;
	while ((it1 != it2) && (*(it2-1) == ' ' || *(it2-1) == '\t'))
		it2--;
	str = std::string(it1, it2);
}

void IniParcer::print() {	// ������ �����������
	std::cout << std::endl;
	for (auto& sec : sections) {
		std::cout << "[" << sec.first << "]" << std::endl;
		for (auto& var : sec.second) {
			std::cout << var.first << "=";
			if (var.second.type() == typeid(int))
				std::cout << std::any_cast<int>(var.second);
			if (var.second.type() == typeid(double))
				std::cout << std::any_cast<double>(var.second);
			if (var.second.type() == typeid(std::string))
				std::cout << std::any_cast<std::string>(var.second);
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	std::cout << "--------------\n";
}
