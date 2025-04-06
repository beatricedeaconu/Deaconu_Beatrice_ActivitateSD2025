#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct StructuraMasina {
	int id;
	int nrUsi;
	float pret;
	char* model;
	char* numeSofer;
	unsigned char serie;
};
typedef struct StructuraMasina Masina;

struct Nod {
	Masina masina;
	struct Nod* next;
};
typedef struct Nod Nod;

struct HashTable {
	int dim;
	Nod** tabela; //am un vector de elemente de tipul Nod*
};
typedef struct HashTable HashTable;

Masina citireMasinaDinFisier(FILE* file)
{
	Masina m;
	char buffer[100];
	fgets(buffer, 100, file);
	char sep[4] = ",\n";

	m.id = atoi(strtok(buffer, sep));
	m.nrUsi = atoi(strtok(NULL, sep));
	m.pret = atof(strtok(NULL, sep));

	char* aux = strtok(NULL, sep);
	m.model = (char*)malloc(strlen(aux) + 1);
	strcpy(m.model, aux);

	aux = strtok(NULL, sep);
	m.numeSofer = (char*)malloc(strlen(aux) + 1);
	strcpy(m.numeSofer, aux);

	m.serie = *strtok(NULL, sep);

	return m;
}

void afisareMasina(Masina masina)
{
	printf("Id: %d\n", masina.id);
	printf("Nr. usi: %d\n", masina.nrUsi);
	printf("Pret: %.2f\n", masina.pret);
	printf("Model: %s\n", masina.model);
	printf("Nume sofer: %s\n", masina.numeSofer);
	printf("Serie: %c\n\n", masina.serie);
}

void afisareListaMasini(Nod* cap)
{
	while (cap)
	{
		afisareMasina(cap->masina);
		cap = cap->next;
	}
}

void adaugareMasinaInLista(Nod* cap, Masina masinaNoua)
{
	Nod* p = cap;
	while (p->next)
	{
		p = p->next;
	}
	Nod* nou = (Nod*)malloc(sizeof(Nod));
	nou->masina = masinaNoua; //shallow cpoy
	nou->next = NULL;
	p->next = nou;
}

HashTable initializareHashTable(int dimensiune)
{
	HashTable ht;
	ht.dim = dimensiune;
	ht.tabela = (Nod**)malloc(dimensiune * sizeof(Nod*));
	for (int i = 0; i < dimensiune; i++)
	{
		ht.tabela[i] = NULL;
	}

	return ht;
}

int calculeazaHash(const char* numeSofer, int dimensiune)
{
	int suma = 0;
	for (int i = 0; i < strlen(numeSofer) + 1; i++)
	{
		suma += numeSofer[i];
	}
	return suma % dimensiune;
}

void inserareMasinaInTabela(HashTable hash, Masina masina)
{
	int pozitie = calculeazaHash(masina.numeSofer, hash.dim);
	if (hash.tabela[pozitie] == NULL)
	{
		hash.tabela[pozitie] = (Nod*)malloc(sizeof(Nod));
		hash.tabela[pozitie]->masina = masina;
		hash.tabela[pozitie]->next = NULL;
	}
	else
	{
		adaugareMasinaInLista(hash.tabela[pozitie], masina);
	}

}

HashTable citireMasiniDinFisier(const char* numeFisier, int dimensiune)
{
	FILE* f = fopen(numeFisier, "r");
	HashTable hash = initializareHashTable(dimensiune);
	while (!feof(f))
	{
		Masina masina = citireMasinaDinFisier(f);
		inserareMasinaInTabela(hash, masina);
	}

	fclose(f);
	return hash;
}

void afisareTabelaDeMasini(HashTable ht)
{
	for (int i = 0; i < ht.dim; i++)
	{
		if (ht.tabela[i] != NULL)
		{
			printf("Masinile de pe pozitia %d sunt:\n", i);
			afisareListaMasini(ht.tabela[i]);
		}
		else
		{
			printf("Pe pozitia %d nu avem masini.\n\n", i);
		}
	}
}

void dezalocareListaMasini(Nod** cap)
{
	Nod* p = (*cap);
	while (p)
	{
		Nod* temp = p;
		p = p->next;
		if (temp->masina.model != NULL)
		{
			free(temp->masina.model);
		}
		if (temp->masina.numeSofer != NULL)
		{
			free(temp->masina.numeSofer);
		}
		free(temp);
	}
	(*cap) = NULL;
}

void dezalocareTabelaDeMasini(HashTable* ht)
{
	for (int i = 0; i < ht->dim; i++)
	{
		dezalocareListaMasini(&ht->tabela[i]);
	}
	free(ht->tabela);
	ht->tabela = NULL;
	ht->dim = 0;
}

float calculeazaMedieLista(Nod* cap)
{
	float suma = 0;
	int nrElemente = 0;
	while (cap != NULL)
	{
		suma += cap->masina.pret;
		nrElemente++;
		cap = cap->next;
	}

	return (nrElemente > 0 ? (suma / nrElemente) : 0);
}

float* calculeazaPreturiMediiPerClustere(HashTable ht, int* nrClustere)
{
	float* preturi;
	*nrClustere = 0;

	for (int i = 0; i < ht.dim; i++)
	{
		if (ht.tabela[i] != NULL)
		{
			(*nrClustere)++;
		}
	}
	preturi = (float*)malloc(sizeof(float) * (*nrClustere));

	int contor = 0;
	for (int i = 0; i < ht.dim; i++)
	{
		if (ht.tabela[i] != NULL)
		{
			preturi[contor] = calculeazaMedieLista(ht.tabela[i]);
			contor++;
		}
	}
	return preturi;
}

Masina getMasinaDinLista(Nod* cap, const char* nume)
{
	Masina m;
	m.id = -1;
	while (cap && strcmp(cap->masina.numeSofer, nume) != 0)
	{
		cap = cap->next;
	}
	if (cap)
	{
		m = cap->masina;
		m.model = (char*)malloc(strlen(cap->masina.model) + 1);
		strcpy(m.model, cap->masina.model);
		m.numeSofer = (char*)malloc(strlen(cap->masina.numeSofer) + 1);
		strcpy(m.numeSofer, cap->masina.numeSofer);
	}
	return m;
}

Masina getMasinaDupaNumeSofer(HashTable ht, const char* numeCautat)
{
	Masina m;
	m.id = -1;
	int poz = calculeazaHash(numeCautat, ht.dim);
	if (poz >= 0 && poz < ht.dim)
	{
		return getMasinaDinLista(ht.tabela[poz], numeCautat);
	}
}

int main()
{
	HashTable ht = citireMasiniDinFisier("masini.txt", 7);
	afisareTabelaDeMasini(ht);

	int nrClustere = 0;
	float* preturi = calculeazaPreturiMediiPerClustere(ht, &nrClustere);
	printf("Preturi medii:\n");
	for (int i = 0; i < nrClustere; i++)
	{
		printf("%.2f ", preturi[i]);
	}
	printf("\n\n");

	Masina masina = getMasinaDupaNumeSofer(ht, "Gigel");
	printf("Masina lui Gigel:\n");
	if (masina.id != -1)
	{
		afisareMasina(masina);

		if (masina.model != NULL)
		{
			free(masina.model);
		}
		if (masina.numeSofer != NULL)
		{
			free(masina.numeSofer);
		}
	}
	else
	{
		printf("Masina nu a fost gasita. Soferul respectiv u are nicio masina.\n");
	}

	dezalocareTabelaDeMasini(&ht);
	afisareTabelaDeMasini(ht);
	return 0;
}