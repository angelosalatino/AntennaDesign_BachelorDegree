/* Algoritmo per il calcolo del codice pdepoly, creato da Angelo
Antonio Salatino, mat. 541532, Ingegneria Informatica. Antenne e
Compatibilità Elettromagnetica.*/

#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<math.h>
#include<malloc.h>

#define FMAX 2400 //2,4G[Hz]
#define FMIN 800 //800M[Hz]
#define VE_LUCE 299792458 //[m/s]
#define PIGRECO 3.141592

struct elem{
       float altez;
       float tang;
       float tanplus;
       float tanminus;
       };


void stampa(float *, float *, int);
void inverti(float *, float *, float *, float *, int);
void stampa_a_schermo(float *, float *, int);
void stampa_su_file(float *, float *, int);
void carica_yvect(float *, struct elem *, int *, int);
void carica_xvect(float *, struct elem *, int *, int, bool);
void crea_sequenza(int *, int, int, bool);
int adatta(float *, float *, float *, float *,float *, float *, int);

main()
{     
      /*
      dichiarazione di variabili
      */
      double lambdamax, lambdamin;
      float tau, alfa, spessore;
      float h1, htot, htemp; // h1 è la minima altezza, htot e la massima altezza
      float prec_altezza;
      float sum;
      float tangente;
      int i,j,l,num;
      float *xvect,*yvect, *xvecti, *yvecti;
      int *sequ;
      struct elem *tempvect;
      bool second_mode=false;
      /*
      istruzioni di calcolo
      */      
      lambdamax = VE_LUCE/FMIN;
      lambdamax = lambdamax/1000000;
      lambdamin = VE_LUCE/FMAX;
      lambdamin = lambdamin/1000000;
      h1 = lambdamin/4;
      htot = lambdamax/4;
           printf("Inserisci valore di tau:\n");
           scanf("%f",&tau);
           printf("Inserisci valore dell'angolo di apertura:\n");
           scanf("%f",&alfa);
           printf("Inserisci lo spessore dell'antenna:\n");
           scanf("%f",&spessore);
      alfa = (alfa*PIGRECO)/180;
      tangente = tan(alfa); 
      spessore = spessore/2;
      sum = 0;
      i = 0;
      
      do{
          sum = sum + h1*pow(tau,i);
          i++;                 
      }while(sum<=htot);
      
      if(htot-(sum-h1*pow(tau,i))<=spessore){
                                            second_mode=true;
                                            printf("\nSecond_mode = %d",second_mode);
                                            i--;
                                            }
      num = i;
      printf("\nN = %d\n",num);
      // i è il numero dei nodi, j è il numero delle altezze
      if(second_mode){
                      i = (num*4) + 2;
                      j = (num*2) + 1;
                      }
                      else{
                           i = num*4;
                           j = num*2;
                           }  
      xvect = (float *)malloc(i*sizeof(float));
      yvect = (float *)malloc(i*sizeof(float));
      xvecti = (float *)malloc(i*sizeof(float));
      yvecti = (float *)malloc(i*sizeof(float));
      
      tempvect = (struct elem *)malloc(j*sizeof(struct elem));
      
      /*Carico il vettore temporaneo
      Il suo caricamento è valido per tutti e due i metodi */
      tempvect[0].altez = 0;
      prec_altezza=0;
      l=0;
      for(int k=1;k<j-1;k=k+2){
              htemp = h1*pow(tau,l);
              tempvect[k].altez = prec_altezza + htemp - spessore;
              tempvect[k+1].altez = prec_altezza + htemp + spessore;
              prec_altezza = prec_altezza + htemp;
              l++;
              printf("h%d = %f\n",l,htemp);
              }        
      tempvect[j-1].altez = htot;
      printf("htot = %f\n",htot);
      //fine caricamento
      /*eseguo calcoli*/
      for(int k=0;k<j;k++){
              tempvect[k].tang = tempvect[k].altez*tangente;
              tempvect[k].tanplus = tempvect[k].tang+spessore;
              tempvect[k].tanminus = tempvect[k].tang-spessore;
              
              }
      
               //alloco vettore sequenza e lo inizializzo      
            sequ = (int *)malloc(i*sizeof(int));
                for(int k=0;k<i;k++){
                        sequ[k] = 0; //[0,0,...,0] per z volte
                        }
            /*
            Sezione di estrazione sequenze e stampa dei comandi
            */         
            crea_sequenza(sequ,num,i,second_mode);
            printf("\n\nSequenza finale = ");
            for(int t=0;t<i;t++){
                  printf("%d ",sequ[t]);
                  } 
            printf("\n");
                  /*
                  caricamento dei vettori
                  yvect non cambia a seconda della sequenza, mentre xvect ha due metodi speculari
                  */
                  carica_yvect(yvect, tempvect, sequ, i);
                  carica_xvect(xvect, tempvect, sequ, i, second_mode);
                  free(tempvect);//dealloco un vettore oramai in disuso
                  //     Inversione di valori
                  inverti(xvect, yvect, xvecti,yvecti, i);
                  //usero xvecttemp e yvecttemp come vettori finali nel qualo potro estrapolare un solo codice
                  l = (i*2) - 3;
                  float *xvecttemp = (float *)malloc(l*sizeof(float));
                  float *yvecttemp = (float *)malloc(l*sizeof(float));
                  //adattamento dei due vettori
                  i = adatta(xvect, yvect, xvecti, yvecti, xvecttemp,yvecttemp, i);
                  /*
                  Stampa a video e su file
                  */
                  stampa(xvecttemp, yvecttemp, i);              
                  //deallocazione memoria
                  free(xvect);
                  free(yvect);
                  free(xvecti);
                  free(yvecti);
                  free(xvecttemp);
                  free(yvecttemp);
    system("PAUSE");
}


