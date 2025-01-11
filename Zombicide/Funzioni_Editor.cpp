#include "dati.h"
#include "utilità.h"
#include "Matteo Draw.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost\filesystem.hpp>
using namespace std;

bool confronta_cordinate_tessera_con_riquadro(int& sx,int& sy,int& sw,int& sh,int i, int j,int x_dim_tessera, int y_dim_tessera,int x_riquadro, int y_riquadro,int x_dim_riquadro, int y_dim_riquadro)
{
	if (x_riquadro >= (x_dim_tessera*j) && x_riquadro <= ((x_dim_tessera*j) + x_dim_tessera) && y_riquadro >= (y_dim_tessera*i) && y_riquadro <= ((y_dim_tessera*i) + y_dim_tessera))
	{
		sx = x_riquadro - (x_dim_tessera*j);
		sy = y_riquadro - (y_dim_tessera*i);
		if (sx + x_dim_riquadro <= x_dim_tessera)
			sw = sx + x_dim_riquadro;
		else
			sw = x_dim_tessera;
		if (sy + y_dim_riquadro <= y_dim_tessera)
			sh = sy + y_dim_riquadro;
		else
			sh = y_dim_tessera;
		return true;
	}
	else
		return false;
}
void aggiungi_parametri_ruotati_di_oggetto_tessere_editor(Parametri_Tessera p, Parametri_Tessera& t, int y_dim_tessera)
{
	Parametri_Tessera pt;
	size_t lato_matrice_quadrata = sqrt(p.x_zona.size());

	//90 gradi in senso orario
	//ruoto i nuovi indici in una matrice a parte per poi usarli per prelevare le info da t nell'ordine giusto e li metto dritti con push back
	// su pt
	vector<vector<int>> m;
	int contatore = 0;
	//Matrice normale
	for (size_t i = 0; i < lato_matrice_quadrata; i++)
	{
		vector<int> v;
		for (size_t j = 0; j < lato_matrice_quadrata; j++)
		{
			v.push_back(contatore);
			contatore++;
		}
		m.push_back(v);
	}
	contatore = 0;
	//Matrice ruotata
	for (int i = (int)lato_matrice_quadrata - 1; i >= 0; i--)
		for (size_t j = 0; j < lato_matrice_quadrata; j++)
		{
			m[j][i] = contatore;
			contatore++;
		}
	//Inserisco i dati con push back prelevando da t[ m[i][j] ]
	for (size_t i = 0; i<lato_matrice_quadrata; i++)
	{
		for (size_t j = 0; j < lato_matrice_quadrata; j++)
		{
			pt.n_zona.push_back(p.n_zona[m[i][j]]);
			pt.x_zona.push_back(
				y_dim_tessera - p.y_zona[m[i][j]] -
				p.y_dim_zona[m[i][j]]);
			pt.y_zona.push_back(p.x_zona[m[i][j]]);
			pt.x_dim_zona.push_back(p.y_dim_zona[m[i][j]]);
			pt.y_dim_zona.push_back(p.x_dim_zona[m[i][j]]);
			pt.cercabile.push_back(p.cercabile[m[i][j]]);
			vector<bool> vb;
			vb.push_back(p.direzione_fusione[m[i][j]][3]);
			vb.push_back(p.direzione_fusione[m[i][j]][2]);
			vb.push_back(p.direzione_fusione[m[i][j]][0]);
			vb.push_back(p.direzione_fusione[m[i][j]][1]);
			pt.direzione_fusione.push_back(vb);
			vb.clear();
			vb.push_back(p.direzione_mura[m[i][j]][3]);
			vb.push_back(p.direzione_mura[m[i][j]][2]);
			vb.push_back(p.direzione_mura[m[i][j]][0]);
			vb.push_back(p.direzione_mura[m[i][j]][1]);
			pt.direzione_mura.push_back(vb);
			pt.fila_della_zona = p.fila_della_zona; //visto che cmq sono matrici quadrate senza buchi...
		}
	}
	for (size_t i = 0; i <p.x_porta.size(); i++)
	{
		pt.x_porta.push_back(
			y_dim_tessera -
			p.y_porta[i] - 90); //90 è la dim della porta (90x90)
		pt.y_porta.push_back(p.x_porta[i]);
	}
	//ora devo cambiare n zone
	//strategia: nuovo array di stesse dim di pt.n_zone
	vector<int> v = pt.n_zona;
	for (size_t i = 0; i < v.size(); i++)
		v[i] = -1;
	vector<int> zone_tombini;
	contatore = 0;
	int contatore_per_zone_porta = 0;
	bool almeno_un_indice_inserito = false;
	vector<vector<int>> zone_porte = p.zone_porta;
	for (size_t i = 0; i < zone_porte.size(); i++)
		for (size_t j = 0; j < zone_porte[i].size(); j++)
			zone_porte[i][j] = -1;

	for (size_t i = 0; i < pt.n_zona.size(); i++)
	{//ora scorro pt.n_zona e cerco se pt.n_zona[i] è solo li o anche in altre zone della matrice. Mi segno questi indici in un array.
		vector<int> indici_da_segnare;
		for (size_t j = 0; j < pt.n_zona.size(); j++)
			if (pt.n_zona[i] == pt.n_zona[j])
				indici_da_segnare.push_back(j);
		//ora inserisco i valori del contatore in tutti gli indici dell'array che mi sono segnato solo se non sono già stati inseriti precedentemente (-1)
		for (size_t j = 0; j < indici_da_segnare.size(); j++)
			if (v[indici_da_segnare[j]] == -1)
			{
				almeno_un_indice_inserito = true;
				v[indici_da_segnare[j]] = contatore;
			}
		if (almeno_un_indice_inserito == true)//verifico di aver aggiunto qualcosa altrimenti il contatore non deve aumentare
		{
			contatore++;
			almeno_un_indice_inserito = false;
		}
	}
	//Grazie alla conversione del n_zone messo in v, mi converto anche le zone delle porte e le zone dei tombini
	for (size_t i = 0; i < pt.n_zona.size(); i++)
	{
		for (size_t j = 0; j < p.zone_tombini.size(); j++)
			if (pt.n_zona[i] == p.zone_tombini[j])
				zone_tombini.push_back(v[i]);

		for (size_t j = 0; j < p.zone_porta.size(); j++)
			for (size_t k = 0; k < p.zone_porta[j].size(); k++)
				if (pt.n_zona[i] == p.zone_porta[j][k])
					zone_porte[j][k] = v[i];
	}
	//devo inserire anche le zone_porte negative ruotate
	for(size_t i=0; i<p.zone_porta.size(); i++)
		for(size_t j=0; j<p.zone_porta[i].size(); j++)
			switch (p.zone_porta[i][j])
			{
			case -1: //su devediventare dx, quindi -3
				zone_porte[i][j] = -3;
				break;
			case -2: //giù deve diventare sx, quindi -4
				zone_porte[i][j] = -4;
				break;
			case -3: //destra deve diventare giù, quindi -2
				zone_porte[i][j] = -2;
				break;
			case -4: //sinistra deve diventare su, quindi -1
				zone_porte[i][j] = -1;
				break;
			}
	pt.zone_porta = zone_porte;
	pt.n_zona = v;
	pt.zone_tombini = zone_tombini;
	t = pt;
}
void trova_zone_adiacenti_fondibili(int i, int j, int k, vector<vector<Tessere_editor>> Mappa_matrice, vector<vector<int>>& zone_fondibili)
{
	int nuova_i = 0;
	int nuova_j = 0;
 	int direzione = 0; //direzione di fusione dell'altra zona. Es: zona1 fondibili a dx, l'altra zona deve essere fondibile a Sx
	bool b = false;
	for (int l = 0; l < 4; l++)
	{
		if(Mappa_matrice[i][j].nome!="VUOTA")
			if (Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].direzione_fusione[k][l] == true)
			{
				switch (l)
				{
				case 0: //Su
					if (i>0) //c'è una fila più su.
					{
						if ((int)Mappa_matrice[i - 1].size() > j) //e anche la relativa colonna 
						{
							if (Mappa_matrice[i - 1][j].nome != "VUOTA")
							{
								b = true;
								nuova_i = i - 1;
								nuova_j = j;
								direzione = 1;//giù
							}
						}
					}
					break;
				case 1: //Giù
					if (i + 1<(int)Mappa_matrice.size()) //c'è una fila più giù.
					{
						if ((int)Mappa_matrice[i + 1].size() > j) //e anche la relativa colonna 
						{
							if (Mappa_matrice[i + 1][j].nome != "VUOTA")
							{
								b = true;
								nuova_i = i + 1;
								nuova_j = j;
								direzione = 0; //su
							}
						}
					}
					break;
				case 2: //Dx
					if (j + 1 < (int)Mappa_matrice[i].size()) //c'è un altra tessera a destra
					{
						if (Mappa_matrice[i][j + 1].nome != "VUOTA")
						{
							b = true;
							nuova_i = i;
							nuova_j = j + 1;
							direzione = 3; //sx
						}
					}
					break;
				case 3: //Sx
					if (j> 0) //c'è una tessera a sx
					{
						if (Mappa_matrice[i][j - 1].nome != "VUOTA")
						{
							b = true;
							nuova_i = i;
							nuova_j = j - 1;
							direzione = 2; //dx
						}
					}
					break;
				}
				if (b == true)
				{
					b = false;
					for (size_t k_seconda = 0;
						k_seconda <
						Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].direzione_fusione.size();
						k_seconda++)
					{
						if (Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].
							direzione_fusione[k_seconda][direzione] == true)
						{
							if (direzione == 0 || direzione == 1) //fusione verticale verificando se esiste almeno una X in comune
							{
								if ((Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].x_zona[k_seconda] >=
									Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].x_zona[k] &&
									Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].x_zona[k_seconda] <=
									Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].x_zona[k] +
									Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].x_dim_zona[k]) ||
									(Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].x_zona[k] >=
										Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].x_zona[k_seconda] &&
										Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].x_zona[k] <=
										Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].x_zona[k_seconda] +
										Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].x_dim_zona[k_seconda]))
								{
									b = true;
								}

							}
							else if (direzione == 2 || direzione == 3) //fusione orizontale
							{
								if ((Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].y_zona[k_seconda] >=
									Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].y_zona[k] &&
									Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].y_zona[k_seconda] <=
									Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].y_zona[k] +
									Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].y_dim_zona[k]) ||
									(Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].y_zona[k] >=
										Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].y_zona[k_seconda] &&
										Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].y_zona[k] <=
										Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].y_zona[k_seconda] +
										Mappa_matrice[nuova_i][nuova_j].parametri_per_angolatura[Mappa_matrice[nuova_i][nuova_j].indice_angolo_da_usare].y_dim_zona[k_seconda]))
								{
									b = true;
								}
							}
							if (b == true)
							{
								b = false;
								vector<int> vv; vv.push_back(nuova_i); vv.push_back(nuova_j); vv.push_back(k_seconda);
								for (size_t c = 0; c < zone_fondibili.size(); c++)
									if (zone_fondibili[c] == vv)
										b = true;
								if (b == false)
								{
									zone_fondibili.push_back({ nuova_i,nuova_j,(int)k_seconda });
									trova_zone_adiacenti_fondibili(nuova_i, nuova_j, k_seconda, Mappa_matrice, zone_fondibili);
								}
								else
									b = false;
							}
						}

					}
				}
			}
	}
	for (size_t l = 0; l < zone_fondibili.size(); l++)
		for (size_t m = 0; m < zone_fondibili.size(); m++)
			if (zone_fondibili[l] == zone_fondibili[m] && l != m)
			{
				zone_fondibili.erase(zone_fondibili.begin() + m);
				if (m > l)
				{
					m--;
				}
				else
				{
					l--;
					m--;
				}
			}
}
void unisci_tessere_a_formare_mappa_editor(vector<Tessere_editor> Mappa_array, Missione& t,vector<Porta> &porte_inseribili)
{
	//per prima cosa devo creare la matrice contenente le Tessere della mappa senza quelle vuote (Matrice e non array)
	vector<vector<Tessere_editor>> Mappa_matrice;
	int contatore = 0;
	bool b = false;
	for (int i = 0; i < 9; i++)
	{
		vector<Tessere_editor> vt;
		for (int j = 0; j < 9; j++)
		{	
			vt.push_back(Mappa_array[contatore]);
			b = true;
			contatore++;
		}
		if (b == true)
		{
			Mappa_matrice.push_back(vt);
			b = false;
		}
	}
	bool fine_fila = false;
	for (int i = 0; i < 9; i++)
	{

		while (fine_fila == false)
		{
			if (Mappa_matrice[i][Mappa_matrice[i].size() - 1].nome == "VUOTA")
				Mappa_matrice[i].erase(Mappa_matrice[i].end() - 1);
			else
				fine_fila = true;

			if (Mappa_matrice[i].size() == 0)
				fine_fila = true;
		}
		fine_fila = false;
	}
	while (Mappa_matrice[Mappa_matrice.size() - 1].size() == 0)
		Mappa_matrice.erase(Mappa_matrice.end()-1);
	//ora devo creare i nuovi n_zona. Lo faccio grazie alle variabili che mi dicono in che fila si trova una zona.
	//cerco la tessera che contiene + file. E' sepre 3 con le tessere del gioco base
	contatore = 0;
	for (size_t i = 0; i < Mappa_matrice.size(); i++)
		for (size_t j = 0; j < Mappa_matrice[i].size(); j++)
			if(Mappa_matrice[i][j].nome!="VUOTA")
				for (size_t k = 0; k<Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].fila_della_zona.size(); k++)
					if (contatore < Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].fila_della_zona[k])
						contatore = Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].fila_della_zona[k];
	int file_max = contatore + 1; // contatore==2 cioè l'indice della 3° fila
	contatore = 0;
	int contatore_2 = 0;
	int contatore_3 = 0;
	int contatore_per_tombini = 0;
	vector<vector<vector<bool>>> matrice_direzioni_mura;
	vector<bool> vb;
	for (int i = 0; i < 4; i++)
		vb.push_back(false);
	vector<vector<int>> matrice_zone;
	for (size_t i = 0; i < Mappa_matrice.size(); i++)
	{
		for (int f = 0; f < file_max; f++)
		{
			vector<int> v;
			vector<vector<bool>> vvb;
			for (size_t j = 0; j < Mappa_matrice[i].size(); j++)
			{
				if (Mappa_matrice[i][j].nome != "VUOTA")
				{

					for (size_t k = 0; k < Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].n_zona.size(); k++)
						if (Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].fila_della_zona[k] == f)
						{
							v.push_back(-1);
							vvb.push_back(vb);
						}
				}
			}
			matrice_zone.push_back(v);
			matrice_direzioni_mura.push_back(vvb);
		}
	}
	vector<vector<vector<int>>> vettore_zone_di_matrice_tessere;
	for (size_t i = 0; i < Mappa_matrice.size(); i++)
	{
		vector<vector<int>> vvi;
		for (size_t j = 0; j < Mappa_matrice[i].size(); j++)
		{
			if (Mappa_matrice[i][j].nome != "VUOTA")
				vvi.push_back(Mappa_matrice[i][j].parametri_per_angolatura[Mappa_matrice[i][j].indice_angolo_da_usare].n_zona);
			else
			{
				vector<int> v;
				vvi.push_back(v);
			}
		}
		vettore_zone_di_matrice_tessere.push_back(vvi);
	}
	vector<vector<int>> mi_segno_le_stanze_grosse; //[0]: i e [1] k per trovare la tessera nella Mappa matrice. [2]: indice zona in questione [3]: valore da assegnare
	for (size_t i = 0; i < Mappa_matrice.size(); i++) //per ogni fila di tessere
	{	
		for (int j = 0; j < file_max; j++)		//scorro le file di zone
		{
			for (size_t k = 0; k < Mappa_matrice[i].size(); k++) //di ogni tessera
			{
				if (Mappa_matrice[i][k].nome != "VUOTA")
				{
					for (size_t l = 0; l < Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].n_zona.size(); l++)
					{
						if (Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].fila_della_zona[l] == j)
						{
							for (size_t n = 0; n < mi_segno_le_stanze_grosse.size(); n++) //controllo prima se mi sono già segnato che valore dargli
							{
								if (mi_segno_le_stanze_grosse[n][0] == i && mi_segno_le_stanze_grosse[n][1] == k && mi_segno_le_stanze_grosse[n][2] == l)
								{
									matrice_zone[contatore][contatore_2] = mi_segno_le_stanze_grosse[n][3];
									vettore_zone_di_matrice_tessere[i][k][l] = mi_segno_le_stanze_grosse[n][3];
									matrice_direzioni_mura[contatore][contatore_2] = Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].direzione_mura[l];
									vector<int> tomb;
									for (size_t m = 0; m < Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].zone_tombini.size(); m++)
										if (Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].zone_tombini[m] ==
											Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].n_zona[l])
										{
											tomb.push_back(mi_segno_le_stanze_grosse[n][3]);
										}
									if (tomb.size()>0)
										t.tombini.push_back(tomb);
									int contatore_4 = 1;
									bool tessera_di_stanza_già_presente = false;
									for (size_t m = 0; m < Mappa_matrice.size(); m++)
										for (size_t ti = 0; ti < Mappa_matrice[m].size(); ti++)
										{
											if (i == m && k == ti)
											{
												for (size_t u = 0; u < t.dati[mi_segno_le_stanze_grosse[n][3]].tessera_numero.size(); u++)
													if (t.dati[mi_segno_le_stanze_grosse[n][3]].tessera_numero[u] == contatore_4)
														tessera_di_stanza_già_presente = true;
												if (tessera_di_stanza_già_presente == false)
													t.dati[mi_segno_le_stanze_grosse[n][3]].tessera_numero.push_back(contatore_4);
												else
													tessera_di_stanza_già_presente = false;
											}
											contatore_4++;
										}
								}
							}
							if (matrice_zone[contatore][contatore_2] == -1) //ok gli metto n_zona = contatore 3. 
							{
								matrice_zone[contatore][contatore_2] = contatore_3;
								vettore_zone_di_matrice_tessere[i][k][l] = contatore_3;
								vector<vector<int>> stanze_da_fondere; // cerco se questa stanza si estende in altre zone sia di altre tessere "fondibili"
								trova_zone_adiacenti_fondibili(i, k, l, Mappa_matrice, stanze_da_fondere);
								for (size_t n = 0; n < stanze_da_fondere.size(); n++)
									stanze_da_fondere[n].push_back(contatore_3);
								//Cerco di mettere le cordinate giuste rispetto alla mappa globale
								if (stanze_da_fondere.size() > 0)
								{
									bool verifica = false;
									for (size_t m = 0; m < stanze_da_fondere.size(); m++)
										if (stanze_da_fondere[m][0] == i && stanze_da_fondere[m][1] == k &&
											stanze_da_fondere[m][2] == l)
											verifica = true;
									if (verifica == false)
										stanze_da_fondere.push_back({ (int)i, (int)k, (int)l, contatore_3 });
									int x_tessera = i; int y_tessera = k;
									for (size_t n = 0; n < stanze_da_fondere.size(); n++)
										if (x_tessera >= stanze_da_fondere[n][0]) //cerco la tessera + piccola come x nella matrice (quella più a su)
											x_tessera = stanze_da_fondere[n][0];
									for (size_t n = 0; n < stanze_da_fondere.size(); n++)
										if (stanze_da_fondere[n][0] == x_tessera)
											if (y_tessera>stanze_da_fondere[n][1]) //e di quelle più su, cerco quella + a sx
												y_tessera = stanze_da_fondere[n][1];
									//metto la x e la y della zona fusa. Prendo la x e la y della tessera che ho scelto
									for (size_t n = 0; n < stanze_da_fondere.size(); n++)
										if (x_tessera == stanze_da_fondere[n][0] && y_tessera == stanze_da_fondere[n][1])
										{
											t.x_cordinate_zone_mappa.push_back(Mappa_matrice[stanze_da_fondere[n][0]][stanze_da_fondere[n][1]].parametri_per_angolatura[Mappa_matrice[x_tessera][y_tessera].indice_angolo_da_usare].x_zona[stanze_da_fondere[n][2]] +
												(stanze_da_fondere[n][1] * (982)));
											t.y_cordinate_zone_mappa.push_back(Mappa_matrice[stanze_da_fondere[n][0]][stanze_da_fondere[n][1]].parametri_per_angolatura[Mappa_matrice[x_tessera][y_tessera].indice_angolo_da_usare].y_zona[stanze_da_fondere[n][2]] +
												(stanze_da_fondere[n][0] * (982)));
										}
									int x_dim_tessera = 0; int y_dim_tessera = 0;
									for (size_t n = 0; n < stanze_da_fondere.size(); n++)
										if (x_dim_tessera <= stanze_da_fondere[n][0]) //cerco la tessera + alta come x_dim nella matrice (quella più giù)
											x_dim_tessera = stanze_da_fondere[n][0];
									for (size_t n = 0; n < stanze_da_fondere.size(); n++)
										if (stanze_da_fondere[n][0] == x_dim_tessera)
											if (y_dim_tessera<stanze_da_fondere[n][1]) //e di quelle più giù cerco quella + a dx
												y_dim_tessera = stanze_da_fondere[n][1];

									for (size_t n = 0; n < stanze_da_fondere.size(); n++)
									{
										if (x_dim_tessera == stanze_da_fondere[n][0] && y_dim_tessera == stanze_da_fondere[n][1])
										{
											t.x_dimensione_zone_mappa.push_back(Mappa_matrice[stanze_da_fondere[n][0]][stanze_da_fondere[n][1]].parametri_per_angolatura[Mappa_matrice[x_dim_tessera][y_dim_tessera].indice_angolo_da_usare].x_zona[stanze_da_fondere[n][2]] +
												(stanze_da_fondere[n][1] * (982)) + (Mappa_matrice[stanze_da_fondere[n][0]][stanze_da_fondere[n][1]].parametri_per_angolatura[Mappa_matrice[x_dim_tessera][y_dim_tessera].indice_angolo_da_usare].x_dim_zona[stanze_da_fondere[n][2]]) -
												t.x_cordinate_zone_mappa[t.x_cordinate_zone_mappa.size() - 1]);
											t.y_dimensione_zone_mappa.push_back(Mappa_matrice[stanze_da_fondere[n][0]][stanze_da_fondere[n][1]].parametri_per_angolatura[Mappa_matrice[x_dim_tessera][y_dim_tessera].indice_angolo_da_usare].y_zona[stanze_da_fondere[n][2]] +
												(stanze_da_fondere[n][0] * (982)) + (Mappa_matrice[stanze_da_fondere[n][0]][stanze_da_fondere[n][1]].parametri_per_angolatura[Mappa_matrice[x_dim_tessera][y_dim_tessera].indice_angolo_da_usare].y_dim_zona[stanze_da_fondere[n][2]]) -
												t.y_cordinate_zone_mappa[t.y_cordinate_zone_mappa.size() - 1]);
										}
									}
								}
								else
								{
									t.x_cordinate_zone_mappa.push_back(Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].x_zona[l] + (k * (982)));
									t.y_cordinate_zone_mappa.push_back(Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].y_zona[l] + (i * (982)));
									t.x_dimensione_zone_mappa.push_back(Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].x_dim_zona[l]);
									t.y_dimensione_zone_mappa.push_back(Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].y_dim_zona[l]);
								}
								//fine inserimento cordinate globali
								for (size_t n = 0; n < stanze_da_fondere.size(); n++)
								{
									mi_segno_le_stanze_grosse.push_back(stanze_da_fondere[n]);
									for (int ti = 0; ti<4; ti++)
										if (Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].direzione_mura[l][ti] == true ||
											Mappa_matrice[stanze_da_fondere[n][0]][stanze_da_fondere[n][1]].parametri_per_angolatura[Mappa_matrice[stanze_da_fondere[n][0]][stanze_da_fondere[n][1]].indice_angolo_da_usare].direzione_mura[stanze_da_fondere[n][2]][ti] == true)
										{
											Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].direzione_mura[l][ti] = true;
											Mappa_matrice[stanze_da_fondere[n][0]][stanze_da_fondere[n][1]].parametri_per_angolatura[Mappa_matrice[stanze_da_fondere[n][0]][stanze_da_fondere[n][1]].indice_angolo_da_usare].direzione_mura[stanze_da_fondere[n][2]][ti] = true;
										}
								}
								matrice_direzioni_mura[contatore][contatore_2] = Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].direzione_mura[l];
								//Sia nella stessa tessera. Me li segno così quando ci passo non gli metto contatore 3
								//che ho in quel momento ma questo di ora			
								for (size_t h = 0; h < Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].n_zona.size(); h++)
									if (Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].n_zona[h] ==
										Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].n_zona[l] && h != l)
									{
										mi_segno_le_stanze_grosse.push_back({ (int)i , (int)k , (int)h ,  contatore_3 });
									}
								//mi segno in che tessera si trova questa stanza
								int contatore_4 = 1;
								DatiStanza d;
								for (size_t n = 0; n < Mappa_matrice.size(); n++)
									for (size_t m = 0; m < Mappa_matrice[n].size(); m++)
									{
										if (i == n && k == m)
											d.tessera_numero.push_back(contatore_4);
										contatore_4++;
									}
								d.cercabile = Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].cercabile[l];
								if (d.cercabile == true) //In prison outbreak ci sono corridoi interni in cui non si può cerca e le celle. Da rivedere più in la
								{
									d.cercabile = true;
									d.tipo = 1; //stanza interna
								}
								else
								{
									d.tipo = 0; //strada
									d.cercabile = false;
								}
								t.dati.push_back(d);
								//se c'è un tombino me lo segno
								vector<int> tomb;
								for (size_t n = 0; n < Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].zone_tombini.size(); n++)
									if (Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].zone_tombini[n] ==
										Mappa_matrice[i][k].parametri_per_angolatura[Mappa_matrice[i][k].indice_angolo_da_usare].n_zona[l])
									{
										tomb.push_back(contatore_3);
									}
								if (tomb.size() > 0)
									t.tombini.push_back(tomb);

								contatore_3++;

							}
							contatore_2++;
						}
					}
				}
			}
			contatore++;
			contatore_2 = 0;
		}
 	}
	//ora devo inserire le collisioni e il loro orientamento.
		//inizializzo a 0 la matrice delle collisioni (connessioni) e quella dell'orientamento connessioni
	contatore = 0;
	vector<int> vi;
	for (size_t i = 0; i < matrice_zone.size(); i++)
		for (size_t j = 0; j < matrice_zone[i].size(); j++)
			if (contatore < matrice_zone[i][j])
				contatore = matrice_zone[i][j];
	contatore++;
	for(int i=0; i<contatore; i++)
		vi.push_back(0);
	for (int i = 0; i < contatore; i++)
	{
		t.collisioni.push_back(vi);
		t.orientamento_connessioni.push_back(vi);
	}
	int nuova_direzione = 0;
	int nuova_i = 0;
	bool connettibile = false;
	for (size_t i = 0; i < matrice_zone.size(); i++)
		for (size_t j = 0; j < matrice_zone[i].size(); j++)
		{
			for (int k = 0; k < 4; k++)
			{
				vector<int> nuova_j;
				
					switch (k)
					{
					case 0: //su
						if (i > 0)
						{
							for(size_t n=0; n<matrice_zone[i-1].size(); n++)
								if ((t.x_cordinate_zone_mappa[matrice_zone[i - 1][n]] >= t.x_cordinate_zone_mappa[matrice_zone[i][j]] &&
									t.x_cordinate_zone_mappa[matrice_zone[i - 1][n]] <= t.x_cordinate_zone_mappa[matrice_zone[i][j]] + t.x_dimensione_zone_mappa[matrice_zone[i][j]]) ||
									(t.x_cordinate_zone_mappa[matrice_zone[i][j]] >= t.x_cordinate_zone_mappa[matrice_zone[i - 1][n]] &&
										t.x_cordinate_zone_mappa[matrice_zone[i][j]] <= t.x_cordinate_zone_mappa[matrice_zone[i - 1][n]] + t.x_dimensione_zone_mappa[matrice_zone[i - 1][n]]))
								{
									nuova_direzione = 1;
									nuova_i=i - 1;
									nuova_j .push_back(n);
									connettibile = true;
								}
							/*if (matrice_direzioni_mura[i-1].size()> j)
							{
								nuova_direzione = 1;
								nuova_i = i - 1;
								nuova_j = j;
								connettibile = true;
							}*/
						}
						break;
					case 1: //Giù
						if (i + 1 < matrice_direzioni_mura.size())
						{
							for (size_t n = 0; n<matrice_zone[i + 1].size(); n++)
								if ((t.x_cordinate_zone_mappa[matrice_zone[i + 1][n]] >= t.x_cordinate_zone_mappa[matrice_zone[i][j]] &&
									t.x_cordinate_zone_mappa[matrice_zone[i + 1][n]] <= t.x_cordinate_zone_mappa[matrice_zone[i][j]] + t.x_dimensione_zone_mappa[matrice_zone[i][j]]) ||
									(t.x_cordinate_zone_mappa[matrice_zone[i][j]] >= t.x_cordinate_zone_mappa[matrice_zone[i + 1][n]] &&
										t.x_cordinate_zone_mappa[matrice_zone[i][j]] <= t.x_cordinate_zone_mappa[matrice_zone[i + 1][n]] + t.x_dimensione_zone_mappa[matrice_zone[i + 1][n]]))
								{
									nuova_direzione = 0;
									nuova_i =i + 1;
									nuova_j.push_back(n);
								}
							/*if (matrice_direzioni_mura[i + 1].size()< j)
							{
								nuova_direzione = 0;
								nuova_i = i + 1;
								nuova_j = j;
							}*/
						}
						break;
					case 2: //dx
						if (j + 1<matrice_direzioni_mura[i].size())
						{
							nuova_i=i;
							nuova_j.push_back(j + 1);
							nuova_direzione = 3;
							connettibile = true;
						}
						break;
					case 3: //sx
						if (j>0)
						{
							nuova_i=i;
							nuova_j .push_back(j - 1);
							nuova_direzione = 2;
							connettibile = true;
						}
						break;
					}
					if (connettibile == true)
					{
						connettibile = false;
						for (size_t n = 0; n < nuova_j.size(); n++)
						{
							if (matrice_direzioni_mura[nuova_i][nuova_j[n]][nuova_direzione] == false)
							{
								t.collisioni[matrice_zone[i][j]][matrice_zone[nuova_i][nuova_j[n]]] = 1;
								t.collisioni[matrice_zone[nuova_i][nuova_j[n]]][matrice_zone[i][j]] = 1;	
							}	
							t.orientamento_connessioni[matrice_zone[i][j]][matrice_zone[nuova_i][nuova_j[n]]] = k;
							t.orientamento_connessioni[matrice_zone[nuova_i][nuova_j[n]]][matrice_zone[i][j]] = nuova_direzione;
							//per le 9 direzioni non agisco. E' un problema di fondo e quindi lo cambierò sicuramente.
						}
					}
			}
		}
	for (size_t i = 0; i < t.collisioni.size(); i++)
		t.orientamento_connessioni[i][i] = 8; //centro
	contatore = 0;
	//Ora che ho mappato i nuovi n_zone nella vecchia matrice di tessere e ho le collisioni e orientamenti, posso modificare e aggiungere le porte
	vector<Porta> vp;
	for(size_t i_p=0; i_p<Mappa_matrice.size(); i_p++)
		for(size_t j_p=0; j_p<Mappa_matrice[i_p].size(); j_p++)
			if (Mappa_matrice[i_p][j_p].nome != "VUOTA")
				for (size_t k_p = 0; k_p < Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].zone_porta.size(); k_p++)
				{
					Porta p;
					for (size_t l_p = 0; l_p < Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].zone_porta[k_p].size(); l_p++)
					{
						if (Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].zone_porta[k_p][l_p] >= 0)
						{
							for (size_t l = 0; l < Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].n_zona.size(); l++)
							{
								if (Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].n_zona[l] ==
									Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].zone_porta[k_p][l_p])
								{
									p.zone[l_p] = vettore_zone_di_matrice_tessere[i_p][j_p][l];
								}
							}
						}
						else // negativo, quindi va cercata la stanza con cui si è legata dell'altra tessera e lasciare solo 1 delle 2 porte ridondanti
						{
							p.zone[l_p] = Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].zone_porta[k_p][l_p];
							switch (Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].zone_porta[k_p][l_p])
							{
							case -1: //su
								nuova_direzione = 0;
								break;
							case -2: //giù
								nuova_direzione = 1;
								break;
							case -3://dx
								nuova_direzione = 2;
								break;
							case -4://sx
								nuova_direzione = 3;
								break;
							}
							vector<int> zone_connesse_da_questa_porta;
							//in realtà ci sono n_porte per n_connessioni, ma escono fuori ambiguità perchè sono connesse nella stessa direzione, con una zona in comune.
							//Bisogna calcolare qual'è la porta che connette queste due zone, usando le cordinate x_y di porta e zone
							for (size_t i = 0; i < Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].zone_porta[k_p].size(); i++)
								if (Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].zone_porta[k_p][i] >= 0)
									for (size_t k = 0; k< Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].n_zona.size(); k++)
										if (Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].zone_porta[k_p][i] ==
											Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].n_zona[k])
											for (size_t j = 0; j < t.collisioni[vettore_zone_di_matrice_tessere[i_p][j_p][k]].size(); j++)
												if (t.collisioni[vettore_zone_di_matrice_tessere[i_p][j_p][k]][j] == 1 &&
													t.orientamento_connessioni[vettore_zone_di_matrice_tessere[i_p][j_p][k]][j] == nuova_direzione)
												{
													bool verifica_ridon = false;
													for (size_t n = 0; n < zone_connesse_da_questa_porta.size(); n++)
														if (zone_connesse_da_questa_porta[n] == j)
															verifica_ridon = true;
													if (verifica_ridon == false)
														zone_connesse_da_questa_porta.push_back(j);
												}
													
							if (zone_connesse_da_questa_porta.size() == 1)//Caso semplice. Zona trovata
							{
								p.zone[l_p] = zone_connesse_da_questa_porta[0];
							}
							else if (zone_connesse_da_questa_porta.size() > 1)//Caso ambiguo. Risolviamo ambiguità
							{
								p.zone[l_p] = zone_connesse_da_questa_porta[0];
							}
						}
						p.x_cordinata = Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].x_porta[k_p] + (j_p * 982);
						p.y_cordinata = Mappa_matrice[i_p][j_p].parametri_per_angolatura[Mappa_matrice[i_p][j_p].indice_angolo_da_usare].y_porta[k_p] + (i_p * 982);
						p.dim = 90;
					}
					if (p.zone[0] >= 0 && p.zone[1] >= 0) //se non ha trovato connessioni allora non mi serve. Vuol dire che non ho messo tessere in quella direzione
					{
						bool doppione = false;
						for (size_t i = 0; i < vp.size(); i++) //aggiungo facendo attenzione ai doppioni
							if (((vp[i].zone[0] == p.zone[0] && vp[i].zone[1] == p.zone[1]) || (vp[i].zone[0] == p.zone[1] && vp[i].zone[1] == p.zone[0])))
								doppione = true;
						if (doppione == false)
							vp.push_back(p);
						if (vp.size() == 0) //almeno 1 lo deve mettere prima di controllare se ci sono doppioni
							vp.push_back(p);
					}
				}
	for (size_t i = 0; i < t.dati.size(); i++)
		for(int j=0; j<6; j++)
			t.dati[i].ModificaValoreGiocatori(false,j);
	for (int i = 0; i < 6; i++)
	{
		t.dati[0].ModificaValoreGiocatori(true,i);
	}
	porte_inseribili = vp;
	t.Mappa_matrice = Mappa_matrice;
}
void inserisci_oggetto_in_zona(StatoGioco& stato,int indice_oggetto, int zona,Missione& t, string inserisci_elimina)
{
	switch (indice_oggetto)
	{
	case 0: //Porta neutra
		if (inserisci_elimina == "inserisci")
		{
			t.porta.push_back(stato.porte_inseribili_editor[zona]);
			t.porta[t.porta.size() - 1].colore = 1;
			t.porta[t.porta.size() - 1].stato = 0;
		}
		break;
	case 1: //Porta neutra sfondata
		if (inserisci_elimina == "inserisci")
		{
			t.porta.push_back(stato.porte_inseribili_editor[zona]);
			t.porta[t.porta.size() - 1].colore = 1;
			t.porta[t.porta.size() - 1].stato = 1;
		}
		break;
	case 2: //Porta blu
		if (inserisci_elimina == "inserisci")
		{
			t.porta.push_back(stato.porte_inseribili_editor[zona]);
			t.porta[t.porta.size() - 1].colore = 0;
			t.porta[t.porta.size() - 1].stato = 0;
		}
		break;
	case 3: //Porta blu sfondata
		if (inserisci_elimina == "inserisci")
		{
			t.porta.push_back(stato.porte_inseribili_editor[zona]);
			t.porta[t.porta.size() - 1].colore = 0;
			t.porta[t.porta.size() - 1].stato = 1;
		}
		break;
	case 4: //Porta verde
		if (inserisci_elimina == "inserisci")
		{
			t.porta.push_back(stato.porte_inseribili_editor[zona]);
			t.porta[t.porta.size() - 1].colore = 2;
			t.porta[t.porta.size() - 1].stato = 0;
		}
		break;
	case 5: //Porta verde sfondata
		if (inserisci_elimina == "inserisci")
		{
			t.porta.push_back(stato.porte_inseribili_editor[zona]);
			t.porta[t.porta.size() - 1].colore = 2;
			t.porta[t.porta.size() - 1].stato = 1;
		}
		break;
	case 6: //Start
		if (inserisci_elimina == "inserisci")
		{
			for (size_t i = 0; i < t.dati.size(); i++)
				for (int j = 0; j < 6; j++)
				{
					t.dati[i].ModificaValoreGiocatori(false,j);
				}
			for (int i = 0; i < 6; i++)
				t.dati[zona].ModificaValoreGiocatori(true,i);
			for (int i = 0; i < 6; i++)
				stato.giocatore[i].posizione = zona;
		}
		break;
	case 7: //spawn rosso
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValoreSpawn(1, DatiStanza::rosso);
		else if (inserisci_elimina == "elimina")
			if (t.dati[zona].spawn_rosso > 0)
				t.dati[zona].SommaValoreSpawn(-1, DatiStanza::rosso);
		break;
	case 8: //spawn blu
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValoreSpawn(1, DatiStanza::blu);
		else if (inserisci_elimina == "elimina")
			if (t.dati[zona].spawn_blu > 0)
				t.dati[zona].SommaValoreSpawn(-1, DatiStanza::blu);;
		break;
	case 9: //Obietttivo rosso
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValoreObiettivo(1, DatiStanza::rosso);
		else if (inserisci_elimina == "elimina")
			if (t.dati[zona].obiettivi_rossi > 0)
				t.dati[zona].SommaValoreObiettivo(-1, DatiStanza::rosso);
		break;
	case 10: //obiettivo blu
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValoreObiettivo(1, DatiStanza::blu);
		else if (inserisci_elimina == "elimina")
			if (t.dati[zona].obiettivi_blu > 0)
				t.dati[zona].SommaValoreObiettivo(-1, DatiStanza::blu);
		break;
	case 11://obiettivo verde
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValoreObiettivo(1, DatiStanza::verde);
		else if (inserisci_elimina == "elimina")
			if (t.dati[zona].obiettivi_verdi>0)
				t.dati[zona].SommaValoreObiettivo(-1, DatiStanza::verde);
		break;
	case 12: //rumore
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValoreRumore(1);
		else if (inserisci_elimina == "elimina")
			if (t.dati[zona].rumore>0)
				t.dati[zona].SommaValoreRumore(-1);;
		break;
	case 13: //polizia
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValorePolizia(1);
		else if (inserisci_elimina == "elimina")
			if(t.dati[zona].polizia>0)
				t.dati[zona].SommaValorePolizia(-1);
		break;
	case 14: //pimp
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValorePimp_mobile(1);
		else if (inserisci_elimina == "elimina")
			if (t.dati[zona].pimp_mobile > 0)
				t.dati[zona].SommaValorePimp_mobile(-1);
		break;
	case 15: //Exit
		if (inserisci_elimina == "inserisci")
			t.dati[zona].ModificaValoreExit(true);
		else if (inserisci_elimina == "elimina")
			t.dati[zona].ModificaValoreExit(false);
		break;
	case 16: //Walker
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValoreWalker(1);
		else if (inserisci_elimina == "elimina")
			if (t.dati[zona].walker>0)
				t.dati[zona].SommaValoreWalker(-1);
		break; 
	case 17: //Runner
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValoreRunner(1);
		else if (inserisci_elimina == "elimina")
			if (t.dati[zona].runner > 0)
				t.dati[zona].SommaValoreRunner(-1);
		break;
	case 18: //fat
		if (inserisci_elimina == "inserisci")
			t.dati[zona].SommaValoreFat(1);
		else if (inserisci_elimina == "elimina")
			if (t.dati[zona].fat > 0)
				t.dati[zona].SommaValoreFat(-1);
		break;
	case 19: //abominio
		if (inserisci_elimina == "inserisci")
		{
			for (size_t i = 0; i < t.dati.size(); i++)
				t.dati[i].ModificaValoreAbominio(false);
			t.dati[zona].ModificaValoreAbominio(true);
		}
		else if (inserisci_elimina == "elimina")
			t.dati[zona].ModificaValoreAbominio(false);
		break;
	}
	if(indice_oggetto<=5&&inserisci_elimina=="elimina") //elimina porte, che va bene con qualunque porta selezionata
		for (size_t i = 0; i < t.porta.size(); i++)
			if (t.porta[i].zone[0] == stato.porte_inseribili_editor[zona].zone[0] &&
				t.porta[i].zone[1] == stato.porte_inseribili_editor[zona].zone[1])
				t.porta.erase(t.porta.begin() + i);
}
void salva_mappa(Missione& t, string nome)
{
	//prima modifico le collisioni date dall'aggiunta di porte
	t.nome_missione = nome;
	int lunghezza_max_mappa = 0;
	for (size_t i = 0; i < t.Mappa_matrice.size(); i++)
		if (lunghezza_max_mappa < (int)t.Mappa_matrice[i].size())
			lunghezza_max_mappa = t.Mappa_matrice[i].size();
	t.w_h_mappa.push_back(lunghezza_max_mappa*982);
	t.w_h_mappa.push_back(t.Mappa_matrice.size()*982);
	for (size_t i = 0; i<t.porta.size(); i++)
		if (t.porta[i].stato == 0) //chiusa
		{
			t.collisioni[t.porta[i].zone[0]][t.porta[i].zone[1]] = 2;
			t.collisioni[t.porta[i].zone[1]][t.porta[i].zone[0]] = 2;
		}
		else if (t.porta[i].stato == 1)//sfondata
		{
			t.collisioni[t.porta[i].zone[0]][t.porta[i].zone[1]] = 1;
			t.collisioni[t.porta[i].zone[1]][t.porta[i].zone[0]] = 1;
			if (t.x_cordinate_zone_mappa[t.porta[i].zone[0]] + t.x_dimensione_zone_mappa[t.porta[i].zone[0]] <
				t.x_cordinate_zone_mappa[t.porta[i].zone[1]] || t.x_cordinate_zone_mappa[t.porta[i].zone[1]] +
				t.x_dimensione_zone_mappa[t.porta[i].zone[1]] < t.x_cordinate_zone_mappa[t.porta[i].zone[0]])
			{
				//orientamento orizontale
				if (t.x_cordinate_zone_mappa[t.porta[i].zone[0]] > t.x_cordinate_zone_mappa[t.porta[i].zone[1]])
				{
					t.orientamento_connessioni[t.porta[i].zone[0]][t.porta[i].zone[1]] = 3; //sx
					t.orientamento_connessioni[t.porta[i].zone[1]][t.porta[i].zone[0]] = 2; //sx
				}
				else
				{
					t.orientamento_connessioni[t.porta[i].zone[0]][t.porta[i].zone[1]] = 2; //dx
					t.orientamento_connessioni[t.porta[i].zone[1]][t.porta[i].zone[0]] = 3; //dx

				}
			}
			else
			{
				//orientamento verticale
				if (t.y_cordinate_zone_mappa[t.porta[i].zone[0]] > t.y_cordinate_zone_mappa[t.porta[i].zone[1]])
				{
					t.orientamento_connessioni[t.porta[i].zone[0]][t.porta[i].zone[1]] = 0; //su
					t.orientamento_connessioni[t.porta[i].zone[1]][t.porta[i].zone[0]] = 1; //su
				}
				else
				{
					t.orientamento_connessioni[t.porta[i].zone[0]][t.porta[i].zone[1]] = 1; //giù
					t.orientamento_connessioni[t.porta[i].zone[1]][t.porta[i].zone[0]] = 0; //giù
				}
			}
		}
	//ora creo un file e lo scrivo con la convenzione da me decisa e che il programma sa leggere	
	string percorso = "Missioni/" + nome;
	boost::filesystem::path dir(percorso);
	if (!(boost::filesystem::exists(dir)))
		boost::filesystem::create_directory(dir);

	ofstream scrivi = ofstream(percorso+'/'+nome+".txt");
	for (size_t i = 0; i < t.Mappa_matrice.size(); i++)
	{
		for (size_t j = 0; j < t.Mappa_matrice[i].size(); j++)
		{
			scrivi << t.Mappa_matrice[i][j].nome;
			scrivi << ' ';
			scrivi << to_string(t.Mappa_matrice[i][j].angolatura[t.Mappa_matrice[i][j].indice_angolo_da_usare]).c_str();
			scrivi << ' ';
		}
		if (i != t.Mappa_matrice.size() - 1)
			scrivi << "-1";
		else
			scrivi << "-2";
		scrivi << '\n';
	}	
	scrivi << to_string(t.dati.size())+'\n'; //n_zone
	for (size_t i = 0; i < t.collisioni.size(); i++) //tutte le connessioni e gli orientamenti delle zone
	{
		for (size_t j = 0; j < t.collisioni[i].size(); j++)
			if (t.collisioni[i][j] == 1 || t.collisioni[i][j] == 2)
			{
				
				scrivi << to_string(i)+ ' ';
				scrivi << to_string(j)+ ' ';
				scrivi << to_string(t.collisioni[i][j]);
				scrivi << to_string(t.orientamento_connessioni[i][j]);
				scrivi << ' ';
			}
	}
	scrivi<<"-1";
	scrivi << "\n";//fine connessioni e orientamenti
	for (size_t i = 0; i < t.dati.size(); i++) //ogni zona di che tessera fa parte, separato da zeri
	{
		for (size_t j = 0; j < t.dati[i].tessera_numero.size(); j++)
		{
			scrivi << to_string(t.dati[i].tessera_numero[j]);
			scrivi << ' ';
		}
		scrivi << to_string(0);
		scrivi << ' ';
	}
	scrivi << "\n";
	scrivi << to_string(t.tombini.size());//numero tombini
	scrivi << "\n";
	for (size_t i = 0; i < t.tombini.size(); i++)
	{
		for (size_t j = 0; j < t.tombini[i].size(); j++)
		{
			scrivi << to_string(t.tombini[i][j]);
			scrivi << ' ';
		}
		scrivi << to_string(-1);
		scrivi << ' ';
	}
	scrivi << "\n";
	//inserisci oggetti
	for (size_t i = 0; i < t.dati.size(); i++)
	{
		bool verifica = false;
		string s;
		s = "| " + to_string(i)+'\n';
		if (t.dati[i].obiettivi_rossi > 0)
		{
			s = s + "obiettivo_rosso ";
			s = s + to_string(t.dati[i].obiettivi_rossi).c_str() + '\n';
			verifica = true;
		}
		if (t.dati[i].obiettivi_blu > 0)
		{
			s = s + "obiettivo_blu ";
			s = s + to_string(t.dati[i].obiettivi_blu).c_str() + '\n';
			verifica = true;
		}
		if (t.dati[i].obiettivi_verdi > 0)
		{
			s = s + "obiettivo_verde ";
			s = s + to_string(t.dati[i].obiettivi_verdi).c_str() + '\n';
			verifica = true;
		}
		if (t.dati[i].abominio ==true)
		{
			s = s + "abominio ";
			s = s + '\n';
			verifica = true;
		}
		if (t.dati[i].fat>0)
		{
			s = s + "fat ";
			s = s + to_string(t.dati[i].fat).c_str() + '\n';
			verifica = true;
		}
		if (t.dati[i].runner>0)
		{
			s = s + "runner ";
			s = s + to_string(t.dati[i].runner).c_str() + '\n';
			verifica = true;
		}
		if (t.dati[i].walker>0)
		{
			s = s + "walker ";
			s = s + to_string(t.dati[i].walker).c_str() + '\n';
			verifica = true;
		}
		if (t.dati[i].exit == true)
		{
			s = s + "exit";
			s = s + '\n';
			verifica = true;
		}
		if (t.dati[i].giocatori[0] == true || t.dati[i].giocatori[1] == true || t.dati[i].giocatori[2] == true || t.dati[i].giocatori[3] == true ||
			t.dati[i].giocatori[4] == true || t.dati[i].giocatori[5] == true)
		{
			s = s + "start 8"; //per ora così poi lo faccio meglio
			s = s + '\n';
			verifica = true;
		}
		if (t.dati[i].pimp_mobile > 0)
		{
			s = s + "pimp_mobile ";
			s = s + to_string(t.dati[i].pimp_mobile).c_str() + '\n';
			verifica = true;
		}
		if (t.dati[i].polizia > 0)
		{
			s = s + "polizia ";
			s = s + to_string(t.dati[i].polizia).c_str() + '\n';
			verifica = true;
		}
		if (t.dati[i].rumore > 0)
		{
			s = s + "rumore ";
			s = s + to_string(t.dati[i].rumore).c_str() + '\n';
			verifica = true;
		}
		if (t.dati[i].spawn_rosso > 0)
		{
			s = s + "spawn_rosso ";
			s = s + to_string(t.dati[i].spawn_rosso).c_str() + '\n';
			verifica = true;
		}
		if (t.dati[i].spawn_blu > 0)
		{
			s = s + "spawn_blu ";
			s = s + to_string(t.dati[i].spawn_blu).c_str() + '\n';
			verifica = true;
		}
		if (verifica == true)
			scrivi << s;
	}
	scrivi << '"';
	scrivi << "\n";
	scrivi<<"cercabile";
	scrivi << "\n";
	for (size_t i = 0; i < t.dati.size(); i++)
		if (t.dati[i].cercabile == true)
			scrivi << to_string(i)+' ';
	scrivi << "-1 ";
	scrivi<<'\n';
	scrivi << "stanze\n";
	for (size_t i = 0; i < t.dati.size(); i++)
		if (t.dati[i].cercabile == true)
			scrivi << to_string(i) + ' ';
	scrivi << "-1 ";
	scrivi<<'\n';
	scrivi << '"';
	scrivi.close();
	scrivi = ofstream(percorso + '/' + nome + "_cordinate.dat");
	for (size_t i = 0; i < t.dati.size(); i++)
	{
		scrivi << to_string(t.x_cordinate_zone_mappa[i]).c_str(); scrivi << ' ';
		scrivi << to_string(t.y_cordinate_zone_mappa[i]).c_str(); scrivi << ' ';
		scrivi << to_string(t.x_dimensione_zone_mappa[i]).c_str(); scrivi << ' ';
		scrivi << to_string(t.y_dimensione_zone_mappa[i]).c_str(); scrivi;
		scrivi << '\n';
	}
	for (size_t i = 0; i < t.porta.size(); i++)
	{
		if (t.porta[i].colore == 1&&t.porta[i].stato==0)
			scrivi << "porta_neutra";
		else if (t.porta[i].colore == 1 && t.porta[i].stato == 1)
			scrivi << "porta_neutra_sfondata";
		else if (t.porta[i].colore == 0 && t.porta[i].stato == 0)
			scrivi << "porta_blu";
		else if (t.porta[i].colore == 0 && t.porta[i].stato == 1)
			scrivi << "porta_blu_sfondata";
		else if (t.porta[i].colore == 2 && t.porta[i].stato == 0)
			scrivi << "porta_verde";
		else if (t.porta[i].colore == 2 && t.porta[i].stato == 1)
			scrivi << "porta_verde_sfondata";
		scrivi << '\n';
		scrivi << to_string(t.porta[i].x_cordinata).c_str(); scrivi << ' ';
		scrivi << to_string(t.porta[i].y_cordinata).c_str(); scrivi << ' ';
		scrivi << to_string(t.porta[i].zone[0]).c_str(); scrivi << ' ';
		scrivi << to_string(t.porta[i].zone[1]).c_str();
		scrivi << '\n';
	}
	scrivi << "end";
	scrivi.close();
}

