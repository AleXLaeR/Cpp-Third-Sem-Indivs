/*
* Постановка задачи: Моя структура - личный кабинет пользователя на сайте - содержит данные об имени, фамилии, никнейме,
* дате рождения, пароле, (возрасте, персональном айди, ранге, секретном вопросе - заполн. авт.), ответе на секретный вопрос.
* Ранг - поле типа енам, секретный вопрос/ответ - вложенная структура.
* Возможные действия: ввод дополнительного пользователя (с записью в файл), удаление пользователя (с перезаписью в файл),
* поиск по одному или нескольким критериям, вывод информационной сводки о найденных пользователях,
* ввод секретных вопросов (с записью в файл), изменение секретных вопросов (с перезаписью в файл),
* восстановление пароля по индивидуальному секретному вопросу/ответу, изменение пароля (с перезаписью в файл),
* вывод полной информационной сводки по всем старо- и новодобавленным пользователям (чтение поструктурно из файла).
*
* Выполнил студент группы МФ-11, Лазаренко Александр.
*/

#define _CRT_SECURE_NO_WARNINGS

// дата рождения, год рождения
#define YEAR_POS 6 // позиция, с которой начинается год при использовании формата даты рождения XX.XX.XXXX
#define YOB_STR_LENGTH 4 // длина года при использовании формата даты рождения XX.XX.XXXX
#define DOB_STR_LENGTH 10 // длина даты рождения формата XX.XX.XXXX

// коды ошибок
#define WRONG_INPUT -3 // код ошибки при неверном вводе данных
#define BAD_REQUEST 400 // код выхода из программы при неуспешном открытии одного из файлов
#define NO_USERS_LEFT -2 // код выхода из программы при удалении всех пользователей
#define MAX_WRONG_INPUT_VALUE 3 // максимальное кол-во вводов, неверных формату одного из полей структуры

// мусорные/немусорные присваивания
#define JUNK_VALUE -10 // мусорное значения для первоначальной инициализации абстрактной структуры
#define STR_NULL_VALUE ' ' // искусственное мусорное значение для первоначальной инициализации полей абстрактной структуры
#define STR_NOT_NULL_VALUE "" // немусорное значение для инициализации полей абстрактной структуры, соответствующих критериям

// необходимые значения для формульного рассчета
#define MIL 1000000 // константа для обозначения миллиона
#define CURRENT_YEAR 2021 // текущий год - 2021

// константа - позиция первого символа пароля в записи структуры
#define PASSWORD_FILE_POS 404 // позиция адреса пароля (в байтах) от начала структуры в бинарном файле 

// константы-ограничители
#define NUMBER_OF_QUESTIONS 5 // кол-во возможных секретных вопросов
#define MAX_NUMBER_OF_FRIENDS 120 // максимальное кол-во друзей на сайте, число друзей генерируется по модулю этой константы
#define MAX_NUMBER_OF_CRITERIAS 10 // максимальное кол-во критериев поиска для ввода
#define CABINET_STRUCTURE_FIELD_LENGTH 100 // максимально возможная длина строки всех строковых полей структуры cabinet
#define MAX_SECRET_QUESTION_STR_LENGTH 100 // максимально возможная длина строки - сектретного вопроса

#define AGE_LOWER_LIMIT 1 // минимальный допустимый возраст 
#define AGE_UPPER_LIMIT 100 // максимальный допустимый возраст 
#define MIN_PASSWORD_LENGTH 8 // минимальное кол-во символов для создания пароля

#include <time.h> // для функций, использующих время srand, clock
#include <stdio.h>
#include <string.h> // для строковых функций strlen, strcpy, strpbrk
#include <stdlib.h> // для функций работы с памятью

// перечисляемый тип, содержащий 5 рангов, возможных для получения на сайте
typedef enum Rank_On_Site { Authorized_User, Advanced_User = 10, VIP = 20, Moderator = 50, Admin = 100 } user_rank;

typedef struct Restore_Password // структура, содержащая привязанный к аккаунту секретный вопрос и верный ответ на него
{
	char question[CABINET_STRUCTURE_FIELD_LENGTH];
	char answer[CABINET_STRUCTURE_FIELD_LENGTH];

} password;