void stampa(float *xvect, float *yvect, int i){
     //   Stampa a schermo
     stampa_a_schermo(xvect, yvect, i);
     //     Scrittura su file
     stampa_su_file(xvect, yvect, i);
}

void inverti(float *xvect, float *yvect, float *xvecti, float *yvecti, int i){
      for(int j=0;j<i;j++){
             xvecti[j] = xvect[j] * (-1);
             yvecti[j] = yvect[j] * (-1);
             }
}

void stampa_a_schermo(float *xvect, float *yvect, int i){
      printf("\n\n");
     printf("pdepoly([");
     for(int t=0;t<i;t++){
                  printf("%f ",xvect[t]);
                  }
     printf("],[");
     for(int t=0;t<i;t++){
                  printf("%f ",yvect[t]);
                  }
     printf("]);");
     printf("\n\n");
}

void stampa_su_file(float *xvect, float *yvect, int i){
     FILE *fp;
     fp = fopen("code.txt","w");
     fprintf(fp,"pdepoly([");
     for(int t=0;t<i;t++){
                  fprintf(fp,"%f ",xvect[t]);
                  }
     fprintf(fp,"],[");
     for(int t=0;t<i;t++){
                  fprintf(fp,"%f ",yvect[t]);
                  }
     fprintf(fp,"]);");
     fprintf(fp,"\n");
     fflush(fp);
     fclose(fp);
}

void carica_yvect(float *yvect, struct elem *tempvect, int *sequ, int i){
     for(int k=0;k<i;k++){
                          yvect[k]=tempvect[sequ[k]].altez;
                          }
}

void carica_xvect(float *xvect, struct elem*tempvect, int *sequ, int i, bool second_mode){
                   
                   int temp, k;
                   temp = i/2;
            if(second_mode){                            
                            //carico corretta sequenza
                            for(k=0;k<temp-1;k++){
                                    if(k%4<=1){
                                    xvect[k]=tempvect[sequ[k]].tanplus;
                                    }
                                    else{
                                         xvect[k]=tempvect[sequ[k]].tanminus;
                                         }
                            } 
                            for(k=i-1;k>=temp+1;k--){
                                  if(k%4>1){
                                    xvect[k]=tempvect[sequ[k]].tanplus;
                                    }else{
                                          xvect[k]=tempvect[sequ[k]].tanminus;
                                          }
                            } 
                            //modifico le parti centrali
                            k = temp - 1;
                                  if(k%4>1){
                                    xvect[k]=tempvect[sequ[k]].tanplus;
                                    xvect[k+1]=tempvect[sequ[k+1]].tanminus;
                                    }else{
                                          xvect[k]=tempvect[sequ[k]].tanminus;
                                          xvect[k+1]=tempvect[sequ[k+1]].tanplus;
                                          }  
                            //modifico i segni
                            for(k=0;k<i;k++){
                                    if(k%4<=1){
                                               xvect[k] = xvect[k] * (-1);
                                    }
                            } 
                            }//close if
            else{   
                   //carico corretta sequenza                                                       
                   for(k=0;k<i;k++){
                          if(k%4<=1){
                                    xvect[k]=tempvect[sequ[k]].tanplus;
                                    }
                                    else{
                                         xvect[k]=tempvect[sequ[k]].tanminus;
                                         }
                          } 
                  //modifico i segni
                  for(k=0;k<temp;k++){
                          if(k%4<=1){
                                    xvect[k] = xvect[k] * (-1);
                                    }        
                          } 
                  for(k=i-1;k>=temp;k--){
                          if(k%4>1){
                                    xvect[k] = xvect[k] * (-1);
                                    }
                          } 
                          }//close else                   
}

