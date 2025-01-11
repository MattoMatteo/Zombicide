#include "azioni.h"
#include "utilità.h"
#include "Azioni_zombie.h"
#include "armi_speciale.h"
#include "Script gestione equip.h"
#include "algorithm"
#include "Attacco.h"
#include "Matteo Draw.h"
#include <iostream>
#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

using namespace std;

bool movimento_fattibile(StatoGioco& stato)
{
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno]-1].posizione;
	int peso=0;
	int costo_azione=0;
	for (size_t i = 0; i < stato.elenco_azioni.size(); i++)
		if (stato.elenco_azioni[i].nome == "Muovi" || stato.elenco_azioni[i].nome == "Muovi (Gratis)")
			costo_azione = stato.elenco_azioni[i].costo_di_azioni;
	stato.indicatore.considerare_zombie_per_zone_disponibili_a_spostamento = true;
	for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].tipo == Abilità::MODIFICATORE_MOVIMENTO)
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].funzione
			(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1, i, Abilità::CODICE_ESEGUI);
	if (stato.indicatore.considerare_zombie_per_zone_disponibili_a_spostamento == true)
	{
		peso = stato.missione.dati[posizione].walker + stato.missione.dati[posizione].runner + stato.missione.dati[posizione].fat;
		if (stato.missione.dati[posizione].abominio == true)
			peso++;
	}
	for (size_t i = 0; i < stato.missione.dati.size()&&costo_azione+peso<=stato.numero_azioni; i++)
	{
		if (stato.missione.collisioni[posizione][i] == 1 || stato.missione.collisioni[posizione][i] == 3)
			return true;
	}
	return false;
}
int movimento_esegui(StatoGioco& stato, ALLEGRO_EVENT& event) 
{
	vector<int>zone_di_movimento_disponibili;
	stato.indicatore.numero_spostamenti_con_un_movimento = 1;
	stato.indicatore.considerare_zombie_per_zone_disponibili_a_spostamento = true;
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	int peso = stato.missione.dati[posizione].walker + stato.missione.dati[posizione].runner + stato.missione.dati[posizione].fat;
	if (stato.missione.dati[posizione].abominio == true)
		peso++;
	for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].tipo == Abilità::MODIFICATORE_MOVIMENTO)
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].funzione
			(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1, i, Abilità::CODICE_ESEGUI);
	stato.Cursori["Scegli zona"]["Lista zone"];
	stato.Cursori["Scegli zona"]["Input"];
	zone_di_movimento_disponibili.push_back(posizione);
	stanze_raggiungibili_fino_a_x_distanza_da_un_punto(stato, zone_di_movimento_disponibili, stato.indicatore.numero_spostamenti_con_un_movimento, posizione,
		stato.indicatore.considerare_zombie_per_zone_disponibili_a_spostamento); //trovo tutte le zone		
	creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(stato, zone_di_movimento_disponibili, zone_di_movimento_disponibili[0]); //le ordino
	vector<int> solo_zone_connesse_con_iteratore = zone_di_movimento_disponibili; //le copio
	for (size_t i = 0; i < zone_di_movimento_disponibili.size(); i++) //nella copia tolgo quelle non connesse con l'iteratore che punta in quella originale
		if (stato.missione.collisioni[zone_di_movimento_disponibili[stato.Cursori["Scegli zona"]["Lista zone"].iteratore]][zone_di_movimento_disponibili[i]] != 1 &&
			zone_di_movimento_disponibili[stato.Cursori["Scegli zona"]["Lista zone"].iteratore] != zone_di_movimento_disponibili[i])
		{
			for (size_t j = 0; j < solo_zone_connesse_con_iteratore.size(); j++)
				if(solo_zone_connesse_con_iteratore[j]== zone_di_movimento_disponibili[i])
					solo_zone_connesse_con_iteratore.erase(solo_zone_connesse_con_iteratore.begin() + j);
		}
	creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(stato, solo_zone_connesse_con_iteratore, //ordino il nuovo vettore
		solo_zone_connesse_con_iteratore[0]);
	for (size_t i = 0; i < solo_zone_connesse_con_iteratore.size(); i++) //punto il nuovo iteratore sul vettore copia nella stessa zona che punta il primo iteratore sul vettore originale
		if (solo_zone_connesse_con_iteratore[i] == zone_di_movimento_disponibili[stato.Cursori["Scegli zona"]["Lista zone"].iteratore])
			stato.Cursori["Scegli zona"]["Input"].iteratore = i;
	vector<int> vettore_per_input_zone_ordinate = creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(stato, solo_zone_connesse_con_iteratore,
		solo_zone_connesse_con_iteratore[stato.Cursori["Scegli zona"]["Input"].iteratore]); //creo il vettore per l'input
	int contatore_indice_iteratore = 0; //cambio l'iteratore in base agli indici del vettore per input
	for (size_t i = 0; i < vettore_per_input_zone_ordinate.size(); i++)
		if (i != 4)
		{
			for (int j = 0; j < vettore_per_input_zone_ordinate[i]; j++)
				contatore_indice_iteratore++;
		}
		else
			stato.Cursori["Scegli zona"]["Input"].iteratore = contatore_indice_iteratore;

	input_tastiera_9_direzioni(stato.Cursori["Scegli zona"]["Input"].iteratore,event, vettore_per_input_zone_ordinate,false, false, false); //uso la copia e il nuovo iteratore per l'input

	for (size_t i = 0; i < zone_di_movimento_disponibili.size(); i++) //allineo sulla stessa zona anche il primo vettore sul vettore di zone originale
		if (solo_zone_connesse_con_iteratore[stato.Cursori["Scegli zona"]["Input"].iteratore] == zone_di_movimento_disponibili[i])
			stato.Cursori["Scegli zona"]["Lista zone"].iteratore = i;

	if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (event.keyboard.keycode == ALLEGRO_KEY_ENTER&&zone_di_movimento_disponibili[stato.Cursori["Scegli zona"]["Lista zone"].iteratore]!=posizione)
		{
			stato.missione.dati[posizione].ModificaValoreGiocatori(false, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1);
			stato.missione.dati[zone_di_movimento_disponibili[stato.Cursori["Scegli zona"]["Lista zone"].iteratore]].ModificaValoreGiocatori(true,
				stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1);
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno]-1].posizione = zone_di_movimento_disponibili[stato.Cursori["Scegli zona"]["Lista zone"].iteratore];
			stato.Cursori["Scegli zona"]["Lista zone"].iteratore = 0;
			if (stato.indicatore.considerare_zombie_per_zone_disponibili_a_spostamento == true)
				stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].costo_di_azioni =
				stato.azioni_disponibili[stato.Cursori["Turno giocatore"]["Scelta azioni"].iteratore].costo_di_azioni + peso;
			return 1;
		}
		if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		{
			stato.Cursori["Scegli zona"]["Lista zone"].iteratore = 0;
			return -1;
		}
	}
	return 0;
}
void movimento_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	vector<int>zone_di_movimento_disponibili;
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
	al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.collage_menu, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
		al_map_rgba(255, 255, 255, 0), 10, 0,300, -1, 0, "In quale zona?");
	zone_di_movimento_disponibili.push_back(posizione);
	stanze_raggiungibili_fino_a_x_distanza_da_un_punto(stato, zone_di_movimento_disponibili, stato.indicatore.numero_spostamenti_con_un_movimento, posizione,
		stato.indicatore.considerare_zombie_per_zone_disponibili_a_spostamento); //creo vettore di tutte le zone
	creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(stato,zone_di_movimento_disponibili, zone_di_movimento_disponibili[0]); //le ordino
	for (size_t i = 0; i < zone_di_movimento_disponibili.size(); i++) //le disegno tutte di blu tranne quella in cui punta l'iteratore
		if (i != stato.Cursori["Scegli zona"]["Lista zone"].iteratore)
			draw_selezione_zona_mappa(stato, stato.missione.mappa, zone_di_movimento_disponibili[i], al_map_rgba(0, 0, 100, 0.2));
	draw_selezione_zona_mappa(stato, stato.missione.mappa, zone_di_movimento_disponibili[stato.Cursori["Scegli zona"]["Lista zone"].iteratore], al_map_rgba(0, 100, 0, 0.2));
	//disegno di verde quello in cui punta l'iteratore
}