typedef struct Personal_Cabinet // структура "личного кабинета" пользователя со всей информацией о нем
{
	char name[CABINET_STRUCTURE_FIELD_LENGTH];
	char surname[CABINET_STRUCTURE_FIELD_LENGTH];
	unsigned int age;
	char date_of_birth[CABINET_STRUCTURE_FIELD_LENGTH];

	char nickname[CABINET_STRUCTURE_FIELD_LENGTH];
	char password[CABINET_STRUCTURE_FIELD_LENGTH];
	long int personal_id;

	user_rank rank;
	password secret_question;

} cabinet;

/* Цель: записать в бинарный файл n заранее заданных секретных вопросов
* Функция принимает: бинарный файл для записи в него секретных вопросов
* Функция возвращает: ---
*/
void secret_question_input(FILE* secret_questions);

/* Цель: предложить пользователю изменить некоторые секретные вопросы или же оставить их по умолчанию
* Функция принимает: бинарный файл с секретными вопросами
* Функция возвращает: ---
*/
void secret_question_change(FILE* secret_questions);

/* Цель: Ввести всю запрашиваемую информацию о пользователе:
* (имя, фамилия, никнейм, дата рождения, пароль, ответ на секр. вопрос);
* возраст, id, ранг заполняются автоматически; секретный вопрос заполняется автоматически из файла, где они записаны
* Функция принимает: динам. массив типа cabinet, номер текущего пользователя для ввода,
* бинарный файл с информацией о пользователях, бинарный файл с секретными вопросами
* Функция возвращает: ---
*/
void user_information_input(cabinet* user_database, int current_user, FILE* user_list, FILE* secret_questions);

/* Цель: вычислить возраст пользователя
* Функция принимает: строку - дату рождения пользователя
* Функция возвращает: возраст пользователя
*/
int age_assignment(char* user_date_of_birth);

/* Цель: вычислить уникальный id пользователя по формуле
* Функция принимает: строку - дату рождения пользователя
* Функция возвращает: id пользователя
*/
int id_assignment(char* user_date_of_birth);

/* Цель: выбрать один из рангов пользователя в зависимости от переданного в функцию значения
* Функция принимает: кол-во друзей пользователя
* Функция возвращает: один из возможных рангов пользователя
*/
int rank_assignment(int number_of_friends);

/* Цель: дать возможность пользователю выбрать одно из действий:
* (осуществить поиск, удалить пользователя, восстановить пароль по секр. вопросу, сменить пароль, вывести список пользователей)
* Функция принимает: динам. массив типа cabinet, кол-во пользователей (кол-во элементов массива cabinet),
* бинарный файл с информацией о пользователях, бинарный файл с секретными вопросами
* Функция возвращает: ---
*/
void action_buff(cabinet* user_database, int number_of_users, FILE* user_list, FILE* secret_questions);

/* Цель: дать пользователю возможность ввести критерии, по которым он хочет осуществить поиск (имя, ранг, возраст, дата рождения...)
* Функция принимает: динам. массив типа cabinet, кол-во пользователей (кол-во элементов массива cabinet)
* Функция возвращает: кол-во введенных критериев, или -1, если ввод был неудачным
*/
int custom_search_criteria(int value_container[]);

/* Цель: осуществить поиск среди всех добавленных пользователей по заданным критериям
* Функция принимает: массив структур, кол-во пользователей (кол-во элементов массива cabinet)
* Функция возвращает: ---
*/
void custom_user_search(cabinet* user_database, int number_of_users);

/* Цель: вывести информацию о найденном (и соответствующему критериям) пользователе
* Функция принимает: массив структур, кол-во пользователей (кол-во элементов массива cabinet)
* Функция возвращает: ---
*/
void found_user_information_output(cabinet* user_database, int found_user);

/* Цель: восстановить пароль от аккаунта, предварительно запросив у пользователя ответ на соответствующий ему секретный вопрос
* Функция принимает: массив структур, кол-во пользователей (кол-во элементов массива cabinet)
* Функция возвращает: ---
*/
void password_restoration(cabinet* user_database, int number_of_users);

/* Цель: поменять пароль, предварительно запросив у пользователя id и старый пароль
* Функция принимает: массив структур, кол-во пользователей (кол-во элементов массива cabinet),
* бинарный файл с информацией о пользователях
* Функция возвращает: ---
*/
void change_password(cabinet* user_database, int number_of_users, FILE* user_list);

