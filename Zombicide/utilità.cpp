#include "dati.h"
#include "Abilità.h"
#include "azioni.h"
#include "Azioni_zombie.h"
#include "armi_speciale.h"
#include "Matteo Draw.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5\allegro_primitives.h>

using namespace std;

void percorsi(StatoGioco& stato)
{
	vector<int> vettore_vuoto;
	vector<vector<int>> vettore_di_vettore;
	stato.percorsi_minimi.clear();
	for (size_t i = 0; i < stato.missione.dati.size(); i++)
	{
		vettore_vuoto.clear();
		vettore_vuoto.push_back(i);
		for (size_t j = 0; j < stato.missione.dati.size(); j++)
		{
			vettore_di_vettore.push_back(vettore_vuoto);
		}
		stato.percorsi_minimi.push_back(vettore_di_vettore);
		vettore_di_vettore.clear();
	}
	vector<vector<int>> costi;
	for (size_t i = 0; i < stato.missione.dati.size(); i++)
		vettore_vuoto.push_back(0);
	for (size_t i = 0; i < stato.missione.dati.size(); i++)
		costi.push_back(vettore_vuoto);
	for (size_t i = 0; i < stato.missione.dati.size(); i++)
		for (size_t j = 0; j < stato.missione.dati.size(); j++)
		{
			if (stato.missione.collisioni[i][j] == 0)
				costi[i][j] = -1;
			if (stato.missione.collisioni[i][j] == 1 || i == j)
				costi[i][j] = 1;
			if (stato.missione.collisioni[i][j] == 2)
				costi[i][j] = 10000;
		}
	for (size_t h = 0; h < stato.missione.dati.size(); h++) //nodo che si mette in mezzo al percorso tra due nodi
		for (size_t i = 0; i < stato.missione.dati.size(); i++)
			for (size_t j = 0; j < stato.missione.dati.size(); j++)
			{
				if (costi[i][h] != -1 && costi[h][j] != -1)
					if (costi[i][j] == -1 || costi[i][j]>costi[i][h] + costi[h][j])
					{
						costi[i][j] = costi[i][h] + costi[h][j];
						stato.percorsi_minimi[i][j] = stato.percorsi_minimi[h][j]; //sovrascrivo tutto
					}
					else if (costi[i][j] == costi[i][h] + costi[h][j])
					{
						//aggiungo tutti quelli non già presenti
						for (size_t k = 0; k < (int)stato.percorsi_minimi[h][j].size(); k++)
						{
							int valore = stato.percorsi_minimi[h][j][k];
							if (find(stato.percorsi_minimi[i][j].begin(), stato.percorsi_minimi[i][j].end(), valore) == stato.percorsi_minimi[i][j].end())
								stato.percorsi_minimi[i][j].push_back(valore);
						}
					}
			}
} //in i si deve inserire l'obiettivo e in j la posizione di partenza. il risultato è la zona o le zone adiacenti più convenienti
int cerca_direzione_stanza_lontana(StatoGioco& stato, int posizione, int obiettivo)
{
	if (stato.percorsi_minimi[obiettivo][posizione].size() > 0)
	{
		if (stato.missione.collisioni[stato.percorsi_minimi[obiettivo][posizione][0]][posizione] > 0 || stato.percorsi_minimi[obiettivo][posizione][0] == posizione)
			return stato.missione.orientamento_connessioni[posizione][stato.percorsi_minimi[obiettivo][posizione][0]];
		else
			return cerca_direzione_stanza_lontana(stato, posizione, stato.missione.orientamento_connessioni[posizione][stato.percorsi_minimi[posizione][obiettivo][0]]);
	}
	return -1;
}
bool cerca_carta_in_un_mazzo(Carta_equipaggiamento& carta, string nome, vector<Carta_equipaggiamento>& mazzo, bool togli_da_mazzo)
{
	for (int i = 0; i < (int)mazzo.size(); i++)
	{
		if (mazzo[i].nome == nome)
		{
			carta = mazzo[i];
			if (togli_da_mazzo == true)
				mazzo.erase(mazzo.begin() + i);
			return true;
		}
	}
	return false;
}
void scarica_free_action(StatoGioco& stato, int n_giocatore)
{
	vector<string> lista_abilità_da_scaricare;
	for (size_t i = 0; i < stato.giocatore[n_giocatore].abilità.size(); i++)
	{
		if (stato.giocatore[n_giocatore].abilità[i].scarica == false &&
			stato.giocatore[n_giocatore].abilità[i].quando_si_scarica == Abilità::DOPO_ESECUZIONE_AZIONE)
		{
			bool nessuna_corrispondenza = true;
			for (size_t j = 0; j < lista_abilità_da_scaricare.size(); j++)
				if (lista_abilità_da_scaricare[j] == stato.giocatore[n_giocatore].abilità[i].nome)
					nessuna_corrispondenza = false;
			if (nessuna_corrispondenza == true)
			{
				lista_abilità_da_scaricare.push_back(stato.giocatore[n_giocatore].abilità[i].nome);
				stato.giocatore[n_giocatore].abilità[i].
					funzione(stato, n_giocatore, i, Abilità::CODICE_SCARICA);
			}
		}
	}
}
bool verifica_ridondanza_nella_lista_di_combinazioni(StatoGioco& stato, vector<Carta_equipaggiamento> reagenti)
{
	for (size_t j = 0; j < stato.indicatore.lista_combinabili.size(); j++) //scorri lista delle combinazioni già messe in conto
	{
		int reagenti_trovati = 0;
		for (size_t k = 0; k < stato.indicatore.lista_combinabili[j].size(); k++) //scorri il vettore in esame della lista
			for (size_t t = 0; t < reagenti.size(); t++)	//scorri i reagenti per questa combinazione (molotov)
				if (stato.indicatore.lista_combinabili[j][k].nome == reagenti[t].nome) //confrontali
				{
					reagenti_trovati++; //per ogni reagente che corrisponde incremento il contatore
					break;
				}
		if (reagenti_trovati == reagenti.size()) //se alla fine corrispondono tutti allora questa combinazione è già in lista ed è ridondante metterla di nuovo
			return true;
	}
	return false;
}
void uccidi_giocatore(StatoGioco& stato, int giocatore)
{
	stato.giocatore[giocatore].stato = 0;
	stato.missione.dati[stato.giocatore[giocatore].posizione].ModificaValoreGiocatori(false, giocatore);
	int i_giocatore = 0;
	for (size_t i = 0; i < stato.ordine_giocatori.size(); i++)
		if (stato.ordine_giocatori[i]-1 == giocatore)
			i_giocatore = i;
	if (stato.Stato_gioco_gioco != 4)//non turno zombie, quindi turno dei giocatori
	{
		if (i_giocatore < stato.indicatore.giocatore_di_turno) //ha già giocato il suo turno
		{
			if (i_giocatore == 0) //se era il primo del turno, bisogna segnalare di non passare il "primo giocatore" al prox, perchè succede già con l'erase
				stato.indicatore.primo_giocatore_morto = true;
			stato.indicatore.giocatore_di_turno--; //per tenerlo in linea con la modifica della size(). E si continua tranquillamente il suo turno
		}
		else if (i_giocatore == stato.indicatore.giocatore_di_turno)
		{
			scarica_free_action(stato,giocatore);
			stato.indicatore.giocatore_di_turno--; //per tenerlo in linea con la modifica della size()
			stato.Stato_gioco_gioco = 1; //era il suo turno e quindi si interrompe e si passa al prox

		}
		//se capita ad un giocatore nei turni successivi si modifica solo la size() senza modificare il giocatore di turno e non si interrompe il suo turno
	}
	for (size_t i = 0; i < stato.ordine_giocatori.size(); i++)
		if (stato.ordine_giocatori[i]-1 == giocatore)
		{
			stato.ordine_giocatori.erase(stato.ordine_giocatori.begin() + i);
			break;
		}
	stato.numero_giocatori = stato.ordine_giocatori.size();
}
int calcola_pericolo(StatoGioco& stato)
{
	int esperienza=0;
	for (int i = 0; i < stato.numero_giocatori; i++)
		if (esperienza < stato.giocatore[stato.ordine_giocatori[i] - 1].esperienza)
			esperienza = stato.giocatore[stato.ordine_giocatori[i] - 1].esperienza;
	if (esperienza < 7)
		return 0;
	if (esperienza < 19)
		return 1;
	if (esperienza < 43)
		return 2;
	return 3;
}
bool cerca_abominio(StatoGioco& stato)
{
	for (size_t i = 0; i < stato.missione.dati.size(); i++)
		if (stato.missione.dati[i].abominio == true)
			return true;
	return false;
}
void abort_game(const char* message)
{
	cout << message << endl;
	exit(1);
}
void ricarica_armi(StatoGioco& stato)
{
	for (int i = 0; i < stato.numero_giocatori; i++)
		for (int j = 0; j < (int)stato.giocatore[stato.ordine_giocatori[i] - 1].equipaggiamento.size(); j++)
		{
			stato.giocatore[stato.ordine_giocatori[i] - 1].equipaggiamento[j].scarica_melee = false;
			stato.giocatore[stato.ordine_giocatori[i] - 1].equipaggiamento[j].scarica_ranged = false;
		}
}
void inquadra_zona_mappa(StatoGioco& stato, int zona, int larghezza, int altezza)
{
	int x = stato.indicatore.x_riquadro_mappa;
	int y = stato.indicatore.y_riquadro_mappa;
	int width_riquadro = stato.indicatore.w_riquadro_mappa;
	int height_riquadro = stato.indicatore.h_riquadro_mappa;
	double zoom = stato.indicatore.zoom;
	int cont = 0;
	for (size_t i = 0; i < stato.missione.Mappa_matrice.size(); i++)
		if (cont < (int)stato.missione.Mappa_matrice[i].size())
			cont = stato.missione.Mappa_matrice[i].size();

	int width_1 = 982 * cont; int height_1 = 982 * stato.missione.Mappa_matrice.size();
	int width_finale = 0; int height_finale = 0;
	if ((double)width_1 / height_1 >(double)width_riquadro / height_riquadro)
	{
		//width_finale deve essere uguale a width_1 (della mappa)
		//mentre height_finale devessere aumentato con dello spazione nero
		width_finale = width_1;
		height_finale = width_1 * height_riquadro / width_riquadro;
	}
	else
	{
		height_finale = height_1;
		width_finale = height_1 * width_riquadro / height_riquadro;
	}
	int w = (width_finale / zoom), h = (height_finale / zoom);
	//le x e le y cordinate devono riferisi alla mappa fittizzia (esempio: 982*n_tessere) ma tenendo conto delle nuove proporzioni di w e h
	// quindi: width_finale e height_finale servono (per capire la metà della grandezza della lente di focus in modo da centrare la zona)
	if (((stato.missione.x_cordinate_zone_mappa[zona]+(stato.missione.x_dimensione_zone_mappa[zona] / 2))-(w /2))>= 0)
		if ((stato.missione.x_cordinate_zone_mappa[zona] + (stato.missione.x_dimensione_zone_mappa[zona] / 2)
			- (w / 2))< width_finale - w)
			stato.indicatore.x_riquadro_mappa = (stato.missione.x_cordinate_zone_mappa[zona] + (stato.missione.x_dimensione_zone_mappa[zona] / 2)
				- (w / 2));
		else
			stato.indicatore.x_riquadro_mappa = width_finale - w;
	else
		stato.indicatore.x_riquadro_mappa = 0;
	if ((((stato.missione.y_cordinate_zone_mappa[zona] +
		stato.missione.y_dimensione_zone_mappa[zona] / 2))-(h /2))>= 0)
		if ((((stato.missione.y_cordinate_zone_mappa[zona] +
			stato.missione.y_dimensione_zone_mappa[zona] / 2))-(h /2))< height_finale - h)
			stato.indicatore.y_riquadro_mappa =
			(((stato.missione.y_cordinate_zone_mappa[zona] +
				stato.missione.y_dimensione_zone_mappa[zona] / 2))-(h /2));
		else
			stato.indicatore.y_riquadro_mappa = height_finale - h;
	else
		stato.indicatore.y_riquadro_mappa = 0;
}
bool timer_mio(int &iteratore, float secondi)
{
	if (iteratore < secondi * 60)
	{
		iteratore++;
		return false;
	}
	iteratore = 0;
	return true;
}
void crea_o_ripristina_backup_equipaggiamento(StatoGioco& stato, string crea_o_ripristina, vector<int> personaggi_di_interesse)
{
	int comando=0;
	if (crea_o_ripristina == "crea")
		comando = 0;
	else if(crea_o_ripristina == "ripristina")
		comando = 1;
	switch (comando)
	{
	case 0: //crea
		for (size_t i = 0; i < personaggi_di_interesse.size(); i++)	//creo il backup dell'equip
		{
			stato.giocatore[personaggi_di_interesse[i]].equipaggiamento_di_backup.clear();
			for (size_t j = 0; j < stato.giocatore[personaggi_di_interesse[i]].equipaggiamento.size(); j++)
			{
				stato.giocatore[personaggi_di_interesse[i]].equipaggiamento_di_backup.push_back
					(stato.giocatore[personaggi_di_interesse[i]].equipaggiamento[j]);
			}
		}
		break;
	case 1: //ripristina
		for (size_t i = 0; i < personaggi_di_interesse.size(); i++)	//ripristino equip da backup
		{
			stato.giocatore[personaggi_di_interesse[i]].equipaggiamento.clear();
			for (size_t j = 0; j < stato.giocatore[personaggi_di_interesse[i]].equipaggiamento_di_backup.size(); j++)
				stato.giocatore[personaggi_di_interesse[i]].equipaggiamento.push_back
				(stato.giocatore[personaggi_di_interesse[i]].equipaggiamento_di_backup[j]);
		}
		break;
	}
	
}
void determina_stanze_edificio(StatoGioco& stato, vector<int>& edificio, vector<int> inserisci_un_vettore_vuoto,int prima_stanza_edificio, int stanza_apertura_porta)
{
	vector<int> stanze_in_sospeso = inserisci_un_vettore_vuoto;
	edificio.push_back(prima_stanza_edificio);
	for (int verso = 0; verso < 8; verso++)
	{
		for (size_t i = 0; i < stato.missione.dati.size(); i++)
			if (stato.missione.collisioni[prima_stanza_edificio][i] == 1 && //se sono collegate
				stato.missione.dati[i].tipo==1&& //è una stanza di un edificio interno
				stato.missione.orientamento_connessioni[prima_stanza_edificio][i] == verso && //trovane una in quel verso
				i != stanza_apertura_porta&&	//non contare la stanza da dove hai aperto
				find(edificio.begin(), edificio.end(), i) == edificio.end() && //non è già nella lista delle stanze dell'edificio
				find(stanze_in_sospeso.begin(), stanze_in_sospeso.end(), i) == stanze_in_sospeso.end()) //non è stato già messo in lista delle stanze da controllare
				stanze_in_sospeso.push_back(i);
	}
	if (stanze_in_sospeso.size() > 0)
	{
		int prossima_stanza = stanze_in_sospeso[0];
		stanze_in_sospeso.erase(stanze_in_sospeso.begin());
		determina_stanze_edificio(stato, edificio, stanze_in_sospeso, prossima_stanza, stanza_apertura_porta);
	}
}
bool edificio_aperto(StatoGioco& stato,vector<int> edificio)
{
	for (int i = 0; i < (int)stato.missione.numero_porte; i++)
		if (stato.missione.porta[i].stato == 1)
			for (int j = 0; j <(int)edificio.size(); j++)
				if (stato.missione.porta[i].zone[0] == edificio[j] || stato.missione.porta[i].zone[1] == edificio[j])
					return true;
	return false;
}
float rotazione_in_base_direzione_zona(int direzione)
{
	switch (direzione)
	{
	case 0://su
		return 1.5 * ALLEGRO_PI;// 270;
	case 1://giù
		return  ALLEGRO_PI / 2;// 90;
	case 2://dx
		return 0 * ALLEGRO_PI;
	case 3://sx
		return ALLEGRO_PI;//180;
	case 4://su sx
		return 5 / 4 * ALLEGRO_PI;// 225;
	case 5://su dx
		return -ALLEGRO_PI / 4;// 415;
	case 6://giu sx
		return 3 / 4 * ALLEGRO_PI;// 135;
	case 7://giù dx
		return ALLEGRO_PI / 4;// 45;
	case 8://centro
		return 360;
	}
	return 360;
}
int stanze_raggiungibili_fino_a_x_distanza_da_un_punto(StatoGioco& stato,vector<int>& stanze,int numero_passi_disponibili,int partenza,bool peso)
{
	vector<int> V;
	if (numero_passi_disponibili > 0)
	{
		for (size_t i = 0; i < stato.missione.dati.size(); i++)
		{
			if (stato.missione.collisioni[partenza][i] == 1)
				if (find(stanze.begin(), stanze.end(), i) == stanze.end()) //non già presente
				{
					stanze.push_back(i);
					V.push_back(i);
					if ((stato.missione.dati[i].abominio == false && stato.missione.dati[i].walker == 0 && stato.missione.dati[i].runner == 0 &&
						stato.missione.dati[i].fat == 0) || peso == false) //se non ci sono zombie
						stanze_raggiungibili_fino_a_x_distanza_da_un_punto(stato, stanze, (numero_passi_disponibili - 1), i, peso);
				}
		}
	}
	return 1;
}
bool conversione_allegro_keycode_in_char(ALLEGRO_KEYBOARD_STATE keyboard_state, char& c)
{
	al_get_keyboard_state(&keyboard_state);
	if (al_key_down(&keyboard_state, ALLEGRO_KEY_Q))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c='Q';
		else
			c='q';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_W))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'W';
		else
			c= 'w';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_E))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'E';
		else
			c= 'e';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_F))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'F';
		else
			c= 'f';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_R))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'R';
		else
			c= 'r';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_T))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'T';
		else
			c= 't';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_Y))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'Y';
		else
			c= 'y';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_U))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'U';
		else
			c= 'u';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_I))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'I';
		else
			c= 'i';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_O))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'O';
		else
			c= 'o';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_P))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'P';
		else
			c= 'p';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_A))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'A';
		else
			c= 'a';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_S))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'S';
		else
			c= 's';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_D))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'D';
		else
			c= 'd';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_F))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'F';
		else
			c= 'f';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_G))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'G';
		else
			c= 'g';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_H))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'H';
		else
			c= 'h';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_J))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'J';
		else
			c= 'j';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_K))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'K';
		else
			c= 'k';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_L))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'L';
		else
			c= 'l';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_Z))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'Z';
		else
			c= 'z';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_C))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'C';
		else
			c= 'c';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_X))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'X';
		else
			c= 'x';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_V))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'V';
		else
			c= 'v';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_B))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'B';
		else
			c= 'b';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_N))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'N';
		else
			c= 'n';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_M))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= 'M';
		else
			c= 'm';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_0))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= '=';
		else
			c= '0';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_1))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= '!';
		else
			c= '1';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_2))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= '"';
		else
			c= '2';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_3))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c= '£';
		else
			c= '3';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_4))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c = '$';
		else
			c = '4';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_5))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c = '%';
		else
			c = '5';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_6))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c = '&';
		else
			c = '6';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_7))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c = '/';
		else
			c = '7';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_8))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c = '(';
		else
			c = '8';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_9))
	{
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT))
			c = ')';
		else
			c = '9';
		return true;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_SPACE))
	{
		c= ' ';
		return true;
	}
	return  false;
}
vector<vector<int>> cordinate_elenco_scritte(int x_partenza, int y_partenza, int w, int h, int numero, int numero_colonne, int y_font, int x_incremento, int y_incremento)
{
	vector<vector<int>> risultato;
	int x = x_partenza;
	int y = y_partenza;
	for (int i = 0; i < numero;)
	{
		for (int j = 0; j < numero_colonne; j++)
		{
			vector<int> parziale;
			parziale.push_back(x); parziale.push_back(y); parziale.push_back(x + w); parziale.push_back(y + y_font);
			risultato.push_back(parziale);
			x = x + x_incremento;
			i++;
		}
		x = x_partenza;
		y = y + y_font + y_incremento;
		if (y >= y_partenza+h)
			return risultato;
	}
	return risultato;
}
void converti_cordinate_mappa_per_draw(double zoom, vector<vector<Tessere_editor>> Mappa_matrice,int x_riquadro, int y_riquadro, int w_finestra_su_cui_stampare, int h_finestra_su_cui_stampare,
	int& dx, int& dy, int& dw,int& dh, int& dw_riquadro_zoom,int& dh_riquadro_zoom)
{
	int cont = 0;
	for (size_t i = 0; i < Mappa_matrice.size(); i++)
		if (cont < (int)Mappa_matrice[i].size())
			cont = Mappa_matrice[i].size();

	int width_mappa = 982 * cont; int height_mappa = 982 * Mappa_matrice.size();

	int width_finale = 0; int height_finale = 0;
	if ((double)width_mappa / height_mappa >(double)w_finestra_su_cui_stampare / h_finestra_su_cui_stampare)
	{
		//width_finale deve essere uguale a width_mappa 
		//mentre height_finale devessere aumentato con dello spazione nero
		width_finale = width_mappa;
		height_finale = width_mappa * h_finestra_su_cui_stampare / w_finestra_su_cui_stampare;
	}
	else
	{
		height_finale = height_mappa;
		width_finale = height_mappa * w_finestra_su_cui_stampare / h_finestra_su_cui_stampare;
	}
	dw_riquadro_zoom = (width_finale / zoom);
	dh_riquadro_zoom = (height_finale / zoom);
	dx =(dx-x_riquadro)* w_finestra_su_cui_stampare / dw_riquadro_zoom;
	dy = (dy-y_riquadro)* h_finestra_su_cui_stampare / dh_riquadro_zoom;
	dw = dw * w_finestra_su_cui_stampare / dw_riquadro_zoom;
	dh = dh * h_finestra_su_cui_stampare / dh_riquadro_zoom;
}
//Funzioni di input mie
bool input_mouse(int n,vector<vector<int>> x_y_w_h /*vettore int [4]*/, Cursore& c, bool attiva_scroll,int risoluzione[2],bool movimento_mouse)
{
	ALLEGRO_MOUSE_STATE mouse;
	al_get_mouse_state(&mouse);
	if (attiva_scroll == true)
	{
		if (mouse.z > 0) //scroll up
		{
			al_set_mouse_z(0);
			if (c.modificatore_scorrimento_elenco - 1 >= 0)
			{
				c.modificatore_scorrimento_elenco--;
				if (c.iteratore > (int)x_y_w_h.size() - 1 + c.modificatore_scorrimento_elenco)
					c.iteratore = x_y_w_h.size() - 1 + c.modificatore_scorrimento_elenco;
			}
		}
		else if (mouse.z < 0) //scroll down
		{
			al_set_mouse_z(0);
			if (c.modificatore_scorrimento_elenco + 1 <= n - (int)x_y_w_h.size())
			{
				c.modificatore_scorrimento_elenco++;
				if (c.iteratore < c.modificatore_scorrimento_elenco)
					c.iteratore = c.modificatore_scorrimento_elenco;
			}
		}
	}
	for (size_t i = 0; i < x_y_w_h.size(); i++)
		if (x_y_w_h[i].size()>0)
			if (mouse.x >= x_y_w_h[i][0] * risoluzione[0] / 1920 && mouse.x <= x_y_w_h[i][2] * risoluzione[0] / 1920
				&& mouse.y >= x_y_w_h[i][1] * risoluzione[1] / 1080 && mouse.y <= x_y_w_h[i][3] * risoluzione[1] / 1080)
			{
				if (movimento_mouse == true)
					c.iteratore = i + c.modificatore_scorrimento_elenco;
				return true;
			}
	return false;
}
void input_tastiera_9_direzioni(int& iteratore_cursore, ALLEGRO_EVENT& event, vector<int> vettore_con_le_9_size_di_direzione, bool ruota_90_sx, bool inverti_x, bool inverti_y)
{
	vector<vector<int>> matrice_9xsize_direzioni_delle_zone;
	vector<int> vuoto;
	for (int i = 0; i < 9; i++)
		matrice_9xsize_direzioni_delle_zone.push_back(vuoto);
	for (size_t i = 0; i < vettore_con_le_9_size_di_direzione.size(); i++)
		for (int j = 0; j < vettore_con_le_9_size_di_direzione[i]; j++)
			matrice_9xsize_direzioni_delle_zone[i].push_back(0);
	int contatore = 0;
	for (int i = 0; i < 9; i++)
		for (size_t j = 0; j < matrice_9xsize_direzioni_delle_zone[i].size(); j++)
		{
			matrice_9xsize_direzioni_delle_zone[i][j] = contatore;
			contatore++;
		}
	int riga = 0, colonna = 0, cursore = iteratore_cursore;
	for (int i = 0; i <9; i++)
		for (size_t j = 0; j <matrice_9xsize_direzioni_delle_zone[i].size(); j++)
			if (matrice_9xsize_direzioni_delle_zone[i][j] == cursore)
			{
				riga = i; colonna = j;
			}
	int up, down, left, right, provvisorio = 0;
	if (inverti_y == false)
	{
		up = ALLEGRO_KEY_UP;
		down = ALLEGRO_KEY_DOWN;
	}
	else
	{
		down = ALLEGRO_KEY_UP;
		up = ALLEGRO_KEY_DOWN;
	}
	if (inverti_x == false)
	{
		left = ALLEGRO_KEY_LEFT;
		right = ALLEGRO_KEY_RIGHT;
	}
	else
	{
		left = ALLEGRO_KEY_RIGHT;
		right = ALLEGRO_KEY_LEFT;
	}
	if (ruota_90_sx == true)
	{
		right = provvisorio;
		right = down;
		down = provvisorio;
		left = provvisorio;
		left = up;
		up = provvisorio;
	}
	if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (event.keyboard.keycode == up)
		{
			switch (riga)
			{
			case 0: //su
				break;
			case 1://su_sx
				if (matrice_9xsize_direzioni_delle_zone[0].size() > 0)//su
				{
					riga = 0;
					colonna = 0;
				}
				break;
			case 2://su_dx
				if (matrice_9xsize_direzioni_delle_zone[0].size() > 0)//su
				{
					riga = 0;
					colonna = 0;
				}
				break;
			case 3://sx
				if (matrice_9xsize_direzioni_delle_zone[1].size() > 0)//su sx
				{
					riga = 1;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[0].size() > 0)//su
				{
					riga = 0;
					colonna = 0;
				}
				break;
			case 4://centro
				if (matrice_9xsize_direzioni_delle_zone[0].size() > 0)
				{
					riga = 0;
					colonna = (matrice_9xsize_direzioni_delle_zone[riga].size() - 1) / 2;
				}
				break;
			case 5://destra
				if (matrice_9xsize_direzioni_delle_zone[2].size() > 0)//su destra
				{
					riga = 2;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[0].size() > 0)//su
				{
					riga = 0;
					colonna = matrice_9xsize_direzioni_delle_zone[riga].size() - 1;
				}
				break;
			case 6://giu_sx
				if (matrice_9xsize_direzioni_delle_zone[3].size() > 0)//sinistra
				{
					riga = 3;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[1].size() > 0)//su sinistra
				{
					riga = 1;
					colonna = 0;
				}
				break;
			case 7://giu_dx
				if (matrice_9xsize_direzioni_delle_zone[5].size() > 0)//destra
				{
					riga = 5;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[2].size() > 0)//su destra
				{
					riga = 2;
					colonna = 0;
				}
				break;
			case 8://giu
				if (matrice_9xsize_direzioni_delle_zone[4].size() > 0)//centro
				{
					riga = 4;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[0].size() > 0)//su
				{
					riga = 0;
					colonna = (matrice_9xsize_direzioni_delle_zone[riga].size() - 1) / 2;
				}
				break;
			}
		}
		if (event.keyboard.keycode == down)
		{
			switch (riga)
			{
			case 0: //su
				if (matrice_9xsize_direzioni_delle_zone[4].size() > 0)//centro
				{
					riga = 4;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[8].size() > 0)//giù
				{
					riga = 8;
					colonna = (matrice_9xsize_direzioni_delle_zone[riga].size() - 1) / 2;
				}
				break;
			case 1://su_sx
				if (matrice_9xsize_direzioni_delle_zone[3].size() > 0)//sinistra
				{
					riga = 3;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[6].size()>0)//giù sx
				{
					riga = 6;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[8].size() > 0)//giu
				{
					riga = 8;
					colonna = 0;
				}
				break;
			case 2://su_dx
				if (matrice_9xsize_direzioni_delle_zone[5].size() > 0)//destra
				{
					riga = 5;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[7].size()>0)//giù dx
				{
					riga = 7;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[8].size() > 0)//giù
				{
					riga = 8;
					colonna = matrice_9xsize_direzioni_delle_zone[riga].size() - 1;
				}
				break;
			case 3://sx
				if (matrice_9xsize_direzioni_delle_zone[6].size() > 0)//giù sx
				{
					riga = 6;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[8].size() > 0)//giù
				{
					riga = 8;
					colonna = 0;
				}
				break;
			case 4://centro
				if (matrice_9xsize_direzioni_delle_zone[8].size() > 0)//giù
				{
					riga = 8;
					colonna = (matrice_9xsize_direzioni_delle_zone[riga].size() - 1) / 2;
				}
				break;
			case 5://destra
				if (matrice_9xsize_direzioni_delle_zone[7].size() > 0)//giù dx
				{
					riga = 7;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[8].size() > 0)//giù
				{
					riga = 8;
					colonna = matrice_9xsize_direzioni_delle_zone[riga].size() - 1;
				}
				break;
			case 6://giu_sx
				if (matrice_9xsize_direzioni_delle_zone[8].size() > 0)//giù
				{
					riga = 8;
					colonna = 0;
				}
				break;
			case 7://giu_dx
				if (matrice_9xsize_direzioni_delle_zone[8].size() > 0)//giù
				{
					riga = 8;
					colonna = matrice_9xsize_direzioni_delle_zone[riga].size() - 1;
				}
				break;
			case 8://giu
				break;
			}
		}
		if (event.keyboard.keycode == left)
		{
			switch (riga)
			{
			case 0: //su
				if (colonna > 0)
					colonna--;
				else if (matrice_9xsize_direzioni_delle_zone[1].size() > 0)//su sinistra
				{
					riga = 1;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[3].size() > 0)//sinistra
				{
					riga = 3;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[6].size() > 0)//giù sinistra
				{
					riga = 6;
					colonna = 0;
				}
				break;
			case 1://su_sx
				if (colonna + 1<(int)matrice_9xsize_direzioni_delle_zone[riga].size())
					colonna++;
				break;
			case 2://su_dx
				if (colonna>0)
					colonna--;
				else if (matrice_9xsize_direzioni_delle_zone[4].size() > 0)//centro
				{
					riga = 0;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[1].size() > 0)//su sinistra
				{
					riga = 1;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[3].size() > 0)//sinistra
				{
					riga = 3;
					colonna = 0;

				}
				else if (matrice_9xsize_direzioni_delle_zone[6].size() > 0)//giu sx
				{
					riga = 3;
					colonna = 0;
				}
				break;
			case 3://sx
				if (colonna + 1<(int)matrice_9xsize_direzioni_delle_zone[riga].size())
					colonna++;
				break;
			case 4://centro
				if (matrice_9xsize_direzioni_delle_zone[3].size() > 0)//sinistra
				{
					riga = 3;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[1].size() > 0)//su sinistra
				{
					riga = 1;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[6].size() > 0)//giu sx
				{
					riga = 6;
					colonna = 0;
				}
				break;
			case 5://destra
				if (colonna>0)
					colonna--;
				else if (matrice_9xsize_direzioni_delle_zone[4].size() > 0)//centro
				{
					riga = 4;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[3].size() > 0)//sinistra
				{
					riga = 3;
					colonna = 0;

				}
				else if (matrice_9xsize_direzioni_delle_zone[1].size() > 0)//su sinistra
				{
					riga = 1;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[6].size() > 0)//giu sx
				{
					riga = 3;
					colonna = 0;
				}
				break;
			case 6://giu_sx
				if (colonna + 1<(int)matrice_9xsize_direzioni_delle_zone[riga].size())
					colonna++;
				break;
			case 7://giu_dx
				if (colonna>0)
					colonna--;
				else if (matrice_9xsize_direzioni_delle_zone[4].size() > 0)//centro
				{
					riga = 0;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[6].size() > 0)//giu sx
				{
					riga = 6;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[3].size() > 0)//sinistra
				{
					riga = 3;
					colonna = 0;

				}
				else if (matrice_9xsize_direzioni_delle_zone[1].size() > 0)//su sinistra
				{
					riga = 1;
					colonna = 0;
				}
				break;
			case 8://giu
				if (colonna>0)
					colonna--;
				else if (matrice_9xsize_direzioni_delle_zone[6].size() > 0)//giu sx
				{
					riga = 6;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[3].size() > 0)//sinistra
				{
					riga = 3;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[1].size() > 0)//su sinistra
				{
					riga = 1;
					colonna = 0;
				}
				break;
			}
		}
		if (event.keyboard.keycode == right)
			switch (riga)
			{
			case 0: //su
				if (colonna + 1 < (int)matrice_9xsize_direzioni_delle_zone[riga].size())
					colonna++;
				else if (matrice_9xsize_direzioni_delle_zone[7].size() > 0)//su dx
				{
					riga = 7;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[5].size() > 0)//destra
				{
					riga = 5;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[7].size() > 0)//giù dx
				{
					riga = 7;
					colonna = 0;
				}
				break;
			case 1://su_sx
				if (colonna >0)
					colonna--;
				else if (matrice_9xsize_direzioni_delle_zone[4].size() > 0)//centro
				{
					riga = 4;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[2].size() > 0)//su dx
				{
					riga = 2;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[5].size() > 0)//destra
				{
					riga = 5;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[7].size() > 0)//giù destra
				{
					riga = 7;
					colonna = 0;
				}
				break;
			case 2://su_dx
				if (colonna + 1 < (int)matrice_9xsize_direzioni_delle_zone[riga].size())
					colonna++;
				break;
			case 3://sx
				if (colonna >0)
					colonna--;
				else if (matrice_9xsize_direzioni_delle_zone[4].size() > 0)//centro
				{
					riga = 4;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[5].size() > 0)//destra
				{
					riga = 5;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[2].size() > 0)//su dx
				{
					riga = 2;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[7].size() > 0)//giù destra
				{
					riga = 7;
					colonna = 0;
				}
				break;
			case 4://centro
				if (matrice_9xsize_direzioni_delle_zone[5].size() > 0)//destra
				{
					riga = 5;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[2].size() > 0)//su dx
				{
					riga = 2;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[7].size() > 0)//giù destra
				{
					riga = 7;
					colonna = 0;
				}
				break;
			case 5://destra
				if (colonna + 1 < (int)matrice_9xsize_direzioni_delle_zone[riga].size())
					colonna++;
				break;
			case 6://giu_sx
				if (colonna >0)
					colonna--;
				else if (matrice_9xsize_direzioni_delle_zone[4].size() > 0)//centro
				{
					riga = 4;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[7].size() > 0)//giù destra
				{
					riga = 7;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[5].size() > 0)//destra
				{
					riga = 5;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[2].size() > 0)//su dx
				{
					riga = 2;
					colonna = 0;
				}
				break;
			case 7://giu_dx
				if (colonna + 1 < (int)matrice_9xsize_direzioni_delle_zone[riga].size())
					colonna++;
				break;
			case 8://giu
				if (colonna + 1 < (int)matrice_9xsize_direzioni_delle_zone[riga].size())
					colonna++;
				else if (matrice_9xsize_direzioni_delle_zone[7].size() > 0)//giù dx
				{
					riga = 7;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[5].size() > 0)//destra
				{
					riga = 5;
					colonna = 0;
				}
				else if (matrice_9xsize_direzioni_delle_zone[7].size() > 0)//su dx
				{
					riga = 7;
					colonna = 0;
				}
				break;
			}
	}
	iteratore_cursore = matrice_9xsize_direzioni_delle_zone[riga][colonna];
}
void logic_zoom_e_movimento_mappa(StatoGioco& stato, int larghezza, int altezza)
{
	ALLEGRO_KEYBOARD_STATE keyboard_state;
	al_get_keyboard_state(&keyboard_state);
	if (stato.indicatore.zoom < 1)
		stato.indicatore.zoom = 1;
	int max;
	if (larghezza > altezza)
		max = larghezza / 982;
	else
		max=altezza / 982;
	double zoom_relativo = stato.indicatore.zoom * 2 / 100;
	if (al_key_down(&keyboard_state, ALLEGRO_KEY_S))
	{
		if (stato.indicatore.y_riquadro_mappa + (int)(altezza / stato.indicatore.zoom) + 50 < altezza)
			stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa + 50;
		else
			stato.indicatore.y_riquadro_mappa = altezza - (int)(altezza / stato.indicatore.zoom);
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_W))
	{
		if (stato.indicatore.y_riquadro_mappa - 50 > 0)
			stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa - 50;
		else
			stato.indicatore.y_riquadro_mappa = 0;
	}
	if (al_key_down(&keyboard_state, ALLEGRO_KEY_D))
	{
		if (stato.indicatore.x_riquadro_mappa + (int)(larghezza / stato.indicatore.zoom) + 50 < larghezza)
			stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa + 50;
		else
			stato.indicatore.x_riquadro_mappa = larghezza - (int)(larghezza / stato.indicatore.zoom);
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_A))
	{
		if (stato.indicatore.x_riquadro_mappa - 50 > 0)
			stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa - 50;
		else
			stato.indicatore.x_riquadro_mappa = 0;
	}
	double vecchio_zoom = stato.indicatore.zoom;
	if (al_key_down(&keyboard_state, ALLEGRO_KEY_PGDN))
	{
		if (stato.indicatore.zoom - zoom_relativo > 1)
			stato.indicatore.zoom = stato.indicatore.zoom - zoom_relativo;
		else
			stato.indicatore.zoom = 1;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_PGUP))
	{
		if (stato.indicatore.zoom + zoom_relativo < max)
		{
			stato.indicatore.zoom = stato.indicatore.zoom + zoom_relativo;
		}
		else
			stato.indicatore.zoom = max;
	}	
	if (vecchio_zoom != stato.indicatore.zoom)
	{
		int slittamento = ((altezza / vecchio_zoom) - (altezza / stato.indicatore.zoom)) / 2;

		if (stato.indicatore.y_riquadro_mappa + (int)(altezza / stato.indicatore.zoom) + slittamento < altezza)
		{
			if (stato.indicatore.y_riquadro_mappa + slittamento > 0)
				stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa + slittamento;
			else
				stato.indicatore.y_riquadro_mappa = 0;
		}			
		else
		{
			stato.indicatore.y_riquadro_mappa = altezza - (int)(altezza / stato.indicatore.zoom);
		}
			 
		slittamento = ((larghezza / vecchio_zoom) - (larghezza / stato.indicatore.zoom)) / 2;

		if (stato.indicatore.x_riquadro_mappa + (int)(larghezza / stato.indicatore.zoom) + slittamento < larghezza)
		{
			if (stato.indicatore.x_riquadro_mappa + slittamento > 0)
				stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa + slittamento;
			else
				stato.indicatore.x_riquadro_mappa = 0;
		}
		else
			stato.indicatore.x_riquadro_mappa = larghezza - (int)(larghezza / stato.indicatore.zoom);
	}
}
/*
void logic_zoom_e_movimento_mappa(StatoGioco& stato , int larghezza, int altezza)
{
	ALLEGRO_KEYBOARD_STATE keyboard_state;
	al_get_keyboard_state(&keyboard_state);
	if (al_key_down(&keyboard_state, ALLEGRO_KEY_S))
	{
		if (stato.indicatore.y_riquadro_mappa + 50 < altezza - (stato.indicatore.zoom + stato.indicatore.h_riquadro_mappa))
			stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa + 50;
		else
			stato.indicatore.y_riquadro_mappa = altezza - (stato.indicatore.zoom + stato.indicatore.h_riquadro_mappa);
	}

	if (al_key_down(&keyboard_state, ALLEGRO_KEY_W))
	{
		if (stato.indicatore.y_riquadro_mappa - 50 > 0)
			stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa - 50;
		else
			stato.indicatore.y_riquadro_mappa = 0;
	}

	if (al_key_down(&keyboard_state, ALLEGRO_KEY_A))
	{
		if (stato.indicatore.x_riquadro_mappa - 50 > 0)
			stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa - 50;
		else
			stato.indicatore.x_riquadro_mappa = 0;
	}

	if (al_key_down(&keyboard_state, ALLEGRO_KEY_D))
	{
		if (stato.indicatore.x_riquadro_mappa + 50 < larghezza - (stato.indicatore.zoom + stato.indicatore.w_riquadro_mappa))
			stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa + 50;
		else
			stato.indicatore.x_riquadro_mappa = larghezza - (stato.indicatore.zoom + stato.indicatore.w_riquadro_mappa);
	}

	if (al_key_down(&keyboard_state, ALLEGRO_KEY_PGDN))
		if (stato.indicatore.zoom + stato.indicatore.x_riquadro_mappa + 50 + stato.indicatore.w_riquadro_mappa < larghezza &&
			stato.indicatore.zoom + stato.indicatore.y_riquadro_mappa + 50 + stato.indicatore.h_riquadro_mappa < altezza)
		{
			stato.indicatore.zoom = stato.indicatore.zoom + 50;
			if (stato.indicatore.x_riquadro_mappa - 25 > 0)
				stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa - 25;
			else
				stato.indicatore.x_riquadro_mappa = 0;
			if (stato.indicatore.y_riquadro_mappa - 25 > 0)
				stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa - 25;
			else
				stato.indicatore.y_riquadro_mappa = 0;
		}
		else
		{
			if (stato.indicatore.zoom + stato.indicatore.y_riquadro_mappa + 50 + stato.indicatore.h_riquadro_mappa < altezza &&
				stato.indicatore.x_riquadro_mappa - 50 > 0)
			{
				stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa - 50;
				stato.indicatore.zoom = stato.indicatore.zoom + 50;
				if (stato.indicatore.y_riquadro_mappa - 25 > 0)
					stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa - 25;
				else
					stato.indicatore.y_riquadro_mappa = 0;
			}			
			else if (stato.indicatore.zoom + stato.indicatore.x_riquadro_mappa + 50+ stato.indicatore.w_riquadro_mappa < larghezza &&
				stato.indicatore.y_riquadro_mappa - 50 > 0)
			{
				stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa - 50;
				stato.indicatore.zoom = stato.indicatore.zoom + 50;
				if (stato.indicatore.x_riquadro_mappa - 25 > 0)
					stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa - 25;
				else
					stato.indicatore.x_riquadro_mappa = 0;
			}
			else if (stato.indicatore.x_riquadro_mappa - 50 > 0 && stato.indicatore.y_riquadro_mappa - 50 > 0)
			{
				stato.indicatore.x_riquadro_mappa=stato.indicatore.x_riquadro_mappa - 50;
				stato.indicatore.y_riquadro_mappa=stato.indicatore.y_riquadro_mappa - 50;
				stato.indicatore.zoom = stato.indicatore.zoom + 50;
			}
			else if (larghezza <= altezza)
			{
				stato.indicatore.zoom = larghezza - stato.indicatore.w_riquadro_mappa;
				if (stato.indicatore.y_riquadro_mappa > altezza - stato.indicatore.zoom - stato.indicatore.w_riquadro_mappa)
					stato.indicatore.y_riquadro_mappa = altezza - stato.indicatore.zoom - stato.indicatore.w_riquadro_mappa;
				stato.indicatore.x_riquadro_mappa = 0;
			}
			else if (altezza < larghezza)
			{
				stato.indicatore.zoom = altezza - stato.indicatore.h_riquadro_mappa;
				stato.indicatore.y_riquadro_mappa = 0;
				if (stato.indicatore.x_riquadro_mappa > larghezza - stato.indicatore.zoom- stato.indicatore.h_riquadro_mappa)
					stato.indicatore.x_riquadro_mappa=larghezza - stato.indicatore.zoom- stato.indicatore.h_riquadro_mappa;
			}
		}
	if (al_key_down(&keyboard_state, ALLEGRO_KEY_PGUP))
		if (stato.indicatore.zoom - 50 > -500)
		{
			stato.indicatore.zoom = stato.indicatore.zoom - 50;
			if (stato.indicatore.x_riquadro_mappa + 25 + stato.indicatore.w_riquadro_mappa < larghezza)
				stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa + 25;
			if (stato.indicatore.y_riquadro_mappa + 25 + stato.indicatore.h_riquadro_mappa < altezza)
				stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa + 25;
		}
}
*/
void logic_movimento_collage_schede_personaggi(int& scroll, int scroll_max)
{
	ALLEGRO_KEYBOARD_STATE keyboard_state;
	al_get_keyboard_state(&keyboard_state);
	if (scroll_max < 0)
		scroll_max = 0;
	if (al_key_down(&keyboard_state, ALLEGRO_KEY_K)) //giù
	{
		if (scroll + 50 <= scroll_max)
			scroll = scroll + 50;
		else
			scroll = scroll_max;
	}
	else if (al_key_down(&keyboard_state, ALLEGRO_KEY_I)) //sù
	{
		if (scroll - 50 >= 0)
			scroll = scroll - 50;
		else
			scroll = 0;
	}
}
vector<int> creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(StatoGioco& stato, vector<int>& zone_di_movimento_disponibili,
	int posizione)
{
	vector<int> risposta;
	vector<int> su, giu, destra, sinistra, su_sx, su_dx, giu_sx, giu_dx, centro;
	vector<vector<int>> elenco_direzioni_rispettando_gli_indici;
	elenco_direzioni_rispettando_gli_indici.push_back(su);	elenco_direzioni_rispettando_gli_indici.push_back(giu); elenco_direzioni_rispettando_gli_indici.push_back(destra);
	elenco_direzioni_rispettando_gli_indici.push_back(sinistra); elenco_direzioni_rispettando_gli_indici.push_back(su_sx);	elenco_direzioni_rispettando_gli_indici.push_back(su_dx);
	elenco_direzioni_rispettando_gli_indici.push_back(giu_sx);	elenco_direzioni_rispettando_gli_indici.push_back(giu_dx);	elenco_direzioni_rispettando_gli_indici.push_back(centro);
	for (int i = 0; i < (int)zone_di_movimento_disponibili.size(); i++)
	{
		elenco_direzioni_rispettando_gli_indici[cerca_direzione_stanza_lontana(stato, posizione, zone_di_movimento_disponibili[i])].push_back(zone_di_movimento_disponibili[i]);
	}
	risposta.push_back(elenco_direzioni_rispettando_gli_indici[0].size()); risposta.push_back(elenco_direzioni_rispettando_gli_indici[4].size());
	risposta.push_back(elenco_direzioni_rispettando_gli_indici[5].size()); risposta.push_back(elenco_direzioni_rispettando_gli_indici[3].size());
	risposta.push_back(elenco_direzioni_rispettando_gli_indici[8].size()); risposta.push_back(elenco_direzioni_rispettando_gli_indici[2].size());
	risposta.push_back(elenco_direzioni_rispettando_gli_indici[6].size()); risposta.push_back(elenco_direzioni_rispettando_gli_indici[7].size());
	risposta.push_back(elenco_direzioni_rispettando_gli_indici[1].size());
	zone_di_movimento_disponibili.clear();
	for (size_t i = 0; i < elenco_direzioni_rispettando_gli_indici[0].size(); i++)
		zone_di_movimento_disponibili.push_back(elenco_direzioni_rispettando_gli_indici[0][i]);
	for (size_t i = 0; i < elenco_direzioni_rispettando_gli_indici[4].size(); i++)
		zone_di_movimento_disponibili.push_back(elenco_direzioni_rispettando_gli_indici[4][i]);
	for (size_t i = 0; i < elenco_direzioni_rispettando_gli_indici[5].size(); i++)
		zone_di_movimento_disponibili.push_back(elenco_direzioni_rispettando_gli_indici[5][i]);
	for (size_t i = 0; i < elenco_direzioni_rispettando_gli_indici[3].size(); i++)
		zone_di_movimento_disponibili.push_back(elenco_direzioni_rispettando_gli_indici[3][i]);
	for (size_t i = 0; i < elenco_direzioni_rispettando_gli_indici[8].size(); i++)
		zone_di_movimento_disponibili.push_back(elenco_direzioni_rispettando_gli_indici[8][i]);
	for (size_t i = 0; i < elenco_direzioni_rispettando_gli_indici[2].size(); i++)
		zone_di_movimento_disponibili.push_back(elenco_direzioni_rispettando_gli_indici[2][i]);
	for (size_t i = 0; i < elenco_direzioni_rispettando_gli_indici[6].size(); i++)
		zone_di_movimento_disponibili.push_back(elenco_direzioni_rispettando_gli_indici[6][i]);
	for (size_t i = 0; i < elenco_direzioni_rispettando_gli_indici[7].size(); i++)
		zone_di_movimento_disponibili.push_back(elenco_direzioni_rispettando_gli_indici[7][i]);
	for (size_t i = 0; i < elenco_direzioni_rispettando_gli_indici[1].size(); i++)
		zone_di_movimento_disponibili.push_back(elenco_direzioni_rispettando_gli_indici[1][i]);
	return risposta;
}
void input_tastiera_classico(int& iteratore_cursore, ALLEGRO_EVENT& event, vector<int> numero_colonne_per_ogni_riga, bool indici_verticali, 
	bool ruota_matrice_90dx, bool inverti_x, bool inverti_y)
{
	vector<vector<int>> matrice;
	vector<int> riga_della_colonna;
	int riga_più_lunga;
	int contatore = 0;
	if (ruota_matrice_90dx == false)
	{
		for (size_t i = 0; i < numero_colonne_per_ogni_riga.size(); i++)
		{
			for (int j = 0; j < numero_colonne_per_ogni_riga[i]; j++)
			{
				riga_della_colonna.push_back(contatore);
				contatore++;
			}
			matrice.push_back(riga_della_colonna);
			riga_della_colonna.clear();
		}
	}
	else
	{
		riga_più_lunga = 0;
		for (size_t i = 0; i < numero_colonne_per_ogni_riga.size(); i++)
			if (numero_colonne_per_ogni_riga[i]>riga_più_lunga)
				riga_più_lunga = numero_colonne_per_ogni_riga[i];
		int k = 0;
		for (int i = 0; i < riga_più_lunga; i++)
		{
			matrice.push_back({});
			for (int j = numero_colonne_per_ogni_riga.size() - 1; j >= 0; j--)
			{
				if (numero_colonne_per_ogni_riga[j] > i)
				{
					matrice[i].push_back(k);
					k++;
				}
				else
					matrice[i].push_back(-1);
			}
		}
		numero_colonne_per_ogni_riga.clear();
		for (size_t i = 0; i < matrice.size(); i++)
			numero_colonne_per_ogni_riga.push_back(matrice[i].size());
	}
	if (indici_verticali == true)
	{
		vector<vector<int>> M;
		vector<int> riga_per_ogni_colonna;
		riga_più_lunga = 0;
		for (size_t i = 0; i < numero_colonne_per_ogni_riga.size(); i++)
			if (numero_colonne_per_ogni_riga[i]>riga_più_lunga)
				riga_più_lunga = numero_colonne_per_ogni_riga[i];
		int contatore = 0;
		M = matrice;
		for (int i = 0; i <riga_più_lunga; i++)
		{
			for (size_t j = 0; j <numero_colonne_per_ogni_riga.size(); j++)
			{
				if ((int)matrice[j].size()>i)
				{
					if (matrice[j][i] != -1)
					{
						M[j][i] = contatore;
						contatore++;
					}
				}
			}
		}
		matrice = M;
	}
	int riga=0, colonna=0;
	for (size_t i = 0; i < matrice.size(); i++)
	{
		for (size_t j = 0; j < matrice[i].size(); j++)
			if (matrice[i][j] == iteratore_cursore)
			{
				riga = i;
				colonna = j;
			}
	}
	int up, down, left, right, provvisorio = 0;
	if (inverti_y == false)
	{
		up = ALLEGRO_KEY_UP;
		down = ALLEGRO_KEY_DOWN;
	}
	else
	{
		down = ALLEGRO_KEY_UP;
		up = ALLEGRO_KEY_DOWN;
	}
	if (inverti_x == false)
	{
		left = ALLEGRO_KEY_LEFT;
		right = ALLEGRO_KEY_RIGHT;
	}
	else
	{
		left = ALLEGRO_KEY_RIGHT;
		right = ALLEGRO_KEY_LEFT;
	}
	if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (event.keyboard.keycode == left)
		{
			if (colonna - 1 >= 0)
				if (matrice[riga][colonna - 1] != -1)
					colonna--;
		}
		if (event.keyboard.keycode == right)
		{
			if (colonna + 1 < (int)matrice[riga].size())
				if (matrice[riga][colonna + 1] != -1)
					colonna++;
		}
		if (event.keyboard.keycode == up)
		{
			if (riga - 1 >= 0)
				if (matrice[riga - 1][colonna] != -1)
					riga--;
		}
		if (event.keyboard.keycode == down)
		{
			if (riga + 1 < (int)matrice.size())
			{
				if (matrice[riga + 1][colonna] != -1)
				{
					riga++;
					if (colonna > (int)matrice[riga].size() - 1)
						colonna = matrice[riga].size() - 1;
				}
			}
		}
	}
	iteratore_cursore = matrice[riga][colonna];
}
void input_stringa_da_tastiera(ALLEGRO_EVENT &event, string& s,string::iterator& i)
{//se la stringa non è una var globale fai cazzi
	ALLEGRO_KEYBOARD_STATE keyboard_state;
	al_get_keyboard_state(&keyboard_state);
	char c = ' ';
	if (event.keyboard.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (conversione_allegro_keycode_in_char(keyboard_state, c) == true)
		{
			i=s.insert(i, c);
			i = i + 1;
		}
		if (al_key_down(&keyboard_state, ALLEGRO_KEY_BACKSPACE))
		{
			if (i != s.begin())
			{
				i--;
				i = s.erase(i);
			}
		}
	}
}
//Operazione di logic
bool logic_spawn_zombie(StatoGioco& stato, ALLEGRO_EVENT event, vector<int> elenco_stanze_spawn)
{
	switch (stato.indicatore.stato_spawn)
	{
	case 0://faccio backup delle cordinate
		stato.indicatore.x_riquadro_mappa_backup = stato.indicatore.x_riquadro_mappa;
		stato.indicatore.y_riquadro_mappa_backup = stato.indicatore.y_riquadro_mappa_backup;
		stato.indicatore.stato_spawn++;
	case 1://scegli carta e azionala
		if (spawn_zombie(stato, event, elenco_stanze_spawn) == true)
		{
			if (stato.indicatore.check_extraturn == true)
			{
				stato.Mazzo_spawn.erase(stato.Mazzo_spawn.begin() + stato.indicatore.random);
				if (stato.indicatore.azioneZombie_contatore_spawn + 1 < (int)stato.missione.elenco_spawn.size())
					stato.indicatore.azioneZombie_contatore_spawn++;
				stato.indicatore.check_extraturn = false;
				return false;
			}
			stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa_backup;
			stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa_backup;
			return true;
		}
		else if (stato.indicatore.check_extraturn == false)
			stato.indicatore.stato_spawn++;
		break;
	case 2://mostra la carta e passa al prossimo spawn
		if (timer_mio(stato.Cursori["Timer"]["Mostra carta spawn"].iteratore, 2) == true || (event.type == ALLEGRO_EVENT_KEY_DOWN&&event.keyboard.keycode == ALLEGRO_KEY_ENTER))
		{
			stato.Mazzo_spawn.erase(stato.Mazzo_spawn.begin() + stato.indicatore.random);
			if (stato.Mazzo_spawn.size() == 0)
				stato.Mazzo_spawn = stato.Mazzo_spawn_originale;
			stato.indicatore.azioneZombie_contatore_spawn++;

			stato.indicatore.stato_spawn--;
		}
		break;
	}
	return false;
}
bool menu_opzioni_logic(StatoGioco& stato, ALLEGRO_EVENT event, ALLEGRO_DISPLAY*& display)
{
	int indice_max_numero_opzioni = 2;
	input_tastiera_classico(stato.Cursori["Menu opzioni"]["Lista verticale"].iteratore, event, { indice_max_numero_opzioni + 1 }, false, true, false, false);
	switch (stato.Cursori["Menu opzioni"]["Lista verticale"].iteratore)
	{
	case 0://risoluzioni
		input_tastiera_classico(stato.Cursori["Menu opzioni"]["Risoluzioni"].iteratore, event, { (int)stato.indicatore.risoluzioni.size() }, false, false, false, false);
		break;
	case 1://schermo
		input_tastiera_classico(stato.Cursori["Menu opzioni"]["Schermo"].iteratore, event, { 2 }, false, false, false, false);
		break;
	case 2://fonts
		input_tastiera_classico(stato.Cursori["Menu opzioni"]["Fonts_provvisorio"].iteratore, event, { (int)stato.font.size() }, false, false, false, false);
		break;
	}
	if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
		{
			stato.Cursori["Menu opzioni"]["Fonts"].iteratore= stato.Cursori["Menu opzioni"]["Fonts_provvisorio"].iteratore;
			stato.Cursori["Menu opzioni"]["Lista verticale"].iteratore = 0;
			stato.indicatore.risoluzione[0] = stato.indicatore.risoluzioni[stato.Cursori["Menu opzioni"]["Risoluzioni"].iteratore][0];
			stato.indicatore.risoluzione[1] = stato.indicatore.risoluzioni[stato.Cursori["Menu opzioni"]["Risoluzioni"].iteratore][1];
			stato.indicatore.crea_nuovo_display = true;

			if (stato.Cursori["Menu opzioni"]["Schermo"].iteratore == 0)
				stato.indicatore.tag_nuovo_display = ALLEGRO_FULLSCREEN
				;
			else if (stato.Cursori["Menu opzioni"]["Schermo"].iteratore == 1)
				stato.indicatore.tag_nuovo_display = ALLEGRO_WINDOWED;
			
			string percorso = "configurazioni.txt";
			vector<string> tutto;
			string stringa = "risoluzione ";
			stringa = stringa + to_string(stato.indicatore.risoluzione[0]).c_str();
			stringa = stringa + " ";
			stringa = stringa + to_string(stato.indicatore.risoluzione[1]);
			tutto.push_back(stringa);
			stringa.clear();
			stringa = "schermo ";
			if (stato.Cursori["Menu opzioni"]["Schermo"].iteratore == 0)
				stringa = stringa + "full-screen";
			else if (stato.Cursori["Menu opzioni"]["Schermo"].iteratore == 1)
				stringa = stringa + "windowed";
			tutto.push_back(stringa);
			stringa.clear();
			stringa = "font ";
			stringa = stringa + stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome;
			tutto.push_back(stringa);
			ofstream scrivi = ofstream(percorso);
			if (scrivi.is_open())
			{
				for (size_t i = 0; i < tutto.size(); i++)
				{
					scrivi << tutto[i];
					scrivi << "\n";
				}
				scrivi << "fine_file";
				scrivi.close();
			}
			return true;
		}
		if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		{
			stato.Cursori["Menu opzioni"]["Fonts_provvisorio"].iteratore = stato.Cursori["Menu opzioni"]["Fonts"].iteratore;
			stato.Cursori["Menu opzioni"]["Lista verticale"].iteratore = 0;
			ifstream leggi = ifstream("configurazioni.txt");
			string stringa = "";
			int numero = 0;
			while (stringa != "fine_file")
			{
				leggi >> stringa;
				if (stringa == "risoluzione")
				{
					leggi >> numero;
					for (size_t i = 0; i < stato.indicatore.risoluzioni.size(); i++)
						if (stato.indicatore.risoluzioni[i][0] == numero)
						{
							leggi >> numero;
							if (stato.indicatore.risoluzioni[i][1] == numero)
								stato.Cursori["Menu opzioni"]["Risoluzioni"].iteratore = i;
						}
				}
				if (stringa == "schermo")
				{
					leggi >> stringa;
					if (stringa == "full-screen")
						stato.Cursori["Menu opzioni"]["Schermo"].iteratore = 0;
					else if (stringa == "windowed")
						stato.Cursori["Menu opzioni"]["Schermo"].iteratore = 1;
				}
			}
			return true;
		}
	}
	return false;
}
bool assegna_ferite(StatoGioco& stato, ALLEGRO_EVENT event, ALLEGRO_KEYBOARD_STATE& keyboard_state)
{
	Carta_equipaggiamento ferita; //da inizializzare
	Immagini Immagini;
	cerca_carta_in_un_mazzo(ferita, "Wounded", stato.Mazzo_speciale, false);
	if (stato.indicatore.ferite == 0 || stato.indicatore.scelta_personaggio_da_ferire.size() == 0) // in caso di conclusione
	{
		for (size_t i = 0; i < stato.indicatore.personaggi_morti.size(); i++) //assegna le morti
			uccidi_giocatore(stato, stato.indicatore.personaggi_morti[i]);
		for (size_t i = 0; i < stato.indicatore.scelta_personaggio_da_ferire.size(); i++) //assegna i feriti
			if (cerca_carta_in_un_mazzo(ferita, "Wounded",
				stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[i]].equipaggiamento, false) == true)
				stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[i]].ferite = 1;
		stato.indicatore.ferite = 0;
		stato.indicatore.stato_menu_ferite = 0;
		return true;
	}
	int r1, r2;
	int risultato=-1;
	stato.Cursori["Assegna ferite"]["Scelta personaggio"];
	stato.Cursori["Scelta equip personaggio"]["Lista"];
	switch (stato.indicatore.stato_menu_ferite)
	{
	case 0:	//scegli umano
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_UP)
			{
				if (stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore > 1)
					stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore = stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore - 2;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
			{

				if (stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore < (int)stato.indicatore.scelta_personaggio_da_ferire.size() - 1 - 1)
					stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore = stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore + 2;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
			{
				if (stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore % 2 != 0 && stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore - 1 >= 0)
					stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore--;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
			{
				if (stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore % 2 == 0 && stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore + 1 <
					(int)stato.indicatore.scelta_personaggio_da_ferire.size())
					stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore++;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				for (size_t i = 0; i < stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore]].abilità.size(); i++)
				{
					if (stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore]].abilità[i].tipo ==
						Abilità::DIFESA)
						risultato = stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore]].abilità[i].funzione
						(stato, stato.indicatore.scelta_personaggio_da_ferire[stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore], i, Abilità::CODICE_ESEGUI);
				}
				if (risultato == 1)
					break;
				if (cerca_carta_in_un_mazzo(ferita, "Wounded",	//se ferito... uccidi direttamente
					stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore]].equipaggiamento, false) == true)
				{
					stato.indicatore.personaggi_morti.push_back(stato.indicatore.scelta_personaggio_da_ferire[stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore]);
					stato.indicatore.scelta_personaggio_da_ferire.erase(stato.indicatore.scelta_personaggio_da_ferire.begin() + stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore);
					stato.indicatore.ferite--;
					stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore = 0;
				}
				else if (stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore]]
					.equipaggiamento.size() == 0) //se non ha equip, ferisci direttamente
				{
					stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore]]
						.equipaggiamento.push_back(ferita);
					stato.indicatore.ferite--;
					stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore = 0;

				}
				else//altrimenti vai a sostituire ferita con equip
				{
					stato.indicatore.stato_menu_ferite = 1;
					stato.indicatore.azioneZombie_umanoDaFerire = stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore;
					stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore = 0;
				}
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_TAB)
				stato.indicatore.stato_menu_ferite = 2;
		}
		break;
	case 1:	//distribuisci ferite
		r1 = 0; r2 = 0;
		if ((int)stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.indicatore.azioneZombie_umanoDaFerire]].equipaggiamento.size() > 2)
		{
			r1 = 2; r2 = (int)stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.indicatore.azioneZombie_umanoDaFerire]].equipaggiamento.size() - r1;
		}
		else
			r1 = (int)stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.indicatore.azioneZombie_umanoDaFerire]].equipaggiamento.size();
		input_tastiera_9_direzioni(stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore, event, { r1,0,0,0,0,0,0,0,r2 }, false, false, true);
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				if (stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.indicatore.azioneZombie_umanoDaFerire]].
					equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore].tipologia&Carta_equipaggiamento::DIFESA)
					stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.indicatore.azioneZombie_umanoDaFerire]].
					equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore].speciale(stato, event);
				else
					stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.indicatore.azioneZombie_umanoDaFerire]]
					.equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore] = ferita;

				stato.indicatore.ferite--;
				stato.indicatore.azioneZombie_umanoDaFerire = 0;
				stato.indicatore.stato_menu_ferite = 0;
				stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore = 0;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.indicatore.stato_menu_ferite = 0;
				stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore = 0;
			}
		}
		break;
	case 2: //visualizza mappa
		if (event.type == ALLEGRO_EVENT_KEY_DOWN && (event.keyboard.keycode == ALLEGRO_KEY_ENTER || event.keyboard.keycode == ALLEGRO_KEY_ESCAPE))
			stato.indicatore.stato_menu_ferite = 0;
		break;
	}
	return false;
}
bool aggiungi_abilità_giocatore(StatoGioco& stato, ALLEGRO_EVENT event, int indice_giocatore)
{
	vector<Abilità> scelta;
	vector<int> input;
	stato.indicatore.colore_abilità_da_inserire = 0;
	switch (stato.indicatore.stato_assegna_abilità)
	{
	case 0://conta quante abilità deve inserire
		stato.indicatore.numero_abilità_da_inserire = 0;
		if (stato.giocatore[indice_giocatore].abilità.size() == 0)
			stato.indicatore.numero_abilità_da_inserire++;
		if (stato.giocatore[indice_giocatore].esperienza > 6 && stato.giocatore[indice_giocatore].abilità.size() < 2)
			stato.indicatore.numero_abilità_da_inserire++;
		if (stato.giocatore[indice_giocatore].esperienza > 18 && stato.giocatore[indice_giocatore].abilità.size() < 3)
			stato.indicatore.numero_abilità_da_inserire++;
		if (stato.giocatore[indice_giocatore].esperienza > 42 && stato.giocatore[indice_giocatore].abilità.size() < 4)
			stato.indicatore.numero_abilità_da_inserire++;
		if (stato.indicatore.numero_abilità_da_inserire == 0)
			return true; //finito
		else
			stato.indicatore.stato_assegna_abilità++;
	case 1://verifica colore abilità da aggiungere
		if (stato.giocatore[indice_giocatore].abilità.size() == 0)
			stato.indicatore.colore_abilità_da_inserire = 0;
		else if (stato.giocatore[indice_giocatore].esperienza > 6 && stato.giocatore[indice_giocatore].abilità.size() < 2)
			stato.indicatore.colore_abilità_da_inserire = 1;
		else if (stato.giocatore[indice_giocatore].esperienza > 18 &&stato.giocatore[indice_giocatore].abilità.size() < 3)
			stato.indicatore.colore_abilità_da_inserire = 2;
		else if (stato.giocatore[indice_giocatore].esperienza > 42 && stato.giocatore[indice_giocatore].abilità.size() < 4)
			stato.indicatore.colore_abilità_da_inserire = 3;
		else
			return true; //finito ma dovrebbe controllarlo già nello stato precedente
	}
	for (size_t i = 0; i < stato.elenco_personaggi.size(); i++)
		if (stato.giocatore[indice_giocatore].personaggio == stato.personaggio[i].nome)
			for (size_t j = 0; j < stato.personaggio[i].abilità[stato.indicatore.colore_abilità_da_inserire].size(); j++)
				scelta.push_back(stato.personaggio[i].abilità[stato.indicatore.colore_abilità_da_inserire][j]);
	if (scelta.size() < 2) //solo una
	{
		stato.giocatore[indice_giocatore].abilità.push_back(scelta[0]);
		stato.indicatore.stato_assegna_abilità = 0;
	}
	else if (scelta.size()>1)
	{
		vector<vector<int>> cordinate;
		for (size_t i = 0; i < scelta.size(); i++)
		{
			input.push_back(1);
			int x = 320 + ((357 + ((int)stato.giocatore[indice_giocatore].abilità.size() * 407)) * 1280 / stato.giocatore[indice_giocatore].Scheda_personaggio_con_equip->width);
			int y = 174 + ((183 + (57 * (int)i)) * 732 / stato.giocatore[indice_giocatore].Scheda_personaggio_con_equip->height);
			int w=x+86, h=y+49;
			cordinate.push_back({x,y,w,h});
		}
		input_tastiera_classico(stato.Cursori["Aggiungi abilità giocatore"]["Lista"].iteratore, event, { (int)input.size() }, false, true, false, false);
		bool in_posizione=input_mouse((int)input.size(), cordinate, stato.Cursori["Aggiungi abilità giocatore"]["Lista"], false, stato.indicatore.risoluzione,
			event.type == ALLEGRO_EVENT_MOUSE_AXES);

		if (event.type == ALLEGRO_EVENT_KEY_DOWN&&event.keyboard.keycode == ALLEGRO_KEY_ENTER ||
			(event.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN&&event.mouse.button==1&& in_posizione==true))
		{
			stato.giocatore[indice_giocatore].abilità.push_back(scelta[stato.Cursori["Aggiungi abilità giocatore"]["Lista"].iteratore]);
			stato.Cursori["Aggiungi abilità giocatore"]["Lista"].iteratore = 0;
			stato.indicatore.stato_assegna_abilità = 0;
		}
	}
	return false;
}
//Operazioni di Draw
	//Private:
