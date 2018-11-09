#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <MLV/MLV_all.h>

#define TAILLE_X 1024
#define TAILLE_Y 512
#define INTERLIGNE 30
#define TX 16


typedef struct noeud{
	int val;
	int x;
	int y;
	MLV_Image *img;
	struct noeud *f1, *f2, *f3, *f4;
}Noeud, *Arbre;


void affiche_arbre(Arbre a){
    if(a->f1 != NULL)
    	affiche_arbre(a->f1);
    if(a->f2 != NULL)
    	affiche_arbre(a->f2);
    if(a->f3 != NULL)
    	affiche_arbre(a->f3);
    if(a->f4 != NULL)
    	affiche_arbre(a->f4);
    if(a->img != NULL)
        MLV_draw_image(a->img, a ->x, a->y)    ;
    return;
    
}

Arbre alloueNoeud(int val, int x, int y){
	Arbre noeud = (Arbre) malloc(sizeof(Noeud));
	if(NULL == noeud){
		fprintf(stderr,"Erreur allocation dynamique");
		exit(EXIT_FAILURE);
	}

	noeud->val = val;
	noeud->x = x;
	noeud->y = y;

	noeud->f1 = NULL;
	noeud->f2 = NULL;
	noeud->f3 = NULL;
	noeud->f4 = NULL;
	noeud->img = NULL;

	return noeud;

}

int monochrome(MLV_Image *img){
	int i, j;
	int largeur, longueur;
	int niveau_gris, rouge, bleu, vert;
	MLV_get_pixel_on_image(img, 0, 0, &rouge, &bleu, &vert, NULL);
	niveau_gris = rouge / 3 + bleu / 3 + vert / 3;
	MLV_get_image_size(img, &largeur, &longueur);
	for(i = 0; i < largeur; i++)
		for(j = 0; j < longueur; j++){
			MLV_get_pixel_on_image(img, i, j, &rouge, &bleu, &vert, NULL);
			if((rouge / 3 + bleu / 3 + vert / 3) != niveau_gris)
				return 0;
		}
	return 1;
}

Arbre construire_arbre(Arbre *a, MLV_Image *img, int x, int y, int decoupage){
	int largeur, longueur;
	int f1x, f1y;
	int f2x, f2y;
	int f3x, f3y;
	int f4x, f4y;
	assert(NULL != a);
	assert(NULL != img);
	MLV_get_image_size(img, &largeur, &longueur);
	f1x = x;
	f1y = y;
	f2x = x + largeur / 2;
	f2y = y;
	f3x = x;
	f3y = y + longueur / 2;
	f4x = x + largeur / 2;
	f4y = y + longueur / 2;
	
	if((largeur == 1 && longueur == 1 ) || (monochrome(img))) {
		*a = alloueNoeud(1, x, y);
		(*a) -> img = img;
	}
	if(largeur != 1 && longueur != 1 && (!monochrome(img))){
		*a = alloueNoeud(0, x, y);
		(*a)->f1 = construire_arbre(&((*a) -> f1), MLV_copy_partial_image(img, 0, 0, largeur / 2, longueur / 2), f1x, f1y, decoupage);
		(*a)->f2 = construire_arbre(&((*a) -> f2), MLV_copy_partial_image(img, largeur/2, 0, largeur / 2, longueur / 2), f2x, f2y, decoupage);
		(*a)->f3 = construire_arbre(&((*a) -> f3), MLV_copy_partial_image(img, 0, longueur/2, largeur / 2, longueur / 2),  f3x, f3y, decoupage);
		(*a)->f4 = construire_arbre(&((*a) -> f4), MLV_copy_partial_image(img, largeur/2, longueur/2, largeur / 2, longueur / 2), f4x, f4y, decoupage);
	}
	
	return *a;


}

void recuperer_adresse_noeud(Arbre *a, Arbre tab[], int niveau) {

    assert(NULL != a);
    int i;
    i = 0;
    if (0 == niveau)
    {
        while(tab[i] != 0)
            i++;

        tab[i] = *a;
        return ;
    }
    if (0 < niveau)
    {
        recuperer_adresse_noeud(&((*a) -> f1), tab, niveau -1) ;
        recuperer_adresse_noeud(&((*a) -> f2), tab, niveau -1) ;
        recuperer_adresse_noeud(&((*a) -> f3), tab, niveau -1) ;
        recuperer_adresse_noeud(&((*a) -> f4), tab, niveau -1) ;

    }

}

void afficher_tableau(Arbre tab[],int taille){
	int i;
	for (i = 0; i < taille; ++i)
	{
		if (NULL != tab[i])
		{
			printf("adress -> %d\n",tab[i]->y);	
		}
	}
}