/* Цель: удалить некий аккаунт из списка, предварительно запросив у пользователя id и пароль от него
* Функция принимает: массив структур, кол-во пользователей (кол-во элементов массива cabinet),
* бинарный файл с информацией о пользователях
* Функция возвращает: кол-во оставшихся пользователей (n если удаление невозможно, n - 1  если удаление возможно)
*/
int delete_user(cabinet* user_database, int number_of_users, FILE* user_list);

/* Цель: найти соответствующий переданному в функцию id номер пользователя в массиве
* Функция принимает: массив структур, кол-во пользователей (кол-во элементов массива cabinet),
* введенное пользователем id
* Функция возвращает: номер пользователя, чей id совпадает с введенным, или -1, если такового id нету в базе данных
*/
int id_search(cabinet* user_database, int number_of_users, int id);

/* Цель: вывести всю доступную информацию о всех добавленных пользователях
* Функция принимает: массив структур, кол-во пользователей (кол-во элементов массива cabinet),
* бинарный файл с информацией о пользователях
* Функция возвращает: ---
*/
void provide_all_users_list(int number_of_users, FILE* user_list);

/* Цель: заполнить поля абстрактной структуры NULL-ами (если строковое поле) или -1 (если численное поле)
* Функция принимает: ---
* Функция возвращает: пустую структуру cabinet
*/
cabinet abstract_null_initialisation(void);

/* Цель: заполнить часть полей абстрактной структуры не мусорными значениями в зависимости от введенных критериев поиска
* Функция принимает: массив структур, массив, содержащий номера введенных пользователем критериев
* Функция возвращает: непустую структуру cabinet
*/
cabinet abstract_criteria_initialisation(cabinet abstract_user_database, int criterias_container[]);

/* Цель: проверить, был ли открыт/создан переданный в функцию файл и, если нет, завершить программу
* Функция принимает: бинарный файл
* Функция возвращает: ---
*/
void bad_request_check(FILE* file);

/* Цель: проверить, является ли введенный возраст верным (>= 1 и <= 100) и, если нет, завершить программу
* Функция принимает: возраст текущего пользователя
* Функция возвращает: ---
*/
void correct_age_check(int age);



#include "header.h"

int main(void)
{
	FILE* user_list = NULL, * secret_questions = NULL;
	clock_t start = clock(), stop; int number_of_users = 1, is_continue = 1;
	cabinet* user_database = (cabinet*)malloc(number_of_users * sizeof(cabinet));

	secret_questions = fopen("secret_questions.bin", "wb+"); bad_request_check(secret_questions);

	secret_question_input(secret_questions); secret_question_change(secret_questions);

	user_list = fopen("user_list.bin", "wb+"); bad_request_check(user_list);

	for (int i = 0; is_continue; i++)
	{
		user_information_input(user_database, i, user_list, secret_questions);
		printf("\nDo you want to add another user? (Yes - 1; No - 0): ");
		scanf_s("%d", &is_continue);

		if (is_continue)
			if (user_database != NULL) user_database = (cabinet*)realloc(user_database, ++number_of_users * sizeof(cabinet));
	}

	action_buff(user_database, number_of_users, user_list, secret_questions);

	fclose(user_list); fclose(secret_questions);
	printf("The program was active for %d seconds.\n", stop = clock() / CLK_TCK); system("pause");
	return 0;
}



#include "header.h"

void secret_question_input(FILE* secret_questions)
{
	char* temp_buff = (char*)malloc(MAX_SECRET_QUESTION_STR_LENGTH * sizeof(char));

	strcpy(temp_buff, "What is your father's name?"); fwrite(temp_buff, sizeof(*temp_buff), MAX_SECRET_QUESTION_STR_LENGTH, secret_questions);
	strcpy(temp_buff, "What was the name of your first pet?"); fwrite(temp_buff, sizeof(*temp_buff), MAX_SECRET_QUESTION_STR_LENGTH, secret_questions);
	strcpy(temp_buff, "What kind of job you dreamed of as a child?"); fwrite(temp_buff, sizeof(*temp_buff), MAX_SECRET_QUESTION_STR_LENGTH, secret_questions);
	strcpy(temp_buff, "What was the name of your best childhood friend?"); fwrite(temp_buff, sizeof(*temp_buff), MAX_SECRET_QUESTION_STR_LENGTH, secret_questions);
	strcpy(temp_buff, "What school have you attended in the sixth grade?"); fwrite(temp_buff, sizeof(*temp_buff), MAX_SECRET_QUESTION_STR_LENGTH, secret_questions);

	free(temp_buff);
}

