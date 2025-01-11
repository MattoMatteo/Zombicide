#include"Azioni_zombie.h"
#include"dati.h"
#include"utilità.h"
#include "Matteo Draw.h"
#include<iostream>
#include<allegro5\allegro_primitives.h>

using namespace std;


bool attiva_zombie(StatoGioco& stato, ALLEGRO_EVENT event, string zombie_da_attivare) //true se ha concluso tutto e false se ancora deve fare altro
{
	ALLEGRO_KEYBOARD_STATE keyboard_state;
	al_get_keyboard_state(&keyboard_state);
	vector<int>zone_di_movimento_abominio;
	Carta_equipaggiamento ferita;
	DatiStanza vuoto;
	cerca_carta_in_un_mazzo(ferita, "Wounded", stato.Mazzo_speciale, false);
	int zona_abominio = 0;
	int risposta;
	stato.Cursori["Muovi abominio"]["Lista"];
	switch (stato.indicatore.stato_azioniZombie)
	{
	case 0: //Attivazioni zombie
		switch (stato.indicatore.sub_stato_azioniZombie_attivazione)
		{
		case 0:						//creo una mappa con le future modifiche vuota
			stato.missione.copia_dati.clear();
			for (size_t i = 0; i < stato.missione.dati.size(); i++)
			{
				stato.missione.copia_dati.push_back(vuoto);
				stato.missione.copia_dati[i].abominio = stato.missione.dati[i].abominio;
			}
			stato.indicatore.sub_stato_azioniZombie_attivazione++;
			break;
		case 1:						//prima attivazione zombie richiesto
			risposta=azione_zombie(stato, zombie_da_attivare);
			switch (risposta)
			{
			case 0://fine attivazione
				stato.indicatore.stato_azioniZombie = 3;
				break;
			case 1://richiesta assegna ferite
				stato.indicatore.stato_azioniZombie = 1;
				break;
			case 2://richiesta scelta movimento abominio
				stato.indicatore.x_riquadro_mappa_backup=stato.indicatore.x_riquadro_mappa;
				stato.indicatore.y_riquadro_mappa_backup = stato.indicatore.y_riquadro_mappa;
				stato.indicatore.stato_azioniZombie = 2;
				break;
			}
			break;
		case 2:							 //seconda attivazione runner se necessario
			risposta=azione_zombie(stato, "runner");
			switch (risposta)
			{
			case 0://fine attivazione
				stato.indicatore.stato_azioniZombie = 3;
				break;
			case 1://richiesta assegna ferite
				stato.indicatore.stato_azioniZombie = 1;
				break;
			}
			break;
		}
		break;
	case 1: //uccidi umani : menu assegna ferite
		if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		{
			stato.indicatore.ferite=calcola_ferite(stato, zombie_da_attivare);
			crea_o_ripristina_backup_equipaggiamento(stato, "ripristina", stato.indicatore.scelta_personaggio_da_ferire);
		}	
		if (assegna_ferite(stato,event,keyboard_state) == true)
		{
			stato.indicatore.stato_azioniZombie = 0;
			stato.indicatore.azioneZombie_contatore_zona++;
		}
		break;
	case 2://muovi abominio
		zone_di_movimento_abominio.clear();
		scelta_movimento_abominio(stato, zone_di_movimento_abominio, zona_abominio);
		input_tastiera_9_direzioni(stato.Cursori["Muovi abominio"]["Lista"].iteratore, event,
			creazione_vettore_per_input_direzioni_con_ordinamento_di_zone
			(stato,zone_di_movimento_abominio, zona_abominio), false, false, false);
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				stato.indicatore.x_riquadro_mappa = stato.indicatore.x_riquadro_mappa_backup;
				stato.indicatore.y_riquadro_mappa = stato.indicatore.y_riquadro_mappa_backup;
				stato.missione.dati[zona_abominio].abominio = false;
				stato.missione.dati[zone_di_movimento_abominio[stato.Cursori["Muovi abominio"]["Lista"].iteratore]].abominio = true;
				stato.Cursori["Muovi abominio"]["Lista"].iteratore = 0;
				stato.indicatore.stato_azioniZombie = 0;
				stato.indicatore.azioneZombie_amobinio_mosso_manualmente = true;
			}
		}
		break;
	case 3://somma i dati ottenuti
		if (mostra_risultati(stato,event) == true)
		{
			for (size_t i = 0; i < stato.missione.dati.size(); i++)
			{
				if (zombie_da_attivare == "walker" || zombie_da_attivare == "tutti")
					stato.missione.dati[i].SommaValoreWalker(stato.missione.copia_dati[i].walker);
				if (zombie_da_attivare == "fat" || zombie_da_attivare == "tutti")
					stato.missione.dati[i].SommaValoreFat(stato.missione.copia_dati[i].fat);
				if (zombie_da_attivare == "runner" || zombie_da_attivare == "tutti")
					stato.missione.dati[i].SommaValoreRunner(stato.missione.copia_dati[i].runner);
				if (zombie_da_attivare == "abominio" || zombie_da_attivare == "tutti")
					if (stato.indicatore.azioneZombie_amobinio_mosso_manualmente == false)
						stato.missione.dati[i].ModificaValoreAbominio(stato.missione.copia_dati[i].abominio);
			}
			if (stato.indicatore.azioneZombie_amobinio_mosso_manualmente == true)
				stato.indicatore.azioneZombie_amobinio_mosso_manualmente = false;
			stato.indicatore.azioneZombie_contatore_zona = 0;
			stato.indicatore.azioneZombie_umanoDaFerire = 0; //forse non serve +
			stato.indicatore.stato_menu_ferite = 0;
			stato.indicatore.zone_spostamenti_zombie.clear();
			stato.indicatore.direzioni_spostamenti_zombie.clear();
			if ((zombie_da_attivare == "tutti" || zombie_da_attivare == "runner") && stato.indicatore.sub_stato_azioniZombie_attivazione == 1)
			{
				stato.indicatore.sub_stato_azioniZombie_attivazione = 2;
				stato.indicatore.stato_azioniZombie = 0;
				stato.missione.copia_dati.clear();
				for (size_t i = 0; i < stato.missione.dati.size(); i++)
				{
					stato.missione.copia_dati.push_back(vuoto);
					stato.missione.copia_dati[i].abominio = stato.missione.dati[i].abominio;
				}
				return false;
			}
			stato.indicatore.stato_azioniZombie = 0;
			stato.indicatore.sub_stato_azioniZombie_attivazione = 0;
			return true;
		}
		break;
	}
	return false;
}