bool aprire_porta_fattibile(StatoGioco& stato)
{
	for (size_t i = 0; i < stato.missione.dati.size(); i++)
		if (stato.missione.collisioni[stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione][i] == 2)
			for (size_t j = 0; j < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size() && j<2; j++)
				if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[j].apre_porte >0)
					return true;
	return false;
}
int aprire_porta_esegui(StatoGioco& stato, ALLEGRO_EVENT& event)
{
	vector<int> stanze_edificio;
	vector<int> porte_disponibili;	
	vector<int> zone_di_porte_disponibili;
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	int numero_di_armi_in_mano;
	if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size()>1)
		numero_di_armi_in_mano = 2;
	else
		numero_di_armi_in_mano = 1;
	stato.Cursori["Azione apri, lista porte"]["Lista"];
	stato.Cursori["Scelta equip personaggio"]["Lista"];
	switch (stato.indicatore.stato_apri_porte)
	{
	case 0://scegli arma per aprire
		input_tastiera_9_direzioni(stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore, event, { numero_di_armi_in_mano,0,0,0,0,0,0,0,0 }, false, false, false);
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore].apre_porte != 0)
				{
					stato.indicatore.equipaggiamento_selezionato =
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore];
					stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore = 0;
					stato.indicatore.stato_apri_porte++;
				}
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				return -1;
		}
		break;
	case 1: //scelta porta
		for (size_t i = 0; i < stato.missione.porta.size(); i++)		//cerco le porte disponibili confrontandole con la posizione del giocatore di turno
			if ((stato.missione.porta[i].zone[0] == posizione	&& stato.missione.porta[i].stato == 0) ||
				(stato.missione.porta[i].zone[1] == posizione	&& stato.missione.porta[i].stato == 0))
					porte_disponibili.push_back(i);

		for (int i = 0; i < (int)porte_disponibili.size(); i++)
			for (int j = 0; j < 2; j++)
				if (stato.missione.porta[porte_disponibili[i]].zone[j] != posizione)
					zone_di_porte_disponibili.push_back(stato.missione.porta[porte_disponibili[i]].zone[j]);
		input_tastiera_9_direzioni(stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore, event,
			creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(stato,zone_di_porte_disponibili, posizione), false, false, false);
		{
			vector<int> v;
			for (size_t i = 0; i < zone_di_porte_disponibili.size(); i++)
				for (size_t j = 0; j<porte_disponibili.size(); j++)
					for (int k = 0; k < 2; k++)
						if (zone_di_porte_disponibili[i] == stato.missione.porta[porte_disponibili[j]].zone[k])
							v.push_back(porte_disponibili[j]);
			porte_disponibili = v;
		}
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				for (int i = 0; i < 2; i++)
					if (stato.missione.porta[porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]].zone[i] !=
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione&&
						stato.missione.dati[stato.missione.porta[porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]].zone[i]].tipo == 1)
					{
						stato.indicatore.prima_stanza_edificio = stato.missione.porta[porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]].zone[i];
						determina_stanze_edificio(stato, stanze_edificio, stanze_edificio, stato.indicatore.prima_stanza_edificio,
							stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione);
						if (edificio_aperto(stato, stanze_edificio) == false)
							stato.indicatore.stato_apri_porte++;
					}
				stato.missione.collisioni[stato.missione.porta[porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]].zone[0]]
					[stato.missione.porta[porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]].zone[1]] = 1;
				stato.missione.collisioni[stato.missione.porta[porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]].zone[1]]
					[stato.missione.porta[porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]].zone[0]] = 1;
				stato.missione.ModificaStatoPorta(1, porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]); //cioè sfondata
				if (stato.indicatore.equipaggiamento_selezionato.apre_porte == 1)
					for (int i = 0; i < 2; i++)
						if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione ==
							stato.missione.porta[porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]].zone[i])
							stato.missione.dati[stato.missione.porta[porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]].zone[i]].SommaValoreRumore(1);
				stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore = 0;
				percorsi(stato);
				if (stato.indicatore.stato_apri_porte == 1)
				{
					stato.indicatore.stato_apri_porte = 0;
					return 1;
				}
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore = 0;
				stato.indicatore.stato_apri_porte--;
			}
		}	
		break;
	case 2: //spawn zombie
		determina_stanze_edificio(stato, stanze_edificio, stanze_edificio, stato.indicatore.prima_stanza_edificio,
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione);
		sort(stanze_edificio.begin(), stanze_edificio.end());
		if (logic_spawn_zombie(stato, event, stanze_edificio) == true)
		{
			stato.indicatore.stato_apri_porte = 0;
			return 1;
		}
		break;
	}
	return 0;
}
void aprire_porta_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	vector<int> porte_disponibili;
	vector<int> stanze_edificio;
	vector<int> zone_di_porte_disponibili;
	switch (stato.indicatore.stato_apri_porte)
	{
	case 0://scegli equipaggiamento per aprire
		draw_cursore_su_equip_personaggio(stato, immagini_e_fonts, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno]-1].equipaggiamento.size(),al_map_rgba(0,150,0,0.2),
			stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore);
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_scaled_bitmap(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].Scheda_personaggio_con_equip,0,0,
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].Scheda_personaggio_con_equip->width,
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].Scheda_personaggio_con_equip->height,
			320, 174, 1280, 732, 0);
		break;
	case 1://apri porta
		for (size_t i = 0; i < stato.missione.porta.size(); i++)		//cerco le porte disponibili confrontandole con la posizione del giocatore di turno
			if ((stato.missione.porta[i].zone[0] == stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione
					&& stato.missione.porta[i].stato == 0) || (stato.missione.porta[i].zone[1] == stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione
						&& stato.missione.porta[i].stato == 0))
					porte_disponibili.push_back(i);
		for (size_t i = 0; i < porte_disponibili.size(); i++)
			for (int j = 0; j < 2; j++)
				if (stato.missione.porta[porte_disponibili[i]].zone[j] != stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione)
					zone_di_porte_disponibili.push_back(stato.missione.porta[porte_disponibili[i]].zone[j]);

		creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(stato, zone_di_porte_disponibili, stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione);
		{
			vector<int> v;
			for (size_t i = 0; i < zone_di_porte_disponibili.size(); i++)
				for(size_t j=0; j<porte_disponibili.size(); j++)
					for (int k = 0; k < 2; k++)
						if (zone_di_porte_disponibili[i] == stato.missione.porta[porte_disponibili[j]].zone[k])
							v.push_back(porte_disponibili[j]);
			porte_disponibili = v;
		}
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
		al_draw_scaled_text(stato.font, immagini_e_fonts,immagini_e_fonts.collage_menu,stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
			10, 0,300,-1,0, "Seleziona porta");

		draw_selezione_segnalini_porte(stato, immagini_e_fonts, stato.missione.porta[porte_disponibili[stato.Cursori["Azione apri, lista porte"]["Lista"].iteratore]], stato.missione, stato.indicatore.zoom,
			stato.indicatore.x_riquadro_mappa, stato.indicatore.y_riquadro_mappa, stato.indicatore.w_riquadro_mappa, stato.indicatore.h_riquadro_mappa,
			0, 255, 0, 10);
		
		break;
	case 2://spawn zombie dentro edificio
		determina_stanze_edificio(stato, stanze_edificio, stanze_edificio, stato.indicatore.prima_stanza_edificio,
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione);
		sort(stanze_edificio.begin(), stanze_edificio.end());
		draw_spawn_zombie(stato, immagini_e_fonts,stanze_edificio);
		break;
	}
}