void secret_question_change(FILE* secret_questions)
{
	int change_questions, question_number = 0;
	char* temp_buff = (char*)malloc(MAX_SECRET_QUESTION_STR_LENGTH * sizeof(char));

	rewind(secret_questions);
	printf("\nThe list of secret questions");
	for (int i = 0; i < NUMBER_OF_QUESTIONS; i++)
	{
		fread(temp_buff, sizeof(*temp_buff), MAX_SECRET_QUESTION_STR_LENGTH, secret_questions);
		printf("\n#%d: %s", i, temp_buff);
	}

	printf("\n\nWould you like to change them? (1 - yes, 0 - no): ");
	scanf_s("%d", &change_questions);

	while (change_questions)
	{
		printf("\nEnter the number of question you would like to change (0 to %d): ", NUMBER_OF_QUESTIONS - 1);
		scanf_s("%d", &question_number);

		if ((question_number < 0) || (question_number > NUMBER_OF_QUESTIONS - 1)) break;

		printf("\nEnter new question: ");
		getchar(); gets(temp_buff);

		fseek(secret_questions, MAX_SECRET_QUESTION_STR_LENGTH * question_number, SEEK_SET);
		fwrite(temp_buff, sizeof(*temp_buff), MAX_SECRET_QUESTION_STR_LENGTH, secret_questions);

		printf("\nDo you want to change another one? (1 - yes, 0 - no): ");
		scanf_s("%d", &change_questions);

		rewind(secret_questions);
		printf("\nThe list of new secret questions");
		for (int i = 0; i < NUMBER_OF_QUESTIONS; i++)
		{
			fread(temp_buff, sizeof(*temp_buff), MAX_SECRET_QUESTION_STR_LENGTH, secret_questions);
			printf("\n#%d: %s", i, temp_buff);
		}
		puts("");
	}
	free(temp_buff);
}

void user_information_input(cabinet* user_database, int current_user, FILE* user_list, FILE* secret_questions)
{
	srand(time(0));
	fseek(user_list, sizeof(cabinet) * current_user, SEEK_SET);

	printf("\nInput the user information, please\n");

	// name
	printf("\nEnter the name of a user: ");
	scanf("%s", user_database[current_user].name);

	// surname
	printf("\nEnter the surname of a user: ");
	scanf("%s", user_database[current_user].surname);

	// nickname
	printf("\nEnter the nickname of a user: ");
	scanf("%s", user_database[current_user].nickname);

	// password
	printf("\nEnter the password (it should have >= 8 characters): ");
	scanf("%s", user_database[current_user].password);
	while (strlen(user_database[current_user].password) < MIN_PASSWORD_LENGTH)
	{
		printf("\nYou have entered inappropriate password; Please, do it again: ");
		scanf("%s", user_database[current_user].password);
	}

	// secret question reading from file
	fseek(secret_questions, (rand() % NUMBER_OF_QUESTIONS) * MAX_SECRET_QUESTION_STR_LENGTH, SEEK_SET);
	fread(user_database[current_user].secret_question.question, sizeof(*user_database[current_user].secret_question.question), MAX_SECRET_QUESTION_STR_LENGTH, secret_questions);

	// secret question answer
	printf("\nEnter the secret question answer (\"%s\"): ", user_database[current_user].secret_question.question);
	scanf("%s", user_database[current_user].secret_question.answer);

	// date of birth
	printf("\nEnter the date of birth of a user (format is XX.XX.XXXX): ");
	scanf("%s", user_database[current_user].date_of_birth);

	while ((strlen(user_database[current_user].date_of_birth) != DOB_STR_LENGTH)
		|| (user_database[current_user].date_of_birth[2] != '.') 
		|| (user_database[current_user].date_of_birth[5] != '.'))
	{
		printf("\nYou have entered incorrect format; Please, enter the dob again: ");
		scanf("%s", user_database[current_user].date_of_birth);
	}
	// age
	user_database[current_user].age = age_assignment(user_database[current_user].date_of_birth);

	// id
	user_database[current_user].personal_id = id_assignment(user_database[current_user].date_of_birth);

	// rank
	user_database[current_user].rank = rank_assignment(rand() % MAX_NUMBER_OF_FRIENDS);

	fwrite(&user_database[current_user], sizeof(user_database[current_user]), 1, user_list);
}

