packet MyNetMsg {
	int inicial = false;
	int numSeq;
	int origen;
	int ruta[4];
	simtime_t rutaTimes[4]; 
}