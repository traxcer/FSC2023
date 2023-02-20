#include <stdio.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE 8 //Nº de enteros en el payload (dimension matriz)
#define MAX_PDU_SIZE 34 //2 bytes (short size)+ 4 bytes (int)*8 (MAX_PAYLOAD_SIZE)=34

struct PDU{
    short size;
    int payload [MAX_PAYLOAD_SIZE];
};

//Prototipo funciones
void pack_pdu(char *dst, struct PDU *src); //recibe punteros
void unpack_pdu(struct PDU *dst, char *src); //recibe punteros

int main(){
    
    struct PDU pdu_packaged;
    struct PDU pdu_unpackaged;
    char buffer[MAX_PDU_SIZE];

    pdu_packaged.size=5; //numero de datos en el payload
    char a[]={87,28,82,58,87}; //datos para el payload

    memcpy(pdu_packaged.payload,a,pdu_packaged.size*sizeof(int)); //meto datos en payload

    pack_pdu(buffer,&pdu_packaged); //empaqueta
        printf("pdu_packeded en buffer = {");
            for(int i=0;i<MAX_PAYLOAD_SIZE-1;i++)
                printf("%d",buffer[i]);
        printf("}\n");

    unpack_pdu(&pdu_unpackaged,buffer); //desempaqueta
        printf("pdu_unpacked.size = %d\n",pdu_unpackaged.size);
        printf("pdu_unpacked.payload = {");
            for(int i=0;i<MAX_PAYLOAD_SIZE;i++)
                printf("%d, ",pdu_unpackaged.payload[i]);
        printf("}\n");
    return 0;
}

void pack_pdu(char *dst, struct PDU *src){
    memcpy(dst,&src->size,sizeof(short));
    memcpy(dst+sizeof(short),src->payload,src->size*sizeof(int));
}

void unpack_pdu(struct PDU *dst, char *src){ //recibe punteros
    memcpy(&dst->size,src,sizeof(sizeof(short)));
    memcpy(&dst->payload,src+sizeof(short),dst->size*sizeof(int));
}