int age_assignment(char* user_date_of_birth)
{
	int age; char* year_of_birth_to_int = (char*)malloc(YOB_STR_LENGTH * sizeof(char));

	for (int i = 0; i < YOB_STR_LENGTH; i++) year_of_birth_to_int[i] = user_date_of_birth[i + YEAR_POS];

	correct_age_check(age = CURRENT_YEAR - atoi(year_of_birth_to_int));
	free(year_of_birth_to_int); return age;
}

int id_assignment(char* user_date_of_birth)
{
	static int id_coef = 0; int symbol_counter = 0, user_id;
	char* dob_to_int = (char*)malloc(DOB_STR_LENGTH * sizeof(char));

	for (int i = 0; user_date_of_birth[i] != '\0'; i++)
	{
		if (user_date_of_birth[i] != '.')
			dob_to_int[symbol_counter++] = user_date_of_birth[i];
	}

	user_id = MIL + atoi(dob_to_int) + id_coef++;
	free(dob_to_int); 
	return user_id;
}

int rank_assignment(int number_of_friends)
{
	if (number_of_friends < 10) return Authorized_User;
	else if ((number_of_friends >= 10) && (number_of_friends < 20)) return Advanced_User;
	else if ((number_of_friends >= 20) && (number_of_friends < 50)) return VIP;
	else if ((number_of_friends >= 50) && (number_of_friends < 100))  return Moderator;
	else return Admin;
}

void action_buff(cabinet* user_database, int number_of_users, FILE* user_list, FILE* secret_questions)
{
	int action = JUNK_VALUE;

	while (action != 0)
	{
		printf("\nEnter the action you want to do (1 - criteria search, 2 - delete user, 3 - all user list,\n"
			" 4 - restore password, 5 - change password, 6 - add new user, 0 - exit program): ");
		scanf_s("%d", &action);

		switch (action)
		{
			case 0:
			{
				printf("\nYou have exited the program. ");
				if (user_database != NULL) free(user_database); return;
			}
			case 1: { custom_user_search(user_database, number_of_users); break; }
			case 2: { number_of_users = delete_user(user_database, number_of_users, user_list); break; }
			case 3: { provide_all_users_list(number_of_users, user_list); break; }
			case 4: { password_restoration(user_database, number_of_users); break; }
			case 5: { change_password(user_database, number_of_users, user_list); break; }
			case 6:
			{
				if (user_database != NULL) user_database = (cabinet*)realloc(user_database, ++number_of_users * sizeof(cabinet));
				user_information_input(user_database, number_of_users - 1, user_list, secret_questions); break;
			}
			default: { printf("\nInnacceptable action identificator.\n"); break; }
		}
	}
}

int custom_search_criteria(int criterias_container[])
{
	int i, number = JUNK_VALUE;

	printf("\nEnter the criterias you want to apply (1 - age, 2 - date of birth, 3 - name, 4 - surname, 5 - nickname,\n"
		"6 - personal id, 7 - rank, 0 - start searching): ");
	for (i = 0; number != 0; i++)
	{
		scanf_s("%d", &number);
		if ((number >= 0) && (number <= 7)) criterias_container[i] = number;
		else { printf("\nYou have entered a non-acceptable criteria identificator.\n"); return WRONG_INPUT; }
	}
	if (i - 1 == 0) { printf("\nYou haven't entered any criterias.\n"); return WRONG_INPUT; }

	return --i;
}

