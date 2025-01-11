#include "Script gestione equip.h"
#include"dati.h"
#include"utilità.h"
#include "allegro5\allegro.h"
#include "allegro5\allegro_primitives.h"
#include "Matteo Draw.h"

//Public
int scambia(StatoGioco&stato,ALLEGRO_EVENT event)
{
	int risultato;
	switch (stato.indicatore.stato_menu_scambia_e_organizza)
	{
	case 0://selezione personaggi
		risultato = Azione_gestisciEqup_selezione_personaggi(stato, event);
		switch (risultato)
		{
		case -1://escape
			stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore=0;
			return -1;
		case 1://enter
			stato.indicatore.stato_menu_scambia_e_organizza++;
			break;
		}
		break;
	case 1://seleziona equip giocatore di turno x scambio
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 0; //primo item
		risultato = Azione_gestisciEquip_selezione_equip(stato, event,"scambia","verde", stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
			stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore);
		switch (risultato)
		{
		case -1://escape
			crea_o_ripristina_backup_equipaggiamento(stato, "ripristina",
			{ stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1 ,
				stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore]});
			stato.indicatore.stato_menu_scambia_e_organizza = 0;
			break;
		case 1://enter
			stato.indicatore.stato_menu_scambia_e_organizza++;
			break;
		case 2://space
			stato.indicatore.stato_menu_scambia_e_organizza += 2;
			break;
		}
		break;
	case 2://seleziona equip altro giocatore x scambio
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 1; //secondo item
		risultato = Azione_gestisciEquip_selezione_equip(stato, event, "scambia","verde",
			 stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore],
			stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
		switch (risultato)
		{
		case -1://escape
			stato.indicatore.stato_menu_scambia_e_organizza = 1;
			break;
		case 1://enter
			Azione_gestisciEquip_scambioEquip(stato, event, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
				stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore],
				stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore,
				stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
			stato.indicatore.stato_menu_scambia_e_organizza--;
			break;
		case 2://space
			stato.indicatore.stato_menu_scambia_e_organizza++;
			break;
		}
		break;
	case 3://seleziona equip giocatore di turno x organizza
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 0; //primo item
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size() > 0)
		{
			risultato = Azione_gestisciEquip_selezione_equip(stato, event, "organizza","verde", stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
				stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore);
			switch (risultato)
			{
			case -1://escape
				crea_o_ripristina_backup_equipaggiamento(stato, "ripristina",
				{ stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1 ,
					stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore]});
				stato.indicatore.stato_menu_scambia_e_organizza = 1;
				break;
			case 1://enter
				stato.indicatore.stato_menu_scambia_e_organizza++;
				break;
			case 2://space
				stato.indicatore.stato_menu_scambia_e_organizza = 0;
				stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore = 0;
				return 1;
			case 3://tab
				stato.indicatore.stato_menu_scambia_e_organizza += 2;
				break;
			}
		}
		else if(stato.giocatore[stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore]].equipaggiamento.size()>0)
			stato.indicatore.stato_menu_scambia_e_organizza+=2;
		break;
	case 4:
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 1; //Secondo item
		risultato = Azione_gestisciEquip_selezione_equip(stato, event, "organizza","verde", stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
			stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
		switch (risultato)
		{
		case -1://escape
			stato.indicatore.stato_menu_scambia_e_organizza--;
			break;
		case 1://enter
			stato.indicatore.stato_menu_scambia_e_organizza--;
			Azione_gestisciEquip_ordinamento(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1, 
				stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore,
				stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
			break;
		case 2://space
			stato.indicatore.stato_menu_scambia_e_organizza = 0;
			stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore = 0;
			return 1;
		case 3://tab
			stato.indicatore.stato_menu_scambia_e_organizza++;
			break;
		}
		break;
	case 5://seleziona equip altro giocatore x organizza
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 0; //primo item
		if (stato.giocatore[stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore]].equipaggiamento.size() > 0)
		{
			risultato = Azione_gestisciEquip_selezione_equip(stato, event, "organizza","verde",
				stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore],
				stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore);
			switch (risultato)
			{
			case -1://escape
				crea_o_ripristina_backup_equipaggiamento(stato, "ripristina",
				{ stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
					stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore]});
				stato.indicatore.stato_menu_scambia_e_organizza = 1;
				break;
			case 1://enter
				stato.indicatore.stato_menu_scambia_e_organizza++;
				break;
			case 2://space
				stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore = 0;
				stato.indicatore.stato_menu_scambia_e_organizza = 0;
				return 1;
			case 3://tab
				stato.indicatore.stato_menu_scambia_e_organizza = 3;
				break;
			}
		}
		else if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size() > 0)
			stato.indicatore.stato_menu_scambia_e_organizza-=2;
		break;
	case 6:
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 1; //secondo item
		risultato = Azione_gestisciEquip_selezione_equip(stato, event, "organizza","verde",
			stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore],
			stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
		switch (risultato)
		{
		case -1://escape
			stato.indicatore.stato_menu_scambia_e_organizza--;
			break;
		case 1://enter
			stato.indicatore.stato_menu_scambia_e_organizza--;
			Azione_gestisciEquip_ordinamento(stato, stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore],
				stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore, stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
			break;
		case 2://space
			stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore = 0;
			stato.indicatore.stato_menu_scambia_e_organizza = 0;
			return 1;
		case 3://tab
			stato.indicatore.stato_menu_scambia_e_organizza = 3;
			break;
		}
		break;
	}
	return 0;
}

