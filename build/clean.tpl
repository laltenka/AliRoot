
#  Clean option for the whole module
clean-@MODULE@:
ifndef ALIQUIET
		@echo "***** Cleaning @MODULE@ *****"
endif
		$(MUTE)rm @MODULE@/module.mk
		$(MUTE)rm -rf @MODULE@/tgt_$(ALICE_TARGET) 
		$(MUTE)rm -f $(@MODULE@LIBS)
		$(MUTE)rm -f $(@MODULE@BINS)

clean-check-@MODULE@:
ifndef ALIQUIET
		@echo "***** Cleaning code check for @MODULE@ *****"
endif
		$(MUTE)rm -f @MODULE@/check/*.i @MODULE@/check/*.ii @MODULE@/check/*.viol

clean-reveng-@MODULE@:
ifndef ALIQUIET
		@echo "***** Cleaning reverse engineering files for @MODULE@ *****"
endif
		$(MUTE)rm -f @MODULE@/check/*.dot

