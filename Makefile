

# Constantes liees au projet


DIRS=jpeg_format rt_YUV_format YUV_format

#
# La cible generale
#

all: $(patsubst %, _dir_%, $(DIRS))

$(patsubst %,_dir_%,$(DIRS)):
	cd $(patsubst _dir_%,%,$@) && make



debug: $(patsubst %, _debug_%, $(DIRS))

$(patsubst %,_debug_%,$(DIRS)):
	cd $(patsubst _debug_%,%,$@) && make debug
#
# La cible de nettoyage
#

clean: $(patsubst %, _clean_%, $(DIRS))

$(patsubst %,_clean_%,$(DIRS)):
	cd $(patsubst _clean_%,%,$@) && make clean