int calcola_ferite(StatoGioco& stato, string zombie_da_attivare)
{
	int ferite = 0;
	stato.indicatore.scelta_personaggio_da_ferire.clear();
	for (int i = 0; i < stato.numero_giocatori; i++)	//personaggi interessati
	{
		if (stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].giocatori[stato.ordine_giocatori[i] - 1] == true)
			stato.indicatore.scelta_personaggio_da_ferire.push_back(stato.ordine_giocatori[i] - 1);
	}
	//totale delle ferite da assegnare
	if (stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].abominio == true && zombie_da_attivare == "tutti"&&stato.indicatore.sub_stato_azioniZombie_attivazione == 0)//l'abominio conta 1 ferita
		ferite++;
	if (zombie_da_attivare == "tutti"&&stato.indicatore.sub_stato_azioniZombie_attivazione == 1)
		ferite = ferite +
		stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].walker +
		stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].runner +
		stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].fat;
	if (zombie_da_attivare == "walker")
		ferite = ferite +
		stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].walker;
	if (zombie_da_attivare == "runner" || (zombie_da_attivare == "tutti"&&stato.indicatore.sub_stato_azioniZombie_attivazione == 2))
		ferite = ferite +
		stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].runner;
	if (zombie_da_attivare == "fat")
		ferite = ferite +
		stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].fat;
	return ferite;
}

