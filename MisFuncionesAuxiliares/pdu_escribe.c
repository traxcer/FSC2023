/**
 * Función que envía una PDU por el socket 
 * en una única llamada a write_n().
 * La función devuelve:
 * 	-1: en caso de error
 *   n: el nº de bytes escritos en otro caso
 */
int enviarPDU(int sd, struct PDU * pdu) {
	int buffer_size = sizeof(int) + pdu->size;
	char b[buffer_size];
	
	//empaqueto en el buffer
	pack_pdu(b, pdu);

	//envío el buffer
	if (write_n(sd, b, buffer_size) != buffer_size){
		perror("write_n buffer");
		return -1;
	}

 	return buffer_size;
}


void imprimirPDU(struct PDU * pdu) {
	pdu->payload[pdu->size] = '\0';
	printf("PDU = %s (%d)\n",pdu->payload, pdu->size);
}

void pack_pdu(char * b, struct PDU * pdu) {
	uint32_t size_big_endian = htonl(pdu->size);
	memcpy(b, &size_big_endian, sizeof(size_big_endian));
	memcpy(b+sizeof(size_big_endian), pdu->payload, pdu->size);
}