void crea_sequenza(int *sequ, int num, int z, bool second_mode){
    int a=2;
    int b, c, index;
    int *sequtemp, *sequtemp1;
    
    sequtemp1=(int *)malloc(a*sizeof(int));
    for(int i=0;i<a;i++){
            sequtemp1[i] = 0; //[0,0] 
    }
    //carico il vettore in modalita standard
    for(int i=0;i<num;i++){
            c = a;
            a = a + 4;
            sequtemp = (int *)malloc(a*sizeof(int));
                    for(int j=0;j<a;j++){
                            sequtemp[j] = 0; //inizializzazione vettore
                            } 
                    b = c/2;
                    /*copio meta vettore ad un estremo e l'altra meta 
                    nell'altro estremo cosi che posso 
                    successivamente elaborare la parte centrale*/
                    for(int j=0;j<c;j++){
                            if(j<b){
                                    sequtemp[j] = sequtemp1[j];
                                    }else{
                                          sequtemp[j+4] = sequtemp1[j];
                                          }
                            }                            
                    //carico i 4 spazi vuoti creati
                    if(sequtemp1[b]<sequtemp1[b-1]){
                                          sequtemp[b] = sequtemp[b-1] + 1;
                                          sequtemp[b+1] = sequtemp[b];
                                          sequtemp[b+2] = sequtemp[b+1] + 1;
                                          sequtemp[b+3] = sequtemp[b+2];
                                          }else{//if >=
                                                sequtemp[b+3] = sequtemp[b+4] + 1;
                                                sequtemp[b+2] = sequtemp[b+3];
                                                sequtemp[b+1] = sequtemp[b+2] + 1;
                                                sequtemp[b] = sequtemp[b+1];                                                
                                                }                                             
                    //vettore completato...ora copio i vettori
                    free(sequtemp1);
                    sequtemp1 = (int *)malloc(a*sizeof(int));
                    for(int k=0;k<a;k++){
                            sequtemp1[k]=sequtemp[k];                          
                            }                                                         
                    }//close for    
            //si necessita di un controllo sulla modalità di lavoro e continuare con eventuali modifiche.
                    if(!second_mode){                                     
                                     b = a/2;                                     
                                     if((num%2)==0){//è pari
                                              for(int i=0;i<a;i++){
                                                      if(i<b){
                                                              sequ[i] = sequtemp[i];
                                                              }else{
                                                                    sequ[i] = sequtemp[i+2];
                                                                    }
                                                                 }
                                     }else{//dispari
                                              for(int i=0;i<a;i++){
                                                      if(i<(b-2)){
                                                            sequ[i] = sequtemp[i];
                                                      }else{
                                                            sequ[i] = sequtemp[i+2];
                                                            }
                                                            }                                     
                                     }
                                     }else{
                                           for(int i=0;i<z;i++){
                                                   sequ[i]=sequtemp[i];
                                                   }
                                           }
} 

int adatta(float *xvect, float *yvect, float *xvecti, float *yvecti, float *xvecttemp, float *yvecttemp, int i){
     int j;
     int b;
     int a = i;
     i = (i*2) - 3;
     for(int k=0;k<a-1;k++){
             xvecttemp[k] = xvect[k];
             yvecttemp[k] = yvect[k];
             }
     b = 1;
     for(j=a-1;j<i;j++){
             xvecttemp[j] = xvecti[b];
             yvecttemp[j] = yvecti[b];
             b++;
             }
     return i;
}
