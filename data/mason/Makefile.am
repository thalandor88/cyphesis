RULES = animals.xml basic.xml buildings.xml characters.xml clothing.xml \
        construction.xml devices.xml food.xml mason.xml operations.xml \
        plants.xml properties.xml tasks.xml terrain.xml tools.xml \
        weapons.xml

rulesetdir = $(sysconfdir)/cyphesis/mason.d
dist_ruleset_DATA = $(RULES)

confbackupdir = $(datadir)/doc/cyphesis-$(VERSION)/conf/mason
confbackup_DATA = $(RULES)

check-local:
	@for rule in $(MINIMAL_RULES) ; do \
	  echo $(XMLLINT) -noout $(srcdir)/$$rule; \
	  $(XMLLINT) -noout $(srcdir)/$$rule; \
	done;
