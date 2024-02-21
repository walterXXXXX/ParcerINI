#include <fstream>
#include <sstream>
#include <exception>
#include <string>
#include <typeinfo>
#include "parcer.h"

IniParcer::IniParcer(std::string fileName) {
	std::ifstream fin(fileName);
	if (!fin.is_open()) throw std::runtime_error("Файл " + fileName + " не найден\n");
	_fileName = fileName;

	int countStr = 0;			// счетчик строк
	std::string str = "";		// считываемая строка
	std::string secName = "";	// имя текущей секции
	bool isEmpty = true;		// флаг отсутствия считанных секций в парсере

	while (std::getline(fin, str)) {
		countStr++; // номер текущей строки

		// убираем из строки комментарий - все, начиная с символа ';'
		str.erase(find(str.cbegin(), str.cend(), ';'), str.end()); 
		trimStr(str); // и пробелы в начале и в конце строки 
//		std::cout << str << std::endl;
		if (str == "") continue; // если в итоге стока осталась пустой, переходим на следующую
		
		auto it = str.cbegin();
		if (*it != '[' && isEmpty)
			throw std::logic_error("Ошибка в строке " + std::to_string(countStr) +
				". Не найдено объявление ни одной секции\n");

		if (*it == '[') {
			// считываем имя секции
			auto it1 = find(it, str.cend(), ']');
			if (it1 == str.cend())
				throw std::logic_error("Ошибка в строке " + std::to_string(countStr) +
					". Не найден символ ']' в объявлении имени секции\n");
			if (it1 != str.cend()-1)
				throw std::logic_error("Ошибка в строке " + std::to_string(countStr) +
					". Недопустимые символы после имени секции []..\n"); // возможный комментарий и пробелы уже удалены
			
			secName = std::string(it + 1, it1);
			if (secName == "")
				throw std::logic_error("Ошибка в строке " + std::to_string(countStr) +
					". Отсутствует имя секции\n");
			addSection(secName);
			isEmpty = false;
		}
		else {
			// считываем имя переменной
			auto it2 = find(it, str.cend(), '=');
			if (it2 == str.cend())
				throw std::logic_error("Ошибка в строке " + std::to_string(countStr) +
					". Отсутствует знак равенства '=' в объявлении переменной\n");
			if (it2 == str.cbegin())
				throw std::logic_error("Ошибка в строке " + std::to_string(countStr) +
					". Отсутствует имя переменной слева от знака '='\n");

			// получаем имя и значение переменной слева и справа от =
			std::string varName(str.cbegin(), (it2));
			std::string varValue((it2 + 1), str.cend());
			// удаляем пробелы и табуляции в начале и конце полученных значений
			trimStr(varName);
			trimStr(varValue);
			// проверяем допустимость имени переменной
			if (find(varName.cbegin(), varName.cend(), ' ') != varName.cend() || 
				find(varName.cbegin(), varName.cend(), '\t') != varName.cend())
					throw std::logic_error("Ошибка в строке " + std::to_string(countStr) +
						". Недопустимый символ в имени переменной\n");

			// определяем тип переменной и сохраняем ее значение 
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
		throw std::logic_error("Файл " + fileName + " пуст\n");
	fin.close();
}

void IniParcer::addSection(std::string secName) {
	// добавляем секцию secName, если она еще не существует, иначе игнор
	sections.insert(std::make_pair(secName, std::map<std::string, std::any>()));
}

void IniParcer::addVariable(std::string secName, std::string varName, std::any variable) {
	if (sections.find(secName) == sections.cend())
		throw std::invalid_argument("Невозможно добавить переменную, т.к. секция с именем " + secName + " не найдена\n");
	// добавляем переменную varName в найденную секцию с перезаписью значения, если она уже существует
	sections[secName].insert_or_assign(varName, variable);
}	

void IniParcer::trimStr(std::string& str) { 
	// вспомогательная функция, удаляет пробелы и табуляции в начале и конце строки
	auto it1 = str.cbegin();
	auto it2 = str.cend();
	while ((it1 != it2) && (*it1 == ' ' || *it1 == '\t'))
		it1++;
	while ((it1 != it2) && (*(it2-1) == ' ' || *(it2-1) == '\t'))
		it2--;
	str = std::string(it1, it2);
}

void IniParcer::print() {	// печать содержимого
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
