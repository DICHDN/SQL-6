#include<iostream>
#include<string>
#include<pqxx/pqxx>
#include<windows.h>

#pragma execution_character_set ("UTF-8")



class CustomersDB
{
	
	public:
		CustomersDB();
		void add_client(const std::string& first_name, const std::string& last_name, const std::string& email);
		void add_phone(int& client_id, std::string& phone);
		void edit_client(const std::string& first_name, const std::string& last_name, const std::string& email, int client_id);
		void serch_client(const std::string anydata);
		void delete_phone(int& client_id, std::string& phone);
		void delete_client(int& id);

protected:
	std::string conectionString =
		"host=localhost "
		"port=5432 "
		"dbname=Customers "
		"user=postgres "
		"password=postgres ";
	std::string first_name, last_name, email,phone;
	int client_id=0, phone_id=0;
	std::string d = " ";
	pqxx::connection c{ conectionString };
};
CustomersDB::CustomersDB()
{

	pqxx::transaction t(c); //�����, ��������� ��������� ��(�������)
	t.exec(
		"CREATE TABLE IF NOT EXISTS public.clients ("
		"id serial NOT NULL,"
		"first_name varchar NOT NULL,"
		"last_name varchar NULL,"
		"email varchar NOT NULL,"
		"CONSTRAINT client_pk PRIMARY KEY(id));"
			);
	t.exec(
		"CREATE TABLE IF NOT EXISTS public.numbertel("
		"id serial NOT NULL,"
		"telephone varchar NOT NULL,"
		"client_id int NOT NULL,"
		"CONSTRAINT numbertel_pk PRIMARY KEY(id),"
		"CONSTRAINT client_id FOREIGN KEY(client_id) REFERENCES public.clients(id));"
			);
	t.commit();
	c.prepare("add_client", "INSERT INTO clients(first_name,last_name,email) VALUES($1,$2,$3)");//�����, ����������� �������� ������ �������
	c.prepare("add_phone", "INSERT INTO numbertel(client_id,telephone) VALUES($1,$2)");//�����, ����������� �������� ������� ��� ������������� �������
	c.prepare("update","UPDATE clients SET first_name = $1, last_name = $2, email = $3 WHERE id = $4 ");//�����, ����������� �������� ������ � �������
	c.prepare("delete_phone", "DELETE from numbertel where telephone=$2 and client_id=$1");//�����, ����������� ������� ������� ��� ������������� �������
	c.prepare("delete_client", "DELETE from clients where id=$1");	//�����, ����������� ������� ������������� �������
	//�����, ����������� ����� ������� �� ��� ������(�����, �������, email-� ��� ��������)
}
void CustomersDB::add_client(const std::string& first_name, const std::string& last_name, const std::string& email)
{
	pqxx::transaction t(c);
	t.exec_prepared("add_client", t.esc(first_name), t.esc(last_name), t.esc(email));
	t.commit();
};
void CustomersDB::add_phone(int& client_id, std::string& phone)
{
	pqxx::transaction t(c);
	t.exec_prepared("add_phone", client_id, t.esc(phone));
	t.commit();
};
void CustomersDB::edit_client(const std::string& first_name, const std::string& last_name, const std::string& email, int client_id)
{
	pqxx::transaction t(c);
	t.exec_prepared("update", t.esc(first_name), t.esc(last_name), t.esc(email), client_id);
	t.commit();
};
void CustomersDB::serch_client(const std::string anydata)
{
	pqxx::transaction t(c);
	auto resultIt = t.query<int,std::string, std::string,std::string>("SELECT clients.id,first_name,last_name,email "
		"FROM clients " 
		"JOIN numbertel on client_id = clients.id " //������������� ��������� �� ��������
		"WHERE clients.first_name=\'" + t.esc(anydata) + "\' or " //����� �� �����
		"clients.last_name =\' " + t.esc(anydata) + "\' or " //����� �� �������
		"clients.email = \'" + t.esc(anydata) + "\' or " //����� �� ����������� �����
		"telephone =\'" + t.esc(anydata) + "\'" //����� �� ������ ��������
	);
	std::cout << "����� �� " << anydata << std::endl << "���������: " << std::endl;
	for (std::tuple <int, std::string, std::string, std::string> elem : resultIt)
	{
		std::cout << " #id: " << get<0>(elem) << " First name: " << get<1>(elem) << " Last name: " << get<2>(elem) << " e-mail: " << get<3>(elem) << std::endl;
	}
};
void CustomersDB::delete_phone(int& client_id, std::string& phone)
{
	pqxx::transaction t(c);
	t.exec_prepared("delete_phone", client_id, t.esc(phone));
	t.commit();
};
void CustomersDB::delete_client(int& id)
{
	pqxx::transaction t(c);
	t.exec_prepared("delete_client", id);
	t.commit();
};

int main()
{
	//setlocale(LC_ALL, "Russian");
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	std::cout << "Hi" << std::endl;
	try
	{
		CustomersDB clientDB;
		std::cout << "��������� ������ �������..." << std::endl;
		clientDB.add_client("Alexandr", "Ivanov", "kf - op@bk.ru");
		std::cout << "�������!" << std::endl;
		system("pause");
		int id = 1;
		std::cout << "��������� ����� �������� ������������� �������..." << std::endl;
		std::string number = "929234435";
		clientDB.add_phone(id, number);//��� �������� ���� ��������� ������������ ����� id ��������������� ������������� ����� ����� �� ������ �������� ����, � �� ����� �������������� ����� ������������
		std::cout << "�������!" << std::endl;
		system("pause");
		std::cout << "����������� ������������� �������..." << std::endl;
		clientDB.edit_client("Vasiliy", "Morozov", "MV@mail.ru", 1);
		std::cout << "�������!" << std::endl;
		system("pause");
		std::cout << "���� �������..." << std::endl;
		std::string serch_any = "Morozov";
		clientDB.serch_client(serch_any);// �� ������������ ��������, ����� ����������� �������� ������, ��� �� � ������� �� ������ ����...
		std::cout << std::endl;
		system("pause");
		int i = 1;
		std::cout << "������� ������� �������..." << std::endl;
		clientDB.delete_phone( i , number);
		std::cout << "�������!" << std::endl;
		system("pause");
		std::cout << "������� �������..." << std::endl;
		clientDB.delete_client(i);
		std::cout << "�������!" << std::endl;
	}
	catch (const std::exception& e)
	{
		
		std::cout << "ERROR " << e.what() << std::endl;

	}
	

	return 0;
}