#include "clients.h"
#include "list.source.h"

#include <unistd.h>

LIST_GENERATE_SOURCE(int, Clients, clients, close)
