#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <memory>
#include <any>

// Объект класса IniParcer считывает и хранит данные INI файла. 
// Данные хранятся в контейнерах std::map для быстрого доступа по ключу:
// названию секции и переменной. 
// Используется контейнер "верхнего" уровня для хранения секций std::map sections:
// ключ - имя секции std::string, значение - контейнер переменных секции std::map
// Контейнер переменных секции std::map: 
// ключ - имя переменной std::string, значение - значение переменной типа std::any 

class IniParcer {
public:
	IniParcer(std::string fileName);

	template <class T>
	T getValue(std::string varName) {  // varName = "section.value"
		auto it = std::find(varName.cbegin(), varName.cend(), '.');
		if (it == varName.cend() || it == varName.cbegin() || it == (varName.cend() - 1))
			throw std::logic_error("Ошибка в имени запрашиваемой переменной <" + _fileName + ">\n");

		std::string _sec = std::string(varName.cbegin(), it);
		std::string _var(it + 1, varName.cend());
		if (sections.find(_sec) == sections.cend()) {
			std::string res = "Ошибка! Отсутствует секция с именем " + _sec + "\n Допустимые имена секций: \n";
			for (auto& const section : sections)
				res += (section.first + '\n');
			throw std::invalid_argument(res);
		}

		for (auto& const variable : sections[_sec])
			if (variable.first == _var && variable.second.type() == typeid(T))
				return std::any_cast<T>(variable.second);

		std::string res = "В секции отсутствует значение для запрашиваемой переменной " + _var + 
			" типа " + (typeid(T).name() == typeid(std::string).name() ? "std::string" : typeid(T).name()) + 
			". Доступные имена переменных секции: \n";

		for (auto& const variable : sections[_sec]) {
			std::string varType = variable.second.type().name();
			if (varType == typeid(std::string).name())
				varType = "std::string";
			res += (variable.first + " типа " + varType + '\n');
		}
		throw std::invalid_argument(res);
	}

	void print(); // печать содержимого 

private:
	std::string _fileName = "";

 	std::map <std::string, std::map <std::string, std::any>> sections;

	void addSection(std::string name); // добавляет новую секцию

	void addVariable(std::string secName, std::string varName, std::any variable); // добавляет переменную в контейнер секции secName 

	void trimStr(std::string& str); // вспомогательная функция, удаляет пробелы и табуляции в начале и конце строки
};