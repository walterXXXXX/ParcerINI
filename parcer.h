#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <memory>
#include <any>

// ������ ������ IniParcer ��������� � ������ ������ INI �����. 
// ������ �������� � ����������� std::map ��� �������� ������� �� �����:
// �������� ������ � ����������. 
// ������������ ��������� "��������" ������ ��� �������� ������ std::map sections:
// ���� - ��� ������ std::string, �������� - ��������� ���������� ������ std::map
// ��������� ���������� ������ std::map: 
// ���� - ��� ���������� std::string, �������� - �������� ���������� ���� std::any 

class IniParcer {
public:
	IniParcer(std::string fileName);

	template <class T>
	T getValue(std::string varName) {  // varName = "section.value"
		auto it = std::find(varName.cbegin(), varName.cend(), '.');
		if (it == varName.cend() || it == varName.cbegin() || it == (varName.cend() - 1))
			throw std::logic_error("������ � ����� ������������� ���������� <" + _fileName + ">\n");

		std::string _sec = std::string(varName.cbegin(), it);
		std::string _var(it + 1, varName.cend());
		if (sections.find(_sec) == sections.cend()) {
			std::string res = "������! ����������� ������ � ������ " + _sec + "\n ���������� ����� ������: \n";
			for (auto& const section : sections)
				res += (section.first + '\n');
			throw std::invalid_argument(res);
		}

		for (auto& const variable : sections[_sec])
			if (variable.first == _var && variable.second.type() == typeid(T))
				return std::any_cast<T>(variable.second);

		std::string res = "� ������ ����������� �������� ��� ������������� ���������� " + _var + 
			" ���� " + (typeid(T).name() == typeid(std::string).name() ? "std::string" : typeid(T).name()) + 
			". ��������� ����� ���������� ������: \n";

		for (auto& const variable : sections[_sec]) {
			std::string varType = variable.second.type().name();
			if (varType == typeid(std::string).name())
				varType = "std::string";
			res += (variable.first + " ���� " + varType + '\n');
		}
		throw std::invalid_argument(res);
	}

	void print(); // ������ ����������� 

private:
	std::string _fileName = "";

 	std::map <std::string, std::map <std::string, std::any>> sections;

	void addSection(std::string name); // ��������� ����� ������

	void addVariable(std::string secName, std::string varName, std::any variable); // ��������� ���������� � ��������� ������ secName 

	void trimStr(std::string& str); // ��������������� �������, ������� ������� � ��������� � ������ � ����� ������
};