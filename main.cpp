#include <iostream>
#include <exception>
#include "parcer.h"

int main() {
	setlocale(LC_ALL, "");

	try {
		IniParcer parcer("parcer.ini"); // файл parcer.ini необходимо перенести в каталог с exe файлом
//		parcer.print();
		
		auto value = parcer.getValue<double>("Section1.var1");
		std::cout << "Значение переменной Section1.var1 = " << value << std::endl;
	} 
	catch (const std::runtime_error& ex) {
		std::cout << ex.what() << std::endl;
	}
	catch (const std::logic_error& ex) {
		std::cout << ex.what() << std::endl;
	}
	catch (const std::invalid_argument& ex) {
		std::cout << ex.what() << std::endl;
	}
	catch (const std::out_of_range& ex) {
		std::cout << ex.what() << std::endl;
	}

	return 0;
}