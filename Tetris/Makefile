# Déclarations des variables du compilateur
CC=gcc
FLAGS=-Wall -pedantic
INC=-Isrc/
CFLAGS=$(FLAGS) -c -g --std=c99 $(INC)
LFLAGS=$(FLAGS) -lncurses

# Configurations de construction.
CFG=release
ifeq ($(CFG),debug)
FLAGS += -g -DDEBUG -DSMB_DEBUG
endif
ifneq ($(CFG),debug)
ifneq ($(CFG),release)
	@echo "Configuration invalide spécifiée "$(CFG)"."
	@echo "Vous devez spécifier une configuration lors de l'exécution de make, par exemple :"
	@echo "  make CFG=debug"
	@echo "Les choix sont 'release', 'debug'."
	@exit 1
endif
endif

SDL?=no
ifeq ($(SDL),yes)
CFLAGS += `sdl-config --cflags` -DWITH_SDL=1
LFLAGS += `sdl-config --libs` -lSDL_mixer


endif
ifneq ($(SDL),yes)
ifneq ($(SDL),no)
	@echo "Configuration SDL invalide spécifiée "$(SDL)"."
	@echo "Vous devez spécifier la configuration SDL lors de l'exécution de make, par exemple :"
	@echo "  make SDL=yes"
	@echo "Les choix sont 'yes', 'no'."
	@exit 1
endif
endif

# Sources et objets
VPATH = ./src/Modèle:./src/Vue:./src/Contrôleur:./src/Utilitaires

MODEL_SOURCES=$(shell find ./src/Modèle/ -type f -name "*.c")
VIEW_SOURCES=$(shell find ./src/Vue/ -type f -name "*.c")
CONTROLLER_SOURCES=$(shell find ./src/Contrôleur/ -type f -name "*.c")
UTIL_SOURCES=$(shell find ./src/Utilitaires/ -type f -name "*.c")

MODEL_OBJECTS=$(patsubst ./src/Modèle/%.c,bin/$(CFG)/%.o,$(MODEL_SOURCES))
VIEW_OBJECTS=$(patsubst ./src/Vue/%.c,bin/$(CFG)/%.o,$(VIEW_SOURCES))
CONTROLLER_OBJECTS=$(patsubst ./src/Contrôleur/%.c,bin/$(CFG)/%.o,$(CONTROLLER_SOURCES))
UTIL_OBJECTS=$(patsubst ./src/Utilitaires/%.c,bin/$(CFG)/%.o,$(UTIL_SOURCES))

# Cibles principales
.PHONY: all clean

# --- Règle de liaison
bin/$(CFG)/main_ncurses: $(MODEL_OBJECTS) $(VIEW_OBJECTS) $(CONTROLLER_OBJECTS) $(UTIL_OBJECTS)
	$(CC) $^ $(LFLAGS) -o bin/$(CFG)/main_ncurses

# --- Règle de compilation

bin/$(CFG)/%.o: %.c
	$(DIR_GUARD)
	$(CC) $(CFLAGS) $< -o $@

all: bin/$(CFG)/main

GTAGS: $(MODEL_SOURCES) $(VIEW_SOURCES) $(CONTROLLER_SOURCES) $(UTIL_SOURCES)
	gtags

clean:
	rm -rf bin/$(CFG)/* src/*.gch GTAGS GPATH GRTAGS

# --- Règle de dépendance
# Remarque : les dépendances ne sont pas utilisées dans cette version simplifiée

ifneq "$(MAKECMDGOALS)" "clean_all"
-include $(DEPS)
endif
