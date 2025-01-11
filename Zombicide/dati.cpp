#include "dati.h"
#include "Matteo Draw.h"

vector<MATTEO_BITMAP*> MATTEO_BITMAP::immagini_da_distruggere;
vector<MATTEO_BITMAP*> MATTEO_BITMAP::elenco_oggetti_creati;
MATTEO_BITMAP* MATTEO_BITMAP::target_m_bitmap;

void MATTEO_BITMAP::add_to_immagini_da_distruggere()
{
	bool trovato = false;
	for (size_t i = 0; i < immagini_da_distruggere.size(); i++)
		if (this->immagine == immagini_da_distruggere[i]->immagine)
			if(this->width== immagini_da_distruggere[i]->width&&this->height== immagini_da_distruggere[i]->height&&
				this->percorso_file== immagini_da_distruggere[i]->percorso_file)
					trovato = true;
	if (trovato == false)
	{
		immagini_da_distruggere.push_back(this);
	}
}
void MATTEO_BITMAP::destroy()
{
	for (size_t i = 0; i < immagini_da_distruggere.size(); i++)
		if (this->immagine == immagini_da_distruggere[i]->immagine)
			if (this->width == immagini_da_distruggere[i]->width&&this->height == immagini_da_distruggere[i]->height&&
				this->percorso_file == immagini_da_distruggere[i]->percorso_file)
				{
					al_destroy_bitmap(this->immagine);
					immagini_da_distruggere.erase(immagini_da_distruggere.begin() + i);
					this->immagine = nullptr;
				}
}
void MATTEO_BITMAP::rigenerate_bitmap()
{
	if (immagine == nullptr)
	{
		if (carica_crea == MATTEO_BITMAP::CARICA)
		{
			immagine = al_load_bitmap(percorso_file.c_str());
			this->add_to_immagini_da_distruggere();
		}
		else if (carica_crea == MATTEO_BITMAP::CREA)
		{
			immagine = al_create_bitmap(width, height);
			this->add_to_immagini_da_distruggere();
		}
	}
}

bool MATTEO_BITMAP::set_target_bitmap(MATTEO_BITMAP* target)
{
	if(target->immagine==nullptr)
		target->rigenerate_bitmap();

	if (target->immagine != nullptr)
	{
		target_m_bitmap = target;
		al_set_target_bitmap(target->immagine);
		target->add_to_immagini_da_distruggere();
		return true;
	}
	return false;
}
MATTEO_BITMAP* MATTEO_BITMAP::get_target_bitmap()
{
	return target_m_bitmap;
}
MATTEO_BITMAP* MATTEO_BITMAP::create()
{
	MATTEO_BITMAP* o = new MATTEO_BITMAP;
	o->immagine = nullptr;
	elenco_oggetti_creati.push_back(o);
	return o;
}
void MATTEO_BITMAP::destroy_all_object()
{
	for (size_t i = 0; i < elenco_oggetti_creati.size(); i++)
	{
		elenco_oggetti_creati[i]->destroy();
		delete elenco_oggetti_creati[i];
	}
	elenco_oggetti_creati.clear();
}
void MATTEO_BITMAP::destroy_vector_m_bitmap()
{
	for (size_t i = 0; i < immagini_da_distruggere.size(); i++)
	{
		if (immagini_da_distruggere[i] != nullptr)
		{
			immagini_da_distruggere[i]->destroy();
			i--;
		}
	}
}