bool mostra_risultati(StatoGioco& stato,ALLEGRO_EVENT event)
{
	if((event.type == ALLEGRO_EVENT_KEY_DOWN && (event.keyboard.keycode == ALLEGRO_KEY_ENTER|| event.keyboard.keycode == ALLEGRO_KEY_ESCAPE))||
		(stato.indicatore.zone_spostamenti_zombie.size()==0&&stato.indicatore.check_extraturn==false))
		return true;
	return false;
}
int azione_zombie(StatoGioco& stato, string zombie)
{
	for (; stato.indicatore.azioneZombie_contatore_zona < (int)stato.missione.dati.size(); stato.indicatore.azioneZombie_contatore_zona++)
	{
		bool azioni_walker = true;
		bool azioni_fat = true;
		bool azioni_abominio = true;
		bool azioni_runner = true;
		if ((stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].walker>0 && (zombie == "walker" || zombie == "tutti")) ||
			(stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].runner>0 && (zombie == "runner" || zombie == "tutti")) ||
			(stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].fat > 0 && (zombie == "fat" || zombie == "tutti")) ||
			(stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].abominio == true && (zombie == "abominio" || zombie == "tutti")))
		{
			if (attacca_zombie(stato, zombie, stato.indicatore.azioneZombie_contatore_zona, azioni_walker, azioni_fat, azioni_abominio, azioni_runner)==true)
			{
				stato.indicatore.ferite = calcola_ferite(stato, zombie);
				crea_o_ripristina_backup_equipaggiamento(stato, "crea", stato.indicatore.scelta_personaggio_da_ferire);
				return 1; //richiesta assegna ferita
			}
			if (stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].walker > 0 && azioni_walker == true || (stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].fat > 0 && azioni_fat == true) ||
				(stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].runner > 0 && azioni_runner == true) || (stato.missione.dati[stato.indicatore.azioneZombie_contatore_zona].abominio == true && azioni_abominio == true))
			{
				vector<int> vuoto;
				if (find(stato.indicatore.zone_spostamenti_zombie.begin(), stato.indicatore.zone_spostamenti_zombie.end(), stato.indicatore.azioneZombie_contatore_zona)
					== stato.indicatore.zone_spostamenti_zombie.end())
				{
					stato.indicatore.zone_spostamenti_zombie.push_back(stato.indicatore.azioneZombie_contatore_zona);
					stato.indicatore.direzioni_spostamenti_zombie.push_back(vuoto);
				}
				if (muovi_zombie(stato, zombie, stato.missione.copia_dati, stato.indicatore.azioneZombie_contatore_zona, azioni_walker, azioni_fat, azioni_abominio, azioni_runner) == false)
					return 2; //richiesta movimento abominio
			}
		}
	}
	return 0;//fine
}

bool attacca_zombie(StatoGioco& stato, string zombie, int i, bool azioni_walker, bool azioni_fat, bool azioni_abominio, int azioni_runner)
{
	int abominio=0;
	if (stato.missione.dati[i].abominio == true)
		abominio = 1;
	vector<int> umani_da_uccidere;
	for (int j = 0; j < stato.numero_giocatori; j++)
		if (stato.missione.dati[i].giocatori[stato.ordine_giocatori[j]-1] == true)
			umani_da_uccidere.push_back(j);
	if (umani_da_uccidere.size() > 0)
		return true;
	return false;
}

