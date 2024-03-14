﻿#include<iostream>
#include<memory>  // В этой библиотеке находятся все умные указатели, которые необходимы для автоматическо управления памятью
// 4 типа умных указателей: 
// auto_ptr c++17 - deprecated c++20- нерекоменндованы к использонию, начиная с 17-ого стандарта (надо принудительно указывать компилятору); с 20-ого стандарта удалён из библиотеки совсем
//				Позволяет портить данные (пустые указатели). Позволяли вести себя неправильно.
// unique_ptr - сейчас включает в себя в том числе и auto_ptr, с 11-ого стандарта (ранее недоступны). Гарантирует сущ-е единственной ссылку на объект в памяти.
//				является владеющим указателем, его время жизни связано с объектом, на который он указывает (убиваем указатель - убиватся объект)
//				Можно работать напрямую с объектом. Не портит данные, как auto_ptr, а используется так же, как и auto_ptr
//				Не контролирует, какой указатель в них пришёл, это забота программиста. Они только управляют памятью
//				"Личная собственность"
// shared_ptr - с 11-ого стандарта (ранее недоступны). Введёт учёт ссылок на один и тот же объект в памяти, которым управляет несколько модулей в программе.
//				Не позволяет портить данные
//				является владеющим указателем, (объект уничтожится только тогда, когда уничтожится последний shared-указатель на этот объект)
//				Можно работать напрямую с объектом. 
//				"Общедоступная собственность"/
// weak_ptr - с 11-ого стандарта (ранее недоступны). Только хранит адрес памяти внутри себя. Не служит не для чего, кроме передачи адресов между частями приложения.
//				Можно передавать между классами и частями библиотек
//				не является владеющим указателем. Срок жизни никак не влияет на срок жизни объекта.
//				Напрямую работать с объектом нельзя (нельзя напрямую обращаться с методам объекта)

#include<string>
#include<vector>

// Класс св-ва (базовый)
class Trait{
public:
	Trait(std::string name):name_(name){}
	std::string GetName()const { return name_; }
private:
	std::string name_;
};

class Color:public Trait{
public:
	Color(std::string name):Trait(name){}
};

class Material:public Trait {
public:
	Material(std::string name) :Trait(name) {}
};

class Toy {
public:
	// В св-ва цвета нельзя прислать ничего, кроме цвета. Так же с материалом
	Toy(std::string name, std::shared_ptr<Trait> color, std::shared_ptr<Trait> material) :name_(name), color_(color), material_(material) {}
	std::string GetName()const { return name_; }
	std::string GetColor()const{ return color_->GetName(); }
	std::string GetMaterial()const { return color_->GetName(); }
private:
	std::string name_;
	std::shared_ptr<Trait> color_; // Св-во цвета
	std::shared_ptr<Trait> material_; // Св-во материала
};

std::vector<std::shared_ptr<Color>> g_colors{ // Глобальный вектор цвета
	std::make_shared<Color>("red"),
	std::make_shared<Color>("green"),
	std::make_shared<Color>("blue")
};
std::vector<std::shared_ptr<Material>> g_materials{ // Глобальный вектор материалов
	std::make_shared<Material>("Glass"),
	std::make_shared<Material>("Stone"),
	std::make_shared<Material>("Wood")
};

// Функции для демонстрации особенностей умных указателей
// если не передавать по ссылке, то объект, на который указывает автоматический указатель, будет уничтожен (из-за этого автом-ие указатели больше не используют)
// в отличии от unique_ptr auto_ptr не препятстввует созданию своих копий
void F00(std::auto_ptr<std::string>&obj) { 
	std::cout << *obj;
	return; }
void F01(std::unique_ptr<std::string>&obj) {} // & чтобы работал u_ptr (строчка 62)
void F02(std::shared_ptr<std::string>obj) {
	//std::cout << '\n' << obj.use_count(); // Счётчик ссылок увеличился до 2-ух, если разместить здесь, то в консоли будет в порядке возрастания
	// Пока кол-во копий ссылок не достигло 10
	if (obj.use_count() != 10) F02(obj); // рекурсия (кол-во копий одновременно существующих в памяти)
	std::cout << '\n' << obj.use_count(); // Счётчик ссылок увеличился до 2-ух, в консоли в порядке убывания
}

int main1() {
	// ===================== Как не надо создавать умный указатель =================================

	auto tmp = new std::string("qwerty"); // Создаём указатель 
	/*
	// У умного указателя есть конструктор (), но делать, как ниже не стоит:
	std::unique_ptr<std::string> p_str ( new std::string("qwerty"));
	// т.к. объект уничтожиться при выходе за область видимости

	std::unique_ptr<std::string> p_str2(tmp);*/
	// На выходе из программы будет ошибка, поскольку будет попытка освободить память дважды 


	// ======================= Как надо создавать умные указатели ====================================

	// 1. Гарантированно даст указатель, по которому будет создан уникальный объект 
	// 2. нельзя неправильно создать объект в памяти
	auto u_ptr = std::make_unique<std::string>("qwerty"); // Создать копию нельзя, т.к. у этого умного указателя нет конструктора копирования, память будет освобождена корректно
	// make всегда создаёт новый указатель
		
	std::auto_ptr<std::string> a_ptr(new std::string("qwerty"));

	// Функции не работают, т.к. умные указатели не допустят двойного высвобождения одной и той же выделенной памяти
	//F00(tmp);
	//F01(tmp);
	//F02(tmp);

	F00(a_ptr); 
	F01(u_ptr);
	
	delete tmp;

	std::shared_ptr<std::string> sh_ptr = std::make_shared<std::string>("asdasd");

	F02(sh_ptr);

	return 0;
}

int main() {
	std::vector<Toy> toys; 
	toys.reserve(9); // Резервируем вектор под 9 шт. игрушек
	int q{ 1 };
	for (int i = 0; i < g_colors.size(); ++i) {
		for (int j = 0; j < g_materials.size(); ++j) {
			toys.emplace_back( // заполняем вектор с игрушками объектами "Игрушка" (все комбинации цветов и материалов)
				std::string("Toy") + std::to_string(q),
				g_colors[i],
				g_materials[j]
			);
		}
	}
	// Далее можно вывести игрушки в консоль
	for (auto& i : toys) {
		i.GetColor();
	}

	return 0;
}