bool termina_turno_fattibile(StatoGioco& stato)
{
	return true;
}
int termina_turno_esegui(StatoGioco& stato, ALLEGRO_EVENT& event)
{
	stato.Cursori["Azione termina turno"]["Scelta si o no"];
	input_tastiera_9_direzioni(stato.Cursori["Azione termina turno"]["Scelta si o no"].iteratore, event, { 2,0,0,0,0,0,0,0,0 }, true, false, false);
	if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
		{
			if (stato.Cursori["Azione termina turno"]["Scelta si o no"].iteratore == 0)
			{
				stato.numero_azioni = -1;
				return 1;
			}
			else
				return -1;
		}
		if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			return -1;
	}
	return 0;
}
void termina_turno_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
	al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.collage_menu, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
		10, 0, 300, -1, 0, "Terminare il turno?");
	vector<string> si_e_no;
	si_e_no.push_back("Si");
	si_e_no.push_back("No");
	draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
		stato.Cursori["Azione termina turno"]["Scelta si o no"], si_e_no, 0, 110, 45, 0, 1,
		immagini_e_fonts.collage_menu->width, immagini_e_fonts.collage_menu->height, 40, 70, 56,
		NULL, false, al_map_rgb(0, 0, 0), al_map_rgb(0, 0, 0),0);
}

bool fuggi_fattibile(StatoGioco& stato)
{
	if (stato.missione.dati[stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione].exit == true)
		return true;
	return false;
}
int fuggi_esegui(StatoGioco& stato, ALLEGRO_EVENT& event)
{
	uccidi_giocatore(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1);
	return 1;
}
void fuggi_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	;
}

bool scambia_equipaggiamento_fattibile(StatoGioco& stato)
{
	for (int i = 0; i < stato.numero_giocatori; i++)
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione == stato.giocatore[stato.ordine_giocatori[i]-1].posizione
			&& stato.ordine_giocatori[i] - 1 != stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
			return true;
	return false;
}
int scambia_equipaggiamento_esegui(StatoGioco& stato, ALLEGRO_EVENT& event)
{	
	return scambia(stato, event);
}
void scambia_equipaggiamento_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	gestione_equip_grafica(stato, immagini_e_fonts, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1);
}

bool scambia_da_solo_fattibile(StatoGioco& stato)
{
	if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size()>1)
		return true;
	return false;
}
int scambia_da_solo_esegui(StatoGioco& stato, ALLEGRO_EVENT& event)
{
	return modifica_equip(stato, event, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1);
}
void scambia_da_solo_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	MATTEO_BITMAP::set_target_bitmap(stato.giocatore[6].Scheda_personaggio_con_equip);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	scambia_equipaggiamento_grafica(stato, immagini_e_fonts);
}