bool muovi_zombie(StatoGioco& stato, string zombie, vector<DatiStanza>& copia_dati, int i,
 bool azioni_walker, bool azioni_fat, bool azioni_abominio, bool azioni_runner)
{
	vector<int> obiettivi_zombie;
	zombie_cerca_obiettivo(stato, obiettivi_zombie, i);
	vector<int> vie_da_seguire;
	for (int j = 0; j < (int)obiettivi_zombie.size(); j++)
		for (int k = 0; k < (int)stato.percorsi_minimi[obiettivi_zombie[j]][i].size(); k++)
			if(find(vie_da_seguire.begin(),vie_da_seguire.end(),stato.percorsi_minimi[obiettivi_zombie[j]][i][k])== vie_da_seguire.end())
				vie_da_seguire.push_back(stato.percorsi_minimi[obiettivi_zombie[j]][i][k]);
	if (stato.missione.dati[i].walker%vie_da_seguire.size() != 0 && (zombie=="walker" || zombie=="tutti"))
		stato.missione.dati[i].SommaValoreWalker((((stato.missione.dati[i].walker / vie_da_seguire.size())+ 1)*vie_da_seguire.size())- stato.missione.dati[i].walker);
	if (stato.missione.dati[i].runner%vie_da_seguire.size() != 0 && (zombie == "runner" || zombie == "tutti"))
		stato.missione.dati[i].SommaValoreRunner((((stato.missione.dati[i].runner / vie_da_seguire.size()) + 1)*vie_da_seguire.size())- stato.missione.dati[i].runner);
	if (stato.missione.dati[i].fat%vie_da_seguire.size() != 0 && (zombie == "walker" || zombie == "tutti"))
		stato.missione.dati[i].SommaValoreFat((((stato.missione.dati[i].fat / vie_da_seguire.size()) + 1)*vie_da_seguire.size())- stato.missione.dati[i].fat);

	for (int j = 0; j < (int)vie_da_seguire.size(); j++)
	{
		if (stato.missione.collisioni[i][vie_da_seguire[j]] == 1 || i == vie_da_seguire[j])
		{
			stato.indicatore.direzioni_spostamenti_zombie[stato.indicatore.direzioni_spostamenti_zombie.size()-1].push_back(stato.missione.orientamento_connessioni[i][vie_da_seguire[j]]);
			if (stato.missione.dati[i].walker>0 && (zombie == "walker" || zombie == "tutti") && azioni_walker == true)
			{
				copia_dati[vie_da_seguire[j]].SommaValoreWalker(stato.missione.dati[i].walker / vie_da_seguire.size());
				copia_dati[i].SommaValoreWalker(-stato.missione.dati[i].walker / vie_da_seguire.size());
			}
			if (stato.missione.dati[i].fat > 0 && (zombie == "fat" || zombie == "tutti") && azioni_fat == true)
			{
				copia_dati[vie_da_seguire[j]].SommaValoreFat(stato.missione.dati[i].fat / vie_da_seguire.size());
				copia_dati[i].SommaValoreFat(-stato.missione.dati[i].fat / vie_da_seguire.size());
			}
			if (stato.missione.dati[i].runner > 0 && (zombie == "runner" || zombie == "tutti") && azioni_runner == true)
			{
				copia_dati[vie_da_seguire[j]].SommaValoreRunner(stato.missione.dati[i].runner / vie_da_seguire.size());
				copia_dati[i].SommaValoreRunner(-stato.missione.dati[i].runner / vie_da_seguire.size());
			}
			if (stato.missione.dati[i].abominio == true && (zombie == "abominio" || zombie == "tutti") && azioni_abominio == true)
			{
				if (vie_da_seguire.size() > 1)
					return false;
				else
				{
					copia_dati[i].ModificaValoreAbominio(false);
					copia_dati[vie_da_seguire[0]].ModificaValoreAbominio(true);
				}
			}
		}
		
	}
	return true;
}

void zombie_cerca_obiettivo(StatoGioco& stato, vector<int>& obiettivi_zombie, int i)
{
	vector<int> umani_in_vista;
	int somma_rumori = 0;
	int somma_rumori_2 = 0;
	for (int j = 0; j < stato.numero_giocatori; j++)
		if (stato.missione.dati[i].giocatori[stato.ordine_giocatori[j]-1] == true)
		{
			umani_in_vista.push_back(i);
			break;
		}
	for (int j = 0; j < 8; j++) //guarda in tutte le direzioni: 0 su, 1 giù, 2 dx, 3 sx, 4 sx, 5 su dx, 6 giù sx, 7 giù dx
		umano_in_vista(stato, umani_in_vista, i, j, 0); //cerca umano

	int num_obiettivi = 0;
	if (umani_in_vista.size() != 0)
		num_obiettivi = (int)umani_in_vista.size();
	else
		num_obiettivi = (int)stato.missione.dati.size();

	for (int j = 0; j < num_obiettivi; j++)
	{
		int cella_dest = 0;
		if (umani_in_vista.size() != 0)
			cella_dest = umani_in_vista[j];
		else
			cella_dest = j;

		for (int k = 0; k < stato.numero_giocatori; k++)	//somma rumori
			if (stato.missione.dati[cella_dest].giocatori[k] == true)
				somma_rumori_2++;
		somma_rumori_2 = somma_rumori_2 + stato.missione.dati[cella_dest].rumore;
		if (somma_rumori <= somma_rumori_2)		//scegli il più rumoroso
		{
			if (somma_rumori < somma_rumori_2)
				obiettivi_zombie.clear();
			obiettivi_zombie.push_back(cella_dest); //aggiungi un eventuale obiettivo
			somma_rumori = somma_rumori_2;
		}
		somma_rumori_2 = 0;
	}
	somma_rumori = 0;
}

