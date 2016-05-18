all:
	$(MAKE) -C analyzer/
util:
	$(MAKE) -C util/
clean:
	$(MAKE) clean -C analyzer/