void al_draw_scaled_text(vector<fonts>& fonts, Immagini& immagini_e_fonts, MATTEO_BITMAP* immagini_su_cui_stampare, string font, ALLEGRO_COLOR colore, float x, float y,
	float x_dim /*negativo per auto proporzione con altezza e diventa x_dim_MAX*/,
	float y_dim/*setta -1 per autoproporzionare con lunghezza*/, int flags, string text)
{
	int indice_font = 0;
	int indice_font_25 = 0;
	for (size_t i = 0; i < font.size(); i++)
		if (fonts[i].nome == font)
		{
			indice_font = i;
			for (size_t j = 0; j < fonts[i].allegro_font.size(); j++)
			{
				if (fonts[i].dim_font[j] == 25)
					indice_font_25 = j;
			}
		}
	//setto la prima come dim migliore da scalare tra quel tipo di font (esempio: arial-> 2)
	int font_dim_migliore = 0;
	int x_dim_max;
	int font_dim_x;
	if (x_dim > 0)
	{ //cerca in base alla x_dim richiesta
		font_dim_x = 25 * x_dim / al_get_text_width(fonts[indice_font].allegro_font[indice_font_25], text.c_str());
		for (size_t i = 0; i < fonts[indice_font].dim_font.size(); i++)
			if (fonts[indice_font].dim_font[i] >= font_dim_x)
			{
				font_dim_migliore = i;
				break;
			}
	}
	else if (x_dim < 0)
	{//cerca in base all'altezza x_dim richiesta
		for (size_t i = 0; i < fonts[indice_font].height_dim.size(); i++)
			if (fonts[indice_font].height_dim[i] >= y_dim)
			{
				font_dim_migliore = i;
				break;
			}
		x_dim = x_dim*(-1);
		x_dim_max = x_dim;
		x_dim = fonts[indice_font].dim_font[font_dim_migliore] * al_get_text_width(fonts[indice_font].allegro_font[indice_font_25], text.c_str()) / 25;
		if (x_dim > x_dim_max)
		{
			x_dim = x_dim_max;
			font_dim_x = 25 * x_dim / al_get_text_width(fonts[indice_font].allegro_font[indice_font_25], text.c_str());
			for (size_t i = 0; i < fonts[indice_font].dim_font.size(); i++)
				if (fonts[indice_font].dim_font[i] >= font_dim_x)
				{
					font_dim_migliore = i;
					break;
				}
		}
	}
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.testo_scalato);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	al_draw_text(fonts[indice_font].allegro_font[font_dim_migliore], colore, 0, 0, flags, text.c_str());
	MATTEO_BITMAP::set_target_bitmap(immagini_su_cui_stampare);
	if (y_dim == -1)
		y_dim = fonts[indice_font].height_dim[font_dim_migliore];
	m_draw_scaled_bitmap(immagini_e_fonts.testo_scalato, 0, 0,
		al_get_text_width(fonts[indice_font].allegro_font[font_dim_migliore], text.c_str()),
		fonts[indice_font].height_dim[font_dim_migliore], x, y, x_dim, y_dim, 0);
}
void m_draw_scaled_tinted_bitmap(StatoGioco& stato,Immagini& immagini_e_fonts,MATTEO_BITMAP* immagine_da_disegnare,int x,int y,int x_dim,int y_dim,ALLEGRO_COLOR tinta)
{
	MATTEO_BITMAP* immagine_su_cui_stampare;
	immagine_su_cui_stampare = MATTEO_BITMAP::get_target_bitmap();
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.scaled_tinted_bitmap);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	m_draw_scaled_bitmap(immagine_da_disegnare, 0, 0, immagine_da_disegnare->width, immagine_da_disegnare->height,
		0, 0, x_dim, y_dim, 0);
	MATTEO_BITMAP::set_target_bitmap(immagine_su_cui_stampare);
	m_draw_tinted_bitmap_region(immagini_e_fonts.scaled_tinted_bitmap, tinta, 0, 0,x_dim,y_dim,x,y, 0);
}
void draw_selezione_segnalini_zombie_o_personaggi(StatoGioco& stato, Immagini& immagini_e_fonts, vector<int> zombie_o_giocatori_selezionabili,int posizione_cursore,
	int zona,unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	MATTEO_BITMAP* bitmap_corrente;
	bitmap_corrente = MATTEO_BITMAP::get_target_bitmap();
	MATTEO_BITMAP::set_target_bitmap(stato.missione.mappa);
	int x = stato.indicatore.x_riquadro_mappa;
	int y = stato.indicatore.y_riquadro_mappa;
	int null;
	int w, h;
	int width_riquadro = stato.indicatore.w_riquadro_mappa;
	int height_riquadro = stato.indicatore.h_riquadro_mappa;
	converti_cordinate_mappa_per_draw(stato.indicatore.zoom, stato.missione.Mappa_matrice, x, y, width_riquadro, height_riquadro, null, null, null, null, w, h);
	
	float lato_corto;
	if (stato.missione.x_dimensione_zone_mappa[zona] < stato.missione.y_dimensione_zone_mappa[zona])
		lato_corto = stato.missione.x_dimensione_zone_mappa[zona];
	else
		lato_corto = stato.missione.y_dimensione_zone_mappa[zona];

	float scala_dim = lato_corto / 200; //mi riferisco a misure fatte in un quadrato 200x200
	float x_dim = stato.missione.x_dimensione_zone_mappa[zona];
	float y_dim = stato.missione.y_dimensione_zone_mappa[zona];
	float scala_x = x_dim / 200;
	float scala_y = y_dim / 200;

	int dx = 0;
	int dy = 0;
	int dw = 0;
	int dh = 0;
	
	if (zombie_o_giocatori_selezionabili[posizione_cursore] <= 5) //selezione giocatori
	{
		dx = ((stato.missione.x_cordinate_zone_mappa[zona] - x) + (0 * scala_x) + ((33 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
		dy = ((stato.missione.y_cordinate_zone_mappa[zona] - y) + (170 * scala_y) + ((33 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
		dw = (33 * scala_dim)*width_riquadro / w;
		dh = (33 * scala_dim)*height_riquadro / h;
	
		dx = dx + (dw*zombie_o_giocatori_selezionabili[posizione_cursore]);
		al_draw_filled_rectangle(dx,dy,dx+dw,dy+dh,al_map_rgba(r,g,b,a));
	}
	else //selezione zombie
	{
		int n = 0;
		dy = ((stato.missione.y_cordinate_zone_mappa[zona] - y) + (0 * scala_y) + ((50 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
		dw = (50 * scala_dim)*width_riquadro / w;
		dh = (50 * scala_dim)*height_riquadro / h;
		switch (zombie_o_giocatori_selezionabili[posizione_cursore])
		{
		case 6://walker
			n = 0;
			break;
		case 7://runner
			n = 1;
			break;
		case 8://fat
			n = 2;
			break;
		case 9://abominio
			n = 3;
			break;
		case 10://fat e abominio
			n = 2;
			dx = ((stato.missione.x_cordinate_zone_mappa[zona] - x) + ((50*n) * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			al_draw_filled_rectangle(dx, dy, dw, dh, al_map_rgba(r, g, b, a));
			n = 3;
			break;
		}
		dx = ((stato.missione.x_cordinate_zone_mappa[zona] - x) + ((50 * n) * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
		al_draw_filled_rectangle(dx, dy, dx+dw, dy+dh, al_map_rgba(r, g, b, a));
	}
}
void draw_selezione_segnalini_porte(StatoGioco& stato, Immagini& immagini_e_fonts, Porta& p, Missione& m,
	double zoom, int x_riquadro_mappa, int y_riquadro_mappa, int width_riquadro, int height_riquadro,
	unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	MATTEO_BITMAP* bitmap_corrente;
	bitmap_corrente = MATTEO_BITMAP::get_target_bitmap();
	MATTEO_BITMAP::set_target_bitmap(m.mappa);

	int x = x_riquadro_mappa;
	int y = y_riquadro_mappa;
	int null;
	int w, h;
	converti_cordinate_mappa_per_draw(zoom, m.Mappa_matrice, x, y, width_riquadro, height_riquadro, null, null, null, null, w, h);

	int dx = (p.x_cordinata - x)*width_riquadro / w;
	int dy = (p.y_cordinata - y)*height_riquadro / h;
	int dw = p.dim*width_riquadro / w;
	int dh = p.dim*height_riquadro / h;

	al_draw_filled_rectangle(dx, dy, dx + dw, dy + dh, al_map_rgba(r, g, b, a));
	MATTEO_BITMAP::set_target_bitmap(bitmap_corrente);
}
void draw_pulsante_pericolo(StatoGioco& stato, Immagini& immagini_e_fonts, fonts f, float dx, float dy, float dw, float dh)
{
	int pericolo = calcola_pericolo(stato);
	int r=0, g=0, b=0;
	switch (pericolo)
	{
	case 0://blu
		r = 0; g = 0; b = 255;
		break;
	case 1://giallo
		r = 200; g = 200; b = 0;
		break;
	case 2://arancione
		r = 255; g = 128; b = 0;
		break;
	case 3://rosso
		r = 255; g = 0; b = 0;
		break;
	}
	al_draw_filled_rectangle(dx, dy, dx+dw, dy+dh, al_map_rgba(r, g, b, 0.2));
	al_draw_scaled_text(stato.font, immagini_e_fonts, MATTEO_BITMAP::get_target_bitmap(), f.nome, al_map_rgb(0, 0, 0), dx, dy, dw, dh, 0, "Pericolo");
}
void draw_schede_personaggi(StatoGioco& stato, Immagini& immagini_e_fonts, int indice_giocatore)
{
	int x = 0;
	vector<vector<string>> copia_abilità_personaggio;
	vector<string> lista_abilità_k;
	vector<string> copia_abilità_giocatore;
	if (stato.giocatore[indice_giocatore].stato == 1)
	{
		for (size_t j = 0; j < stato.elenco_personaggi.size(); j++)
			if (stato.giocatore[indice_giocatore].personaggio == stato.elenco_personaggi[j])
			{
				//schede personaggio con: equip, segnalino esp, segnalino abilità
				int lunghezza = stato.personaggio[j].immagine_scheda->width;
				int altezza = stato.personaggio[j].immagine_scheda->height;
				MATTEO_BITMAP::set_target_bitmap(stato.giocatore[indice_giocatore].Scheda_personaggio_con_equip);
				al_clear_to_color(al_map_rgba(0, 0, 0, 0));
				m_draw_bitmap(stato.personaggio[j].immagine_scheda, 0, 0, 0);
				for (size_t indice_equip = 2, x = (214 * lunghezza) / 690; indice_equip < 5 && indice_equip < stato.giocatore[indice_giocatore].equipaggiamento.size(); indice_equip++, x = x + (162 * lunghezza) / 690)
					m_draw_scaled_bitmap(stato.giocatore[indice_giocatore].equipaggiamento[indice_equip].Immagine, 0, 0, stato.giocatore[indice_giocatore].equipaggiamento[indice_equip].Immagine->width,
						stato.giocatore[indice_giocatore].equipaggiamento[indice_equip].Immagine->height, x, (135 * altezza) / 369, (139 * lunghezza) / 690, (192 * altezza) / 369, 0);
				for (size_t indice_equip = 0, x = (296 * lunghezza) / 690; indice_equip < 2 && indice_equip < stato.giocatore[indice_giocatore].equipaggiamento.size(); indice_equip++, x = x + (162 * lunghezza) / 690)
					m_draw_scaled_bitmap(stato.giocatore[indice_giocatore].equipaggiamento[indice_equip].Immagine, 0, 0, stato.giocatore[indice_giocatore].equipaggiamento[indice_equip].Immagine->width,
						stato.giocatore[indice_giocatore].equipaggiamento[indice_equip].Immagine->height, x, (160 * altezza) / 369, (139 * lunghezza) / 690, (192 * altezza) / 369, 0);
				m_draw_scaled_bitmap(stato.personaggio[j].immagine_segna_esperienza, 0, 0, stato.personaggio[j].immagine_segna_esperienza->width,
					stato.personaggio[j].immagine_segna_esperienza->height, 39 + (stato.giocatore[indice_giocatore].esperienza * 37), 43, 35, 100, 0);
				copia_abilità_giocatore.clear();
				for (size_t k = 0; k < stato.giocatore[indice_giocatore].abilità.size(); k++)
					copia_abilità_giocatore.push_back(stato.giocatore[indice_giocatore].abilità[k].nome);
				copia_abilità_personaggio.clear();
				lista_abilità_k.clear();
				for (int k = 0; k < 4; k++)
				{
					lista_abilità_k.clear();
					for (size_t l = 0; l < stato.personaggio[j].abilità[k].size(); l++)
						lista_abilità_k.push_back(stato.personaggio[j].abilità[k][l].nome);
					copia_abilità_personaggio.push_back(lista_abilità_k);
				}
				for (size_t indice_abilità = 0; indice_abilità< stato.giocatore[indice_giocatore].abilità.size(); indice_abilità++)
				{
					int x_segnalino = 0;
					int y_segnalino = 0;
					for (int k = 0; k < 4; k++)
						for (size_t l = 0; l < stato.personaggio[j].abilità[k].size(); l++)
							if (copia_abilità_personaggio[k][l] == copia_abilità_giocatore[indice_abilità])
							{
								x_segnalino = 357 + (k * 407);
								y_segnalino = 183 + (l * 57);
								m_draw_scaled_bitmap(immagini_e_fonts.segna_abilità, 0, 0,
									immagini_e_fonts.segna_abilità->width, immagini_e_fonts.segna_abilità->height,
									x_segnalino, y_segnalino, 86, 49, 0);
								copia_abilità_personaggio[k][l] = "";
								copia_abilità_giocatore[indice_abilità] = "";
							}
				}
				//barra esperienza con segnalino
				MATTEO_BITMAP::set_target_bitmap(stato.giocatore[indice_giocatore].Barra_esperienza_con_segnalino);
				al_clear_to_color(al_map_rgba(0, 0, 0, 0));
				m_draw_bitmap(stato.personaggio[j].immagine_esperienza, 0, 0, 0);
				m_draw_scaled_bitmap(stato.personaggio[j].immagine_segna_esperienza, 0, 0,
					stato.personaggio[j].immagine_segna_esperienza->width, stato.personaggio[j].immagine_segna_esperienza->height,
					138 + (34 * stato.giocatore[indice_giocatore].esperienza), 0, 34, 70, 0);
			}
	}
}
void draw_cursore_su_equip_personaggio(StatoGioco& stato, Immagini& immagini_e_fonts, int personaggio, int size_equip, ALLEGRO_COLOR colore_selezione, int indice_equip_selezionato)
{
	int armi_in_mano = 0;
	int armi_in_riserva = 0;
	armi_in_riserva = size_equip - 2;
	if (armi_in_riserva < 0)
		armi_in_riserva = 0;
	armi_in_mano = size_equip - armi_in_riserva;
	int x = 0;
	stato.giocatore[personaggio].redraw_scheda = true;
	MATTEO_BITMAP::set_target_bitmap(stato.giocatore[personaggio].Scheda_personaggio_con_equip);
	for (int j = 2, x = 214; j < armi_in_riserva + armi_in_mano; j++, x = x + 162)
		if (indice_equip_selezionato == j)
			al_draw_filled_rectangle(x*stato.giocatore[personaggio].Scheda_personaggio_con_equip->width / 690,
				135 * stato.giocatore[personaggio].Scheda_personaggio_con_equip->height / 366,
				(x*stato.giocatore[personaggio].Scheda_personaggio_con_equip->width / 690) +
				139 * stato.giocatore[personaggio].Scheda_personaggio_con_equip->width / 690,
				135 * stato.giocatore[personaggio].Scheda_personaggio_con_equip->height / 366 +
				192 * stato.giocatore[personaggio].Scheda_personaggio_con_equip->height / 366, colore_selezione);
	x = 0;
	for (int j = 0, x = 296; j < armi_in_mano; j++, x = x + 162)
		if (indice_equip_selezionato == j)
			al_draw_filled_rectangle(x*stato.giocatore[personaggio].Scheda_personaggio_con_equip->width / 690,
				160 * stato.giocatore[personaggio].Scheda_personaggio_con_equip->height/ 366,
				x*stato.giocatore[personaggio].Scheda_personaggio_con_equip->width / 690 +
				139 * stato.giocatore[personaggio].Scheda_personaggio_con_equip->width / 690,
				160 * stato.giocatore[personaggio].Scheda_personaggio_con_equip->height / 366 +
				192 * stato.giocatore[personaggio].Scheda_personaggio_con_equip->height / 366, colore_selezione);
}
void draw_selezione_zona_mappa(StatoGioco& stato, MATTEO_BITMAP* mappa, int zona,ALLEGRO_COLOR colore)
{
	int x = stato.indicatore.x_riquadro_mappa;
	int y = stato.indicatore.y_riquadro_mappa;
	int width_riquadro = stato.indicatore.w_riquadro_mappa;
	int height_riquadro = stato.indicatore.h_riquadro_mappa;
	double zoom = stato.indicatore.zoom;
	int cont = 0;
	for (size_t i = 0; i < stato.missione.Mappa_matrice.size(); i++)
		if (cont < (int)stato.missione.Mappa_matrice[i].size())
			cont = stato.missione.Mappa_matrice[i].size();

	int width_1 = 982 * cont; int height_1 = 982 * stato.missione.Mappa_matrice.size();
	int width_finale = 0; int height_finale = 0;
	if ((double)width_1 / height_1 >(double)width_riquadro / height_riquadro)
	{
		//width_finale deve essere uguale a width_1 (della mappa)
		//mentre height_finale devessere aumentato con dello spazione nero
		width_finale = width_1;
		height_finale = width_1 * height_riquadro / width_riquadro;
	}
	else
	{
		height_finale = height_1;
		width_finale = height_1 * width_riquadro / height_riquadro;
	}
	int w = (width_finale / zoom), h = (height_finale / zoom);
	int dx = (stato.missione.x_cordinate_zone_mappa[zona]-x) * width_riquadro / w;
	int dy = (stato.missione.y_cordinate_zone_mappa[zona]-y) * height_riquadro / h;
	int dw = (stato.missione.x_dimensione_zone_mappa[zona]-x+ stato.missione.x_cordinate_zone_mappa[zona]) * width_riquadro / w;
	int dh = (stato.missione.y_cordinate_zone_mappa[zona]-y + stato.missione.y_dimensione_zone_mappa[zona]) * height_riquadro / h;
	MATTEO_BITMAP::set_target_bitmap(mappa);
	al_draw_filled_rectangle(dx,dy,dw,dh,colore);
}
void draw_finestra_con_elenco_e_cursore(StatoGioco& stato, Immagini& immagini_e_fonts, string font, ALLEGRO_COLOR colore, Cursore& c,
	vector<string> elenco, int x_partenza, int y_partenza, int incremento_y, int incremento_x, int numero_colonne, int x_dim_max, int y_dim_max,
	int y_dim_font, int x_dim_cursore, int y_dim_cursore,MATTEO_BITMAP* immagine_finestra,bool abilita_tinta_finestra,ALLEGRO_COLOR tinta_finestra,
	ALLEGRO_COLOR tinta_cornice,float spessore_cornice)
{
	int indice_fonts;
	int font_dim_richiesto = 0;
	int indice_font_dim_da_scalare = 0;
	for (size_t i = 0; i < stato.font.size(); i++)
		if (stato.font[i].nome == font)
		{
			indice_fonts = i;
			for (size_t j = 0; j < stato.font[i].height_dim.size(); j++)
				if (stato.font[i].height_dim[j] >= y_dim_font)
				{
					font_dim_richiesto = j;
					break;
				}
		}
	int x_dim_finestra = x_dim_max; //senza contare la x_dim_cursore, solo la stringa + lunga
	x_dim_max = x_dim_max / numero_colonne; //divido per il numero di colonne
	while (c.iteratore<c.modificatore_scorrimento_elenco*numero_colonne) //quando voi salire di scroll
		c.modificatore_scorrimento_elenco--;
	int numero_stringhe_totale_per_colonna = (elenco.size() / numero_colonne);
	if (elenco.size() % numero_colonne > 0)
		numero_stringhe_totale_per_colonna++;
	int numero_stringhe_visualizzabili_per_colonna = y_dim_max / (y_dim_font+incremento_y);
	if ((y_dim_max % (y_dim_font + incremento_y)) / y_dim_font > 0)
		numero_stringhe_visualizzabili_per_colonna++;
	if (numero_stringhe_visualizzabili_per_colonna > numero_stringhe_totale_per_colonna)
		numero_stringhe_visualizzabili_per_colonna = numero_stringhe_totale_per_colonna;
	while (c.iteratore >= ((c.modificatore_scorrimento_elenco-1)*numero_colonne) + (numero_colonne*numero_stringhe_visualizzabili_per_colonna)+numero_colonne) //quando scendi di scroll
		c.modificatore_scorrimento_elenco++;
	vector<int> indici_fonts_di_tutte_le_stringhe;
	vector<int> size_delle_stringhe;
	for (size_t i = 0; i < elenco.size(); i++)
	{
		//cerca il font giusto da scalare
		int width_richiesto = stato.font[indice_fonts].dim_font[font_dim_richiesto] *
			al_get_text_width(stato.font[indice_fonts].allegro_font[2], elenco[i].c_str()) /
			25;
		if (width_richiesto <= x_dim_max) //va bene il font richiesto
		{
			indici_fonts_di_tutte_le_stringhe.push_back(font_dim_richiesto);
			size_delle_stringhe.push_back(width_richiesto);
		}
		else //cerca un font migliore
		{
			indice_font_dim_da_scalare = 25 * x_dim_max /
				al_get_text_width(stato.font[indice_fonts].allegro_font[2], elenco[i].c_str()); //qui in realtà prende il valore assoluto di dim
			for (size_t j = c.modificatore_scorrimento_elenco; j < stato.font[indice_fonts].dim_font.size(); j++)
				if (stato.font[indice_fonts].dim_font[j] >= indice_font_dim_da_scalare)
				{
					indici_fonts_di_tutte_le_stringhe.push_back(j); //qui diventa davvero l'indice del font da scalare
					break;
				}
			size_delle_stringhe.push_back(x_dim_max);
		}
	}
	int contatore = c.modificatore_scorrimento_elenco*numero_colonne;
	vector<vector<string>> matrice_elenco;
	vector<vector<int>> matrice_size_stringhe_per_colonne;
	vector<vector<int>> matrice_y_dim_fonts_da_scalare_per_colonne;
	vector<vector<int>> matrice_indici_iteratore;
	for (int i = 0; i < numero_stringhe_totale_per_colonna-c.modificatore_scorrimento_elenco; i++) //ordino tutto in matrice in base al numero di colonne
	{
		vector<int> riga;
		vector<int> riga_2;
		vector<int> indice_iteratore;
		vector<string> stringa;
		for (int j = 0; j <  numero_colonne; j++)
		{
			if (contatore < (int)size_delle_stringhe.size())
			{
				stringa.push_back(elenco[contatore]);
				riga.push_back(size_delle_stringhe[contatore]);
				riga_2.push_back(stato.font[indice_fonts].height_dim[indici_fonts_di_tutte_le_stringhe[contatore]]);
				indice_iteratore.push_back(contatore);
				contatore++;
			}
		}
		matrice_elenco.push_back(stringa);
		stringa.clear();
		matrice_size_stringhe_per_colonne.push_back(riga);
		riga.clear();
		matrice_y_dim_fonts_da_scalare_per_colonne.push_back(riga_2);
		riga_2.clear();
		matrice_indici_iteratore.push_back(indice_iteratore);
		indice_iteratore.clear();
	}
	vector<int> x_size_colonne;
	for (int i = 0; i < numero_colonne; i++)
		x_size_colonne.push_back(0);
	int size_riga = 0;
	for (size_t i = 0; i < matrice_size_stringhe_per_colonne.size(); i++)
	{
		for (size_t j = 0; j < matrice_size_stringhe_per_colonne[i].size(); j++)
			if (x_size_colonne[j] < matrice_size_stringhe_per_colonne[i][j])
				x_size_colonne[j] = matrice_size_stringhe_per_colonne[i][j];
	}
	MATTEO_BITMAP* bitmap_corrente;
	bitmap_corrente = MATTEO_BITMAP::get_target_bitmap();
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.finestra_con_cursore_e_elenco);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	if (immagine_finestra != NULL) //stampo finestra
	{
		if (abilita_tinta_finestra == true)
			m_draw_scaled_tinted_bitmap(stato, immagini_e_fonts, immagine_finestra, x_partenza, y_partenza, x_dim_finestra + x_dim_cursore + (incremento_x*numero_colonne - 1),
				y_dim_max, tinta_finestra);
		else
			m_draw_scaled_bitmap(immagine_finestra, 0, 0, immagine_finestra->width, immagine_finestra->height,
				x_partenza, y_partenza, x_dim_finestra + x_dim_cursore + (incremento_x*numero_colonne - 1), y_dim_max, 0);
		al_draw_rectangle(x_partenza, y_partenza, x_partenza + x_dim_finestra + x_dim_cursore + (incremento_x*numero_colonne - 1),
			y_partenza + y_dim_max,	tinta_cornice, spessore_cornice);
	}
	int x_spostamento = 0;
	for (int k=0; k < numero_stringhe_visualizzabili_per_colonna; k++)
	{
		for (int j = 0; j < numero_colonne; j++)
		{
			if (j<(int)matrice_elenco[k].size())
			{
				if (c.iteratore == matrice_indici_iteratore[k][j])
					m_draw_scaled_bitmap(c.Immagine_cursore, 0, 0, c.Immagine_cursore->width, c.Immagine_cursore->height,
						x_partenza + x_spostamento, y_partenza + (k*(y_dim_font + incremento_y)) + (y_dim_font / 2) - (y_dim_cursore / 2), x_dim_cursore, y_dim_cursore, 0);
				if (numero_stringhe_visualizzabili_per_colonna< (int)matrice_indici_iteratore.size())//triangolo scorrimento in basso
					m_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_giu, 0, 0,
						immagini_e_fonts.triangolo_scorrimento_giu->width, immagini_e_fonts.triangolo_scorrimento_giu->height,
						x_partenza + x_dim_finestra, y_partenza + y_dim_max,
						50, 50, 0);
				if (c.modificatore_scorrimento_elenco>0)//triangolo scorrimento in alto
					m_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_su, 0, 0,
						immagini_e_fonts.triangolo_scorrimento_giu->width, immagini_e_fonts.triangolo_scorrimento_giu->height,
						x_partenza + x_dim_finestra, y_partenza,
						50,50, 0);
				al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.finestra_con_cursore_e_elenco, font, colore,
					x_partenza + x_dim_cursore + x_spostamento,
					y_partenza + (k*(y_dim_font + incremento_y))+((y_dim_font/2)-(matrice_y_dim_fonts_da_scalare_per_colonne[k][j]/2)),
					-(x_size_colonne[j]),
					matrice_y_dim_fonts_da_scalare_per_colonne[k][j], 0, matrice_elenco[k][j]);
				x_spostamento = x_spostamento + x_size_colonne[j]+x_dim_cursore;
			}
		}
		x_spostamento = 0;
	}
	MATTEO_BITMAP::set_target_bitmap(bitmap_corrente);
	m_draw_scaled_bitmap(immagini_e_fonts.finestra_con_cursore_e_elenco, 0, 0, 1920, 1080, 0, 0, 1920, 1080, 0);

	/*
		//stampa
		if (c.iteratore == i)//se presente stampa il cursore
			al_draw_scaled_bitmap(c.Immagine_cursore, 0, 0, al_get_bitmap_width(c.Immagine_cursore), al_get_bitmap_height(c.Immagine_cursore),
				x_partenza + x_incremento, y_partenza + y_incremento + (stato.font[indice_fonts].height_dim[indice_font_dim_da_scalare] / 2) - (al_get_bitmap_height(c.Immagine_cursore) / 2),
				x_dim_cursore, y_dim_cursore, 0);
		al_draw_scaled_text(stato, immagini_e_fonts, immagini_e_fonts.cursore_e_elenco, font, colore, x_partenza + x_incremento + x_dim_cursore
			, y_partenza + y_incremento, width_richiesto, -1, 0, elenco[i].c_str());
		if ((i + 1) % numero_colonne == 0) //calcola se tornare alla prima colonna
		{
			x_incremento = 0;
			y_incremento = y_incremento + stato.font[indice_fonts].height_dim[indice_font_dim_da_scalare] + incremento_y;
		}
		else //altrimenti incrementa di 1 la colonna su cui stampare
		{
			x_incremento = x_incremento + x_dim_max + incremento_x + al_get_bitmap_height(immagini_e_fonts.cursore);
		}
		if (c.modificatore_scorrimento_elenco > 0) // stampa triangolo in alto, perchè ci sono stringhe non stampate più sù
			al_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_su, 0, 0,
				al_get_bitmap_width(immagini_e_fonts.triangolo_scorrimento_su), al_get_bitmap_height(immagini_e_fonts.triangolo_scorrimento_su),
				x_partenza + x_dim_max, y_partenza, 50, 50, 0);

		if (y_incremento > y_dim_max&&i < elenco.size() - 1) //stampa triangolo in basso perchè,  siamo arrivati a dover stampare una stringa oltre la y_dim_max richiesta 
		{													//e non abbiamo ancora finito di stamparle tutte
			al_draw_scaled_bitmap(immagini_e_fonts.triangolo_scorrimento_giu,
				0, 0, al_get_bitmap_width(immagini_e_fonts.triangolo_scorrimento_giu), al_get_bitmap_height(immagini_e_fonts.triangolo_scorrimento_giu),
				x_partenza + x_dim_max, y_partenza + y_dim_max, 50, 50, 0);
			if (c.iteratore > (int)i) //se il cursore non è tra le stringhe stampate fino ad ora
			{
				c.modificatore_scorrimento_elenco++; //tutto da rifare. Prova a partire da una stringa più avanti e vediamo se troviamo il cursore (si lo trovi)
				draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, font, colore, c, elenco, x_partenza, y_partenza, //diventa ricorsiva
					incremento_y, incremento_x, numero_colonne, x_dim_max, y_dim_max, y_dim_font, x_dim_cursore, y_dim_cursore,
					immagine_finestra, abilita_tinta_finestra, tinta_finestra,tinta_cornice,spessore_cornice);
			}
			break;
		}
		else if (c.iteratore < c.modificatore_scorrimento_elenco) //per caso il cursore sta più sù? caso in cui hai precedentemente scorso l'elenco e ora
		{															//stai scorrendo in su
			c.modificatore_scorrimento_elenco--;
			draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, font, colore, c, elenco, x_partenza, y_partenza,
				incremento_y, incremento_x, numero_colonne, x_dim_max, y_dim_max, y_dim_font, x_dim_cursore, y_dim_cursore,
				immagine_finestra, abilita_tinta_finestra, tinta_finestra,tinta_cornice,spessore_cornice);
		}
	}
	*/
	//stampa finale
}
void draw_schede_personaggi_con_o_senza_selezione(StatoGioco& stato, Immagini& immagini_e_fonts,
	vector<int>schede_personaggi_da_disegnare, int numero_schede_per_colonna, int spazio_tra_schede_x,
	int spazio_tra_schede_y, bool selezione, int iteratore_curosre)
{
	int numero_schede_per_riga = 0;
	if (6 % numero_schede_per_colonna != 0)
		numero_schede_per_riga = 1;
	numero_schede_per_riga = numero_schede_per_riga + (6 / numero_schede_per_colonna);
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display_2);

	for (int j = 0, x = 0; j < (int)schede_personaggi_da_disegnare.size() && j<numero_schede_per_riga; j = j++, x = x + 640 + spazio_tra_schede_x)
		for (int i = 0, y = 0; i < (int)schede_personaggi_da_disegnare.size()
			&& i < numero_schede_per_colonna + numero_schede_per_riga && i + j<(int)schede_personaggi_da_disegnare.size(); i = i + numero_schede_per_riga, y = y + 366 + spazio_tra_schede_y)
			m_draw_scaled_bitmap(stato.giocatore[schede_personaggi_da_disegnare[i + j]].Scheda_personaggio_con_equip, 0, 0,
				stato.giocatore[schede_personaggi_da_disegnare[i + j]].Scheda_personaggio_con_equip->width,
				stato.giocatore[schede_personaggi_da_disegnare[i + j]].Scheda_personaggio_con_equip->height,
				x, y, 640, 366, 0);
	if (selezione == true)
	{
		for (size_t i = 0; i < stato.ordine_giocatori.size(); i++)
			stato.giocatore[stato.ordine_giocatori[i] - 1].redraw_scheda = true;;
		for (int j = 0, x = 0; j < (int)schede_personaggi_da_disegnare.size() && j<numero_schede_per_riga; j = j++, x = x + 640 + spazio_tra_schede_x)
			for (int i = 0, y = 0; i < (int)schede_personaggi_da_disegnare.size()
				&& i < numero_schede_per_colonna + numero_schede_per_riga && i + j<(int)schede_personaggi_da_disegnare.size(); i = i + numero_schede_per_riga, y = y + 366 + spazio_tra_schede_y)
				if (iteratore_curosre == i + j)
					al_draw_filled_rectangle(x, y, x + 640, y + 366, al_map_rgba(0, 255, 0, 50));
	}
}
void disegna_tessera(Immagini& immagini_e_fonts, vector<fonts>& font, Giocatore* giocatore, MATTEO_BITMAP* Immagine_su_cui_disegnare,
	int i_mappa_matrice, int j_mappa_matrice, Missione& m)
{
	MATTEO_BITMAP* bitmap_corrente;
	bitmap_corrente = MATTEO_BITMAP::get_target_bitmap();
	MATTEO_BITMAP::set_target_bitmap(Immagine_su_cui_disegnare);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	m_draw_scaled_bitmap(m.Mappa_matrice[i_mappa_matrice][j_mappa_matrice].
		immagini_in_varie_angolature[m.Mappa_matrice[i_mappa_matrice][j_mappa_matrice].indice_angolo_da_usare],
		0, 0, m.Mappa_matrice[i_mappa_matrice][j_mappa_matrice].
		immagini_in_varie_angolature[m.Mappa_matrice[i_mappa_matrice][j_mappa_matrice].indice_angolo_da_usare]->width,
		m.Mappa_matrice[i_mappa_matrice][j_mappa_matrice].
		immagini_in_varie_angolature[m.Mappa_matrice[i_mappa_matrice][j_mappa_matrice].indice_angolo_da_usare]->height,
		0, 0, Immagine_su_cui_disegnare->width, Immagine_su_cui_disegnare->height, 0);
	MATTEO_BITMAP::set_target_bitmap(bitmap_corrente);
}
void disegna_zone(Immagini& immagini_e_fonts, vector<fonts> font, Giocatore* giocatore, vector<Personaggio>& Personaggi,
	Missione& m, double zoom, int x_riquadro_mappa, int y_riquadro_mappa, int width_riquadro, int height_riquadro)
{
	MATTEO_BITMAP* bitmap_corrente;
	bitmap_corrente = MATTEO_BITMAP::get_target_bitmap();
	MATTEO_BITMAP::set_target_bitmap(m.mappa);
	int x = x_riquadro_mappa;
	int y = y_riquadro_mappa;
	int null;
	int w,h;
	converti_cordinate_mappa_per_draw(zoom, m.Mappa_matrice, x, y, width_riquadro, height_riquadro, null, null, null, null,w, h);
	vector<int> zone_visibili;
	for (size_t i = 0; i < m.dati.size(); i++)
	{
		if (m.x_cordinate_zone_mappa[i] <= (x + w) && (m.x_cordinate_zone_mappa[i]+ m.x_dimensione_zone_mappa[i] >= x)
			&& m.y_cordinate_zone_mappa[i] <= (y + h) && (m.y_cordinate_zone_mappa[i]+ m.y_dimensione_zone_mappa[i] >= y))
 			if (find(zone_visibili.begin(), zone_visibili.end(), i) == zone_visibili.end())
				zone_visibili.push_back(i);
	}
	vector<int> porte_visibili;
	for (size_t i = 0; i < m.porta.size(); i++)
		if (m.porta[i].x_cordinata <= (x + w) && (m.porta[i].x_cordinata + m.porta[i].dim) >= x
			&& m.porta[i].y_cordinata <= (y + h) && (m.porta[i].y_cordinata + m.porta[i].dim) >= y)
			if (find(porte_visibili.begin(), porte_visibili.end(), i) == porte_visibili.end())
				porte_visibili.push_back(i);

	for (size_t n = 0; n < zone_visibili.size(); n++)
	{
		string testo;
		float lato_corto;
		if (m.x_dimensione_zone_mappa[zone_visibili[n]] < m.y_dimensione_zone_mappa[zone_visibili[n]])
			lato_corto = m.x_dimensione_zone_mappa[zone_visibili[n]];
		else
			lato_corto = m.y_dimensione_zone_mappa[zone_visibili[n]];

		float scala_dim = lato_corto / 200; //mi riferisco a misure fatte in un quadrato 200x200
		float x_dim = m.x_dimensione_zone_mappa[zone_visibili[n]];
		float y_dim = m.y_dimensione_zone_mappa[zone_visibili[n]];
		float scala_x = x_dim / 200;
		float scala_y = y_dim / 200;

		int dx = 0;
		int dy = 0;
		int dw = 0;
		int dh = 0;

		if (m.dati[zone_visibili[n]].carta_aaa == true)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (m.x_dimensione_zone_mappa[zone_visibili[n]] / 2) - (immagini_e_fonts.aaahh->width / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (m.y_dimensione_zone_mappa[zone_visibili[n]] / 2) - (immagini_e_fonts.aaahh->height / 2))* height_riquadro / h;
			dw = (100 * scala_dim) * width_riquadro / w;
			dh = (150 * scala_dim) *height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.aaahh, 0, 0, immagini_e_fonts.aaahh->width, immagini_e_fonts.aaahh->height,
				dx, dy, dw, dh, 0);
		}


		if (m.dati[zone_visibili[n]].exit == true)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (0 * scala_x) + ((100 * ((scala_x / scala_dim) - 1)) / 2))* width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (85 * scala_y) + ((50 * ((scala_y / scala_dim) - 1)) / 2))* height_riquadro / h;
			dw = (100 * scala_dim) * width_riquadro / w;
			dh = (50 * scala_dim) * height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.exit_segnalino, 0, 0, immagini_e_fonts.exit_segnalino->width, immagini_e_fonts.exit_segnalino->height,
				dx, dy, dw, dh, 0);
		}

		if (m.dati[zone_visibili[n]].spawn_rosso > 0)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (150 * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (50 * scala_y) + ((100 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (50 * scala_dim)* width_riquadro / w;
			dh = (100 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.spawn_rosso, 0, 0, immagini_e_fonts.spawn_rosso->width, immagini_e_fonts.spawn_rosso->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].spawn_rosso).c_str();
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((150 + 10) * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (60 * scala_y) + ((100 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (25 * scala_dim)*width_riquadro / w;
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 255, 255),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		if (m.dati[zone_visibili[n]].spawn_blu > 0)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (150 * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (100 * scala_y) + ((100 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (50 * scala_dim)*width_riquadro / w;
			dh = (100 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.spawn_blu, 0, 0, immagini_e_fonts.spawn_blu->width, immagini_e_fonts.spawn_blu->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].spawn_blu).c_str();
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((150 + 10) * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (110 * scala_y) + ((100 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (25 * scala_dim)*width_riquadro / w;
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 255, 255),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		if (m.dati[zone_visibili[n]].polizia > 0)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (100 * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (80 * scala_y) + ((100 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (50 * scala_dim)*width_riquadro / w;
			dh = (100 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.polizia, 0, 0, immagini_e_fonts.polizia->width, immagini_e_fonts.polizia->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].polizia).c_str();
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((100 + 10) * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (80 * scala_y) + ((100 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (25 * scala_dim)*width_riquadro / w;
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 100, 0),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		if (m.dati[zone_visibili[n]].pimp_mobile > 0)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (100 * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (100 * scala_y) + ((100 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (50 * scala_dim)*width_riquadro / w;
			dh = (100 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.pimp_mobile, 0, 0, immagini_e_fonts.pimp_mobile->width, immagini_e_fonts.pimp_mobile->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].pimp_mobile).c_str();
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((100 + 10) * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (110 * scala_y) + ((100 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (25 * scala_dim)*width_riquadro / w;
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 255, 255),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		if (m.dati[zone_visibili[n]].rumore > 0)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (130 * scala_x) + ((30 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (120 * scala_y) + ((30 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (30 * scala_dim)*width_riquadro / w;
			dh = (30 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.rumore, 0, 0, immagini_e_fonts.rumore->width, immagini_e_fonts.rumore->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].rumore).c_str();
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((130 + 10) * scala_x) + ((30 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (120 * scala_y) + ((30 * ((scala_y / scala_dim) - 1)) / 2) + 30 * scala_dim)*height_riquadro / h;
			dw = (25 * scala_dim)*width_riquadro / w;
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 150, 0),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		if (m.dati[zone_visibili[n]].obiettivi_rossi > 0)
		{
			dx = (((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (0 * scala_x) + ((30 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w);
			dy = (((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (95 * scala_y) + ((30 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h);
			dw = ((30 * scala_dim)*width_riquadro / w);
			dh = ((30 * scala_dim)*height_riquadro / h);
			m_draw_scaled_bitmap(immagini_e_fonts.obiettivo_rosso, 0, 0, immagini_e_fonts.obiettivo_rosso->width, immagini_e_fonts.obiettivo_rosso->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].obiettivi_rossi).c_str();
			dx = (((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((0 + 10) * scala_x) + ((30 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w);
			dy = (((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (95 * scala_y) + ((30 * ((scala_y / scala_dim) - 1)) / 2) + 30 * scala_dim)*height_riquadro / h);
			dw = ((15 * scala_dim)*width_riquadro / w);
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 255, 255),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		if (m.dati[zone_visibili[n]].obiettivi_blu > 0)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (30 * scala_x) + ((30 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (95 * scala_y) + ((30 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (30 * scala_dim)*width_riquadro / w;
			dh = (30 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.obiettivo_blu, 0, 0, immagini_e_fonts.obiettivo_blu->width, immagini_e_fonts.obiettivo_blu->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].obiettivi_blu).c_str();
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((30 + 10) * scala_x) + ((30 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (95 * scala_y) + ((30 * ((scala_y / scala_dim) - 1)) / 2) + 30 * scala_dim)*height_riquadro / h;
			dw = (15 * scala_dim)*width_riquadro / w;
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 255, 255),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		if (m.dati[zone_visibili[n]].obiettivi_verdi > 0)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (60 * scala_x) + ((30 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (95 * scala_y) + ((30 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (30 * scala_dim)*width_riquadro / w;
			dh = (30 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.obiettivo_verde, 0, 0, immagini_e_fonts.obiettivo_verde->width, immagini_e_fonts.obiettivo_verde->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].obiettivi_verdi).c_str();
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((60 + 10) * scala_x) + ((30 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (95 * scala_y) + ((30 * ((scala_y / scala_dim) - 1)) / 2) + 30 * scala_dim)*height_riquadro / h;
			dw = (15 * scala_dim)*width_riquadro / w;
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 255, 255),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		//zombie
		if (m.dati[zone_visibili[n]].walker > 0)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (0 * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (0 * scala_y) + ((50 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (50 * scala_dim)*width_riquadro / w;
			dh = (50 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.walker, 0, 0, immagini_e_fonts.walker->width, immagini_e_fonts.walker->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].walker).c_str();
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((0 + 10) * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (0 * scala_y) + ((50 * ((scala_y / scala_dim) - 1)) / 2) + 50 * scala_dim)*height_riquadro / h;
			dw = (25 * scala_dim)*width_riquadro / w;
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 255, 255),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		if (m.dati[zone_visibili[n]].runner > 0)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (50 * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (0 * scala_y) + ((50 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (50 * scala_dim)*width_riquadro / w;
			dh = (50 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.runner, 0, 0, immagini_e_fonts.runner->width, immagini_e_fonts.runner->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].runner).c_str();
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((50 + 10) * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (0 * scala_y) + ((50 * ((scala_y / scala_dim) - 1)) / 2) + 50 * scala_dim)*height_riquadro / h;
			dw = (25 * scala_dim)*width_riquadro / w;
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 255, 255),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		if (m.dati[zone_visibili[n]].fat > 0)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (100 * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (0 * scala_y) + ((50 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (50 * scala_dim)*width_riquadro / w;
			dh = (50 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.fat, 0, 0, immagini_e_fonts.fat->width, immagini_e_fonts.fat->height,
				dx, dy, dw, dh, 0);
			testo = "x"; testo = testo + to_string(m.dati[zone_visibili[n]].fat).c_str();
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + ((100 + 10) * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (0 * scala_y) + ((50 * ((scala_y / scala_dim) - 1)) / 2) + 50 * scala_dim)*height_riquadro / h;
			dw = (25 * scala_dim)*width_riquadro / w;
			dh = -1;
			al_draw_scaled_text(font, immagini_e_fonts, m.mappa, "Arial", al_map_rgb(255, 255, 255),
				dx, dy, dw, dh, 0, testo.c_str());
		}
		if (m.dati[zone_visibili[n]].abominio == true)
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (150 * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (0 * scala_y) + ((50 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (50 * scala_dim)*width_riquadro / w;
			dh = (50 * scala_dim)*height_riquadro / h;
			m_draw_scaled_bitmap(immagini_e_fonts.abominio, 0, 0, immagini_e_fonts.abominio->width, immagini_e_fonts.abominio->height,
				dx, dy, dw, dh, 0);
		}

		//giocatori
		bool giocatori_attivi = false;
		for (int k = 0; k < 6; k++)
			if (giocatore[k].stato == 1)
				giocatori_attivi = true;
		if (giocatori_attivi == false)
		{
			for (int k = 0; k < 6; k++)
				if (giocatore[k].posizione == zone_visibili[n])	//token start
				{
					dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (0 * scala_x) + ((50 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
					dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (150 * scala_y) + ((50 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
					dw = (50 * scala_dim)*width_riquadro / w;
					dh = (50 * scala_dim)*height_riquadro / h;
					m_draw_scaled_bitmap(immagini_e_fonts.start_token, 0, 0,
						immagini_e_fonts.start_token->width, immagini_e_fonts.start_token->height,
						dx, dy, dw, dh, 0);
				}
		}
		else
		{
			dx = ((m.x_cordinate_zone_mappa[zone_visibili[n]] - x) + (0 * scala_x) + ((33 * ((scala_x / scala_dim) - 1)) / 2))*width_riquadro / w;
			dy = ((m.y_cordinate_zone_mappa[zone_visibili[n]] - y) + (170 * scala_y) + ((33 * ((scala_y / scala_dim) - 1)) / 2))*height_riquadro / h;
			dw = (33 * scala_dim)*width_riquadro / w;
			dh = (33 * scala_dim)*height_riquadro / h;
			for (int k = 0; k < 6; k++)
			{
				if (giocatore[k].posizione == zone_visibili[n] && giocatore[k].stato == 1)//giocatori
					for (size_t personag = 0; personag<Personaggi.size(); personag++)
						if (Personaggi[personag].nome == giocatore[k].personaggio)
							m_draw_scaled_bitmap(Personaggi[personag].immagine_pedina, 0, 0,
								Personaggi[personag].immagine_pedina->width, Personaggi[personag].immagine_pedina->height,
								dx, dy, dw, dh, 0);
				dx = dx + dw;
			}
		}
			
				
	}
	for (size_t i = 0; i < porte_visibili.size(); i++) //porte
	{
		int dx = (m.porta[porte_visibili[i]].x_cordinata - x)*width_riquadro / w;
		int dy = (m.porta[porte_visibili[i]].y_cordinata - y)*height_riquadro / h;
		int dw = m.porta[porte_visibili[i]].dim*width_riquadro / w;
		int dh = m.porta[porte_visibili[i]].dim*height_riquadro / h;

		if (m.porta[porte_visibili[i]].colore == 0 && m.porta[porte_visibili[i]].stato == 0)
			m_draw_scaled_bitmap(immagini_e_fonts.porta_blu, 0, 0,
				immagini_e_fonts.porta_blu->width, immagini_e_fonts.porta_blu->height, dx, dy, dw, dh, 0);
		if (m.porta[porte_visibili[i]].colore == 0 && m.porta[porte_visibili[i]].stato == 1)
			m_draw_scaled_bitmap(immagini_e_fonts.porta_blu_sfondata, 0, 0,
				immagini_e_fonts.porta_blu_sfondata->width, immagini_e_fonts.porta_blu_sfondata->height, dx, dy, dw, dh, 0);
		if (m.porta[porte_visibili[i]].colore == 1 && m.porta[porte_visibili[i]].stato == 0)
			m_draw_scaled_bitmap(immagini_e_fonts.porta_neutra, 0, 0,
				immagini_e_fonts.porta_neutra->width, immagini_e_fonts.porta_neutra->height, dx, dy, dw, dh, 0);
		if (m.porta[porte_visibili[i]].colore == 1 && m.porta[porte_visibili[i]].stato == 1)
			m_draw_scaled_bitmap(immagini_e_fonts.porta_neutra_sfondata, 0, 0,
				immagini_e_fonts.porta_neutra_sfondata->width, immagini_e_fonts.porta_neutra_sfondata->height, dx, dy, dw, dh, 0);
		if (m.porta[porte_visibili[i]].colore == 2 && m.porta[porte_visibili[i]].stato == 0)
			m_draw_scaled_bitmap(immagini_e_fonts.porta_verde, 0, 0,
				immagini_e_fonts.porta_verde->width, immagini_e_fonts.porta_verde->height, dx, dy, dw, dh, 0);
		if (m.porta[porte_visibili[i]].colore == 2 && m.porta[porte_visibili[i]].stato == 1)
			m_draw_scaled_bitmap(immagini_e_fonts.porta_verde_sfondata, 0, 0,
				immagini_e_fonts.porta_verde_sfondata->width, immagini_e_fonts.porta_verde_sfondata->height, dx, dy, dw, dh, 0);
	}

	MATTEO_BITMAP::set_target_bitmap(bitmap_corrente);
}
//Public:
void disegna_mappa(StatoGioco& stato, Immagini immagini_e_fonts, Missione& T, int width_riquadro, int height_riquadro)
{
	MATTEO_BITMAP::set_target_bitmap(T.mappa);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	int x = stato.indicatore.x_riquadro_mappa;
	int y = stato.indicatore.y_riquadro_mappa;
	double zoom = stato.indicatore.zoom;
	int cont = 0;
	for (size_t i = 0; i < T.Mappa_matrice.size(); i++)
		if (cont < (int)T.Mappa_matrice[i].size())
			cont = T.Mappa_matrice[i].size();

	int width_1 = 982 * cont; int height_1 = 982 * T.Mappa_matrice.size();
	int width_finale = 0; int height_finale = 0;
	if ((double)width_1 / height_1 >(double)width_riquadro / height_riquadro)
	{
		//width_finale deve essere uguale a width_1 (della mappa)
		//mentre height_finale devessere aumentato con dello spazione nero
		width_finale = width_1;
		height_finale = width_1 * height_riquadro / width_riquadro;
	}
	else
	{
		height_finale = height_1;
		width_finale = height_1 * width_riquadro / height_riquadro;
	}

	int x_2 = (x * width_riquadro / width_finale); int y_2 = (y * height_riquadro /height_finale);
	int dim_tessera_senza_zoom = 0; int dim_tessera_con_zoom = 0;
	if (width_1 / width_riquadro > height_1 / height_riquadro)
	{
		dim_tessera_senza_zoom = width_riquadro / cont;
		dim_tessera_con_zoom = (width_riquadro * zoom) / cont;
	}
	else
	{
		dim_tessera_senza_zoom = height_riquadro / T.Mappa_matrice.size();
		dim_tessera_con_zoom = (height_riquadro * zoom) / T.Mappa_matrice.size();
	}

	int i_tessera = y_2 / (dim_tessera_senza_zoom); int j_tessera = x_2 / (dim_tessera_senza_zoom);
	int dx = ((j_tessera*dim_tessera_senza_zoom) - x_2)*zoom; int dy = ((i_tessera*dim_tessera_senza_zoom) - y_2)*zoom;
	int i_max_tessera = ((height_riquadro +(i_tessera*dim_tessera_senza_zoom*stato.indicatore.zoom) - dy) / (dim_tessera_senza_zoom*stato.indicatore.zoom))+1;
	int j_max_tessera = ((width_riquadro +(j_tessera*dim_tessera_senza_zoom*stato.indicatore.zoom) - dx) / (dim_tessera_senza_zoom*stato.indicatore.zoom))+1;
	if (i_max_tessera > (int)T.Mappa_matrice.size())
		i_max_tessera = T.Mappa_matrice.size();
	if (j_max_tessera > cont)
		j_max_tessera =cont;	

	MATTEO_BITMAP* Tessera = MATTEO_BITMAP::create();
	Tessera->init(982, 982, MATTEO_BITMAP::CREA, "");
	int indice_tessera = 0;
	int sw = 982;
	int sh = 982;
	for (int i = i_tessera; i < i_max_tessera; i++)
	{
		int dxx = dx;
		for (int j = j_tessera; j < j_max_tessera; j++)
		{
			indice_tessera++;
			disegna_tessera(immagini_e_fonts, stato.font, stato.giocatore, Tessera, i, j, T);
			MATTEO_BITMAP::set_target_bitmap(T.mappa);
			m_draw_scaled_bitmap(Tessera, 0, 0, sw, sh, dxx, dy, dim_tessera_con_zoom, dim_tessera_con_zoom, 0);
			dxx = dxx + dim_tessera_con_zoom;
		}
		dy = dy + dim_tessera_con_zoom;
	}
	Tessera->destroy();
	disegna_zone(immagini_e_fonts, stato.font, stato.giocatore, stato.personaggio, T, stato.indicatore.zoom, stato.indicatore.x_riquadro_mappa,
		stato.indicatore.y_riquadro_mappa, width_riquadro, height_riquadro);
}
void preparo_mappa_pulita(Missione& m,int dim_tessere)
{
	int x_tessera = 0, y_tessera = 0;
	MATTEO_BITMAP::set_target_bitmap(m.mappa_pulita); //la preparo pulita
	for (size_t i = 0; i < m.Mappa_matrice.size(); i++)
	{
		for (size_t j = 0; j < m.Mappa_matrice[i].size(); j++)
		{
			m_draw_scaled_bitmap(m.Mappa_matrice[i][j].immagini_in_varie_angolature[m.Mappa_matrice[i][j].indice_angolo_da_usare],
				0, 0, m.Mappa_matrice[i][j].immagini_in_varie_angolature[m.Mappa_matrice[i][j].indice_angolo_da_usare]->width,
				m.Mappa_matrice[i][j].immagini_in_varie_angolature[m.Mappa_matrice[i][j].indice_angolo_da_usare]->height,
				x_tessera, y_tessera, 982, 982, 0);
			x_tessera = x_tessera + 982;
		}
		y_tessera = y_tessera + 982;
		x_tessera = 0;
	}
}
void disegna_collage_schede_personaggi(Giocatore giocatore [10],MATTEO_BITMAP* collage_bitmap,vector<int> ordine_giocatori)
{
	MATTEO_BITMAP::set_target_bitmap(collage_bitmap);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	float dy = 0, dw = collage_bitmap->width, dh = collage_bitmap->height/6;
	for (size_t i = 0; i < ordine_giocatori.size(); i++)
	{
		m_draw_scaled_bitmap(giocatore[ordine_giocatori[i]-1].Scheda_personaggio_con_equip,
			0,0, giocatore[ordine_giocatori[i]-1].Scheda_personaggio_con_equip->width, giocatore[ordine_giocatori[i]-1].Scheda_personaggio_con_equip->height,
			0, dy, dw, dh, 0);
		dy = dy + dh;
	}
}
void draw_aggiungi_abilità_giocatore(StatoGioco& stato, Immagini& immagini_e_fonts, int indice_giocatore)
{
	vector<Abilità> scelta;
	vector<int> input;
	for (size_t i = 0; i < stato.elenco_personaggi.size(); i++)
		if (stato.giocatore[indice_giocatore].personaggio == stato.personaggio[i].nome)
			for (size_t j = 0; j < stato.personaggio[i].abilità[stato.indicatore.colore_abilità_da_inserire].size(); j++)
				scelta.push_back(stato.personaggio[i].abilità[stato.indicatore.colore_abilità_da_inserire][j]);
	if (stato.indicatore.stato_assegna_abilità == 1)
	{
		stato.giocatore[indice_giocatore].redraw_scheda=true;
		MATTEO_BITMAP::set_target_bitmap(stato.giocatore[indice_giocatore].Scheda_personaggio_con_equip);
		m_draw_scaled_bitmap(immagini_e_fonts.segna_abilità, 0, 0, immagini_e_fonts.segna_abilità->width, immagini_e_fonts.segna_abilità->height,
			357 + (stato.giocatore[indice_giocatore].abilità.size() * 407),
			183 + (57 * (stato.Cursori["Aggiungi abilità giocatore"]["Lista"].iteratore)), 86, 49, 0);
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_scaled_bitmap(stato.giocatore[indice_giocatore].Scheda_personaggio_con_equip, 0, 0,
			stato.giocatore[indice_giocatore].Scheda_personaggio_con_equip->width,
			stato.giocatore[indice_giocatore].Scheda_personaggio_con_equip->height,
			320, 174, 1280, 732, 0);
	}
}
void draw_menu_opzioni(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	vector<vector<string>> scelte;
	vector<string> scelta;
	string stringa;
	for (size_t i = 0; i < stato.indicatore.risoluzioni.size(); i++)
	{
		stringa = to_string(stato.indicatore.risoluzioni[i][0]).c_str();
		stringa = stringa + "x";
		stringa = stringa + to_string(stato.indicatore.risoluzioni[i][1]).c_str();
		scelta.push_back(stringa);
	}
	scelte.push_back(scelta);
	scelta.clear();
	scelta.push_back("Full-Screen"); scelta.push_back("Windowed");
	scelte.push_back(scelta);
	scelta.clear();
	for (size_t i = 0; i < stato.font.size(); i++)
		scelta.push_back(stato.font[i].nome);
	scelte.push_back(scelta);
	scelta.clear();
	scelta.push_back("Risoluzione"); scelta.push_back("Schermo"); scelta.push_back("Fonts");
	int larghezza_falso_display = immagini_e_fonts.falso_display->width;
	int altezza_falso_display = immagini_e_fonts.falso_display->height;
	m_draw_scaled_bitmap(immagini_e_fonts.menu_opzioni, 0, 0, immagini_e_fonts.menu_opzioni->width,
		immagini_e_fonts.menu_opzioni->height, 0, 0, 1920, 1080, 0);
	draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts_provvisorio"].iteratore].nome,
		al_map_rgba(255, 255, 255, 0), stato.Cursori["Menu opzioni"]["Lista verticale"], scelta,
		200, 240, 80, 0, 1, 900, 700, 80, 70, 56,NULL,false,al_map_rgb(0,0,0), al_map_rgb(0, 0, 0),0);
	scelta.clear();
	scelta.push_back(scelte[0][stato.Cursori["Menu opzioni"]["Risoluzioni"].iteratore].c_str());
	scelta.push_back(scelte[1][stato.Cursori["Menu opzioni"]["Schermo"].iteratore].c_str());
	scelta.push_back(scelte[2][stato.Cursori["Menu opzioni"]["Fonts_provvisorio"].iteratore].c_str());
	for (size_t i = 0; i < scelte.size(); i++)
		al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.falso_display,stato.font[stato.Cursori["Menu opzioni"]["Fonts_provvisorio"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
			1100, 240 + ((80 + 80) * i), -600, 80, 0, scelta[i]);
}
void draw_lista_tasti(StatoGioco& stato, Immagini& immagini_e_fonts, MATTEO_BITMAP* bitmap_in_cui_disegnare)
{
	MATTEO_BITMAP::set_target_bitmap(bitmap_in_cui_disegnare);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	m_draw_scaled_bitmap(immagini_e_fonts.menu, 0, 0, immagini_e_fonts.menu->width, immagini_e_fonts.menu->height,
		0, 0, bitmap_in_cui_disegnare->width, bitmap_in_cui_disegnare->height, 0);
	al_draw_scaled_text(stato.font, immagini_e_fonts, bitmap_in_cui_disegnare, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
		bitmap_in_cui_disegnare->width * 1 / 100, (bitmap_in_cui_disegnare->height* 8 / 100),
		-(0.39*bitmap_in_cui_disegnare->width), 70, 0, "Muovi cursore e selezione");
	for (int i = 0; i < 4; i++)
		m_draw_scaled_rotated_bitmap(immagini_e_fonts.arrow_right_buttton,
			immagini_e_fonts.arrow_right_buttton->width / 2, immagini_e_fonts.arrow_right_buttton->height / 2,
			(bitmap_in_cui_disegnare->width * (80 + 2) / 100) + (immagini_e_fonts.arrow_right_buttton->width / 2 * i),
			(bitmap_in_cui_disegnare->height * (8 + 5) / 100),
			0.5 * (bitmap_in_cui_disegnare->width / 1920),
			0.5 * (bitmap_in_cui_disegnare->height / 1080),
			rotazione_in_base_direzione_zona(i), 0);
	vector<MATTEO_BITMAP*> wasd;
	wasd.push_back(immagini_e_fonts.w_button);	wasd.push_back(immagini_e_fonts.a_button);	wasd.push_back(immagini_e_fonts.s_button);	wasd.push_back(immagini_e_fonts.d_button);
	for (int i = 0; i < 4; i++)
		m_draw_scaled_bitmap(wasd[i], 0, 0, wasd[i]->width, wasd[i]->height,
			(bitmap_in_cui_disegnare->width * 80 / 100) + (wasd[i]->width*i),
			(bitmap_in_cui_disegnare->height * 21 / 100), wasd[i]->width,wasd[i]->height, 0);
	al_draw_scaled_text(stato.font, immagini_e_fonts, bitmap_in_cui_disegnare, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
		bitmap_in_cui_disegnare->width * 1 / 100, bitmap_in_cui_disegnare->height * 21 / 100,
		-(0.39*bitmap_in_cui_disegnare->width), 70, 0, "Muovi mappa");
	vector<MATTEO_BITMAP*> zoom_mappa;
	zoom_mappa.push_back(immagini_e_fonts.page_up_button);	zoom_mappa.push_back(immagini_e_fonts.page_down_button);
	for (int i = 0; i < 2; i++)
		m_draw_scaled_bitmap(zoom_mappa[i], 0, 0, zoom_mappa[i]->width, zoom_mappa[i]->height,
			(bitmap_in_cui_disegnare->width * 80 / 100) + (zoom_mappa[i]->width* i),
			bitmap_in_cui_disegnare->height * 34 / 100, zoom_mappa[i]->width, zoom_mappa[i]->height, 0);
	al_draw_scaled_text(stato.font, immagini_e_fonts, bitmap_in_cui_disegnare, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
		bitmap_in_cui_disegnare->width * 1 / 100, bitmap_in_cui_disegnare->height * 34 / 100,
		-(0.39*bitmap_in_cui_disegnare->width), 70, 0, "Zoom");
	m_draw_scaled_bitmap(immagini_e_fonts.space_bar_button, 0, 0, immagini_e_fonts.space_bar_button->width, immagini_e_fonts.space_bar_button->height,
		(bitmap_in_cui_disegnare->width * 80 / 100), bitmap_in_cui_disegnare->height * 47 / 100,
		immagini_e_fonts.space_bar_button->width, immagini_e_fonts.space_bar_button->height, 0);
	al_draw_scaled_text(stato.font, immagini_e_fonts, bitmap_in_cui_disegnare, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
		bitmap_in_cui_disegnare->width * 1 / 100, bitmap_in_cui_disegnare->height * 47 / 100,
		-(0.39*bitmap_in_cui_disegnare->width), 70, 0, "Conferma durante azione: Scambia e Organizza");
	m_draw_scaled_bitmap(immagini_e_fonts.enter_button, 0, 0, immagini_e_fonts.enter_button->width, immagini_e_fonts.enter_button->height,
		(bitmap_in_cui_disegnare->width * 80 / 100), bitmap_in_cui_disegnare->height * (60 - 4) / 100,
		immagini_e_fonts.enter_button->width, immagini_e_fonts.enter_button->height, 0);
	al_draw_scaled_text(stato.font, immagini_e_fonts, bitmap_in_cui_disegnare, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
		bitmap_in_cui_disegnare->width * 1 / 100, bitmap_in_cui_disegnare->height * 60 / 100,
		-(0.39*bitmap_in_cui_disegnare->width), 70, 0, "Conferma");
	m_draw_scaled_bitmap(immagini_e_fonts.escape_button, 0, 0, immagini_e_fonts.escape_button->width, immagini_e_fonts.escape_button->height,
		(bitmap_in_cui_disegnare->width * 80 / 100), bitmap_in_cui_disegnare->height * 73 / 100,
		immagini_e_fonts.escape_button->width, immagini_e_fonts.escape_button->height, 0);
	al_draw_scaled_text(stato.font, immagini_e_fonts, bitmap_in_cui_disegnare, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
		bitmap_in_cui_disegnare->width * 1 / 100, bitmap_in_cui_disegnare->height * 73 / 100,
		-(0.39*bitmap_in_cui_disegnare->width), 70, 0, "Indietro/Annulla");
	m_draw_scaled_bitmap(immagini_e_fonts.tab_button, 0, 0, immagini_e_fonts.tab_button->width, immagini_e_fonts.tab_button->height,
		(bitmap_in_cui_disegnare->width * 80 / 100), bitmap_in_cui_disegnare->height * 87 / 100,
		immagini_e_fonts.tab_button->width, immagini_e_fonts.tab_button->height, 0);
	al_draw_scaled_text(stato.font, immagini_e_fonts, bitmap_in_cui_disegnare, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
		bitmap_in_cui_disegnare->width * 1 / 100, bitmap_in_cui_disegnare->height * 87 / 100,
		-(0.39*bitmap_in_cui_disegnare->width), 70, 0, "Seleziona altro personaggio in organizza");

}
void draw_dadi(StatoGioco& stato, Immagini& immagini_e_fonts, int precisione)
{
	int x = 0;
	int y = 0;
	int x_dim = 100;
	int y_dim = 100;
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
	if(stato.indicatore.dadi.size()>0)
		al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.collage_menu, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
		5, 0, -300, 50, 0, "Risultatato lancio dadi:");
	for (int i = 0; i < (int)stato.indicatore.dadi.size(); i++)
	{
		if (i % 3 == 0)
		{
			x = 0;
			y = y + 100;
		}
		else
			x = x + 100;
		if (stato.indicatore.dadi[i] >= precisione)
			m_draw_scaled_bitmap(immagini_e_fonts.dadi_successo[stato.indicatore.dadi[i] - 1], 0, 0,
				immagini_e_fonts.dadi_successo[stato.indicatore.dadi[i] - 1]->width, immagini_e_fonts.dadi_successo[stato.indicatore.dadi[i] - 1]->height,
				x, y, x_dim, y_dim, 0);
		else
			m_draw_scaled_bitmap(immagini_e_fonts.dadi_insucceso[stato.indicatore.dadi[i] - 1], 0, 0,
				immagini_e_fonts.dadi_insucceso[stato.indicatore.dadi[i] - 1]->width, immagini_e_fonts.dadi_insucceso[stato.indicatore.dadi[i] - 1]->height,
				x, y, x_dim, y_dim, 0);
	}
	y = y + 130;
	if (stato.indicatore.abilità_dadi_usate.size()>0)
	{
		al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.collage_menu, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
			5, y, -300, 50, 0, "Abilita' utilizzate:");
		y = y + 50;
		draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0), stato.Cursori["Dadi"]["Abilità"],
			stato.indicatore.abilità_dadi_usate, 5, y, 0, 0, 1, 300, 1080 - y, 40, 0, 0, NULL, false, al_map_rgb(0, 0, 0), al_map_rgb(0, 0, 0),0);
	}	
}
void draw_menu_assegna_ferite(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	int width = 0;
	int height = 0;
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
	m_draw_bitmap(immagini_e_fonts.Zombie_horde, 0, 0, 0);
	m_draw_scaled_bitmap(immagini_e_fonts.wounded, 0, 0, immagini_e_fonts.wounded->width, immagini_e_fonts.wounded->height, 1760-(250/2),
		0,250, 344, 0);
	string ferite = "x";
	ferite=ferite+to_string(stato.indicatore.ferite).c_str();
	al_draw_text(stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].allegro_font[4], al_map_rgb(255, 255, 255), 1760, 360, 0, ferite.c_str());
	switch (stato.indicatore.stato_menu_ferite)
	{
	case 0: //scegli umano
		width = immagini_e_fonts.falso_display_2->width;
		height = immagini_e_fonts.falso_display_2->height;
		draw_schede_personaggi_con_o_senza_selezione(stato, immagini_e_fonts, stato.indicatore.scelta_personaggio_da_ferire, 3, 0, 0, true,
			stato.Cursori["Assegna ferite"]["Scelta personaggio"].iteratore);
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_scaled_bitmap(immagini_e_fonts.falso_display_2, 0, 0, width + ((width * 30) / 100), height + ((height * 30) / 100),
			0, 200, 1920, 1080, 0);
		break;
	case 1: //ferisci o uccidi
		width = immagini_e_fonts.falso_display_2->width;
		height = immagini_e_fonts.falso_display_2->height;
		draw_cursore_su_equip_personaggio(stato, immagini_e_fonts, stato.indicatore.scelta_personaggio_da_ferire[stato.indicatore.azioneZombie_umanoDaFerire],
			stato.giocatore[stato.indicatore.scelta_personaggio_da_ferire[stato.indicatore.azioneZombie_umanoDaFerire]].equipaggiamento.size(), al_map_rgba(0, 150, 0, 0.2),
			stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore);
		draw_schede_personaggi_con_o_senza_selezione(stato, immagini_e_fonts, stato.indicatore.scelta_personaggio_da_ferire, 3, 0, 0, false,
			NULL);
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_scaled_bitmap(immagini_e_fonts.falso_display_2, 0, 0, width + ((width * 30) / 100), height + ((height * 30) / 100),
			0, 200, 1920, 1080, 0);
		break;
	case 2: //visualizza mappa
		disegna_mappa(stato, immagini_e_fonts, stato.missione, 1497, 1040);
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_scaled_bitmap(stato.missione.mappa,
			0, 0, stato.missione.mappa->width, stato.missione.mappa->height,//MAPPA
			23, 19, 1497, 1040, 0);
		m_draw_bitmap_region(immagini_e_fonts.collage_menu, 0, 0, 1920 - 1601, 1080, 1601, 0, 0); //Collage menu
		break;
	}
}
void draw_input_stringa_tastiera(StatoGioco& stato,Immagini immagini_e_fonts,string s,fonts f,ALLEGRO_COLOR colore,int font_dim, string::iterator i,MATTEO_BITMAP* bitmap_su_cui_stampare,MATTEO_BITMAP* sfondo_finestra,int x_centro_finestra,int y_centro_finestra)
{
	MATTEO_BITMAP::set_target_bitmap(bitmap_su_cui_stampare);
	int font_dim_reale = 0;
	int indice = 0;
	for (size_t j = 0; j < f.dim_font.size(); j++)
		if (font_dim_reale < f.dim_font[j])
		{
			font_dim_reale = f.dim_font[j];
			indice = j;
		}
	int lunghezza_scritta = al_get_text_width(f.allegro_font[indice], s.c_str());
	int lunghezza_scritta_dopo_scale = font_dim*lunghezza_scritta / font_dim_reale;
	int altezza_font_dopo_scale = font_dim*f.height_dim[indice] / font_dim_reale;
	if (sfondo_finestra != NULL)
		m_draw_scaled_bitmap(sfondo_finestra, 0, 0, sfondo_finestra->width, sfondo_finestra->height,
			x_centro_finestra - (lunghezza_scritta_dopo_scale / 2), y_centro_finestra - (altezza_font_dopo_scale / 2),lunghezza_scritta_dopo_scale,
			altezza_font_dopo_scale,0);
	al_draw_scaled_text(stato.font, immagini_e_fonts, bitmap_su_cui_stampare, f.nome, colore,
		x_centro_finestra - (lunghezza_scritta_dopo_scale / 2), y_centro_finestra - (altezza_font_dopo_scale / 2), lunghezza_scritta_dopo_scale,
		altezza_font_dopo_scale, 0,stato.indicatore.inserisci_stringa_input_tastiera);
	
}
//operazioni di inizializzazione
void inizializza_abilità(StatoGioco& stato)
{
	stato.elenco_abilità.clear();
	Abilità a;
	a.nome = "starts_with_a_pistol";
	a.quando_si_scarica = Abilità::SUBITO;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::START_WITH_A_WEAPON;
	a.funzione = starts_with_a_pistol;
	stato.elenco_abilità.push_back(a);
	a.nome = "medic";
	a.quando_si_scarica = Abilità::DOPO_ESECUZIONE_AZIONE;
	a.quando_ricarica = Abilità::A_INIZIO_TURNO_GIOCATORE;
	a.tipo = Abilità::AZIONE;
	a.funzione = medic;
	stato.elenco_abilità.push_back(a);
	a.nome = "born_leader";
	a.quando_si_scarica = Abilità::DOPO_ESECUZIONE_AZIONE;
	a.quando_ricarica = Abilità::A_INIZIO_TURNO_GIOCATORE;
	a.tipo = Abilità::AZIONE;
	a.funzione = born_leader;
	stato.elenco_abilità.push_back(a);
	a.nome = "born_leader_mark";
	a.quando_si_scarica = Abilità::DOPO_ESECUZIONE_AZIONE;
	a.quando_ricarica = Abilità::A_INIZIO_TURNO_GIOCATORE;
	a.tipo = Abilità::FREE;
	a.funzione = born_leader_mark;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_to_dice_roll_ranged";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::DADI;
	a.funzione = one_to_dice_roll_ranged;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_to_dice_roll_melee";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::DADI;
	a.funzione = one_to_dice_roll_melee;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_to_dice_roll_combact";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::DADI;
	a.funzione = one_to_dice_roll_combact;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_die_ranged";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::DADI;
	a.funzione = one_die_ranged;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_die_melee";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::DADI;
	a.funzione = one_die_melee;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_die_combact";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::DADI;
	a.funzione = one_die_combact;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_re_roll_per_turn";
	a.quando_si_scarica = Abilità::DOPO_LANCIO_DADI;
	a.quando_ricarica = Abilità::A_INIZIO_TURNO_GIOCATORE;
	a.tipo = Abilità::RE_ROLL;
	a.funzione = one_re_roll_per_turn;
	stato.elenco_abilità.push_back(a);
	a.nome = "lucky";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::RE_ROLL;
	a.funzione = lucky;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_free_move_action";
	a.quando_si_scarica = Abilità::DOPO_ESECUZIONE_AZIONE;
	a.quando_ricarica = Abilità::A_INIZIO_TURNO_GIOCATORE;
	a.tipo = Abilità::FREE;
	a.funzione = one_free_move_action;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_free_search_action";
	a.quando_si_scarica = Abilità::DOPO_ESECUZIONE_AZIONE;
	a.quando_ricarica = Abilità::A_INIZIO_TURNO_GIOCATORE;
	a.tipo = Abilità::FREE;
	a.funzione = one_free_search_action;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_free_combact_action";
	a.quando_si_scarica = Abilità::DOPO_ESECUZIONE_AZIONE;
	a.quando_ricarica = Abilità::A_INIZIO_TURNO_GIOCATORE;
	a.tipo = Abilità::FREE;
	a.funzione = one_free_combact_action;
	stato.elenco_abilità.push_back(a);
	a.nome = "matching_set";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::MODIFICATORE_CERCA;
	a.funzione = matching_set;
	stato.elenco_abilità.push_back(a);
	a.nome = "ambidexytous";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::PASSIVA;
	a.funzione = ambidexytous;
	stato.elenco_abilità.push_back(a);
	a.nome = "slippery";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::MODIFICATORE_MOVIMENTO;
	a.funzione = slippery;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_zone_per_move";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::MODIFICATORE_MOVIMENTO;
	a.funzione = one_zone_per_move;
	stato.elenco_abilità.push_back(a);
	a.nome = "tough";
	a.quando_si_scarica = Abilità::A_INIZIO_TURNO_GIOCATORE;
	a.quando_ricarica = Abilità::A_INIZIO_TURNO_ZOMBIE;
	a.tipo = Abilità::DIFESA;
	a.funzione = tough;
	stato.elenco_abilità.push_back(a);
	a.nome = "sniper";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::DADI;
	a.funzione = sniper;
	stato.elenco_abilità.push_back(a);
	a.nome = "one_action";
	a.quando_si_scarica = Abilità::MAI;
	a.quando_ricarica = Abilità::MAI;
	a.tipo = Abilità::PASSIVA;
	a.funzione = one_action;
	stato.elenco_abilità.push_back(a);
}

void prepara_carta_equipaggiamento(string nome, vector<Carta_equipaggiamento>& mazzo, MATTEO_BITMAP* immagine, int tipologia,
	int gittata_min_melee, int gittata_max_melee, int gittata_min_ranged, int gittata_max_ranged, int dadi_melee, int dadi_ranged, int precisione_melee, int precisione_ranged, int danno_melee,
	int danno_ranged, bool rumore_dopo_attacco_melee, bool rumore_dopo_attacco_ranged, bool doppia, int apre_porte, int(*speciale)(StatoGioco& stato, ALLEGRO_EVENT evento))
{
	Carta_equipaggiamento carta;
	carta.Immagine = MATTEO_BITMAP::create();
	carta.Immagine->init(immagine->width, immagine->height, immagine->carica_crea, immagine->percorso_file);
	carta.nome = nome;
	carta.tipologia = tipologia;
	carta.gittata_min_melee = gittata_min_melee;
	carta.gittata_max_melee = gittata_max_melee;
	carta.gittata_min_ranged = gittata_min_ranged;
	carta.gittata_max_ranged = gittata_max_ranged;
	carta.dadi_melee = dadi_melee;
	carta.dadi_ranged = dadi_ranged;
	carta.precisione_melee = precisione_melee;
	carta.precisione_ranged = precisione_ranged;
	carta.danno_melee = danno_melee;
	carta.danno_ranged = danno_ranged;
	carta.apre_porte = apre_porte;
	carta.rumore_dopo_attacco_ranged = rumore_dopo_attacco_ranged;
	carta.rumore_dopo_attacco_melee = rumore_dopo_attacco_melee;
	carta.doppia = doppia;
	carta.speciale = speciale;

	mazzo.push_back(carta);
}
void inizializza_mazzo_equipaggiamento(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	stato.Mazzo_speciale.clear();
	//mazzo speciale
	for (int i = 0; i < 3; i++)
		prepara_carta_equipaggiamento("Pan", stato.Mazzo_speciale, immagini_e_fonts.pan,
			Carta_equipaggiamento::ARMA, 0, 0, 0, 0, 1, 0, 6, 0, 1, 0, false, false, false, 0, pan_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Molotov", stato.Mazzo_speciale, immagini_e_fonts.molotov,
			1, 0, 0, 0, 1, 0, -1, 0, 0, 0, 3, false, true, false, 0, molotov_speciale);
	prepara_carta_equipaggiamento("Evil Twins", stato.Mazzo_speciale, immagini_e_fonts.evil_twins,
		Carta_equipaggiamento::ARMA, 0, 0, 0, 1, 0, 2, 0, 4, 0, 1, false, true, false, 0, evil_twins_speciale);
	prepara_carta_equipaggiamento("Ma's Shotgun", stato.Mazzo_speciale, immagini_e_fonts.ma_s_shotgun,
		Carta_equipaggiamento::ARMA, 0, 0, 0, 1, 1, 2, 3, 3, 2, 2, false, true, false, 0, ma_s_shotgun_speciale);
	for (int i = 0; i < 12; i++)
		prepara_carta_equipaggiamento("Wounded", stato.Mazzo_speciale, immagini_e_fonts.wounded,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, wounded_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Sniper Rifle", stato.Mazzo_speciale, immagini_e_fonts.sniper_rifle,
			Carta_equipaggiamento::ARMA, 1, 3, 0, 0, 1, 0, 3, 0, 1, 0, true, false, false, false, sniper_rifle_speciale);
	//Mazzo equip
	//carte iniziali: 0 pistol, 1 crowbar, 2 fire_axe
	stato.Mazzo_equipaggiamento.clear();
	prepara_carta_equipaggiamento("Pistol", stato.Mazzo_equipaggiamento, immagini_e_fonts.pistol,
		Carta_equipaggiamento::ARMA, 0, 0, 0, 1, 0, 1, 0, 4, 0, 1, false, true, true, 0, pistol_speciale);
	prepara_carta_equipaggiamento("Crowbar", stato.Mazzo_equipaggiamento,immagini_e_fonts.crowbar,
		Carta_equipaggiamento::ARMA, 0, 0, 0, 0, 1, 0, 4, 0, 1, 0, false, false, false, 2, crowbar_speciale);
	prepara_carta_equipaggiamento("Fire Axe", stato.Mazzo_equipaggiamento,immagini_e_fonts.fire_axe,
		Carta_equipaggiamento::ARMA, 0, 0, 0, 0, 1, 0, 4, 0, 2, 0, false, false, false, 1, fire_axe_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Pistol", stato.Mazzo_equipaggiamento, immagini_e_fonts.pistol,
			Carta_equipaggiamento::ARMA, 0, 0, 0, 1, 0, 1, 0, 4, 0, 1, false, true, true, 0, pistol_speciale);
	prepara_carta_equipaggiamento("Crowbar", stato.Mazzo_equipaggiamento, immagini_e_fonts.crowbar,
		Carta_equipaggiamento::ARMA, 0, 0, 0, 0, 1, 0, 4, 0, 1, 0, false, false, false, 2, crowbar_speciale);
	prepara_carta_equipaggiamento("Fire Axe", stato.Mazzo_equipaggiamento, immagini_e_fonts.fire_axe,
		Carta_equipaggiamento::ARMA, 0, 0, 0, 0, 1, 0, 4, 0, 2, 0, false, false, false, 1, fire_axe_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Baseball Bat", stato.Mazzo_equipaggiamento, immagini_e_fonts.baseball_bat,
			Carta_equipaggiamento::ARMA, 0, 0, 0, 0, 1, 0, 3, 0, 1, 0, false, false, false, 0, baseball_bat_speciale);
	for (int i = 0; i < 4; i++)
		prepara_carta_equipaggiamento("Machete", stato.Mazzo_equipaggiamento, immagini_e_fonts.machete,
			Carta_equipaggiamento::ARMA, 0, 0, 0, 0, 1, 0, 4, 0, 2, 0, false, false, true, 0, machete_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Chainsaw", stato.Mazzo_equipaggiamento, immagini_e_fonts.chainsaw,
			Carta_equipaggiamento::ARMA, 0, 0, 0, 0, 5, 0, 5, 0, 2, 0, true, false, false, 1, chainsaw_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Katana", stato.Mazzo_equipaggiamento, immagini_e_fonts.katana,
			Carta_equipaggiamento::ARMA, 0, 0, 0, 0, 2, 0, 4, 0, 1, 0, false, false, false, 0, katana_speciale);
	for (int i = 0; i < 4; i++)
		prepara_carta_equipaggiamento("Sawed Off", stato.Mazzo_equipaggiamento, immagini_e_fonts.sawed_off,
			Carta_equipaggiamento::ARMA, 0, 0, 0, 1, 0, 2, 0, 3, 0, 1, false, true, true, 0, sawed_off_speciale);
	for (int i = 0; i < 3; i++)
		prepara_carta_equipaggiamento("Sub Mg", stato.Mazzo_equipaggiamento, immagini_e_fonts.sub_mg,
			Carta_equipaggiamento::ARMA, 0, 0, 0, 1, 0, 3, 0, 5, 0, 1, false, true, true, 0, sub_mg_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Shotgun", stato.Mazzo_equipaggiamento, immagini_e_fonts.shotgun,
			Carta_equipaggiamento::ARMA, 0, 0, 0, 1, 0, 2, 0, 4, 0, 2, false, true, false, 0, shotgun_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Rifle", stato.Mazzo_equipaggiamento, immagini_e_fonts.rifle,
			(Carta_equipaggiamento::ARMA), 0, 0, 1, 3, 0, 1, 0, 3, 0, 1, false, true, false, 0, rifle_speciale);
	for (int i = 0; i < 3; i++)
		prepara_carta_equipaggiamento("Canned Food", stato.Mazzo_equipaggiamento, immagini_e_fonts.canned_food,
			Carta_equipaggiamento::RIFORNIMENTO, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, canned_food_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Flashlight", stato.Mazzo_equipaggiamento, immagini_e_fonts.flashlight,
			Carta_equipaggiamento::FLASHLIGHT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, flashlight_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Gasoline", stato.Mazzo_equipaggiamento, immagini_e_fonts.gasoline,
			Carta_equipaggiamento::COMBINABILE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, gasoline_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Glass Bottles", stato.Mazzo_equipaggiamento, immagini_e_fonts.glass_bottles,
			Carta_equipaggiamento::COMBINABILE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, glass_bottles_speciale);

	prepara_carta_equipaggiamento("Goalie Mask", stato.Mazzo_equipaggiamento, immagini_e_fonts.goalie_mask,
		Carta_equipaggiamento::DIFESA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, goalie_mask_speciale);
	for (int i = 0; i < 3; i++)
		prepara_carta_equipaggiamento("Bag of Rice", stato.Mazzo_equipaggiamento, immagini_e_fonts.bag_of_rice,
			Carta_equipaggiamento::RIFORNIMENTO, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, bag_of_rice_speciale);
	for (int i = 0; i < 2; i++)
		prepara_carta_equipaggiamento("Scope", stato.Mazzo_equipaggiamento, immagini_e_fonts.scope,
			Carta_equipaggiamento::COMBINABILE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, scope_speciale);
	for (int i = 0; i < 3; i++)
		prepara_carta_equipaggiamento("Water", stato.Mazzo_equipaggiamento, immagini_e_fonts.water,
			Carta_equipaggiamento::RIFORNIMENTO, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, water_speciale);
	for (int i = 0; i < 4; i++)
		prepara_carta_equipaggiamento("Aaahh!!?", stato.Mazzo_equipaggiamento, immagini_e_fonts.aaahh,
			Carta_equipaggiamento::TRAPPOLA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, aaahh_speciale);
	for (int i = 0; i < 3; i++)
		prepara_carta_equipaggiamento("Plenty of Ammo Shotgun", stato.Mazzo_equipaggiamento, immagini_e_fonts.plenty_of_ammo_shotgun,
			Carta_equipaggiamento::MUNIZIONI, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, plenty_of_ammo_shotgun_speciale);
	for (int i = 0; i < 3; i++)
		prepara_carta_equipaggiamento("Plenty of Ammo", stato.Mazzo_equipaggiamento, immagini_e_fonts.plenty_of_ammo,
			Carta_equipaggiamento::MUNIZIONI, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, false, false, 0, plenty_of_ammo_speciale);
	stato.Mazzo_equipaggiamento_originale = stato.Mazzo_equipaggiamento;
	stato.Mazzo_speciale_originale = stato.Mazzo_speciale;
}
void mescola_mazzo_equipaggiamento(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	Carta_equipaggiamento carta;
	stato.Mazzo_equipaggiamento = stato.Mazzo_equipaggiamento_originale;
	for (int i = 0; i < stato.numero_giocatori; i++)
		for (int j = 0; j < (int)stato.giocatore[stato.ordine_giocatori[i] - 1].equipaggiamento.size(); j++)
		{
			bool trovato = false;
			for (int k = 0; k < (int)stato.Mazzo_speciale.size(); k++)
				if (stato.giocatore[stato.ordine_giocatori[i] - 1].equipaggiamento[j].nome == stato.Mazzo_speciale[k].nome)
					trovato = true;
			if (trovato == false)
				cerca_carta_in_un_mazzo(carta, stato.giocatore[stato.ordine_giocatori[i] - 1].equipaggiamento[j].nome, stato.Mazzo_equipaggiamento, true);
		}
}
void inizializza_azioni(StatoGioco& stato)
{
	stato.elenco_azioni.clear();
	Azione a;
	a.fattibile = movimento_fattibile;
	a.esegui = movimento_esegui;
	a.grafica = movimento_grafica;
	a.nome = "Muovi";
	a.costo_di_azioni = 1;
	stato.elenco_azioni.push_back(a);
	a.esegui = attacca_esegui;
	a.fattibile = attacca_fattibile;
	a.grafica = attacca_grafica;
	a.nome = "Attacca";
	a.costo_di_azioni = 1;
	stato.elenco_azioni.push_back(a);
	a.fattibile = aprire_porta_fattibile;
	a.esegui = aprire_porta_esegui;
	a.grafica = aprire_porta_grafica;
	a.nome = "Apri";
	a.costo_di_azioni = 1;
	stato.elenco_azioni.push_back(a);
	a.fattibile = termina_turno_fattibile;
	a.esegui = termina_turno_esegui;
	a.grafica = termina_turno_grafica;
	a.nome = "Termina";
	a.costo_di_azioni = 0;
	stato.elenco_azioni.push_back(a);
	a.fattibile = fuggi_fattibile;
	a.esegui = fuggi_esegui;
	a.grafica = fuggi_grafica;
	a.nome = "Fuggi";
	a.costo_di_azioni = 0;
	stato.elenco_azioni.push_back(a);
	a.fattibile = scambia_equipaggiamento_fattibile;
	a.esegui = scambia_equipaggiamento_esegui;
	a.grafica = scambia_equipaggiamento_grafica;
	a.nome = "Scambia";
	a.costo_di_azioni = 1;
	stato.elenco_azioni.push_back(a);
	a.fattibile = scambia_da_solo_fattibile;
	a.esegui = scambia_da_solo_esegui;
	a.grafica = scambia_da_solo_grafica;
	a.nome = "Mod.Equip";
	a.costo_di_azioni = 1;
	stato.elenco_azioni.push_back(a);
	a.esegui = cerca_esegui;
	a.fattibile = cerca_fattibile;
	a.grafica = cerca_grafica;
	a.nome = "Cerca";
	a.costo_di_azioni = 1;
	stato.elenco_azioni.push_back(a);
	a.esegui = combina_esegui;
	a.fattibile = combina_fattibile;
	a.grafica = combina_grafica;
	a.nome = "Combina (gratis)";
	a.costo_di_azioni = 0;
	stato.elenco_azioni.push_back(a);
	a.nome = "Medic (Gratis)";
	a.costo_di_azioni = 0;
	a.fattibile = medic_fattibile;
	a.esegui = medic_esegui;
	a.grafica = medic_grafica;
	stato.elenco_azioni.push_back(a);
	a.nome = "Born Leader (Gratis)";
	a.costo_di_azioni = 0;
	a.fattibile = born_leader_fattibile;
	a.esegui = born_leader_esegui;
	a.grafica = born_leader_grafica;
	stato.elenco_azioni.push_back(a);

	stato.elenco_azioni_originale = stato.elenco_azioni;
}