void umano_in_vista(StatoGioco& stato, vector<int>& umani_visti, int stanza, int verso, int peso)
{
	for (size_t i = 0; i < stato.missione.dati.size(); i++)
	{
		if (stato.missione.collisioni[stanza][i] == 1 && stato.missione.orientamento_connessioni[stanza][i] == verso && peso + stato.missione.dati[i].tipo<2)
		{
			for (int j = 0; j < stato.numero_giocatori; j++)
				if (stato.missione.dati[i].giocatori[stato.ordine_giocatori[j]-1] == true)
				{
					umani_visti.push_back(i);
					break;
				}
			umano_in_vista(stato,umani_visti, i, verso, peso + stato.missione.dati[i].tipo);
		}
	}
}

void scelta_movimento_abominio(StatoGioco& stato, vector<int>& zone_di_movimento_abominio,int& zona_abominio)
{
	vector<int>obiettivi_abominio;
	for (size_t i = 0; i < stato.missione.dati.size(); i++)
		if (stato.missione.dati[i].abominio == true)
			zona_abominio = i;
	zombie_cerca_obiettivo(stato, obiettivi_abominio, zona_abominio);
	for (int i = 0; i < (int)obiettivi_abominio.size(); i++)
		for (int j = 0; j < (int)stato.percorsi_minimi[obiettivi_abominio[i]][zona_abominio].size(); j++)
			if (find(zone_di_movimento_abominio.begin(),
				zone_di_movimento_abominio.end(),
				stato.percorsi_minimi[obiettivi_abominio[i]][zona_abominio][j])
				== zone_di_movimento_abominio.end())
				zone_di_movimento_abominio.push_back(stato.percorsi_minimi[obiettivi_abominio[i]][zona_abominio][j]);
}