int cerca(StatoGioco& stato, ALLEGRO_EVENT event)
{
	int risultato, carta_casuale,n;
	bool almeno_una_carta_non_aahh = false;
	switch (stato.indicatore.stato_menu_scambia_e_organizza)
	{
	case 0://pesco carta, controllo se aaahh e faccio backup
		stato.indicatore.cercabile = false;
		stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore = 0;
		stato.indicatore.disponibili_a_scambio.clear();
		stato.indicatore.disponibili_a_scambio.push_back(6);
		n = 1;
		for (int i = 0; i < (int)stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento.size(); i++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].equipaggiamento[i].tipologia&Carta_equipaggiamento::FLASHLIGHT)
				n++;
		stato.giocatore[6].equipaggiamento.clear();
		for (int i = 0; i < n; i++)
		{
			carta_casuale = rand() % stato.Mazzo_equipaggiamento.size();
			stato.giocatore[6].equipaggiamento.push_back(stato.Mazzo_equipaggiamento[carta_casuale]);
			stato.Mazzo_equipaggiamento.erase(stato.Mazzo_equipaggiamento.begin() + carta_casuale);
			if (stato.Mazzo_equipaggiamento.size() == 0)
			{
				stato.Mazzo_equipaggiamento = stato.Mazzo_equipaggiamento_scarti;
				stato.Mazzo_equipaggiamento_scarti.clear();
			}
		}
		for (size_t i = 0; i < stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità.size(); i++)
			if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].tipo == Abilità::MODIFICATORE_CERCA)
				stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].abilità[i].funzione
				(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1, i, Abilità::CODICE_ESEGUI);
		for (int i = 0; i < (int)stato.giocatore[6].equipaggiamento.size(); i++)
			if (stato.giocatore[6].equipaggiamento[i].nome == "Aaahh!!?")
			{
				stato.missione.dati[stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione].cercabile = 0;
				stato.missione.dati[stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione].ModificaValoreCarta_aaa(true);
				stato.missione.dati[stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione].SommaValoreWalker(1);
				stato.giocatore[6].equipaggiamento.erase(stato.giocatore[6].equipaggiamento.begin() + i);
				i--;
			}
			else
				almeno_una_carta_non_aahh = true;
		if (almeno_una_carta_non_aahh == false)
			return 1;
		crea_o_ripristina_backup_equipaggiamento(stato, "crea",
		{stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,6});
		stato.indicatore.stato_menu_scambia_e_organizza++;
		break;
	case 1: //selezione equip carta trovata
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 0; //primo item
		risultato = Azione_gestisciEquip_selezione_equip(stato, event, "scambia", "verde", stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
			stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore);
		switch (risultato)
		{
		case -1: //escape
			crea_o_ripristina_backup_equipaggiamento(stato, "ripristina",
			{ stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,6});
			stato.indicatore.stato_menu_scambia_e_organizza = 0;
			return 1;
		case 1://enter
			stato.indicatore.stato_menu_scambia_e_organizza++;
			break;
		case 2://space
			stato.indicatore.stato_menu_scambia_e_organizza += 2;
			break;
		}
		break;
	case 2: //selezione equip giocatore
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 1; //Secondo item
		risultato = Azione_gestisciEquip_selezione_equip(stato, event, "scambia", "verde", 6,
			stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
		switch (risultato)
		{
		case -1://escape
			stato.indicatore.stato_menu_scambia_e_organizza--;
			break;
		case 1://enter
			stato.indicatore.stato_menu_scambia_e_organizza--;
			Azione_gestisciEquip_scambioEquip(stato, event, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,6,
				stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore,
				stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
			break;
		case 2://space
			stato.indicatore.stato_menu_scambia_e_organizza++;
			break;
		}
		break;
	case 3://organizza equip giocatore
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 0; //primo item
		risultato = Azione_gestisciEquip_selezione_equip(stato, event, "organizza", "verde", stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
			stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore);
		switch (risultato)
		{
		case -1://escape
			crea_o_ripristina_backup_equipaggiamento(stato, "ripristina",
			{ stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,6 });
			stato.indicatore.stato_menu_scambia_e_organizza -= 2;
			break;
		case 1://enter
			stato.indicatore.stato_menu_scambia_e_organizza++;
			break;
		case 2://space
			stato.indicatore.stato_menu_scambia_e_organizza = 0;
			return 1; 
		}
		break;
	case 4:
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 1; //secondo item
		risultato= Azione_gestisciEquip_selezione_equip(stato, event, "organizza", "verde", stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
			stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
		switch (risultato)
		{
		case -1://escape
			stato.indicatore.stato_menu_scambia_e_organizza--;
			break;
		case 1://enter
			stato.indicatore.stato_menu_scambia_e_organizza--;
			Azione_gestisciEquip_ordinamento(stato, stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1,
				stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore,
				stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
			break;
		case 2://space
			for (int i = 0; i < (int)stato.giocatore[6].equipaggiamento.size(); i++)
				stato.Mazzo_equipaggiamento_scarti.push_back(stato.giocatore[6].equipaggiamento[i]);
			stato.giocatore[6].equipaggiamento.clear();
			stato.indicatore.stato_menu_scambia_e_organizza = 0;
			return 1;
		}
	}
	return 0;
}

int modifica_equip(StatoGioco&stato, ALLEGRO_EVENT event,int indice_giocatore)
{
	int risultato;
	switch (stato.indicatore.stato_menu_scambia_e_organizza)
	{
	case 0://backup e preparo l'accrocco
		crea_o_ripristina_backup_equipaggiamento(stato, "crea",
		{ indice_giocatore });
		stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore = 0;
		stato.indicatore.disponibili_a_scambio.clear();
		stato.indicatore.disponibili_a_scambio.push_back(6);
		stato.indicatore.stato_menu_scambia_e_organizza++;
		break;
	case 1://organizza equip primo oggetto
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 0; //primo item
		risultato = Azione_gestisciEquip_selezione_equip(stato, event, "organizza", "verde", indice_giocatore,
			stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore);
		switch (risultato)
		{
		case -1://escape
			crea_o_ripristina_backup_equipaggiamento(stato, "ripristina",
			{ indice_giocatore });
			stato.indicatore.stato_menu_scambia_e_organizza = 0;
			return -1;
		case 1://enter
			stato.indicatore.stato_menu_scambia_e_organizza++;
			break;
		case 2://space
			stato.indicatore.stato_menu_scambia_e_organizza = 0;
			return 1;
		}
		break;
	case 2://organizza equip secondo oggetto
		stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore = 1; //secondo item
		risultato= Azione_gestisciEquip_selezione_equip(stato, event, "organizza", "verde", indice_giocatore,
			stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
		switch (risultato)
		{
		case -1://escape
			stato.indicatore.stato_menu_scambia_e_organizza --;
			break;
		case 1://enter
			Azione_gestisciEquip_ordinamento(stato, indice_giocatore,
				stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore,
				stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
			stato.indicatore.stato_menu_scambia_e_organizza--;
			break;
		case 2://space
			stato.indicatore.stato_menu_scambia_e_organizza = 0;
			return 1;
		}
		break;
	}
	return 0;
}
void gestione_equip_grafica(StatoGioco& stato,Immagini& immagini_e_fonts,int giocatore_1)
{
	int x = 0;
	int y = 0;
	int iteratore = 0;
	switch (stato.indicatore.stato_menu_scambia_e_organizza_grafica)
	{
	case 0:	//scegli personaggio con cui scambiare
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_bitmap(immagini_e_fonts.Arsenal, 0, 0, 0);
		m_draw_scaled_bitmap(stato.giocatore[giocatore_1].Scheda_personaggio_con_equip,
			0, 0, stato.giocatore[giocatore_1].Scheda_personaggio_con_equip->width,
			stato.giocatore[giocatore_1].Scheda_personaggio_con_equip->height,
			(1920 / 2) - (640 / 2) + 50, 30, 500, 285, 0);
		for (int i = 1; i < 4 && i < (int)stato.indicatore.disponibili_a_scambio.size(); i = i + 2, x = x + 366 - 20)
			m_draw_scaled_bitmap(stato.giocatore[stato.indicatore.disponibili_a_scambio[i]].Scheda_personaggio_con_equip,
				0, 0, stato.giocatore[stato.indicatore.disponibili_a_scambio[i]].Scheda_personaggio_con_equip->width,
				stato.giocatore[stato.indicatore.disponibili_a_scambio[i]].Scheda_personaggio_con_equip->height, 50, 366 + x, 500, 285, 0);
		for (int i = 0; i < 5 && i < (int)stato.indicatore.disponibili_a_scambio.size(); i = i + 2, y = y + 326)
			m_draw_scaled_bitmap(stato.giocatore[stato.indicatore.disponibili_a_scambio[i]].Scheda_personaggio_con_equip,
				0, 0, stato.giocatore[stato.indicatore.disponibili_a_scambio[i]].Scheda_personaggio_con_equip->width,
				stato.giocatore[stato.indicatore.disponibili_a_scambio[i]].Scheda_personaggio_con_equip->height,
				1920 - 500 - 30, 60 + y, 500, 285, 0);
		x = 0;
		y = 0;
		for (int i = 1; i < 4 && i < (int)stato.indicatore.disponibili_a_scambio.size(); i = i + 2, x = x + 366 - 20)
			if (stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore == i)
				al_draw_filled_rectangle(50, 366 + x, 50 + 500, 366 + x + 285, al_map_rgba(0, 255, 0, 20));
		for (int i = 0; i < 5 && i < (int)stato.indicatore.disponibili_a_scambio.size(); i = i + 2, y = y + 326)
			if (stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore == i)
				al_draw_filled_rectangle(1920 - 500 - 30, 60 + y, 1920 - 30, 60 + y + 285, al_map_rgba(0, 255, 0, 20));
		break;
	case 1:	//scorri tra i 2 personaggi e i loro equip
		ALLEGRO_COLOR colore;
		if (stato.indicatore.selezione_blu_verde == false)
			colore = al_map_rgba(0, 255, 0, 0.2);
		else
			colore = al_map_rgba(0, 0, 255, 0.2);

		ALLEGRO_COLOR verde = al_map_rgba(0, 255, 0, 0.2);
		ALLEGRO_COLOR blu = al_map_rgba(0, 0, 255, 0.2);
		switch (stato.indicatore.sub_stato_scambia_organizza_grafica)
		{
		case 0: //scambia
			switch (stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore)
			{
			case 0: //primo oggetto da selezionare
				draw_cursore_su_equip_personaggio(stato, immagini_e_fonts, stato.indicatore.scheda_personaggio_posizionato,
					5, verde, stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore);
				break;
			case 1://secondo oggetto da selezionare
				draw_cursore_su_equip_personaggio(stato, immagini_e_fonts, giocatore_1,
					5, blu, stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore);
				draw_cursore_su_equip_personaggio(stato, immagini_e_fonts, stato.indicatore.scheda_personaggio_posizionato,
					5, verde, stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
				break;
			}
			break;
		case 1: //organizza
			switch (stato.Cursori["Azione gestisci equip, selezione equip"]["Lista"].iteratore)
			{
			case 0: //primo oggetto da selezionare
				draw_cursore_su_equip_personaggio(stato, immagini_e_fonts, stato.indicatore.scheda_personaggio_posizionato,
					5, verde, stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore);
				break;
			case 1://secondo oggetto da selezionare
				draw_cursore_su_equip_personaggio(stato, immagini_e_fonts, stato.indicatore.scheda_personaggio_posizionato,
					5, blu, stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore);
				draw_cursore_su_equip_personaggio(stato, immagini_e_fonts, stato.indicatore.scheda_personaggio_posizionato,
					5, verde, stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore);
				break;
			}
			break;
		}
		MATTEO_BITMAP::set_target_bitmap(immagini_e_fonts.falso_display);
		m_draw_bitmap(immagini_e_fonts.Scambia_menu, 0, 0, 0);
		m_draw_scaled_bitmap(stato.giocatore[giocatore_1].Scheda_personaggio_con_equip,
			0, 0, stato.giocatore[giocatore_1].Scheda_personaggio_con_equip->width,
			stato.giocatore[giocatore_1].Scheda_personaggio_con_equip->height,
			0, 0, 960, 513, 0);
		m_draw_scaled_bitmap(stato.giocatore[stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore]].Scheda_personaggio_con_equip,
			0, 0, stato.giocatore[stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore]].Scheda_personaggio_con_equip->width,
			stato.giocatore[stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore]].Scheda_personaggio_con_equip->height,
			960, 0, 960, 513, 0);
		switch (stato.indicatore.sub_stato_scambia_organizza_grafica)
		{
		case 0: //scambia
			al_draw_filled_rectangle(25, 975, 625, 980, al_map_rgba(0, 0, 150, 0.2));
			break;
		case 1: //organizza
			al_draw_filled_rectangle(1350, 975, 1875, 980, al_map_rgba(0, 0, 150, 0.2));
			break;
		}
		break;
	}
}
//Private
int Azione_gestisciEqup_selezione_personaggi(StatoGioco& stato,ALLEGRO_EVENT event)
{
	stato.indicatore.stato_menu_scambia_e_organizza_grafica = 0;
	stato.indicatore.disponibili_a_scambio.clear();
	for (int i = 0; i < stato.numero_giocatori; i++)
		if (stato.giocatore[stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1].posizione == stato.giocatore[stato.ordine_giocatori[i] - 1].posizione
			&& stato.ordine_giocatori[i] - 1 != stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1)
			stato.indicatore.disponibili_a_scambio.push_back(stato.ordine_giocatori[i] - 1);
	vector<int> input;
	for (int i = 0; i < 9; i++)
		input.push_back(0);
	int contatore[5] = { 2,3,5,6,7 };
	int indice_contatore = 0;
	for (size_t i = 0; i < stato.indicatore.disponibili_a_scambio.size(); i++)
	{
		input[contatore[indice_contatore]] = 1;
		indice_contatore++;
	}
	stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"];
	input_tastiera_9_direzioni(stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore, event,input, false, false, false);
	if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
		{
			crea_o_ripristina_backup_equipaggiamento(stato, "crea", { stato.ordine_giocatori[stato.indicatore.giocatore_di_turno] - 1 ,
				stato.indicatore.disponibili_a_scambio[stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore]});
			return 1;
		}
		if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		{
			stato.Cursori["Azione gestisci equip, selezione personaggi"]["Lista"].iteratore = 0;
			stato.indicatore.scheda_personaggio_posizionato = 0;
			return -1;
		}
	}
	return 0;
}

int Azione_gestisciEquip_selezione_equip(StatoGioco&stato, ALLEGRO_EVENT event, string scambia_organizza,
	string selezione_blu_verde, int giocatore,int& indice_equip_arma_selezionata)
{
	stato.indicatore.scheda_personaggio_posizionato = giocatore;
	stato.indicatore.stato_menu_scambia_e_organizza_grafica = 1;
	if (selezione_blu_verde == "blu")
		stato.indicatore.selezione_blu_verde = true;
	else if(selezione_blu_verde=="verde")
		stato.indicatore.selezione_blu_verde = false;
	Carta_equipaggiamento ferita;
	cerca_carta_in_un_mazzo(ferita, "Wounded", stato.Mazzo_speciale, false);
	if (scambia_organizza == "scambia")
	{
		stato.indicatore.sub_stato_scambia_organizza_grafica = 0;
		input_tastiera_9_direzioni(indice_equip_arma_selezionata, event, { 0,1,1,0,0,0,1,2 }, false, false, true);
	}
	else if (scambia_organizza == "organizza")
	{
		stato.indicatore.sub_stato_scambia_organizza_grafica = 1;
		int r1 = 0, r2 = 0;
		if (stato.giocatore[giocatore].equipaggiamento.size() > 2)
		{
			r1 = 2;	r2 = stato.giocatore[giocatore].equipaggiamento.size() - r1;
		}
		else
			r1 = stato.giocatore[giocatore].equipaggiamento.size();
		input_tastiera_9_direzioni(indice_equip_arma_selezionata, event, { r1,0,0,0,0,0,0,0,r2 }, false, false, true);
	}
	if (event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
		{
			if(scambia_organizza=="scambia")
				if (indice_equip_arma_selezionata < (int)stato.giocatore[giocatore].equipaggiamento.size())
					if (stato.giocatore[giocatore].equipaggiamento[indice_equip_arma_selezionata].nome == ferita.nome)
						return 0; //non si scambiano ferite
			return 1;
		}
		if (event.keyboard.keycode == ALLEGRO_KEY_SPACE)
		{
			stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore;
			stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore;
			return 2;
		}
		if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		{
			indice_equip_arma_selezionata = 0;
			return -1;
		}
		if (event.keyboard.keycode == ALLEGRO_KEY_TAB)
		{
			if (scambia_organizza == "organizza")
			{
				stato.Cursori["Azione gestisci equip, selezione equip"]["Primo oggetto"].iteratore = 0;
				stato.Cursori["Azione gestisci equip, selezione equip"]["Secondo oggetto"].iteratore = 0;
				return 3;
			}
		}

	}
	return 0;
}

void Azione_gestisciEquip_scambioEquip(StatoGioco&stato, ALLEGRO_EVENT event,int giocatore_1,int giocatore_2,int& indice_equip_giocatore_di_turno, int& indice_equip_altro_giocatore)
{
	if (indice_equip_giocatore_di_turno		// caso 1: scambia arma con arma
		< (int)stato.giocatore[giocatore_1].equipaggiamento.size()
		&& indice_equip_altro_giocatore <
		(int)stato.giocatore[giocatore_2].equipaggiamento.size())
	{
		stato.giocatore[giocatore_1].equipaggiamento.insert
			(stato.giocatore[giocatore_1].equipaggiamento.begin() +
				indice_equip_giocatore_di_turno,
				stato.giocatore[giocatore_2].equipaggiamento[indice_equip_altro_giocatore]);

		stato.giocatore[giocatore_2].equipaggiamento.insert
			(stato.giocatore[giocatore_2].equipaggiamento.begin() + indice_equip_altro_giocatore,
				stato.giocatore[giocatore_1].
				equipaggiamento[indice_equip_giocatore_di_turno + 1]);

		stato.giocatore[giocatore_1].equipaggiamento.erase
			(stato.giocatore[giocatore_1].equipaggiamento.begin()
				+ indice_equip_giocatore_di_turno + 1);

		stato.giocatore[giocatore_2].equipaggiamento.erase
			(stato.giocatore[giocatore_2].equipaggiamento.begin() + indice_equip_altro_giocatore + 1);
	}
	else
	{
		if (indice_equip_giocatore_di_turno		// caso 2: scambia arma con niente
			< (int)stato.giocatore[giocatore_1].equipaggiamento.size()
			&& indice_equip_altro_giocatore >= (int)stato.giocatore[giocatore_2].equipaggiamento.size())
		{
			stato.giocatore[giocatore_2].equipaggiamento.push_back
				(stato.giocatore[giocatore_1].equipaggiamento[indice_equip_giocatore_di_turno]);
			stato.giocatore[giocatore_1].equipaggiamento.erase
				(stato.giocatore[giocatore_1].equipaggiamento.begin() +
					indice_equip_giocatore_di_turno);
		}
		else
		{
			if (indice_equip_giocatore_di_turno		// caso 3: scambia niente con arma
				>= (int)stato.giocatore[giocatore_1].equipaggiamento.size()
				&& indice_equip_altro_giocatore <
				(int)stato.giocatore[giocatore_2].equipaggiamento.size())
			{
				stato.giocatore[giocatore_1].equipaggiamento.push_back
					(stato.giocatore[giocatore_2].equipaggiamento[indice_equip_altro_giocatore]);
				stato.giocatore[giocatore_2].equipaggiamento.erase
					(stato.giocatore[giocatore_2].equipaggiamento.begin() + indice_equip_altro_giocatore);
			}
		}
	}
	indice_equip_giocatore_di_turno = 0;
	indice_equip_altro_giocatore = 0;
}

void Azione_gestisciEquip_ordinamento(StatoGioco&stato,int personaggio,int& indice_primo_oggetto,int& indice_secondo_oggetto)
{
	Carta_equipaggiamento provvisoria;
	provvisoria = stato.giocatore[personaggio].
		equipaggiamento[indice_secondo_oggetto];

	stato.giocatore[personaggio].
		equipaggiamento[indice_secondo_oggetto] =
		stato.giocatore[personaggio].
		equipaggiamento[indice_primo_oggetto];

	stato.giocatore[personaggio].
		equipaggiamento[indice_primo_oggetto] = provvisoria;
	indice_primo_oggetto = 0;
	indice_secondo_oggetto = 0;
}
