
PRAGMA foreign_keys = ON;

/*****************************************************************************************************/
/*****************************************************************************************************/
/**********************************                       ********************************************/
/**********************************   Создание   таблиц   ********************************************/
/**********************************                       ********************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/

CREATE TABLE company
(
	comp_id INTEGER PRIMARY KEY AUTOINCREMENT,
	comp_name VARCHAR(100) NOT NULL									/*название компании*/
);

CREATE TABLE project
(
	proj_id INTEGER PRIMARY KEY AUTOINCREMENT,		
	proj_name VARCHAR(100) NOT NULL,             									/*название проекта*/
	date_begin TIMESTAMP NOT NULL,													/*дата начала*/
	date_end_expect	TIMESTAMP NOT NULL,												/*планируемая дата окончания*/
	is_complete INTEGER CHECK (is_complete = 0 OR is_complete = 1)	/*является ли проект завершенным*/
);

CREATE TABLE developer
(
	dev_login CHAR(20) NOT NULL PRIMARY KEY,						/*Логин разработчика*/
	password VARCHAR(20) NOT NULL,              					/*Пароль разработчика*/
	first_name VARCHAR(20) NOT NULL,								/*Имя разработчика*/
	last_name VARCHAR(20) NOT NULL,             					/*Фамилия разработчика*/
	is_adm INTEGER CHECK (is_adm = 0 OR is_adm = 1),				/*Является ли разработчик администратором*/
	comp_id INTEGER NOT NULL,										/*внешний ключ на таблицу company*/
	FOREIGN KEY(comp_id) REFERENCES company(comp_id) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE problem
(
	prob_id INTEGER PRIMARY KEY AUTOINCREMENT,
	prob_name VARCHAR(100) NOT NULL,											/*название задачи*/
	declaration VARCHAR(200),													/*описание задачи*/
	time_plan INTEGER NOT NULL CHECK (time_plan > 0),							/*планируемое время работы (в часах)*/
	prob_status INTEGER NOT NULL CHECK (prob_status >= 0 AND prob_status <= 2),	/*статус задачи*/
	proj_id INTEGER NOT NULL,													/*id проекта, к которому принадлежит задача*/
	FOREIGN KEY(proj_id) REFERENCES project(proj_id) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE contract
(
	cont_num INTEGER NOT NULL PRIMARY KEY,												/*номер контракта*/
	date_conclusion TIMESTAMP NOT NULL,													/*дата и время заключения договора*/
	cont_status INTEGER NOT NULL CHECK (cont_status >= 0 AND cont_status <= 2),			/*статус контракта*/
	comp_id INTEGER NOT NULL,            												/*компания, с которой заключен договор*/
	proj_id INTEGER NOT NULL,            												/*проект, на выполнение которого заключен договор*/
	FOREIGN KEY(comp_id) REFERENCES company(comp_id) ON DELETE CASCADE ON UPDATE CASCADE,
	FOREIGN KEY(proj_id) REFERENCES project(proj_id) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE report
(
	rep_id INTEGER PRIMARY KEY AUTOINCREMENT,
	date_begin TIMESTAMP NOT NULL,									/*дата начала*/
	date_end TIMESTAMP NOT NULL,									/*дата окончания*/
	declaration VARCHAR(200),										/*описание проделанной деятельности*/
	dev_login CHAR(20) NOT NULL,									/*логин разработчика*/
	prob_id INTEGER NOT NULL,            							/*id проблемы, на основании которой создан этот отчет*/
	FOREIGN KEY(dev_login) REFERENCES developer(dev_login) ON DELETE CASCADE ON UPDATE CASCADE,
	FOREIGN KEY(prob_id) REFERENCES problem(prob_id) ON DELETE CASCADE ON UPDATE CASCADE
);	

CREATE TABLE problem_depend
(
	depend_id INTEGER PRIMARY KEY AUTOINCREMENT,
	prob1_id INTEGER NOT NULL,										/*задача, от которой зависят*/
	prob2_id INTEGER NOT NULL,										/*задача, которая зависит*/
	UNIQUE(prob1_id, prob2_id),
	FOREIGN KEY(prob1_id) REFERENCES problem(prob_id) ON DELETE CASCADE ON UPDATE CASCADE,
	FOREIGN KEY(prob2_id) REFERENCES problem(prob_id) ON DELETE CASCADE ON UPDATE CASCADE	
);

CREATE TABLE distr_proj_dev
(
	distr_id INTEGER PRIMARY KEY AUTOINCREMENT,
	proj_id INTEGER NOT NULL,													/*проект*/	
	dev_login CHAR(20) NOT NULL,												/*логин разработчика*/
	is_manager INTEGER CHECK (is_manager = 0 OR is_manager = 1),	/*является ли разработчик менеджером на данном проекте*/
	UNIQUE(proj_id, dev_login),
	FOREIGN KEY(proj_id) REFERENCES project(proj_id) ON DELETE CASCADE ON UPDATE CASCADE,	
	FOREIGN KEY(dev_login) REFERENCES developer(dev_login) ON DELETE CASCADE ON UPDATE CASCADE
);

BEGIN TRANSACTION;
/*****************************************************************************************************/
/*****************************************************************************************************/
/**********************************                       ********************************************/
/**********************************   Заполнение таблиц   ********************************************/
/**********************************                       ********************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/

/*Заполнение таблицы company*/

INSERT INTO company (comp_name) VALUES ('Roga i koputa');
INSERT INTO company (comp_name) VALUES ('Rhonda');
INSERT INTO company (comp_name) VALUES ('Hot lawa');
INSERT INTO company (comp_name) VALUES ('Nokia');
INSERT INTO company (comp_name) VALUES ('Apple');
INSERT INTO company (comp_name) VALUES ('Microsoft');

/*Заполнение таблицы project*/

INSERT INTO project (proj_name, date_begin, date_end_expect, is_complete) VALUES ('Compiler', '2011-01-10 09:00:00', '2011-04-29 17:00:00', 0);
INSERT INTO project (proj_name, date_begin, date_end_expect, is_complete) VALUES ('Online game', '2011-01-10 09:00:00', '2011-05-31 17:00:00', 0);
INSERT INTO project (proj_name, date_begin, date_end_expect, is_complete) VALUES ('Paint', '2011-01-10 09:00:00', '2011-01-31 17:00:00', 1);

/*Заполнение таблицы developer*/

INSERT INTO developer (dev_login, password, first_name, last_name, is_adm, comp_id) VALUES ('Admin', 'password', 'Admin', 'Administrator', 1, 5);
INSERT INTO developer (dev_login, password, first_name, last_name, is_adm, comp_id) VALUES ('Petya', '123', 'Petr', 'Petrov', 0, 1);
INSERT INTO developer (dev_login, password, first_name, last_name, is_adm, comp_id) VALUES ('Vanya', '456', 'Ivan', 'Ivanov', 0, 1);
INSERT INTO developer (dev_login, password, first_name, last_name, is_adm, comp_id) VALUES ('Sidor', '789', 'Sidor', 'Sidorov', 0, 1);

/*Заполнение таблицы problem*/
/*Compiler*/
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Lexer', 'Lexical scan', 10, 2, 1);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Parser (expression)', 'Parse expression', 10, 2, 1);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Parser', 'Parsing', 36, 1, 1);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Generator', 'Code generation', 20, 0, 1);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Optimizer (low)', 'Low optimization', 6, 0, 1);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Optimizer (high)', 'High optimization', 20, 0, 1);