void dessine_menu(){
	int cpt;
	int width1, length1;
	int width2, length2;
	int width3, length3;
	int width4, length4;
	int width5, length5;
	int x, y;
	cpt = 0;
	MLV_get_size_of_adapted_text_box("CHARGER UNE IMAGE", INTERLIGNE, &width1, &length1);
	MLV_draw_adapted_text_box(TAILLE_X/2 - width1/2, cpt++ * TAILLE_Y / 10, "CHARGER UNE IMAGE", INTERLIGNE, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
	MLV_get_size_of_adapted_text_box("CHARGER UN QUADTREE", INTERLIGNE, &width2, &length2);
	MLV_draw_adapted_text_box(TAILLE_X/2 - width2/2, cpt++ * TAILLE_Y / 10, "CHARGER UN QUADTREE", INTERLIGNE, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
	MLV_get_size_of_adapted_text_box("PUZZLE 16 PIECES", INTERLIGNE, &width3, &length3);
	MLV_draw_adapted_text_box(TAILLE_X/2 - width3/2, cpt++ * TAILLE_Y / 10, "PUZZLE 16 PIECES", INTERLIGNE, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
	MLV_get_size_of_adapted_text_box("PUZZLE 64 PIECES", INTERLIGNE, &width4, &length4);
	MLV_draw_adapted_text_box(TAILLE_X/2 - width4/2, cpt++ * TAILLE_Y / 10, "PUZZLE 64 PIECES", INTERLIGNE, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
	MLV_get_size_of_adapted_text_box("QUITTER", INTERLIGNE, &width5, &length5);
	MLV_draw_adapted_text_box(TAILLE_X/2 - width5/2, cpt++ * TAILLE_Y / 10, "QUITTER", INTERLIGNE, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);

	MLV_actualise_window();
	MLV_wait_mouse(&x,&y);
	if(x > TAILLE_X/2 - width1 / 2 && x < TAILLE_X / 2 + width1 / 2  && y > 0 && y < 2 * INTERLIGNE )
		printf("Charger une image\n"); 
	if(x > TAILLE_X/2 - width2 / 2 && x < TAILLE_X / 2 + width2 / 2 && y > TAILLE_Y / 10 && y < TAILLE_Y / 10 + 2 * INTERLIGNE)
		printf("Charger un quadTree\n");
	if(x > TAILLE_X/2 - width3 / 2 && x < TAILLE_X / 2 + width3 / 2 && y > TAILLE_Y / 5 && y < TAILLE_Y / 5 + 2 * INTERLIGNE)
		printf("Puzzle 16 pièces\n");
	if(x > TAILLE_X/2 - width4 / 2 && x < TAILLE_X / 2 + width4 / 2 && y > 3 * TAILLE_Y / 10 && y < 3 * TAILLE_Y / 10 + 2 * INTERLIGNE)
		printf("Puzzle 64 pièces\n");
	if(x > TAILLE_X/2 - width5 / 2 && x < TAILLE_X / 2 + width5 / 2 && y > 4 * TAILLE_Y / 10 && y < 4 * TAILLE_Y / 10 + 2 * INTERLIGNE)
		printf("Quitter \n");
	

}

MLV_Image *gestion_image(int pixel){
	int width, length;
	MLV_Image *image;
	int rouge, bleu, vert, nuance;
	MLV_Color couleur;
	int i, j;
	image = MLV_load_image("image.jpg");
	if(image != NULL){
		MLV_get_image_size(image, &width, &length);
		if(width < pixel || length < pixel)
			printf("Image trop petite \n");
		else if(width != pixel || length != pixel)
			MLV_resize_image(image, pixel, pixel);

		for(i = 0; i < pixel; i++){
			for(j = 0; j < pixel; j++){
				MLV_get_pixel_on_image(image, i, j, &rouge, &bleu, &vert, NULL);
				nuance = (rouge + bleu + vert) / 3;
				couleur = MLV_rgba(nuance, nuance, nuance, 255);
				MLV_set_pixel_on_image(i, j, couleur, image);
			}
		}

		//MLV_draw_image(image,TAILLE_X / 20, TAILLE_Y / 20);
		MLV_actualise_window();
	}
	return image;
}

void initialise_tableau(Arbre tab[], int x){
    int i;
    for (i = 0; i < x; ++i){
        tab[i] = 0;
        
    }

}


void melanger(Arbre* tab,int nb)
{
    int i,nb2;
    nb2 = nb;
    for(i=0;i<nb;i++)
    {
        Arbre tmp;
        int index = rand()%nb2;
        tmp = tab[index];
        tab[index] = tab[nb2-1];
        tab[nb-i-1] = tmp;
        nb2--;
    }
}

void reconstruire_arbre(Arbre *b, Arbre tab[], int taille, int *i, int niveau){
	assert(NULL != b);
    if (0 == niveau)
    {
    	printf("ok\n" );
    	//printf(" i->%d\n", *i);
        *b = tab[*i];
        *i = *i + 1;
        return ;
    }
    if (0 < niveau)
    {
        reconstruire_arbre(&((*b) -> f1), tab, taille, i, niveau -1) ;
        reconstruire_arbre(&((*b) -> f2), tab, taille, i, niveau -1) ;
        reconstruire_arbre(&((*b) -> f3), tab, taille, i, niveau -1) ;
        reconstruire_arbre(&((*b) -> f4), tab, taille, i, niveau -1) ;

    }
}

void afficher(Arbre a){
	
}

int main(int argc, char const *argv[])
{
	MLV_Image *image;
    Arbre a;
    Arbre b;
    Arbre tab[TX];
    int i = 0;
    
	MLV_create_window("quadTree", "quadTree", TAILLE_X, TAILLE_Y);
	image = gestion_image(256);
	initialise_tableau(tab,TX);
	dessine_menu();
    construire_arbre(&a, image, TAILLE_X / 20, TAILLE_Y / 20, 16);
    affiche_arbre(a);
    b = a;
    recuperer_adresse_noeud(&a, tab, 4);
   	afficher_tableau(tab,TX);
   	melanger(tab,TX);
   	printf("\n\n\n");
   	afficher_tableau(tab,TX);
   	reconstruire_arbre(&b, tab, TX, &i, 4);
   	printf("ok\n");
   	affiche_arbre(b);
    MLV_actualise_window();
    MLV_wait_seconds(2);
	MLV_free_image(image);
	MLV_free_window();
	return 0;
}

