# RRT-Entregable2
Entregable correspondiente a la practica 2 de RRT

Esta practica se ha realizado implementando el protocolo Stop&Wait, como se comentara en la presentación, se han diseñado tres modulos distintos, una Fuente que genera muestras infinitas, un Nodo
que recibe el trafico desde distintas entradas y lo reenvia al siguiente nodo en función de una probabilidad, y un Fin que sera el nodo final y donde acaban los mensajes que se envian.

Dentro de la Fuente, se han generado distintas funciones entre las que encontramos:
    virtual void initialize() override; --> Inicializa las variables de estadisticas y envia el primer mensaje a la vez que activa el envio de los siguientes mensajes.
    virtual double getDepartures(double lambda); --> Generador de tiempos para el envio de los paquetes.
    virtual double getLengths(double mu); --> Generador de tamaños para la generación de los paquetes.
    virtual MyNetMsg* getPacket(); --> Generación de los paquetes inicializando los campos.
    virtual void handleMessage(cMessage *msg) override; --> Motor del envio continuo de paquetes.
    virtual void finish() override; --> Empleado para la recolección de estadísticas.
Dentro del Nodo, se han generado distintas funciones entre las que encontramos: 
    virtual void initialize() override; --> Define los canales a emplear y las colas de estos.
    virtual void handleMessage(cMessage *msg) override; --> Gestiona la recepción de mensajes, es donde se implementa la lógica del protocolo. Distintas funcionalidades si se recibe un paquete, un ACK, un NAK o un Timeout.
    virtual void sendNew(MyNetMsg *pkt); --> Metodo para el envio de paquetes, en caso de que la cola este vacia, se inserta y se envia, sino se encola al final.
    virtual void sendNext(int gateIndex); --> Empleado para hacer los reenvios de los paquetes cuando se recibe un NAK o un Timeout.
    virtual void sendPacket(MyNetMsg *pkt, int gateIndex); --> Encargado del envio del paquete, se emplea en el resto de metodos "send"
Dentro del Fin, se han generado distintas funciones entre las que encontramos: 
	virtual void handleMessage(cMessage *msg) override; --> Se encarga de realizar el envio de ACK o NAK para confirmar la recepción.

Los parametros que se pueden configurar desde el .ini son:

	MyNet.fuente1.lambda=2
	MyNet.fuente1.tampaquete=1000
	MyNet.fuente2.lambda=2
	MyNet.fuente2.tampaquete=1000
	MyNet.fuente3.lambda=2
	MyNet.fuente3.tampaquete=1000