bool cerca_fattibile(StatoGioco& stato)
{
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	if(stato.missione.dati[posizione].cercabile==true &&
		stato.indicatore.cercabile==true&&stato.missione.dati[posizione].walker==0&&
		stato.missione.dati[posizione].runner==0&&stato.missione.dati[posizione].fat==0&&
		stato.missione.dati[posizione].abominio==false)
		return true;
	return false;
}
int cerca_esegui(StatoGioco& stato, ALLEGRO_EVENT& event)
{
	return cerca(stato, event);
}
void cerca_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	MATTEO_BITMAP::set_target_bitmap(stato.giocatore[6].Scheda_personaggio_con_equip);
	al_clear_to_color(al_map_rgba(0, 0, 0, 0));
	m_draw_bitmap(immagini_e_fonts.scheda_personaggio, 0, 0, 0);
	int lunghezza = stato.giocatore[6].Scheda_personaggio_con_equip->width;
	int altezza = stato.giocatore[6].Scheda_personaggio_con_equip->height;
	int dy = (135 * altezza) / 369;
	int dw = (139 * lunghezza) / 690;
	int dh = (192 * altezza) / 369;
	for (size_t j = 2, x = (214*lunghezza)/690; j < 5 && j < stato.giocatore[6].equipaggiamento.size(); j++, x = x + (162*lunghezza)/690)
		m_draw_scaled_bitmap(stato.giocatore[6].equipaggiamento[j].Immagine,0,0,
			stato.giocatore[6].equipaggiamento[j].Immagine->width,stato.giocatore[6].equipaggiamento[j].Immagine->height,
			x, dy, dw, dh, 0);
	dy = (160 * altezza) / 369;
	for (size_t j = 0, x = (296*lunghezza)/690; j < 2 && j < stato.giocatore[6].equipaggiamento.size(); j++, x = x + (162*lunghezza)/690)
		m_draw_scaled_bitmap(stato.giocatore[6].equipaggiamento[j].Immagine,0, 0,
			stato.giocatore[6].equipaggiamento[j].Immagine->width, stato.giocatore[6].equipaggiamento[j].Immagine->height,
			x, dy, dw, dh, 0);
	scambia_equipaggiamento_grafica(stato, immagini_e_fonts);
}