void MATTEO_BITMAP::init(int width, int height, int carica_crea, string percorso_file)
{
	if (immagine != nullptr)
	{
		destroy();
		immagine = nullptr;
	}
	this->carica_crea = carica_crea;
	if (carica_crea == CARICA)
	{
		this->percorso_file = percorso_file;
		immagine = al_load_bitmap(percorso_file.c_str());
		this->height = al_get_bitmap_height(immagine);
		this->width = al_get_bitmap_width(immagine);
		al_destroy_bitmap(immagine);
		immagine = nullptr;
	}
	else if (carica_crea == MATTEO_BITMAP::CREA)
	{
		this->width = width;
		this->height = height;
	}
}
/*void MATTEO_BITMAP::createSubVectorDrawLists(vector<vector<Draw_Madre*>>& V1, vector<vector<Draw_Madre*>>& V2)
{
	int dx1, dy1, dx_dim1, dy_dim1; //cordinate del primo draw da confrontare
	int dx2, dy2, dx_dim2, dy_dim2; //cordinate del secondo draw da confrontare
									//prima la vecchia lista di draw
	for (size_t i = 0; i < vecchia_lista_draw.size(); i++)
	{
		vecchia_lista_draw[i]->get_d_cordinate(dx1, dy1, dx_dim1, dy_dim1);
		vector<Draw_Madre*> subV;
		for (size_t j = 0; j < vecchia_lista_draw.size(); j++)
			if (i != j) //non bisogna inserire se stessi
			{
				vecchia_lista_draw[j]->get_d_cordinate(dx2, dy2, dx_dim2, dy_dim2);
				if (dx1 >= dx2 && dx1 <= dx_dim2)
				{
					if ((dy1 >= dy2 && dy1 <= dy_dim2) || (dy2 >= dy1 && dy2 <= dy_dim1))
						subV.push_back(vecchia_lista_draw[j]);
				}
				else if (dx2 >= dx1 && dx2 <= dx_dim1)
				{
					if ((dy1 >= dy2 && dy1 <= dy_dim2) || (dy2 >= dy1 && dy2 <= dy_dim1))
						subV.push_back(vecchia_lista_draw[j]);
				}	
			}
		V1.push_back(subV);
	}
	//poi la nuova lista di draw
	for (size_t i = 0; i < lista_draw.size(); i++)
	{
		lista_draw[i]->get_d_cordinate(dx1, dy1, dx_dim1, dy_dim1);
		vector<Draw_Madre*> subV;
		for (size_t j = 0; j < lista_draw.size(); j++)
			if (i != j) //non bisogna inserire se stessi
			{
				lista_draw[j]->get_d_cordinate(dx2, dy2, dx_dim2, dy_dim2);
				if (dx1 >= dx2 && dx1 <= dx_dim2)
				{
					if ((dy1 >= dy2 && dy1 <= dy_dim2) || (dy2 >= dy1 && dy2 <= dy_dim1))
						subV.push_back(lista_draw[j]);
				}
				else if (dx2 >= dx1 && dx2 <= dx_dim1)
				{
					if ((dy1 >= dy2 && dy1 <= dy_dim2) || (dy2 >= dy1 && dy2 <= dy_dim1))
						subV.push_back(lista_draw[j]);
				}
			}
		V2.push_back(subV);
	}
}
void MATTEO_BITMAP::createSubVectorOldDrawListCommonAndNotCommon(vector<Draw_Madre*>& in_comune, vector<Draw_Madre*>& non_in_comune)
{
	for (size_t i = 0; i < vecchia_lista_draw.size(); i++)
	{
		for (size_t n = 0; n<lista_draw.size(); n++)
			if (lista_draw[n]->operator==(vecchia_lista_draw[i]))
			{
				lista_draw[n]->drawable = false;
				in_comune.push_back(lista_draw[n]); //mi segno quelli in comune
			}
	}
	for (size_t i = 0; i < vecchia_lista_draw.size(); i++)
	{
		bool trovato = false;
		for (size_t j = 0; j < in_comune.size(); j++)
			if (vecchia_lista_draw[i]->operator==(in_comune[j]))
				trovato = true;
		if (trovato == false)
			non_in_comune.push_back(vecchia_lista_draw[i]);
	}
}

bool MATTEO_BITMAP::checkIntegrityDrwaList()
{
	//Il ragionamento è che un immagine per essere integra, deve mantenere la sua lista di draw.
	//Per controllarla per bene, devo dividere i draw in categorie in base alle connessioni con le cordinate
	//Se queste liste tra i vecchi e i nuovi draw sono diversi anche solo nell'ordine, non va bene.
	int contatore = 0;
	for (size_t i = 0; i < vecchia_lista_draw.size(); i++)
		for (size_t j = 0; j < lista_draw.size(); j++)
			if (vecchia_lista_draw[i]->operator==(lista_draw[j]))
			{
				contatore++;
				break;
			}
	if (contatore != vecchia_lista_draw.size() || vecchia_lista_draw.size() != lista_draw.size())
		return false;
	vector<vector<Draw_Madre*>> V1; //Vecchia lista draw
	vector<vector<Draw_Madre*>> V2; //Nuova lista draw
	createSubVectorDrawLists(V1, V2);
	if (V1.size() != V2.size())
		return false;
	for (size_t i = 0; i < V1.size(); i++)
	{
		if (V1[i].size() != V2[i].size())
			return false;
		for (size_t j = 0; j < V1[i].size(); j++)
			if (!V1[i][j]->operator==(V2[i][j]))
				return false;
	}
	return true;
}
void MATTEO_BITMAP::checkIfDrawableDrawInvalidOtherRecursive(int i,vector<vector<Draw_Madre*>>V2, vector<Draw_Madre*> in_comune)
{
	if (lista_draw[i]->drawable == true)
		for (size_t j = 0; j < V2[i].size(); j++) //controllo se tocca qualche
			for (size_t m = 0; m < in_comune.size(); m++) //draw che prima c'era e che c'è anche ora
				if (in_comune[m]->operator==(V2[i][j])) //se lo trovo
					for (size_t l = 0; l < lista_draw.size(); l++) //vedo in che posizione si trova nel nuovo vettore di draw
						if (lista_draw[l]->operator==(in_comune[m]))
							if (i < l) //e se si trova dopo quello nuovo, allora lo devo necessariamente ristampare
								if (lista_draw[l]->drawable == false)
								{
									lista_draw[l]->drawable = true;
									checkIfDrawableDrawInvalidOtherRecursive(l, V2, in_comune);
								}			
}
bool MATTEO_BITMAP::compare(MATTEO_BITMAP* altro)
{
	bool risultato = true;
	if (this->carica_crea == altro->carica_crea)
	{
		if (this->carica_crea == "carica" && this->percorso_file == altro->percorso_file)
			return true;
		if (this->carica_crea == "crea" && this->width == altro->width&&this->height == altro->height)
		{
			if(checkIntegrityDrwaList())
				return true;
		}	
	}
	return false;
}
void MATTEO_BITMAP::Draw()
{
	setDrawableList();
	for (size_t i = 0; i < vecchia_lista_draw.size(); i++)
		if (vecchia_lista_draw[i]->drawable == true)
			vecchia_lista_draw[i]->anti_draw(this);
	for (size_t i = 0; i < lista_draw.size(); i++)
		if (lista_draw[i]->drawable == true)
			lista_draw[i]->Draw(this);
}
void MATTEO_BITMAP::anti_draw()
{
	for (size_t i = 0; i < lista_draw.size(); i++)
		lista_draw[i]->anti_draw(this);
}
void MATTEO_BITMAP::AddDrawToList(Draw_Madre* d)
{
	lista_draw.push_back(d);
}
void MATTEO_BITMAP::StartNewDrawList()
{
	vecchia_lista_draw = lista_draw;
	lista_draw.clear();
}
void MATTEO_BITMAP::setDrawableList()
{
	//crea vettori per ogni draw presente nei vettori vecchi e nuovi, in cui sono presenti i draw con cui
	//si interseca il draw in questione (non presente nel vettore, lo capisci perchè mantiene gli indici del
	//vettore di draw di partenza)
	vector<vector<Draw_Madre*>> V1; //vecchi
	vector<vector<Draw_Madre*>> V2; //nuovi
	createSubVectorDrawLists(V1, V2);
	vector<Draw_Madre*> in_comune;
	vector<Draw_Madre*> non_in_comune;
	//Setto tutti i draw della nuova lista in true e poi metto in false solo quelli comuni con la vecchia lista
	for (size_t i = 0; i < lista_draw.size(); i++)
		lista_draw[i]->drawable = true;
	createSubVectorOldDrawListCommonAndNotCommon(in_comune, non_in_comune);
	for (size_t i = 0; i < vecchia_lista_draw.size(); i++)
		vecchia_lista_draw[i]->drawable = false;
	//Setto NELLA VECCHIA LISTA tutti i draw che non ci sono più e li metto "Drawable" nel senso che subiranno un anti-draw
	for (size_t i = 0; i < vecchia_lista_draw.size(); i++)
		for (size_t j = 0; j < non_in_comune.size(); j++)
			if (vecchia_lista_draw[i]->operator==(non_in_comune[j]))
				vecchia_lista_draw[i]->drawable = true;

	//Controllo 1:tutti i draw che non ci sono più corrompono le loro immagini associate se presenti anche nel nuovo vettore
	for (size_t i = 0; i<non_in_comune.size(); i++)
	{
		for (size_t n = 0; n<vecchia_lista_draw.size(); n++)
			if (vecchia_lista_draw[n]->operator==(non_in_comune[i]))
			{
				for (size_t j = 0; j < V1[n].size(); j++)
				{
					for (size_t m = 0; m < lista_draw.size(); m++)
						if (lista_draw[m]->operator==(V1[n][j]))
							lista_draw[m]->drawable = true;
				}
			}
	}
	//Controllo 2: Controllo se i Draw che verranno disegnati sono associati con draw in comune con il vecchio vettore
	        //a quel punto: se sono messi prima allora non importa (tanto ci disegna sopra), se al contrario devono
			//essere ridisegnati, perchè il nuovo draw deve stare sotto e quindi il suo disegnamento invalida il vecchio draw
	if (in_comune.size() != 0) //ovviamente se non ci sono draw in comune non mi preoccupo
		for (size_t i = 0; i < lista_draw.size(); i++)
			checkIfDrawableDrawInvalidOtherRecursive(i, V2, in_comune);
}*/