bool spawn_zombie(StatoGioco& stato, ALLEGRO_EVENT event, vector<int>& lista_punti_spawn)                                                                                                             
{
	vector<int> tombini;
	string zombie;
	int pericolo;
	bool fine = true;	
	if (stato.Mazzo_spawn.size() == 0)
		stato.Mazzo_spawn = stato.Mazzo_spawn_originale;
	for (; stato.indicatore.azioneZombie_contatore_spawn < (int)lista_punti_spawn.size();)
	{
		if (stato.indicatore.check_extraturn == false) //controllo se è in corso un extra turn
		{
			stato.indicatore.random = rand() % stato.Mazzo_spawn.size();
			stato.indicatore.pescata = stato.Mazzo_spawn[stato.indicatore.random];
			stato.indicatore.check_extraturn = true;
		}
		stato.indicatore.check_extraturn = false;
		pericolo = calcola_pericolo(stato);
		switch (stato.Mazzo_spawn[stato.indicatore.random].tipologia)
		{
		case 0: //carta normale
			switch (stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][0])
			{
			case 0: //walker
				stato.missione.dati[lista_punti_spawn[stato.indicatore.azioneZombie_contatore_spawn]].SommaValoreWalker
				(stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][1]);
				break;
			case 1: //runner
				stato.missione.dati[lista_punti_spawn[stato.indicatore.azioneZombie_contatore_spawn]].SommaValoreRunner
				(stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][1]);
				break;
			case 2: //fat
				stato.missione.dati[lista_punti_spawn[stato.indicatore.azioneZombie_contatore_spawn]].SommaValoreFat
				(stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][1]);
				stato.missione.dati[lista_punti_spawn[stato.indicatore.azioneZombie_contatore_spawn]].SommaValoreWalker
				((stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][1]) * 2);
				break;
			case 3: //abominio
				if (!cerca_abominio(stato))
					stato.missione.dati[lista_punti_spawn[stato.indicatore.azioneZombie_contatore_spawn]].ModificaValoreAbominio(true);
				else
				{
					stato.missione.dati[lista_punti_spawn[stato.indicatore.azioneZombie_contatore_spawn]].SommaValoreFat(1);
					stato.missione.dati[lista_punti_spawn[stato.indicatore.azioneZombie_contatore_spawn]].SommaValoreWalker(2);
				}
				break;
			}
			break;
		case 1: //carta tombino
			tombini_attivi(stato, tombini);
			switch (stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][0])
			{
			case 0: //walker
				for (int j = 0; j <(int)tombini.size(); j++)
					stato.missione.dati[tombini[j]].SommaValoreWalker(stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][1]);
				break;
			case 1: //runner
				for (int j = 0; j < (int)tombini.size(); j++)
					stato.missione.dati[tombini[j]].SommaValoreRunner(stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][1]);
				break;
			case 2: //fat
				for (int j = 0; j < (int)tombini.size(); j++)
				{
					stato.missione.dati[tombini[j]].SommaValoreFat(stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][1]);
					stato.missione.dati[tombini[j]].SommaValoreWalker((stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][1]) * 2);
				}
				break;
			}
			break;
		case 2: //carta extra turn
			if (pericolo>0)
			{
				stato.indicatore.check_extraturn = true;
				switch (stato.Mazzo_spawn[stato.indicatore.random].zombie[pericolo][0])
				{
				case 0:
					zombie = "walker";
					break;
				case 1:
					zombie = "runner";
					break;
				case 2:
					zombie = "fat";
					break;
				}
				if (attiva_zombie(stato, event, zombie) == true)
					return true;
			}
		}
		return false;
	}
	stato.indicatore.azioneZombie_contatore_spawn = 0;
	return true;
}