bool attacca_fattibile(StatoGioco& stato)
{
	vector<int> stanze_di_interesse;
	Carta_equipaggiamento arma;
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	int raggio_max = 0;
	int gittata_min = 0;
	int gittata_max = 0;
	for (int i = 0; i <(int)stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size()&&i<2; i++)
	{
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].tipologia & Carta_equipaggiamento::ARMA) //è un arma prima di tutto
		{
			switch (arma_melee_ranged_o_doppia(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i]))
			{
			case 0: //melee
				gittata_min = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].gittata_min_melee;
				gittata_max = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].gittata_max_melee;
				break;
			case 1://ranged
				gittata_min = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].gittata_min_ranged;
				gittata_max = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].gittata_max_ranged;
				break;
			case 2://gunblade
				gittata_min = gittata_min = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].gittata_min_ranged;
				gittata_max = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].gittata_max_ranged;
				if (gittata_min > stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].gittata_min_melee)
					gittata_min = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].gittata_min_melee;
				if (gittata_max < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].gittata_max_melee)
					gittata_max = gittata_max = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].gittata_max_melee;
				break;
			}
			if (gittata_min == 0) //verifico prima nella stessa zona
			{
				if (stato.missione.dati[posizione].walker > 0 || stato.missione.dati[posizione].runner>0 || //vanno bene sia zombie
					stato.missione.dati[posizione].fat>0 || stato.missione.dati[posizione].abominio == true)
					return true;
				else
				{
					for (int j = 0; j < stato.numero_giocatori; j++) //che giocatori
						if (stato.missione.dati[posizione].giocatori[stato.ordine_giocatori[j] - 1] == true &&
							stato.ordine_giocatori[j] - 1 != stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
							return true;
				}
			}
			if(gittata_max>0)
				for (int j = 0; j < 8; j++) //ora controllo in tutte le direzioni
					if (cerca_zombie(stato, stanze_di_interesse, posizione, j, 0,gittata_min,gittata_max, 1) == true)
						return true;
		}	
	}
	return false;
}
int attacca_esegui(StatoGioco& stato, ALLEGRO_EVENT& event)
{
	vector<int> stanze_di_interesse;
	vector<int> fattibilità_arma_selezionata;
	ALLEGRO_KEYBOARD_STATE keyboard_state;
	al_get_keyboard_state(&keyboard_state);
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	int r1;
	stato.Cursori["Scelta equip personaggio"]["Lista"];
	switch (stato.indicatore.stato_azione_attacca)
	{
	case 0: //scegli arma
		r1 = 0;
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size() > 1)
			r1 = 2;
		else
			r1 = 1;
		input_tastiera_9_direzioni(stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore
		, event, { r1,0,0,0,0,0,0,0,0 }, false, false,false);
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore = 0;
				return -1;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore].
					tipologia & Carta_equipaggiamento::ARMA)
				{
					stato.indicatore.arma_selezionata =
						stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore];
					stato.indicatore.tipo_arma_selezionata= arma_melee_ranged_o_doppia(stato.indicatore.arma_selezionata);
					for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
						if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].nome == "ambidexytous")
							stato.indicatore.arma_selezionata.doppia = true;
					switch (stato.indicatore.tipo_arma_selezionata)
					{
					case 0: //melee
						if (fattibilità_attacco_arma_selezionata(stato, posizione, stanze_di_interesse,
							stato.indicatore.arma_selezionata.gittata_min_melee, stato.indicatore.arma_selezionata.gittata_max_melee)==true)
							fattibilità_arma_selezionata.push_back(0);
						break;
					case 1://ranged
						if (fattibilità_attacco_arma_selezionata(stato, posizione, stanze_di_interesse,
							stato.indicatore.arma_selezionata.gittata_min_ranged, stato.indicatore.arma_selezionata.gittata_max_ranged) == true)
							fattibilità_arma_selezionata.push_back(1);
						break;
					case 2: //gunblade
						if (fattibilità_attacco_arma_selezionata(stato, posizione, stanze_di_interesse,
							stato.indicatore.arma_selezionata.gittata_min_melee, stato.indicatore.arma_selezionata.gittata_max_melee) == true)
							fattibilità_arma_selezionata.push_back(0);
						if (fattibilità_attacco_arma_selezionata(stato, posizione, stanze_di_interesse,
							stato.indicatore.arma_selezionata.gittata_min_ranged, stato.indicatore.arma_selezionata.gittata_max_ranged) == true)
							fattibilità_arma_selezionata.push_back(1);
						break;
					}
					if (fattibilità_arma_selezionata.size() > 0)
					{
						stato.indicatore.stato_azione_attacca = 1;
					}
				}
			}
		}
		break;
	case 1: //esecuzione arma
		return stato.indicatore.arma_selezionata.speciale(stato, event);
		break;
		}
	return 0;
}
void attacca_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	vector<int> stanze_di_interesse;
	int posizione = stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione;
	vector<string> elenco;
	vector<int> personaggi_in_zona;
	int gittata_min = 0;
	int gittata_max = 0;
	int obiettivo;
	int tipo;
	int x_dim;
	int y_dim;
	switch (stato.indicatore.stato_azione_attacca)
	{
	case 0:	//scegli arma
		draw_cursore_su_equip_personaggio(stato, immagini_e_fonts, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size(),al_map_rgba(0,150,0,0.2),
			stato.Cursori["Scelta equip personaggio"]["Lista"].iteratore);
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_scaled_bitmap(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].Scheda_personaggio_con_equip, 0, 0,
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].Scheda_personaggio_con_equip->width,
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].Scheda_personaggio_con_equip->height,
			320, 174, 1280, 732, 0);
		break;
	case 1: //azioni arma
		switch (stato.indicatore.stato_azione_arma_grafica)
		{
		case 0:			//scegli zona
			switch (stato.indicatore.tipo_arma_selezionata)//scegliere le gittate adatte al tipo di arma, per poi trovare le stanze di interesse
			{
			case 0://melee
				gittata_min = stato.indicatore.arma_selezionata.gittata_min_melee;
				gittata_max = stato.indicatore.arma_selezionata.gittata_max_melee;
				break;
			case 1://ranged
				gittata_min = stato.indicatore.arma_selezionata.gittata_min_ranged;
				gittata_max = stato.indicatore.arma_selezionata.gittata_max_ranged;
				break;
			}
			fattibilità_attacco_arma_selezionata(stato, posizione, stanze_di_interesse, gittata_min,gittata_max);//cerca stanze
			creazione_vettore_per_input_direzioni_con_ordinamento_di_zone(stato,stanze_di_interesse, posizione);//ordinale
			for (size_t i = 0; i < stanze_di_interesse.size(); i++)
				if(i!= stato.Cursori["Scegli zona"]["Lista zone"].iteratore)
					draw_selezione_zona_mappa(stato, stato.missione.mappa, stanze_di_interesse[i], al_map_rgba(0, 0, 100, 0.2));
			if(stanze_di_interesse.size()>0)
				draw_selezione_zona_mappa(stato, stato.missione.mappa, stanze_di_interesse[stato.Cursori["Scegli zona"]["Lista zone"].iteratore],al_map_rgba(0,100,0,0.2));//stamparle
			break;
		case 1:			//risultato dadi e menu re-roll
			MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
			tipo = arma_melee_ranged_o_doppia(stato.indicatore.arma_selezionata);
			switch (tipo) //dadi
			{
			case 0: //melee
				draw_dadi(stato, immagini_e_fonts, stato.indicatore.arma_selezionata.precisione_melee);
				break;
			case 1: //ranged
				draw_dadi(stato, immagini_e_fonts, stato.indicatore.arma_selezionata.precisione_ranged);
				break;
			case 2: //gunblade
				if (stato.indicatore.tipo_arma_selezionata == 0)
					draw_dadi(stato, immagini_e_fonts, stato.indicatore.arma_selezionata.precisione_melee);
				else if (stato.indicatore.tipo_arma_selezionata == 1)
					draw_dadi(stato, immagini_e_fonts, stato.indicatore.arma_selezionata.precisione_ranged);
				break;
			}
			if (stato.indicatore.Lista_re_roll.size() > 0)
			{
				elenco = stato.indicatore.Lista_re_roll;
				elenco.push_back("Annulla");
				MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
				x_dim = 640; y_dim = 320;
				draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
					al_map_rgb(255, 255, 255), stato.Cursori["Lista re-roll"]["Lista"], elenco,(1920 / 2) - (x_dim / 2), (1080 / 2) - (y_dim / 2),0,0,1, x_dim, y_dim,
					40,70, 56, immagini_e_fonts.Sfondo_dadi, true, al_map_rgba(80, 80, 160, 170), al_map_rgb(0, 0, 0),4);
			}
			break;
		case 2: //arma melee
			switch (stato.indicatore.sub_stato_melee_azione_attacca)
			{
			case 0://selezione
				draw_dadi(stato, immagini_e_fonts,stato.indicatore.arma_selezionata.precisione_melee);
				draw_selezione_segnalini_zombie_o_personaggi(stato, immagini_e_fonts,
					stato.indicatore.zombie_o_giocatori_selezionabili_melee, stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore, stato.indicatore.stanza_selezionata,
					0, 255, 0, 10);
				break;
			case 1: //menu ferite
				draw_menu_assegna_ferite(stato, immagini_e_fonts);
				break;
			}
			break;
		case 3: //arma ranged
			switch (stato.indicatore.sub_stato_ranged_azione_attacca)
			{
			case 0://visualizza dadi
				draw_dadi(stato, immagini_e_fonts, stato.indicatore.arma_selezionata.precisione_ranged);
				obiettivo = obiettivo_ranged(stato, stato.indicatore.stanza_selezionata);
				for (int i = 0; i < stato.numero_giocatori; i++)
					if (stato.giocatore[stato.ordine_giocatori[i] - 1].posizione == stato.indicatore.stanza_selezionata&&
						stato.ordine_giocatori[i] - 1!= stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
						personaggi_in_zona.push_back(stato.ordine_giocatori[i]-1);
				if (obiettivo > 5)
					draw_selezione_segnalini_zombie_o_personaggi(stato, immagini_e_fonts, { obiettivo }, 0, stato.indicatore.stanza_selezionata,
					0, 100, 0, 0.5);
				else if(obiettivo<=5&&obiettivo>=0)
					for(size_t i=0; i<personaggi_in_zona.size(); i++)
						if(personaggi_in_zona[i]!= stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
							draw_selezione_segnalini_zombie_o_personaggi(stato, immagini_e_fonts,personaggi_in_zona, i,
							stato.indicatore.stanza_selezionata, 0, 100, 0, 0.5);
				break;
			case 1: //visualizza ferite
				if (obiettivo_ranged(stato, stato.indicatore.stanza_selezionata) <= 5)
				{
					if (stato.indicatore.arma_selezionata.danno_ranged <2)
						draw_menu_assegna_ferite(stato, immagini_e_fonts);
					else if (stato.indicatore.arma_selezionata.danno_ranged > 1)
					{
						vector<int> personaggi_da_ferire;
						for (int i = 0; i < stato.numero_giocatori; i++)
							if (stato.missione.dati[stato.indicatore.stanza_selezionata].giocatori[stato.ordine_giocatori[i] - 1] == true &&
								stato.ordine_giocatori[i] - 1 != stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
								personaggi_da_ferire.push_back(stato.ordine_giocatori[i] - 1);
						draw_selezione_segnalini_zombie_o_personaggi(stato, immagini_e_fonts, personaggi_da_ferire,
							stato.Cursori["Seleziona obiettivi in zona"]["Lista"].iteratore, posizione,
							0, 255, 0, 20);
					}
				}
				break;
			}
			break;
		case 4: //Scelta generica + messaggio + dadi
			MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.collage_menu);
			tipo=arma_melee_ranged_o_doppia(stato.indicatore.arma_selezionata);
			switch (tipo) //dadi
			{
			case 0: //melee
				draw_dadi(stato, immagini_e_fonts,stato.indicatore.arma_selezionata.precisione_melee);
				break;
			case 1: //ranged
				draw_dadi(stato, immagini_e_fonts, stato.indicatore.arma_selezionata.precisione_ranged);
				break;
			case 2: //gunblade
				if(stato.indicatore.tipo_arma_selezionata==0)
					draw_dadi(stato, immagini_e_fonts, stato.indicatore.arma_selezionata.precisione_melee);
				else if(stato.indicatore.tipo_arma_selezionata == 1)
					draw_dadi(stato, immagini_e_fonts, stato.indicatore.arma_selezionata.precisione_ranged);
				break;
			}
			al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.collage_menu,
				stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome, al_map_rgba(255, 255, 255, 0),
				0, 50, immagini_e_fonts.collage_menu->width, -1, 0,stato.indicatore.messaggio_generico.c_str());
			draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts,stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
				al_map_rgba(255, 255, 255,0),stato.Cursori["Azione attacca, scelta generica"]["Lista"], stato.indicatore.scelte_messaggio_generico,
				0, 250,50, 0, 1,319-70-20,1080-250-20,50,70,56,NULL,false,al_map_rgba(0,0,0,0), al_map_rgba(0, 0, 0,0),0);
			break;
		case 5: //apri grafica riorganizza equip
			scambia_da_solo_grafica(stato, immagini_e_fonts);
			break;
		}
		break;
	}
}