MATTEO_BITMAP* Cursore::immagine=MATTEO_BITMAP::create();
Cursore::Cursore() : iteratore(0), modificatore_scorrimento_elenco(0)
{
	if (immagine->immagine == nullptr)
		immagine->init(0, 0, MATTEO_BITMAP::CARICA, "Immagini/segnalini/cursore.png");

	Immagine_cursore = immagine;
}
int Cursore::mouse_scroll;

void Missione::ModificaStatoPorta(int valore, int indice_porta)
{
	if (porta[indice_porta].stato != valore)
		porta[indice_porta].stato = valore;
}
DatiStanza::DatiStanza()
{
	for (int i = 0; i < 7; i++)
		giocatori[i] = 0;;
	rumore = 0;;
	walker=0;
	runner = 0;
	fat = 0;
	abominio = false;
	polizia = 0;
	pimp_mobile = 0;
	obiettivi_rossi = 0;
	obiettivi_blu = 0;
	obiettivi_verdi = 0;
	spawn_rosso = 0;
	spawn_blu = 0;
	spawn_verde = 0;
	exit = false;
	carta_aaa = 0;
	cercabile = 0;
	tipo = 0; //0-strada, 1-stanza interna
}
void DatiStanza::SommaValoreRumore(int valore)
{
	if (rumore != rumore + valore)
		rumore = rumore + valore;
};
void DatiStanza::SommaValoreWalker(int valore)
{
	if (walker != walker + valore)
		walker = walker + valore;
};
void DatiStanza::SommaValoreRunner(int valore)
{
	if (runner != runner + valore)
		runner = runner + valore;
};
void DatiStanza::SommaValoreFat(int valore)
{
	if (fat != fat + valore)
		fat = fat + valore;
};
void DatiStanza::ModificaValoreAbominio(bool valore)
{
	if (abominio != valore)
		abominio = valore;
};
void DatiStanza::SommaValorePolizia(int valore)
{
	if (polizia != polizia + valore)
		polizia = polizia + valore;
	
};
void DatiStanza::SommaValorePimp_mobile(int valore)
{
	if (pimp_mobile != pimp_mobile + valore)
		pimp_mobile = pimp_mobile + valore;
};
void DatiStanza::SommaValoreObiettivo(int valore, int colore)
{
	switch (colore)
	{
	case rosso:
		if (obiettivi_rossi != obiettivi_rossi + valore)
			obiettivi_rossi = obiettivi_rossi + valore;
		break;
	case blu:
		if (obiettivi_blu != obiettivi_blu + valore)
			obiettivi_blu = obiettivi_blu + valore;
		break;
	case verde:
		if (obiettivi_verdi != obiettivi_verdi + valore)
			obiettivi_verdi = obiettivi_verdi + valore;
		break;
	}
};
void DatiStanza::SommaValoreSpawn(int valore,int colore)
{
	switch (colore)
	{
	case rosso:
		if (spawn_rosso != spawn_rosso + valore)
			spawn_rosso = spawn_rosso + valore;
		break;
	case blu:
		if (spawn_blu != spawn_blu + valore)
			spawn_blu = spawn_blu + valore;
		break;
	case verde:
		if (spawn_verde != spawn_verde + valore)
			spawn_verde = spawn_verde + valore;
		break;
	}
};
void DatiStanza::ModificaValoreExit(bool valore)
{
	if (exit != valore)
		exit = valore;
};
void DatiStanza::ModificaValoreCarta_aaa(bool valore)
{
	if (carta_aaa != valore)
		carta_aaa = valore;
};
void DatiStanza::ModificaValoreGiocatori(bool valore, int indice_giocatore)
{
	if (giocatori[indice_giocatore] != valore)
		giocatori[indice_giocatore] = valore;
}