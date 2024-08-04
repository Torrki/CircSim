#pragma once

#include <list>
#include "Component.h"
#include "Hotpoints.h"

class Circuit{
public:
	list<Component*> Componenti;
	list<Connection*> Connessioni;
	list<HotpointDrawable*> HotpointLinee;
};