bool combina_fattibile(StatoGioco& stato)
{
	bool almeno_una_fattibile = false;
	stato.indicatore.lista_combinabili.clear();
	stato.indicatore.prodotti_combinazione.clear();
	for (int i = 0; i < (int)stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size(); i++)
	{
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].tipologia&Carta_equipaggiamento::COMBINABILE)
		{
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].speciale(stato, {0}) == 1)
			{
				almeno_una_fattibile = true;
			}
				
		}	
	}
	if (almeno_una_fattibile == true)
	{
		return true;
	}
	return false;
}
int combina_esegui(StatoGioco& stato, ALLEGRO_EVENT& event)
{
	vector<int> elenco_scelte;
	switch(stato.indicatore.stato_combina)
	{
	case 0: //scelta combinazione
		for (size_t i = 0; i < stato.indicatore.prodotti_combinazione.size(); i++)
			elenco_scelte.push_back(1);
		stato.Cursori["Scelta azione combina"]["Lista"];
		input_tastiera_9_direzioni(stato.Cursori["Scelta azione combina"]["Lista"].iteratore, event, {(int)elenco_scelte.size(),0,0,0,0,0,0,0,0 }, true, false, false);
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				for (size_t i = 0; i < stato.indicatore.lista_combinabili[stato.Cursori["Scelta azione combina"]["Lista"].iteratore].size(); i++)
					for (size_t j = 0; j < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size(); j++)
						if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[j].nome ==
							stato.indicatore.lista_combinabili[stato.Cursori["Scelta azione combina"]["Lista"].iteratore][i].nome)
						{
							stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.erase
								(stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.begin() + j);
							break;
						}
				stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.push_back
					(stato.indicatore.prodotti_combinazione[stato.Cursori["Scelta azione combina"]["Lista"].iteratore]);
				stato.Cursori["Scelta azione combina"]["Lista"].iteratore = 0;
				stato.indicatore.stato_combina++;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.Cursori["Scelta azione combina"]["Lista"].iteratore = 0;
				return -1;
			}
		}
		break;
	case 1: //riorganizzazione equip
		if (modifica_equip(stato, event, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1) != 0)
		{
			stato.indicatore.stato_combina = 0;
			return -1;
		}
		break;
	}
	return 0;
}
void combina_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	int x = 100, y = 100;
	switch (stato.indicatore.stato_combina)
	{
	case 0://selezione combinazione
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_scaled_bitmap(immagini_e_fonts.menu, 0, 0, 840, 1080, 0, 0, 1920, 1080, 0);
		for (size_t i = 0; i < stato.indicatore.lista_combinabili.size(); i++,y=y+ 440 + 50)
		{
			if (stato.Cursori["Scelta azione combina"]["Lista"].iteratore == i)
				m_draw_bitmap(immagini_e_fonts.cursore, 0, y + (440/2), 0);
			for (size_t j = 0; j < stato.indicatore.lista_combinabili[i].size(); j++,x=x+320+50)
			{
				m_draw_scaled_bitmap(stato.indicatore.lista_combinabili[i][j].Immagine, 0,0, stato.indicatore.lista_combinabili[0][0].Immagine->width,
					stato.indicatore.lista_combinabili[0][0].Immagine->height,x, y, 320,
					440,0);
				if(j<stato.indicatore.lista_combinabili[i].size()-1)
					al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.falso_display, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
						al_map_rgba(0, 0, 255, 0),x + 320 + 5,y +(440/2) - 20,50 ,-1, 0, "+");
			}
			m_draw_scaled_bitmap(stato.indicatore.prodotti_combinazione[i].Immagine,0,0, stato.indicatore.prodotti_combinazione[i].Immagine->width,
				stato.indicatore.prodotti_combinazione[i].Immagine->height,x, y,320,440,0);
			al_draw_scaled_text(stato.font, immagini_e_fonts, immagini_e_fonts.falso_display, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
				al_map_rgba(0, 255, 0, 0),x - 45, y + (440/2) - 20, 50,-1, 0, "=");
			x = 100;
		}
		break;
	case 1://riorganizza equip
		scambia_da_solo_grafica(stato, immagini_e_fonts);
		break;
	}
}