void draw_spawn_zombie(StatoGioco& stato,Immagini& immagini_e_fonts,vector<int>elenco_spawn)
{
	int larghezza_mappa = 0, altezza_mappa = 0;
	for (size_t i = 0; i < stato.missione.Mappa_matrice.size(); i++)
		if (larghezza_mappa < (int)stato.missione.Mappa_matrice[i].size())
			larghezza_mappa = (int)stato.missione.Mappa_matrice[i].size();
	larghezza_mappa = larghezza_mappa * 982;
	altezza_mappa = (int)stato.missione.Mappa_matrice.size()*982;
	if (stato.indicatore.stato_spawn > 0)
	{
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
		m_draw_bitmap(stato.indicatore.pescata.Immagine, 0, 0, 0);
		int pericolo = calcola_pericolo(stato);
		int y = 328 - (75 * pericolo);
		al_draw_rectangle(51 - 5, y - 5, 51 + 220, y + 70, al_map_rgba(0, 0, 180, 0), 5);
		if(stato.indicatore.azioneZombie_contatore_spawn<(int)elenco_spawn.size())
		{
			draw_selezione_zona_mappa(stato, stato.missione.mappa, elenco_spawn[stato.indicatore.azioneZombie_contatore_spawn], al_map_rgba(0, 100, 0, 0.5));
			inquadra_zona_mappa(stato, elenco_spawn[stato.indicatore.azioneZombie_contatore_spawn], larghezza_mappa, altezza_mappa);
		}
	}
	switch (stato.indicatore.stato_spawn)
	{
	case 1://solo nel caso di extra turn
		//sia la grafica della carta
		if (stato.indicatore.check_extraturn == true)
		{
			inquadra_zona_mappa(stato, elenco_spawn[stato.indicatore.azioneZombie_contatore_spawn],larghezza_mappa,altezza_mappa);
			//sia la grafica delle frecce
			MATTEO_BITMAP::set_target_bitmap(stato.missione.mappa);
			float rotazione;
			for (size_t i = 0; i < stato.indicatore.zone_spostamenti_zombie.size(); i++)
				for (size_t j = 0; j < stato.indicatore.direzioni_spostamenti_zombie[i].size(); j++)
				{
					rotazione = rotazione_in_base_direzione_zona(stato.indicatore.direzioni_spostamenti_zombie[i][j]);
					int width_finale = 0; int height_finale = 0;
					{
						int cont = 0;
						for (size_t i = 0; i < stato.missione.Mappa_matrice.size(); i++)
							if (cont < (int)stato.missione.Mappa_matrice[i].size())
								cont = stato.missione.Mappa_matrice[i].size();
						int width_1 = 982 * cont; int height_1 = 982 * stato.missione.Mappa_matrice.size();
						if ((double)width_1 / height_1 > (double)stato.indicatore.w_riquadro_mappa / stato.indicatore.h_riquadro_mappa)
						{
							//width_finale deve essere uguale a width_1 (della mappa)
							//mentre height_finale devessere aumentato con dello spazione nero
							width_finale = width_1;
							height_finale = width_1 * stato.indicatore.h_riquadro_mappa / stato.indicatore.w_riquadro_mappa;
						}
						else
						{
							height_finale = height_1;
							width_finale = height_1 * stato.indicatore.w_riquadro_mappa / stato.indicatore.h_riquadro_mappa;
						}
					}
					int w = (width_finale / stato.indicatore.zoom), h = (height_finale / stato.indicatore.zoom);
					if (rotazione != 360)
					{
						int dx = (stato.missione.x_cordinate_zone_mappa[stato.indicatore.zone_spostamenti_zombie[i]] +
							(stato.missione.x_dimensione_zone_mappa[stato.indicatore.zone_spostamenti_zombie[i]] / 2) - stato.indicatore.x_riquadro_mappa)*stato.indicatore.w_riquadro_mappa / w;
						int dy = (stato.missione.y_cordinate_zone_mappa[stato.indicatore.zone_spostamenti_zombie[i]] +
							(stato.missione.y_dimensione_zone_mappa[stato.indicatore.zone_spostamenti_zombie[i]] / 2) - stato.indicatore.y_riquadro_mappa)*stato.indicatore.h_riquadro_mappa / h;

						m_draw_scaled_rotated_bitmap(immagini_e_fonts.freccia_a_destra, 0, 28,
							dx, dy, stato.indicatore.zoom*0.70, stato.indicatore.zoom*0.7, rotazione, 0);
					}
				}
		}
		//apri menu ferite se devi assegnarle
		if (stato.indicatore.stato_azioniZombie == 1)
		{
			draw_menu_assegna_ferite(stato, immagini_e_fonts);
		}
		break;
	}
}
	
void tombini_attivi(StatoGioco& stato,vector<int>& tombini_attivi)
{
	vector<int> tessere_di_interesse;
	//cerco le Tessere in cui ci sono giocatori e le metto in tessere di interesse, controllando man mano che non le ho già messe
	for (int i = 0; i < stato.numero_giocatori; i++)
		for (int j = 0; j < (int)stato.missione.dati[stato.giocatore[stato.ordine_giocatori[i] - 1].posizione].tessera_numero.size(); j++)
			if(find(tessere_di_interesse.begin(),
				tessere_di_interesse.end(),
				stato.missione.dati[stato.giocatore[stato.ordine_giocatori[i] - 1].posizione].tessera_numero[j])== tessere_di_interesse.end())
				tessere_di_interesse.push_back(stato.missione.dati[stato.giocatore[stato.ordine_giocatori[i] - 1].posizione].tessera_numero[j]);
	for (int i = 0; i < (int)tessere_di_interesse.size(); i++)
	{
		for (size_t j = 0; j < stato.missione.tombini.size(); j++)
			if(stato.missione.tombini[j][1]==tessere_di_interesse[i]) //tombini[j][1] si riferisce alla tessera di quel tombino
				tombini_attivi.push_back(stato.missione.tombini[j][0]); //tombini[j][0] su riferisce alla zona di quel tombino
	}
		
}