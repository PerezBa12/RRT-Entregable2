//Iñigo Pérez Bada - Máster Teleco - RRT
//
simple Nodo
{
    parameters:
        @display("i=abstract/router2");
        double probability;
    gates:
        input inPort[];
        output outPort[];
}

simple Fin
{
    parameters:
        @display("i=abstract/server");
    gates:
        input inPort[];
        output outPort[];
}
simple Fuente
{
    parameters:
        @display("i=abstract/dispatcher");
        double tampaquete = default(1000/3);
        double lambda = default(2);
    gates:
        output outPort;
}

network MyNet
{
    @display("bgb=431,369");
    types:
        channel Channel extends ned.DatarateChannel
        {
            datarate = 3000bps;
            delay = 100us;
            per = 0.25;
        }
    submodules:
        nodo1: Nodo {
            probability = 1/4;
            @display("p=146,180");
            gates:
                inPort[3];
                outPort[2];
        }
        nodo2: Nodo {
            probability = 1/3;
            @display("p=239,121");
            gates:
                inPort[5];
                outPort[4];
        }
        nodo3: Fin {
            @display("p=364,121");
            gates:
                inPort[1];
                outPort[1];
        }
        nodo4: Fin {
            @display("p=364,242");
            gates:
                inPort[2];
                outPort[2];
        }
        nodo5: Nodo {
            probability = 1/2;
            @display("p=218,242");
            gates:
                inPort[4];
                outPort[3];
        }
        fuente1: Fuente {
            @display("p=38,180");
        }
        fuente2: Fuente {
            @display("p=239,23");
        }
        fuente3: Fuente {
            @display("p=218,325");
        }
    connections:
        nodo1.outPort[0] --> Channel --> nodo2.inPort[2];
        nodo1.inPort[0] <-- Channel <-- nodo2.outPort[2];

        nodo1.outPort[1] --> Channel --> nodo5.inPort[2];
        nodo1.inPort[1] <-- Channel <-- nodo5.outPort[2];

        nodo2.outPort[0] --> Channel --> nodo3.inPort[0];
        nodo2.inPort[0] <-- Channel <-- nodo3.outPort[0];

        nodo2.outPort[1] --> Channel --> nodo4.inPort[0];
        nodo2.inPort[1] <-- Channel <-- nodo4.outPort[0];

        nodo5.outPort[0] --> Channel --> nodo2.inPort[3];
        nodo5.inPort[0] <-- Channel <-- nodo2.outPort[3];

        nodo5.outPort[1] --> Channel --> nodo4.inPort[1];
        nodo5.inPort[1] <-- Channel <-- nodo4.outPort[1];

        fuente1.outPort --> nodo1.inPort[2];
        fuente2.outPort --> nodo2.inPort[4];
        fuente3.outPort --> nodo5.inPort[3];
}