bool medic_fattibile(StatoGioco& stato)
{
	bool forse_fattibile = false;
	for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].nome == "medic"&&
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].scarica == false)
		{
			forse_fattibile = true;
			break;
		}
	if (forse_fattibile == true)
	{
		for (int i = 0; i<stato.numero_giocatori; i++)
		{
			if(stato.giocatore[i].posizione==stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno]-1].posizione) //compreso anche se stesso
				for (size_t j = 0; j < stato.giocatore[i].equipaggiamento.size(); j++)
					if (stato.giocatore[i].equipaggiamento[j].nome == "Wounded")
						return true;
		}
	}
	return false;
}
int medic_esegui(StatoGioco& stato, ALLEGRO_EVENT& event)
{
	vector<int> giocatori_da_curare;
	for (int i = 0; i < stato.numero_giocatori; i++)
		if (stato.giocatore[i].posizione == stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione)
			for (size_t j = 0; j < stato.giocatore[i].equipaggiamento.size(); j++)
				if(stato.giocatore[i].equipaggiamento[j].nome=="Wounded")
					giocatori_da_curare.push_back(i);
	vector<int> input_tastiera;
	switch (stato.indicatore.stato_azione_medic)
	{
	case 0://scelta personaggio da curare
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_UP)
			{
				if (stato.Cursori["Azione Medic"]["Lista malati"].iteratore > 1)
					stato.Cursori["Azione Medic"]["Lista malati"].iteratore = stato.Cursori["Azione Medic"]["Lista malati"].iteratore - 2;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
			{

				if (stato.Cursori["Azione Medic"]["Lista malati"].iteratore+2 < (int)giocatori_da_curare.size())
					stato.Cursori["Azione Medic"]["Lista malati"].iteratore = stato.Cursori["Azione Medic"]["Lista malati"].iteratore + 2;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
			{
				if (stato.Cursori["Azione Medic"]["Lista malati"].iteratore % 2 != 0)
					stato.Cursori["Azione Medic"]["Lista malati"].iteratore--;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
			{
				if (stato.Cursori["Azione Medic"]["Lista malati"].iteratore % 2 == 0&&
					stato.Cursori["Azione Medic"]["Lista malati"].iteratore+1<(int)giocatori_da_curare.size())
					stato.Cursori["Azione Medic"]["Lista malati"].iteratore++;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				for (size_t i = 0; i < stato.giocatore[giocatori_da_curare[stato.Cursori["Azione Medic"]["Lista malati"].iteratore]].equipaggiamento.size(); i++)
					if (stato.giocatore[giocatori_da_curare[stato.Cursori["Azione Medic"]["Lista malati"].iteratore]].equipaggiamento[i].nome == "Wounded")
						stato.giocatore[giocatori_da_curare[stato.Cursori["Azione Medic"]["Lista malati"].iteratore]].equipaggiamento.erase
							(stato.giocatore[giocatori_da_curare[stato.Cursori["Azione Medic"]["Lista malati"].iteratore]].equipaggiamento.begin() + i);
				if (stato.giocatore[giocatori_da_curare[stato.Cursori["Azione Medic"]["Lista malati"].iteratore]].equipaggiamento.size()>2)
				{
					stato.indicatore.giocatore_curato = giocatori_da_curare[stato.Cursori["Azione Medic"]["Lista malati"].iteratore];
					stato.Cursori["Azione Medic"]["Lista malati"].iteratore = 0;
					stato.indicatore.stato_azione_medic++;
					//accrocco per far funzionare riorganizza
					stato.giocatore[6].equipaggiamento.clear();
					stato.indicatore.stato_menu_scambia_e_organizza_grafica = 1;
					stato.indicatore.sub_stato_scambia_organizza_grafica = 1;
				}
				else
				{
					stato.Cursori["Azione Medic"]["Lista malati"].iteratore = 0;
					return 1;
				}
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.Cursori["Azione Medic"]["Lista malati"].iteratore = 0;
				return -1;
			}
		}
		break;
	case 1: //ri-organizza inventario
		if (modifica_equip(stato, event, stato.indicatore.giocatore_curato) != 0)
		{
			stato.indicatore.stato_azione_medic = 0;
			return 1;
		}
		break;
	}
	return 0;
}
void medic_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	vector<int> giocatori_da_curare;
	for (int i = 0; i < stato.numero_giocatori; i++)
		if (stato.giocatore[i].posizione == stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione)
			for (size_t j = 0; j < stato.giocatore[i].equipaggiamento.size(); j++)
				if (stato.giocatore[i].equipaggiamento[j].nome == "Wounded")
					giocatori_da_curare.push_back(i);
	switch (stato.indicatore.stato_azione_medic)
	{
	case 0://scelta personaggio da curare
		draw_schede_personaggi_con_o_senza_selezione(stato, immagini_e_fonts, giocatori_da_curare, 3, 0, 0, true, stato.Cursori["Azione Medic"]["Lista malati"].iteratore);
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_scaled_bitmap(immagini_e_fonts.health_sfondo, 0, 0, immagini_e_fonts.health_sfondo->width, immagini_e_fonts.health_sfondo->height,
			0, 0, 1920, 1080, 0);
		m_draw_scaled_bitmap(immagini_e_fonts.falso_display_2, 0, 0, immagini_e_fonts.falso_display_2->width, immagini_e_fonts.falso_display_2->height,
			0, 0, immagini_e_fonts.falso_display_2->width, immagini_e_fonts.falso_display_2->height, 0);
		break;
	case 1://ri-organizza inventario
		MATTEO_BITMAP::set_target_bitmap(stato.giocatore[6].Scheda_personaggio_con_equip);
		al_clear_to_color(al_map_rgba(0, 0, 0, 0));
		gestione_equip_grafica(stato, immagini_e_fonts, stato.indicatore.giocatore_curato);
		break;
	}
}

