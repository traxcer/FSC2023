// Empaqueta en un buffer una pdu


#include <stdio.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE 8
#define MAX_PDU_SIZE 34 //2 pdu-> 2(short)+8(MAX_PAYLOAD_PDU)* 4(int)=34

struct PDU{
    short size;
    int payload[MAX_PAYLOAD_SIZE];
};

void pack_pdu(char *buffer,struct PDU *pdu); //prototipo de empaquetador

int main(){

    struct PDU pdu;
    int a[]={5,7,3,2}; //datos

    pdu.size=4; // ponemos los datos en la pdu
    memcpy(pdu.payload,a,pdu.size*sizeof(int));

    char buffer[MAX_PDU_SIZE]; //dispongo un buffer

    pack_pdu(buffer, &pdu); //cargo el buffer
    
    return 0;
}

void pack_pdu(char *buffer,struct PDU *pdu){
    memcpy(buffer, &pdu->size,sizeof(short));
    memcpy(buffer+sizeof(pdu->size),pdu->payload,pdu->size*sizeof(int));

}
