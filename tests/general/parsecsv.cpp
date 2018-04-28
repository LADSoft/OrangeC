#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

struct enderecos
{
	string nome;
	string empresa;
	string endereco;
	string bairro;
	string cep;
	string cidade;
	string estado;
};

vector<string> &split(const string &s, char delimiter, vector<string> &elementos)
{
	stringstream ss(s);
	string item;
	while (getline(ss, item, delimiter))
		elementos.push_back(item);
	return elementos;
}

vector<string> split(const string &s, char delimiter)
{
	vector<string> elementos;
	split(s, delimiter, elementos);
	return elementos;
}

void parseCSV(vector<enderecos> &detalhes)
{
	string line;
	ifstream  file("enderecos.csv");
	while (getline(file, line))
	{
		enderecos tempEnd;
		vector<string> elementos = split(line, ';');
		tempEnd.nome = elementos[0];
		tempEnd.empresa = elementos[1];
		tempEnd.endereco = elementos[2];
		tempEnd.bairro = elementos[3];
		tempEnd.cep = elementos[4];
		tempEnd.cidade = elementos[5];
		tempEnd.estado = elementos[6];
		detalhes.push_back(tempEnd);
	}
	file.close();
}

void GenPcl(vector<enderecos> &detalhes)
{
	char ESC = (char)0x1B;
	char PAG = (char)0x0C;

	int lin = 0;
	int linha = lin;
	int coluna = 3670;
	int fontSize = 11;
	int linDist = fontSize * 4;

	std::stringstream fmt;
	fmt << ESC << "(s1p" << fontSize << "v0s0b16602T";
	string font = fmt.str();

	ofstream outfile;
	outfile.open("out.pcl", ios::out);

	outfile << ESC << "%-12345X@PJL ENTER LANGUAGE=PCL" << endl;
	outfile << ESC << "E" << endl;
	outfile << ESC << "&l27A" << endl; // A3 - l27A | A4 - l26A
	outfile << ESC << "&u300D" << endl;
	outfile << ESC << "&s1C" << endl;
	outfile << ESC << "&l0S" << endl;
	outfile << ESC << "&l1O" << endl; // 1 - Landscape | 0 - retrato
	outfile << ESC << "&l3E" << endl; // TOP MARGIN
	outfile << ESC << "(12U" << endl;

	for (int i = 0; i < detalhes.size(); i++)
	{
		struct enderecos det = detalhes[i];

		linha = lin;
		outfile << ESC << "*p" << linha << "y" << coluna << "X" << font << det.nome << endl;
		linha += linDist;

		outfile << ESC << "*p" << linha << "y" << coluna << "X" << font << det.empresa << endl;
		linha += linDist;

		outfile << ESC << "*p" << linha << "y" << coluna << "X" << font << det.endereco << endl;
		linha += linDist;

		if (det.bairro.length() != 0)
		{
			outfile << ESC << "*p" << linha << "y" << coluna << "X" << font << det.bairro << endl;
			linha += linDist;
		}

		outfile << ESC << "*p" << linha << "y" << coluna << "X" << font << det.cep << " - " << det.cidade << " - " << det.estado << endl;
		linha += linDist;

		outfile << PAG << endl;
	}
	/*for each (enderecos det in detalhes)
	{
		linha = lin;
		outfile << ESC << "*p" << linha << "y" << coluna << "X" << font << det.nome << endl;
		linha += linDist;

		outfile << ESC << "*p" << linha << "y" << coluna << "X" << font << det.empresa << endl;
		linha += linDist;

		outfile << ESC << "*p" << linha << "y" << coluna << "X" << font << det.endereco << endl;
		linha += linDist;

		if (det.bairro.length() != 0)
		{
			outfile << ESC << "*p" << linha << "y" << coluna << "X" << font << det.bairro << endl;
			linha += linDist;
		}

		outfile << ESC << "*p" << linha << "y" << coluna << "X" << font << det.cep << " - " << det.cidade << " - " << det.estado << endl;
		linha += linDist;

		outfile << PAG << endl;
	}*/
	outfile << ESC << "E" << endl;
	outfile << ESC << "%-12345" << endl;

	outfile.close();
}

int main()
{
	vector<enderecos> detalhes;
	parseCSV(detalhes);
	GenPcl(detalhes);
	return 0;
}