bool born_leader_fattibile(StatoGioco& stato)
{
	for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].nome == "born_leader"&&
			stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].scarica == false &&
			stato.numero_giocatori>1)//altrimenti a chi la da?
			return true;
	return false;
}
int born_leader_esegui(StatoGioco& stato, ALLEGRO_EVENT& event)
{
	vector<int> giocatori_target;
	for (int i = 0; i < stato.numero_giocatori; i++)
		if(i!=stato.ordine_giocatori[stato.indicatore.giocatore_di_turno]-1)
			giocatori_target.push_back(i);
	switch (stato.indicatore.stato_azione_born_leader)
	{
	case 0: //scelta giocatore
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_UP)
			{
				if (stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore > 1)
					stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore = stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore - 2;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
			{

				if (stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore + 2 < (int)giocatori_target.size())
					stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore = stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore + 2;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
			{
				if (stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore % 2 != 0)
					stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore--;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
			{
				if (stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore % 2 == 0 &&
					stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore + 1 < (int)giocatori_target.size())
					stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore++;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				stato.indicatore.stato_azione_born_leader++;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore = 0;
				stato.Cursori["Azione born leader"]["Lista azioni"].iteratore = 0;
				return -1;
			}
		}
		break;
	case 1://scelta azione gratuita da assegnare
		input_tastiera_classico(stato.Cursori["Azione born leader"]["Lista azioni"].iteratore, event, { (int)stato.elenco_azioni_originale.size() }, false, true, false, false);
		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
			{
				stato.indicatore.azione_gratuita_assegnata_dal_born_leader = stato.elenco_azioni_originale[stato.Cursori["Azione born leader"]["Lista azioni"].iteratore].nome;
				for (size_t i = 0; i < stato.elenco_abilità.size(); i++)
					if (stato.elenco_abilità[i].nome == "born_leader_mark")
						stato.giocatore[giocatori_target[stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore]].abilità.push_back
							(stato.elenco_abilità[i]);
				stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore = 0;
				stato.Cursori["Azione born leader"]["Lista azioni"].iteratore = 0;
				return 1;
			}
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				stato.indicatore.stato_azione_born_leader--;
			}
		}
		break;
	}
	return 0;
}
void born_leader_grafica(StatoGioco& stato, Immagini& immagini_e_fonts)
{
	vector<int> giocatori_target;
	for (int i = 0; i < stato.numero_giocatori; i++)
		if (i != stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
			giocatori_target.push_back(i);
	vector<string> elenco_azioni;
	for (size_t i = 0; i < stato.elenco_azioni_originale.size(); i++)
		elenco_azioni.push_back(stato.elenco_azioni_originale[i].nome);

	draw_schede_personaggi_con_o_senza_selezione(stato, immagini_e_fonts, giocatori_target, 3, 0, 0, true, stato.Cursori["Azione born leader"]["Lista giocatori"].iteratore);
	MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
	m_draw_scaled_bitmap(immagini_e_fonts.scacchi_sfondo, 0, 0, immagini_e_fonts.scacchi_sfondo->width, immagini_e_fonts.scacchi_sfondo->height,
		0, 0, 1920, 1080, 0);//sfondo
	m_draw_scaled_bitmap(immagini_e_fonts.falso_display_2, 0, 0, immagini_e_fonts.falso_display_2->width, immagini_e_fonts.falso_display_2->height,
		0, 0, immagini_e_fonts.falso_display_2->width, immagini_e_fonts.falso_display_2->height, 0); //giocatori
	ALLEGRO_COLOR tinta_cornice=al_map_rgb(0,0,0);
	switch (stato.indicatore.stato_azione_born_leader)
	{
	case 0:
		al_draw_rectangle(0, 0, 1280, 1080, al_map_rgba(160, 80, 80, 0), 8);
		stato.Cursori["Azione born leader"]["Lista azioni"].Immagine_cursore = immagini_e_fonts.cursore_vuoto;
		tinta_cornice = al_map_rgba(0, 0, 0, 0);
		break;
	case 1:
		stato.Cursori["Azione born leader"]["Lista azioni"].Immagine_cursore = immagini_e_fonts.cursore;
		tinta_cornice = al_map_rgba(160, 80, 80, 0);
		break;
	}
	draw_finestra_con_elenco_e_cursore(stato, immagini_e_fonts, stato.font[stato.Cursori["Menu opzioni"]["Fonts"].iteratore].nome,
		al_map_rgb(255,255,255), stato.Cursori["Azione born leader"]["Lista azioni"], elenco_azioni,1530, 100,0,0,1,300, 350,
		40, 70, 56, immagini_e_fonts.menu,true,al_map_rgba(120, 60, 60, 170), tinta_cornice,8); //draw finestra elenco
}