void custom_user_search(cabinet* user_database, int number_of_users)
{
	cabinet abstract_user_database = abstract_null_initialisation();
	int criteria_container[MAX_NUMBER_OF_CRITERIAS], number_of_criteria;
	int similarity_counter, found_user_counter = 0, wrong_input_counter = 0;

	if ((number_of_criteria = custom_search_criteria(criteria_container)) == WRONG_INPUT) return;
	abstract_user_database = abstract_criteria_initialisation(abstract_user_database, criteria_container);

	for (int i = 0; i < number_of_users; i++)
	{
		similarity_counter = 0;

		if (abstract_user_database.rank != JUNK_VALUE)
		{
			wrong_input_counter = 0;

			if (i == 0)
			{
				printf("\nEnter the rank identificator (0 - Authorized user, 10 - Advanced user, 20 - VIP, 50 - Moderator, 100 - Admin): ");
				scanf_s("%d", &abstract_user_database.rank);

				while ((abstract_user_database.rank != Authorized_User) && (abstract_user_database.rank != Advanced_User)
					&& (abstract_user_database.rank != VIP) && (abstract_user_database.rank != Moderator) &&
					(abstract_user_database.rank != Admin))
				{
					printf("\nYou have entered wrong rank identificator; You have %d more tries: ", MAX_WRONG_INPUT_VALUE - wrong_input_counter++);
					scanf_s("%d", &abstract_user_database.rank);

					if (wrong_input_counter == MAX_WRONG_INPUT_VALUE) return;
				}
			}

			if (user_database[i].rank == abstract_user_database.rank)
				similarity_counter++;
		}

		if (abstract_user_database.age != JUNK_VALUE)
		{
			wrong_input_counter = 0;

			if (i == 0)
			{
				printf("\nEnter the age (It should be >= 1 and <= 100): ");
				scanf_s("%d", &abstract_user_database.age);

				while ((abstract_user_database.age < AGE_LOWER_LIMIT) || (abstract_user_database.age > AGE_UPPER_LIMIT))
				{
					printf("\nYou have entered wrong age; You have %d more tries: ", MAX_WRONG_INPUT_VALUE - wrong_input_counter++);
					scanf_s("%d", &abstract_user_database.age);

					if (wrong_input_counter == MAX_WRONG_INPUT_VALUE) return;
				}
			}

			if (user_database[i].age == abstract_user_database.age)
				similarity_counter++;
		}

		if (abstract_user_database.date_of_birth[0] != STR_NULL_VALUE)
		{
			wrong_input_counter = 0;

			if (i == 0)
			{
				printf("\nEnter the date of birth (the format is XX.XX.XXXX): ");
				scanf("%s", abstract_user_database.date_of_birth);

				while ((abstract_user_database.date_of_birth[2] != '.') || (abstract_user_database.date_of_birth[5] != '.')
					|| (strlen(abstract_user_database.date_of_birth) != DOB_STR_LENGTH))
				{
					printf("\nYou have entered inappropriate format; Format is XX.XX.XXXX. You have %d more tries: ", MAX_WRONG_INPUT_VALUE - wrong_input_counter++);
					scanf("%s", abstract_user_database.date_of_birth);

					if (wrong_input_counter == MAX_WRONG_INPUT_VALUE) return;
				}
			}

			if (strcmp(abstract_user_database.date_of_birth, user_database[i].date_of_birth) == 0)
				similarity_counter++;
		}

		if (abstract_user_database.name[0] != STR_NULL_VALUE)
		{
			if (i == 0)
			{
				printf("\nEnter the name: ");
				scanf("%s", abstract_user_database.name);
			}

			if (strcmp(abstract_user_database.name, user_database[i].name) == 0)
				similarity_counter++;
		}

		if (abstract_user_database.surname[0] != STR_NULL_VALUE)
		{
			if (i == 0)
			{
				printf("\nEnter the surname: ");
				scanf("%s", abstract_user_database.surname);
			}

			if (strcmp(abstract_user_database.surname, user_database[i].surname) == 0)
				similarity_counter++;
		}

		if (abstract_user_database.nickname[0] != STR_NULL_VALUE)
		{
			if (i == 0)
			{
				printf("\nEnter the nickname: ");
				scanf("%s", abstract_user_database.nickname);
			}

			if (strcmp(abstract_user_database.nickname, user_database[i].nickname) == 0)
				similarity_counter++;
		}

		if (abstract_user_database.personal_id != JUNK_VALUE)
		{
			if (i == 0)
			{
				printf("\nEnter the personal id: ");
				scanf_s("%d", &abstract_user_database.personal_id);
			}

			if (abstract_user_database.personal_id == user_database[i].personal_id)
				similarity_counter++;
		}

		if (similarity_counter == number_of_criteria)
		{
			found_user_information_output(user_database, i);
			found_user_counter++;
		}
	}

	if (found_user_counter == 0) printf("\nNo users were found by the criterias.\n");
}