/*Paint*/
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('FreeHand', 'Mode FreeHand', 3, 2, 3);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Line && Rectangle', 'Realization of construction line and rectangle', 4, 2, 3);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Tools', 'Advanced set of tools', 8, 2, 3);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Zoom && Scroll', 'Zooming and scrolling', 4, 2, 3);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Save && Open', 'Saving, opening and loading ', 4, 2, 3);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Select', 'Selection', 4, 2, 3);
INSERT INTO problem (prob_name, declaration, time_plan, prob_status, proj_id) VALUES ('Edit', 'Editing', 4, 2, 3);
--INSERT INTO problem (id, name, declaration, time_plan, status, project_id) VALUES ((SELECT gen_id(problem_id, 1) FROM RDB$DATABASE), 'Undo && Redo', 'Отменить и посторить', 6, 0, 3);
--INSERT INTO problem (id, name, declaration, time_plan, status, project_id) VALUES ((SELECT gen_id(problem_id, 1) FROM RDB$DATABASE), 'Timer', 'Работа с таймером', 4, 0, 3);

/*Заполнение таблицы contract*/

INSERT INTO contract (cont_num, date_conclusion, cont_status, comp_id, proj_id) VALUES (1, '2010-12-31 12:00:00', 1, 1, 1);	--Roga i koputa - Compiler
INSERT INTO contract (cont_num, date_conclusion, cont_status, comp_id, proj_id) VALUES (2, '2010-12-31 12:00:00', 1, 1, 2);	--Roga i koputa - Online game
INSERT INTO contract (cont_num, date_conclusion, cont_status, comp_id, proj_id) VALUES (3, '2010-12-31 12:00:00', 1, 1, 3);	--Roga i koputa - Paint
INSERT INTO contract (cont_num, date_conclusion, cont_status, comp_id, proj_id) VALUES (4, '2010-12-31 13:00:00', 1, 2, 1);	--Rhonda - Compiler
INSERT INTO contract (cont_num, date_conclusion, cont_status, comp_id, proj_id) VALUES (6, '2010-12-31 15:00:00', 1, 3, 2);	--Hot lawa - Online game
INSERT INTO contract (cont_num, date_conclusion, cont_status, comp_id, proj_id) VALUES (7, '2010-12-31 16:00:00', 1, 4, 2);	--Nokia - Online game
INSERT INTO contract (cont_num, date_conclusion, cont_status, comp_id, proj_id) VALUES (5, '2010-12-31 14:00:00', 1, 5, 1);	--Apple - Compiler

