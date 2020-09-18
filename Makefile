OPEN_SPIEL_PRIVATE =../open_spiel-private
EXE_DIR :=$(OPEN_SPIEL_PRIVATE)/build/bin/ltbr

ALG_GROUPS :=rla-cfr
GAMES :=tiny_bridge leduc kuhn_3p goofspiel random_goofspiel
DET_MODES :=fixed sim
DET_SSV_FILES :=$(foreach alg_group,$(ALG_GROUPS),\
	$(foreach game,$(GAMES),\
		$(foreach mode,$(DET_MODES),\
			data/$(alg_group).$(game).null.$(mode).gen.ssv)))
SEEDS :=$(shell seq 1 5)
RNG_MODES :=#shuffled
RNG_SSV_FILES :=$(foreach alg_group,$(ALG_GROUPS),\
	$(foreach game,$(GAMES),\
		$(foreach mode,$(RNG_MODES),\
			$(foreach seed,$(SEEDS),data/$(alg_group).$(game).null.$(mode).$(seed).gen.ssv))))
SSV_FILES :=$(DET_SSV_FILES) $(RNG_SSV_FILES)

default: results/mal_cfr_data.npy results/cor_gap_data.npy
	@true

data:
	mkdir $@

results:
	mkdir $@

$(EXE):
	cd $(OPEN_SPIEL_PRIVATE) && $(MAKE)

data/%.gen.ssv: | data
	python3 bin/run_experiment.py --exe_dir $(EXE_DIR) -a $* > $@

runs_remaining.gen.sh: bin/list_runs_remaining.sh Makefile
	$< > $@

results/mal_cfr_data.npy: $(SSV_FILES)
	python3 bin/save_data.py -x rla -o $@

results/cor_gap_data.npy: $(wildcard data/cor_gap.*.dat)
	python3 bin/save_data.py -x cor_gap -o $@

print-%:
	@echo $* = $($*)
