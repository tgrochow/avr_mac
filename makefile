all:
	cd uart        && $(MAKE)
	cd crypto_util && $(MAKE)
	cd skinny      && $(MAKE)
	cd dovemac     && $(MAKE)
	cd dovemac_pp  && $(MAKE)
	cd zmac        && $(MAKE)

clean:
	cd uart        && $(MAKE) clean
	cd crypto_util && $(MAKE) clean
	cd skinny      && $(MAKE) clean
	cd dovemac     && $(MAKE) clean
	cd dovemac_pp  && $(MAKE) clean
	cd zmac        && $(MAKE) clean