void found_user_information_output(cabinet* user_database, int found_user)
{
	printf("\nThe following user was found\n");
	printf("\nNickname: %s", user_database[found_user].nickname);
	printf("\nFull name is: %s %s", user_database[found_user].name, user_database[found_user].surname);
	printf("\nPersonal id is: %ld", user_database[found_user].personal_id);
	printf("\nRank on site: ");
	switch (user_database[found_user].rank)
	{
		case Authorized_User: { printf("Authorized User"); break; }
		case Advanced_User: { printf("Advanced User"); break; }
		case VIP: { printf("VIP"); break; }
		case Moderator: { printf("Moderator"); break; }
		case Admin: { printf("Admin"); break; }
	}
	printf("\nAge: %d", user_database[found_user].age);
	printf("\nDate of birth: %s", user_database[found_user].date_of_birth);
	puts("");
}

void password_restoration(cabinet* user_database, int number_of_users)
{
	int id, user;
	char* answer = (char*)malloc(CABINET_STRUCTURE_FIELD_LENGTH * sizeof(char));

	printf("\nIf you want to restore the password, please, enter the user id: ");
	printf("\nEnter the id: ");
	scanf_s("%d", &id);

	if ((user = id_search(user_database, number_of_users, id)) == WRONG_INPUT) { free(answer); return; };

	printf("\nThe id does exist.\nAnswer the following question - \"%s\": ", user_database[user].secret_question.question);
	scanf("%s", answer);

	if (strcmp(answer, user_database[user].secret_question.answer) == 0)
		printf("\nThe answer is correct.\nYour password is: %s.\n", user_database[user].password);
	else
		printf("\nThe answer is incorrect.\nThe password cannot be provided.");

	free(answer);
}

void change_password(cabinet* user_database, int number_of_users, FILE* user_list)
{
	int id, user;

	printf("\nIf you want to change the password, please, enter the user id: ");
	printf("\nEnter the id: ");
	scanf_s("%d", &id);

	if ((user = id_search(user_database, number_of_users, id)) == WRONG_INPUT) return;

	char* password = (char*)malloc(CABINET_STRUCTURE_FIELD_LENGTH * sizeof(char));

	printf("\nThe id does exist.\nEnter the account's password: ");
	scanf("%s", password);

	if (strcmp(password, user_database[user].password) == 0)
	{
		printf("\nThe password is correct.\nEnter the new password (it must have more than 8 characters): ");
		scanf("%s", user_database[user].password);

		while (strlen(user_database[user].password) < MIN_PASSWORD_LENGTH)
		{
			printf("\nWrong lenght. Enter the new password again (it must have more than 8 characters): ");
			scanf("%s", user_database[user].password);
		}

		fseek(user_list, (user * sizeof(cabinet)) + PASSWORD_FILE_POS, SEEK_SET);
		fwrite(user_database[user].password, sizeof(*user_database[user].password), CABINET_STRUCTURE_FIELD_LENGTH, user_list);
	}
	else
		printf("\nThe answer is incorrect.\nThe password change cannot be provided.");

	free(password);
}

int delete_user(cabinet* user_database, int number_of_users, FILE* user_list)
{
	int id, user_ind; cabinet struct_temp;

	printf("\nIf you want to delete a certain user, enter his personal id and password");
	printf("\nEnter the id: ");
	scanf_s("%d", &id);

	if ((user_ind = id_search(user_database, number_of_users, id)) == WRONG_INPUT) return number_of_users;

	if (id == user_database[user_ind].personal_id)
	{
		char* password = (char*)malloc(CABINET_STRUCTURE_FIELD_LENGTH * sizeof(char));

		printf("\nEnter the password: ");
		scanf("%s", password);

		if (strcmp(password, user_database[user_ind].password) == 0)
		{
			if (user_ind != number_of_users - 1)
			{
				for (int i = user_ind; i < number_of_users - 1; i++)
				{
					user_database[i].age = user_database[i + 1].age;
					user_database[i].rank = user_database[i + 1].rank;
					strcpy(user_database[i].name, user_database[i + 1].name);
					strcpy(user_database[i].surname, user_database[i + 1].surname);
					user_database[i].personal_id = user_database[i + 1].personal_id;
					strcpy(user_database[i].nickname, user_database[i + 1].nickname);
					strcpy(user_database[i].password, user_database[i + 1].password);
					strcpy(user_database[i].date_of_birth, user_database[i + 1].date_of_birth);
					strcpy(user_database[i].secret_question.answer, user_database[i + 1].secret_question.answer);
					strcpy(user_database[i].secret_question.question, user_database[i + 1].secret_question.question);

					fseek(user_list, (i + 1) * sizeof(cabinet), SEEK_SET); fread(&struct_temp, sizeof(cabinet), 1, user_list);
					fseek(user_list, i * sizeof(cabinet), SEEK_SET); fwrite(&struct_temp, sizeof(cabinet), 1, user_list);
				}
			}
			free(password); printf("\nCorrect data. Successful removal.\n");

			if (--number_of_users == 0) { printf("\nNo users were left.\n"); exit(NO_USERS_LEFT); }
			return number_of_users;
		}
	}
	printf("\nWrong data. Unsucessful removal.\n"); return number_of_users;
}