/*Заполнение таблицы report*/
/*Compiler*/
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-10 09:00:00', '2011-01-10 17:00:00', 'Develop lexer', 'Sidor', 1);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-11 09:00:00', '2011-01-11 17:00:00', 'Develop parser (parse expression)', 'Sidor', 2);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-12 09:00:00', '2011-01-12 12:00:00', 'Develop parser (parse expression)', 'Sidor', 2);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-12 13:00:00', '2011-01-12 17:00:00', 'Develop parser', 'Sidor', 3);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-13 09:00:00', '2011-01-13 17:00:00', 'Develop parser', 'Sidor', 3);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-13 09:00:00', '2011-01-13 17:00:00', 'Develop parser', 'Vanya', 3);

/*Paint*/
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-10 09:00:00', '2011-01-10 13:00:00', 'Develop mode FreeHand', 'Petya', 7);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-10 14:00:00', '2011-01-10 17:00:00', 'Develop realization of construction line and rectangle', 'Petya', 8);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-11 09:00:00', '2011-01-11 16:00:00', 'Develop advanced set of tools', 'Petya', 9);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-11 16:00:00', '2011-01-11 17:00:00', 'Develop advanced set of tools', 'Vanya', 9);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-12 10:00:00', '2011-01-12 14:00:00', 'Develop zooming and scrolling', 'Petya', 10);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-12 14:00:00', '2011-01-12 17:00:00', 'Develop saving and opening', 'Petya', 11);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-13 09:00:00', '2011-01-13 10:00:00', 'Develop saving and opening', 'Petya', 11);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-14 09:00:00', '2011-01-14 13:00:00', 'Develop selection', 'Petya', 12);
INSERT INTO report (date_begin, date_end, declaration, dev_login, prob_id) VALUES ('2011-01-14 13:00:00', '2011-01-14 17:00:00', 'Develop editing', 'Petya', 13);

/*Заполнение таблицы problem_depend*/
/*Compiler*/
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (1, 2);
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (2, 3);
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (3, 4);
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (4, 5);
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (3, 6);
/*Paint*/
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (7, 8);
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (8, 9);
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (9, 10);
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (10, 11);
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (11, 12);
INSERT INTO problem_depend (prob1_id, prob2_id) VALUES (12, 13);

/*Заполнение таблицы distr_proj_dev*/
INSERT INTO distr_proj_dev (proj_id, dev_login, is_manager) VALUES (1, 'Sidor', 1);
INSERT INTO distr_proj_dev (proj_id, dev_login, is_manager) VALUES (1, 'Vanya', 0);
INSERT INTO distr_proj_dev (proj_id, dev_login, is_manager) VALUES (3, 'Petya', 1);
INSERT INTO distr_proj_dev (proj_id, dev_login, is_manager) VALUES (3, 'Vanya', 0);

COMMIT;