OPEN_SPIEL_PRIVATE =../open_spiel-private
EXE_DIR :=$(OPEN_SPIEL_PRIVATE)/build/bin/ltbr

GAMES :=tiny_bridge_2p leduc
MODES :=fixed sim
SAMPLERS :=null
SSV_FILES :=$(foreach game,$(GAMES),\
		$(foreach mode,$(MODES),\
			$(foreach sampler,$(SAMPLERS),\
				data/$(game).$(sampler).$(mode).gen.ssv)))

default: $(SSV_FILES)
	@true

SEEDS :=$(shell seq 1 5)

data:
	mkdir $@

results:
	mkdir $@

$(EXE):
	cd $(OPEN_SPIEL_PRIVATE) && $(MAKE)

data/%.fixed.gen.ssv: | data
	python3 bin/run_experiment.py --exe $(EXE_DIR)/run_fixed_ltbr -a $* > $@
data/%.sim.gen.ssv: | data
	python3 bin/run_experiment.py --exe $(EXE_DIR)/run_simultaneous_ltbr -a $* > $@

runs_remaining.gen.sh: bin/list_runs_remaining.sh Makefile
	$< > $@

print-%:
	@echo $* = $($*)