int id_search(cabinet* user_database, int number_of_users, int id)
{
	for (int i = 0; i < number_of_users; i++)
	{
		if (id == user_database[i].personal_id) return i;
	}
	printf("\nYou have entered a non-exististing id."); return WRONG_INPUT;
}

void provide_all_users_list(int number_of_users, FILE* user_list)
{
	cabinet temp;

	rewind(user_list);
	for (int i = 0; i < number_of_users; i++)
	{
		fread(&temp, sizeof(cabinet), 1, user_list);

		printf("\nNickname: %s", temp.nickname);
		printf("\nName is: %s", temp.name);
		printf("\nSurname is: %s", temp.surname);
		printf("\nPersonal id is: %ld", temp.personal_id);
		printf("\nPassword is: %s", temp.password);
		printf("\nRank on site: ");
		switch (temp.rank)
		{
			case Authorized_User: { printf("Authorized User"); break; }
			case Advanced_User: { printf("Advanced User"); break; }
			case VIP: { printf("VIP"); break; }
			case Moderator: { printf("Moderator"); break; }
			case Admin: { printf("Admin"); break; }
		}
		printf("\nAge: %d", temp.age);
		printf("\nDate of birth: %s", temp.date_of_birth);
		printf("\nSecret question: %s", temp.secret_question.question);
		printf("\nSecret question answer: %s", temp.secret_question.answer);

		puts("");
	}
}

cabinet abstract_null_initialisation(void)
{
	cabinet abstract_user_database;

	abstract_user_database.age = JUNK_VALUE;
	abstract_user_database.date_of_birth[0] = STR_NULL_VALUE;
	abstract_user_database.name[0] = STR_NULL_VALUE;
	abstract_user_database.surname[0] = STR_NULL_VALUE;
	abstract_user_database.nickname[0] = STR_NULL_VALUE;
	abstract_user_database.personal_id = JUNK_VALUE;
	abstract_user_database.rank = JUNK_VALUE;

	return abstract_user_database;
}

cabinet abstract_criteria_initialisation(cabinet abstract_user_database, int value_array[])
{
	for (int i = 0; value_array[i] != 0; i++)
	{
		switch (value_array[i])
		{
		case 1: { abstract_user_database.age = !JUNK_VALUE; break; }
		case 2: { abstract_user_database.date_of_birth[0] = STR_NOT_NULL_VALUE; break; }
		case 3: { abstract_user_database.name[0] = STR_NOT_NULL_VALUE; break; }
		case 4: { abstract_user_database.surname[0] = STR_NOT_NULL_VALUE; break; }
		case 5: { abstract_user_database.nickname[0] = STR_NOT_NULL_VALUE; break; }
		case 6: { abstract_user_database.personal_id = !JUNK_VALUE; break; }
		case 7: { abstract_user_database.rank = !JUNK_VALUE; break; }
		}
	}
	return abstract_user_database;
}

void bad_request_check(FILE* file)
{
	if (file == NULL)
	{
		printf("\nOne of the files cannot be opened.\n");
		exit(BAD_REQUEST);
	}
}

void correct_age_check(int age)
{
	if (age < AGE_LOWER_LIMIT)
	{
		printf("\nAre you from the future? :)\n");
		exit(EXIT_FAILURE);
	}
	else if (age > AGE_UPPER_LIMIT)
	{
		printf("\nAre you more than hundred years old? :)\n");
		exit(EXIT_FAILURE);
	}
}
