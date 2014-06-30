SRC=src/
COMMON=common/

.PHONY: all

all:
	$(MAKE) -C $(SRC)
	$(MAKE) -C $(COMMON)

clean:
	$(MAKE) -C $(SRC) clean
	$(MAKE) -C $(COMMON) clean
